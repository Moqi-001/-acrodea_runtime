#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <so_common/so_logger.h>

#include "ld.h"

// -------------------------------------------------------------------------------------
// NNMalloc：原实现分配时不置零，后续又用「成员 == 0」判断指针有效性，
// 脏内存会导致误判，进而 free 野指针崩溃。这里补上清零。
// -------------------------------------------------------------------------------------
static void *NNMalloc(uint32_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        so_loge("[Disgaea] NNMalloc(size=%u): malloc failed", size);
        return NULL;
    }
    memset(ptr, 0, size);
    return ptr;
}

// -------------------------------------------------------------------------------------
// siSetSndVolume / siSetSndMute：原实现直接解引用 stcAudioItf + 8 处的 SLVolumeItf，
// 全程没有 NULL 判断。fade_Bgm_job() 由渲染主循环每帧调用且不检查 create_player()
// 的返回值，所以「有淡入淡出在跑 + 当前没有 BGM player」时会 data abort
// -------------------------------------------------------------------------------------
static int itf_player_missing(const void *itf) {
    if (!itf) return 1;

    const uint32_t *p = (const uint32_t *)itf;
    // p[0] = SLObjectItf（player 本体），p[2] = stcAudioItf + 8 的 SLVolumeItf。
    // delete_player() 只清 p[0]，之后 p[2] 就是悬空的，两者都要判。
    return p[0] == 0 || p[2] == 0;
}

static uint32_t s_volume_skipped = 0;

static void siSetSndVolume_guard(so_hook_ref *ref, void *itf, uint32_t volume, uint32_t mute) {
    if (itf_player_missing(itf)) {
        if (s_volume_skipped++ == 0) {
            so_logw("[Disgaea] siSetSndVolume(itf=%p, volume=%u, mute=%u): no player, skipped",
                    itf, volume, mute);
        }
        return;
    }

    SO_CONTINUE(void, ref, itf, volume, mute);
}

static so_hook_ref siSetSndVolume_ref_1;
static so_hook_ref siSetSndVolume_ref_2;
static so_hook_ref siSetSndVolume_ref_3;

static void siSetSndVolume_1(void *itf, uint32_t volume, uint32_t mute) { siSetSndVolume_guard(&siSetSndVolume_ref_1, itf, volume, mute); }
static void siSetSndVolume_2(void *itf, uint32_t volume, uint32_t mute) { siSetSndVolume_guard(&siSetSndVolume_ref_2, itf, volume, mute); }
static void siSetSndVolume_3(void *itf, uint32_t volume, uint32_t mute) { siSetSndVolume_guard(&siSetSndVolume_ref_3, itf, volume, mute); }

static so_hook_ref siSetSndMute_ref;

static void siSetSndMute(void *itf, uint32_t mute) {
    if (itf_player_missing(itf)) {
        return;
    }

    SO_CONTINUE(void, &siSetSndMute_ref, itf, mute);
}

// -------------------------------------------------------------------------------------
// convertSjisToUtf8：原实现（ELF 内 0x1E4F2C）对字符串里每一个 2 字节字符都执行：
//   fopen("START_DAT/kanji_conversion_table.dat") → 逐行扫描整张汉字表 → fclose，
//   然后再处理下一个 2 字节字符。每个字符一次 fopen + 一次几乎全表扫描 + 一次 fclose，
//   字符串一长就是几百次全表扫描，弹键盘/设文本时卡几十秒。它只被 myTextInputHandler
//   调用，所以只在弹键盘时暴慢。
//
// 三处细节都是从反汇编里读出来的，这里必须照做：
//   1. 这个 fopen 是模块 import 进来的，由动态库链接器绑定（游戏自带的 libc.so），
//      带资源路径转换（文件在 res.pak 里）；eboot 自己的 fopen 看不到包里的文件。
//      所以要从模块 GOT 槽里取出它实际绑定的 fopen，并配套使用同一个 libc 的
//      fread / fclose —— FILE* 是各 libc 私有的，混用会崩。
//   2. 每条记录固定 20 字节：5 个「3 位 ASCII 数字 + 1 字节分隔符」，末尾一个换行。
//      字段依次是 utf8b1 utf8b2 utf8b3 sjisb1 sjisb2（原代码就是 fread(3)+atoi + fseek(1)）。
//   3. 匹配只比较 sjisb1/sjisb2 两个字节，命中就写 3 个 UTF-8 字节。
//
// 这里把整表一次性读进内存，建成 [2 字节 SJIS -> 3 字节 UTF-8] 直接查找表，之后每次
// 转换 O(n)。首次加载失败则退回原函数，保证功能不劣化。
// -------------------------------------------------------------------------------------

FILE *(*disgaea_fopen)(const char *filename, const char *mode);
size_t (*disgaea_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
int (*disgaea_fclose)(FILE *stream);

static uint32_t    kanji_table[65536];  // key=(sjisb1<<8)|sjisb2, value=u1|u2<<8|u3<<16；0=未映射
static int         kanji_state = 0;     // 0=未尝试 1=加载中 2=就绪 -1=失败
static so_hook_ref convertSjisToUtf8_ref;

// -------------------------------------------------------------------------------------
// 反向表：utf8(3字节) -> sjis(2字节)，供 convertUtf8ToSjis 使用。
// convertUtf8ToSjis 全 ELF 只有 1 个调用点（0x1F1838，关键盘后取名字），每次输入
// 也就十几个字符，所以没必要上哈希：按行序存成紧凑数组，查找就是线性扫，天然满足
// 原函数「同一 utf8 首行优先」的语义，代码也短。
// -------------------------------------------------------------------------------------
typedef struct {
    uint32_t utf8;  // 3 字节 UTF-8 拼成 24 位
    uint16_t sjis;  // 对应 SJIS 双字节
} KanjiRevEntry;

static KanjiRevEntry *kanji_rev     = NULL;
static uint32_t       kanji_rev_num = 0;
static uint32_t       kanji_rev_cap = 0;

static void kanji_rev_add(uint32_t utf8v, uint16_t sjisv) {
    if (!kanji_rev || kanji_rev_num >= kanji_rev_cap) return;  // 容量按「行数上限」预留，不会丢条目
    kanji_rev[kanji_rev_num].utf8 = utf8v;
    kanji_rev[kanji_rev_num].sjis = sjisv;
    kanji_rev_num++;
}

static int kanji_rev_lookup(uint32_t utf8v, uint16_t *out) {
    for (uint32_t i = 0; i < kanji_rev_num; i++) {
        if (kanji_rev[i].utf8 == utf8v) {  // 线性扫；按行序存入，天然「首行优先」
            *out = kanji_rev[i].sjis;
            return 1;
        }
    }
    return 0;
}

// 表里的字段是「3 位 ASCII 数字」，原代码对 3 字节做 atoi；这里等价实现（允许前导空格，
// 遇非数字结束）。返回 -1 表示这 3 字节里没有数字。
static int field_to_int(const char *p) {
    int v = 0, i = 0, got = 0;
    while (i < 3 && (p[i] == ' ' || p[i] == '\t')) i++;
    while (i < 3 && p[i] >= '0' && p[i] <= '9') {
        v = v * 10 + (p[i] - '0');
        i++;
        got = 1;
    }
    return got ? v : -1;
}

static int kanji_table_parse(const char *buf, size_t size) {
    int    n = 0;
    size_t i = 0;

    while (i + 20 <= size) {
        const char *p  = buf + i;
        int         u1 = field_to_int(p), u2 = field_to_int(p + 4), u3 = field_to_int(p + 8);
        int         s1 = field_to_int(p + 12), s2 = field_to_int(p + 16);

        if (u1 >= 0 && u2 >= 0 && u3 >= 0 && s1 >= 0 && s2 >= 0 &&
            u1 <= 0xFF && u2 <= 0xFF && u3 <= 0xFF && s1 <= 0xFF && s2 <= 0xFF) {
            uint32_t key = ((uint32_t)s1 << 8) | (uint32_t)s2;
            if (kanji_table[key] == 0) {  // 与原实现一致：先出现者优先
                kanji_table[key] = (uint32_t)u1 | ((uint32_t)u2 << 8) | ((uint32_t)u3 << 16);
                n++;
            }
            // 反向表同样按行序追加，查找时先出现者优先
            kanji_rev_add(((uint32_t)u1 << 16) | ((uint32_t)u2 << 8) | (uint32_t)u3,
                          (uint16_t)(((uint32_t)s1 << 8) | (uint32_t)s2));
        }

        // 跳到下一行（原代码用 fgets 吃掉本行剩余部分）
        const char *nl = memchr(buf + i + 20, '\n', size - (i + 20));
        if (!nl) break;
        i = (size_t)(nl - buf) + 1;
    }

    return n;
}

static char *kanji_read_all(FILE *fp, size_t *out_size) {
    size_t cap = 64 * 1024;
    size_t len = 0;
    char  *buf = malloc(cap);
    if (!buf) return NULL;

    for (;;) {
        if (len == cap) {
            if (cap >= (16u << 20)) break;
            char *nb = realloc(buf, cap * 2);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap *= 2;
        }

        size_t got = disgaea_fread(buf + len, 1, cap - len, fp);
        if (got == 0) break;
        len += got;
    }

    *out_size = len;
    return buf;
}

static void kanji_table_load(void) {
    kanji_state = 1;

    if (!disgaea_fopen || !disgaea_fread || !disgaea_fclose) {
        so_loge("[Disgaea] convertSjisToUtf8(): module stdio missing (fopen=%p fread=%p fclose=%p), fall back",
                (void *)disgaea_fopen, (void *)disgaea_fread, (void *)disgaea_fclose);
        kanji_state = -1;
        return;
    }

    FILE *fp = disgaea_fopen("START_DAT/kanji_conversion_table.dat", "r");
    if (!fp) {
        so_loge("[Disgaea] convertSjisToUtf8(): open table failed, fall back to original");
        kanji_state = -1;
        return;
    }

    size_t size = 0;
    char  *buf  = kanji_read_all(fp, &size);
    disgaea_fclose(fp);

    if (!buf) {
        so_loge("[Disgaea] convertSjisToUtf8(): read table failed, fall back to original");
        kanji_state = -1;
        return;
    }

    // 反向表容量直接按「每行至少 20 字节」估上限，不用再扫一遍数行。
    // 分配失败只影响反向表，convertUtf8ToSjis 会退化为「查不到 → 0x8148」，与原函数未命中行为一致
    kanji_rev_cap = (uint32_t)(size / 20) + 1;
    kanji_rev     = (KanjiRevEntry *)malloc(kanji_rev_cap * sizeof(KanjiRevEntry));
    if (!kanji_rev) {
        kanji_rev_cap = 0;
    }

    int n = kanji_table_parse(buf, size);
    free(buf);

    if (n > 0) {
        kanji_state = 2;
        so_loga("[Disgaea] convertSjisToUtf8(): table loaded, %d entries (file %u bytes)", n, (unsigned)size);
    } else {
        so_loge("[Disgaea] convertSjisToUtf8(): table has no entries (file %u bytes), fall back to original",
                (unsigned)size);
        kanji_state = -1;
    }
}

static int convertSjisToUtf8(const char *sjis, char *utf8) {
    if (kanji_state == 0) {
        kanji_table_load();
    }
    if (kanji_state != 2) {
        return SO_CONTINUE(int, &convertSjisToUtf8_ref, sjis, utf8);
    }

    uint8_t *in  = (uint8_t *)sjis;
    uint8_t *out = (uint8_t *)utf8;

    while (*in) {
        uint8_t  c0 = *in++;
        uint8_t  c1 = *in++;
        uint32_t v  = kanji_table[((uint32_t)c0 << 8) | c1];
        if (v) {
            *out++ = (uint8_t)(v & 0xFF);
            *out++ = (uint8_t)((v >> 8) & 0xFF);
            *out++ = (uint8_t)((v >> 16) & 0xFF);
        }
    }
    *out = '\0';

    return ((uintptr_t)out - (uintptr_t)utf8) / 3;
}

// -------------------------------------------------------------------------------------
// convertUtf8ToSjis（ELF 内 0x1E3D08）：关闭键盘拿到 inputText 后，游戏用它把 UTF-8 文本
// 转回 SJIS。与 convertSjisToUtf8 同款病理：每个 3 字节字符都 fopen + 全表扫描
// （反汇编确认：主循环 bne 跳回函数开头，fopen@0x1E4A6C 在逐字符路径里）。
//
// 原函数完整行为（全部从反汇编读出，这里照做）：
//   - ASCII（<0x80）：查硬编码阶梯转成全角 SJIS 2 字节（0x20 空格→0x8140、0x30-0x39 数字
//     →0x82,(c+0x1F)、0x41-0x5A→0x82,(c+0x1F)、0x61-0x7A→0x82,(c+0x20)，其余见
//     ascii_to_sjis 表，由 tools/extract_ladder.py 从反汇编机械提取）；'^'(0x5E) 原函数
//     特判：输出 0x8148 并置替换标志；控制字符与 0x7F：无分支命中，跳过不输出。
//   - 0x80-0xBF / 0xF8-0xFF：非法字节，原函数 sprintf 到局部缓冲后直接丢弃，不输出。
//   - 0xC0-0xDF / 0xF0-0xF7：读后续字节后输出 0x8148 并置替换标志。
//   - 0xE0-0xEF（汉字）：先特判 (c1,c2)==(0xBC,0xBC)→0x818F；否则查表
//     （3 字节全匹配、首行优先），未命中输出 0x8148 并置替换标志。
//   - 每字符处理完：若输出长度恰为 2 且等于 0x8140 → 清空（等效：反复删除行首全角空格）。
//   - 字符串结束：反复剔除末尾 0x8140 对（删除行尾全角空格）；返回 输出长度/2。
//   - 替换标志：GOT 槽 0x5C5A90（R_ARM_RELATIVE）指向的字节，置 1。
// -------------------------------------------------------------------------------------

static so_hook_ref convertUtf8ToSjis_ref;
static uint8_t    *sjis_replace_flag = NULL;  // 替换标志字节（ld_patch_disgaea 时从 GOT 槽取）

static const uint8_t ascii_to_sjis[128][2] = {
    {0x00, 0x00},  /* 0x00 . */
    {0x00, 0x00},  /* 0x01 . */
    {0x00, 0x00},  /* 0x02 . */
    {0x00, 0x00},  /* 0x03 . */
    {0x00, 0x00},  /* 0x04 . */
    {0x00, 0x00},  /* 0x05 . */
    {0x00, 0x00},  /* 0x06 . */
    {0x00, 0x00},  /* 0x07 . */
    {0x00, 0x00},  /* 0x08 . */
    {0x00, 0x00},  /* 0x09 . */
    {0x00, 0x00},  /* 0x0A . */
    {0x00, 0x00},  /* 0x0B . */
    {0x00, 0x00},  /* 0x0C . */
    {0x00, 0x00},  /* 0x0D . */
    {0x00, 0x00},  /* 0x0E . */
    {0x00, 0x00},  /* 0x0F . */
    {0x00, 0x00},  /* 0x10 . */
    {0x00, 0x00},  /* 0x11 . */
    {0x00, 0x00},  /* 0x12 . */
    {0x00, 0x00},  /* 0x13 . */
    {0x00, 0x00},  /* 0x14 . */
    {0x00, 0x00},  /* 0x15 . */
    {0x00, 0x00},  /* 0x16 . */
    {0x00, 0x00},  /* 0x17 . */
    {0x00, 0x00},  /* 0x18 . */
    {0x00, 0x00},  /* 0x19 . */
    {0x00, 0x00},  /* 0x1A . */
    {0x00, 0x00},  /* 0x1B . */
    {0x00, 0x00},  /* 0x1C . */
    {0x00, 0x00},  /* 0x1D . */
    {0x00, 0x00},  /* 0x1E . */
    {0x00, 0x00},  /* 0x1F . */
    {0x81, 0x40},  /* 0x20   */
    {0x81, 0x49},  /* 0x21 ! */
    {0x81, 0x68},  /* 0x22 " */
    {0x81, 0x94},  /* 0x23 # */
    {0x81, 0x90},  /* 0x24 $ */
    {0x81, 0x93},  /* 0x25 % */
    {0x81, 0x95},  /* 0x26 & */
    {0x81, 0x66},  /* 0x27 ' */
    {0x81, 0x69},  /* 0x28 ( */
    {0x81, 0x6A},  /* 0x29 ) */
    {0x81, 0x96},  /* 0x2A * */
    {0x81, 0x7B},  /* 0x2B + */
    {0x2A, 0x24},  /* 0x2C , */
    {0x81, 0x7C},  /* 0x2D - */
    {0x81, 0x44},  /* 0x2E . */
    {0x81, 0x5E},  /* 0x2F / */
    {0x82, 0x4F},  /* 0x30 0 */
    {0x82, 0x50},  /* 0x31 1 */
    {0x82, 0x51},  /* 0x32 2 */
    {0x82, 0x52},  /* 0x33 3 */
    {0x82, 0x53},  /* 0x34 4 */
    {0x82, 0x54},  /* 0x35 5 */
    {0x82, 0x55},  /* 0x36 6 */
    {0x82, 0x56},  /* 0x37 7 */
    {0x82, 0x57},  /* 0x38 8 */
    {0x82, 0x58},  /* 0x39 9 */
    {0x81, 0x46},  /* 0x3A : */
    {0x81, 0x47},  /* 0x3B ; */
    {0x81, 0x83},  /* 0x3C < */
    {0x81, 0x81},  /* 0x3D = */
    {0x81, 0x84},  /* 0x3E > */
    {0x81, 0x48},  /* 0x3F ? */
    {0x81, 0x97},  /* 0x40 @ */
    {0x82, 0x60},  /* 0x41 A */
    {0x82, 0x61},  /* 0x42 B */
    {0x82, 0x62},  /* 0x43 C */
    {0x82, 0x63},  /* 0x44 D */
    {0x82, 0x64},  /* 0x45 E */
    {0x82, 0x65},  /* 0x46 F */
    {0x82, 0x66},  /* 0x47 G */
    {0x82, 0x67},  /* 0x48 H */
    {0x82, 0x68},  /* 0x49 I */
    {0x82, 0x69},  /* 0x4A J */
    {0x82, 0x6A},  /* 0x4B K */
    {0x82, 0x6B},  /* 0x4C L */
    {0x82, 0x6C},  /* 0x4D M */
    {0x82, 0x6D},  /* 0x4E N */
    {0x82, 0x6E},  /* 0x4F O */
    {0x82, 0x6F},  /* 0x50 P */
    {0x82, 0x70},  /* 0x51 Q */
    {0x82, 0x71},  /* 0x52 R */
    {0x82, 0x72},  /* 0x53 S */
    {0x82, 0x73},  /* 0x54 T */
    {0x82, 0x74},  /* 0x55 U */
    {0x82, 0x75},  /* 0x56 V */
    {0x82, 0x76},  /* 0x57 W */
    {0x82, 0x77},  /* 0x58 X */
    {0x82, 0x78},  /* 0x59 Y */
    {0x82, 0x79},  /* 0x5A Z */
    {0x81, 0x6D},  /* 0x5B [ */
    {0x81, 0x8F},  /* 0x5C \ */
    {0x81, 0x6E},  /* 0x5D ] */
    {0x81, 0x48},  /* 0x5E ^ */
    {0x81, 0x51},  /* 0x5F _ */
    {0x81, 0x65},  /* 0x60 ` */
    {0x82, 0x81},  /* 0x61 a */
    {0x82, 0x82},  /* 0x62 b */
    {0x82, 0x83},  /* 0x63 c */
    {0x82, 0x84},  /* 0x64 d */
    {0x82, 0x85},  /* 0x65 e */
    {0x82, 0x86},  /* 0x66 f */
    {0x82, 0x87},  /* 0x67 g */
    {0x82, 0x88},  /* 0x68 h */
    {0x82, 0x89},  /* 0x69 i */
    {0x82, 0x8A},  /* 0x6A j */
    {0x82, 0x8B},  /* 0x6B k */
    {0x82, 0x8C},  /* 0x6C l */
    {0x82, 0x8D},  /* 0x6D m */
    {0x82, 0x8E},  /* 0x6E n */
    {0x82, 0x8F},  /* 0x6F o */
    {0x82, 0x90},  /* 0x70 p */
    {0x82, 0x91},  /* 0x71 q */
    {0x82, 0x92},  /* 0x72 r */
    {0x82, 0x93},  /* 0x73 s */
    {0x82, 0x94},  /* 0x74 t */
    {0x82, 0x95},  /* 0x75 u */
    {0x82, 0x96},  /* 0x76 v */
    {0x82, 0x97},  /* 0x77 w */
    {0x82, 0x98},  /* 0x78 x */
    {0x82, 0x99},  /* 0x79 y */
    {0x82, 0x9A},  /* 0x7A z */
    {0x81, 0x6F},  /* 0x7B { */
    {0x81, 0x62},  /* 0x7C | */
    {0x81, 0x70},  /* 0x7D } */
    {0x81, 0x60},  /* 0x7E ~ */
};

static int convertUtf8ToSjis(const char *utf8, char *sjis) {
    if (kanji_state == 0) {
        kanji_table_load();
    }
    if (kanji_state != 2) {
        return SO_CONTINUE(int, &convertUtf8ToSjis_ref, utf8, sjis);
    }

    const uint8_t *in  = (const uint8_t *)utf8;
    uint8_t       *out = (uint8_t *)sjis;
    int            olen = 0;

    while (*in) {
        uint8_t c0 = *in;
        int     clen;

        if (c0 < 0x80) {  // ASCII 阶梯：全角化
            clen = 1;
            if (c0 == 0x5E && sjis_replace_flag) {
                *sjis_replace_flag = 1;  // '^' 原函数特判：0x8148 + 置标志
            }
            uint16_t v = ((uint16_t)ascii_to_sjis[c0][0] << 8) | ascii_to_sjis[c0][1];
            if (v) {
                *out++ = (uint8_t)(v >> 8);
                *out++ = (uint8_t)(v & 0xFF);
                olen += 2;
            }
        } else if (c0 < 0xC0 || c0 >= 0xF8) {  // 非法字节：丢弃
            clen = 1;
        } else if (c0 < 0xE0) {  // 2 字节：0x8148 + 标志
            clen = 2;
            *out++ = 0x81;
            *out++ = 0x48;
            olen += 2;
            if (sjis_replace_flag) *sjis_replace_flag = 1;
        } else if (c0 < 0xF0) {  // 3 字节汉字：查反向表
            clen = 3;
            uint8_t  c1 = in[1], c2 = in[2];
            uint16_t v;
            if (c1 == 0xBC && c2 == 0xBC) {
                v = 0x818F;  // 原函数硬编码特例
            } else if (!kanji_rev_lookup(((uint32_t)c0 << 16) | ((uint32_t)c1 << 8) | c2, &v)) {
                v = 0x8148;  // 未命中：替换并置标志（与原函数一致）
                if (sjis_replace_flag) *sjis_replace_flag = 1;
            }
            *out++ = (uint8_t)(v >> 8);
            *out++ = (uint8_t)(v & 0xFF);
            olen += 2;
        } else {  // 4 字节：0x8148 + 标志
            clen = 4;
            *out++ = 0x81;
            *out++ = 0x48;
            olen += 2;
            if (sjis_replace_flag) *sjis_replace_flag = 1;
        }
        in += clen;

        // 行首全角空格删除：原函数每字符后检查「len==2 且开头是 0x8140」则清空重来
        while (olen == 2 && out[-2] == 0x81 && out[-1] == 0x40) {
            olen = 0;
            out -= 2;
        }
    }

    // 行尾全角空格删除：原函数反复剔除末尾 0x8140 对
    while (olen >= 2 && out[-2] == 0x81 && out[-1] == 0x40) {
        olen -= 2;
        out -= 2;
    }
    *out = '\0';

    return olen / 2;
}


int ld_patch_preinit_disgaea(LdModule *mod) {
    __ldModuleHookAddr(mod, 0x1A6D98, (uintptr_t)&NNMalloc, NULL);

    __ldModuleHookAddr(mod, 0x14B838, (uintptr_t)&siSetSndVolume_1, &siSetSndVolume_ref_1);
    __ldModuleHookAddr(mod, 0x16722C, (uintptr_t)&siSetSndVolume_2, &siSetSndVolume_ref_2);
    __ldModuleHookAddr(mod, 0x1E0A84, (uintptr_t)&siSetSndVolume_3, &siSetSndVolume_ref_3);
    __ldModuleHookAddr(mod, 0x1672E8, (uintptr_t)&siSetSndMute, &siSetSndMute_ref);

    __ldModuleHookAddr(mod, 0x1E4F2C, (uintptr_t)&convertSjisToUtf8, &convertSjisToUtf8_ref);
    __ldModuleHookAddr(mod, 0x1E3D08, (uintptr_t)&convertUtf8ToSjis, &convertUtf8ToSjis_ref);

    return 0;
}

int ld_patch_disgaea(LdModule *mod) {
    disgaea_fopen  = __ldModuleFindImport(mod, "fopen");
    disgaea_fread  = __ldModuleFindImport(mod, "fread");
    disgaea_fclose = __ldModuleFindImport(mod, "fclose");

    // 替换标志：ELF 0x5C5A90 是 R_ARM_RELATIVE 重定位的指针槽，重定位后槽里就是标志字节地址
    sjis_replace_flag = *(uint8_t **)((char *)mod->load_base + mod->load_offset + 0x5C5A90);

    return 0;
}
