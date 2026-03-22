#ifndef SOLOADED_LD_H
#define SOLOADED_LD_H

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#include <so_util/elf.h>
#include <so_util/so_util.h>

// 错误码定义
#define LD_ERROR_MEMORY_ALLOC_FAILED 0xE030001
#define LD_ERROR_OPEN_FILE_FAILED    0xE030700
#define LD_ERROR_EXPAND_LIB_FAILED   0xE030400
#define LD_ERROR                     0xE03FE00

typedef enum {
    LD_MODULE_STATUS_NONE          = 0,
    LD_MODULE_STATUS_RELOCATED     = 1,
    LD_MODULE_STATUS_INITIALIZED   = 2,
    LD_MODULE_STATUS_DEINITIALIZED = 3
} LdModuleStatus;

typedef struct LdContext    LdContext;
typedef struct LdModule     LdModule;
typedef struct LdModuleNode LdModuleNode;

typedef struct IoVtable {
    FILE *(*fopen)(const char *filename, const char *mode);                                 // +0
    size_t (*fread)(void *ptr, void *a2, int a3, size_t size, size_t nmemb, FILE *stream);  // +4
    long (*ftell)(FILE *stream);                                                            // +8
    int (*fseek)(FILE *stream, long int offset, int whence);                                // +12
    int (*fclose)(FILE *stream);                                                            // +16
} IoVtable;

// 未确认，先占位
typedef struct DlVtable {
    void *(*open)(LdContext *context, const char *filepath, int a3, LdModule *a4, int a5, int a6);  // +0
    void (*func_4)(void);                                                                           // +4
    void (*func_8)(void *handle);                                                                   // +8
    void (*func_12)(void *handle);                                                                  // +12
    void (*func_16)(void *handle);                                                                  // +16
    void (*func_20)(void *handle);                                                                  // +20
} DlVtable;

// 模块链表节点
struct LdModuleNode {
    LdModule     *mod;
    LdModuleNode *next;
};

// 动态库加载器上下文
struct LdContext {
    LdModuleNode    *loaded_mods;        // +0   已加载的模块链表（头部）
    LdModuleNode   **tail_next_mod_ref;  // +4   已加载的模块链表尾部的next引用
    LdModuleNode    *unloaded_mods;      // +8   未加载的模块链表（后进先出，实为尾部）
    pthread_mutex_t *mutex;              // +12  线程互斥锁
    DlVtable         dl_vtable;          // +16  6个dl虚函数表 （程序内传入了0）
    IoVtable         io_vtable;          // +40  5个io虚函数表
    int32_t          exit_state;         // +60  退出状态
    char             error_msg[256];     // +64  错误信息
    int32_t          error_code;         // +320 错误代码
};  // +324

// 动态库加载模块结构体
struct LdModule {
    uint32_t       ref_count;     // +0   引用计数，管理模块生命周期
    char          *so_name;       // +4   动态库名称（如 "libexample.so"）
    char          *so_path;       // +8   动态库完整路径
    uint32_t       flags;         // +12  ? 加载标志位 (初始化值为0xFFFFEFFF，添加到context后|0x1000，最终的值应为0xFFFFFFFF)
    uint32_t       status;        // +16  模块加载状态 (0=未加载, 1=已加载, 2=已初始化等)
    LdContext     *context;       // +20  所属加载器上下文（反向引用）
    LdModule      *parent_mod;    // +24  ? 父模块（依赖此模块的上层模块，用于依赖关系管理）
    uint32_t       needed_num;    // +28  依赖的动态库数量 (DT_NEEDED 条目数)
    LdModule     **needed_mods;   // +32  依赖模块指针数组（指向被依赖的子模块）
    uint32_t       unk_36;        // +36  ? 未知 (有个<=0的判断)
    void          *load_mem;      // +40  加载到内存的 SO 文件数据
    uint32_t       load_size;     // +44  加载到内存的 SO 文件数据大小
    uint32_t       load_offset;   // +48  内存加载偏移量（可能用于对齐或分段加载，实际为0未使用）
    void          *tls;           // +52  线程局部存储 (TLS) 初始化数据地址
    uint32_t       tls_filesz;    // +56  TLS 在文件中的大小 (p_filesz)
    uint32_t       tls_memsz;     // +60  TLS 在内存中的大小 (p_memsz)
    uint32_t       tls_align;     // +64  ? TLS 对齐方式或映射标志 (p_align)
    Elf32_Ehdr    *ehdr;          // +68  ELF 文件头指针 (Elf32_Ehdr)
    uint32_t      *hashtab;       // +72  符号哈希表指针 (用于快速查找符号，SHT_HASH)
    char          *strtab;        // +76  字符串表指针 (存放符号名称，DT_STRTAB)
    Elf32_Sym     *symtab;        // +80  符号表指针 (Elf32_Sym 数组，DT_SYMTAB)
    Elf32_Phdr    *phdr;          // +84  程序头表指针 (Elf32_Phdr 数组，用于内存映射)
    Elf32_Phdr    *dynamic_phdr;  // +88  动态段程序头指针 (PT_DYNAMIC 段)
    Elf32_Verdef  *verdef;        // +92  版本定义表指针 (用于符号版本控制，DT_VERDEF)
    uint32_t       verdefnum;     // +96  版本定义数量 (DT_VERDEFNUM)
    Elf32_Verneed *verneed;       // +100 版本需求表指针 (依赖库的版本要求，DT_VERNEED)
    uint32_t       verneednum;    // +104 版本需求数量 (DT_VERNEEDNUM)
    Elf32_Versym  *versym;        // +108 符号版本表指针 (每个符号对应的版本，DT_VERSYM)
    uint32_t       versymnum;     // +112 符号版本表条目数量
};  // 总大小: 116 字节

// 动态库创建模块结构体，使用函数名与函数地址生成虚拟的LdModule
typedef struct LdBindingCreator {
    uint32_t     dynsym_capacity;  // +0   动态符号总容量
    uint32_t     dynsym_num;       // +4   动态符号当前个数
    uint32_t     dynstr_size;      // +8   动态字符串表大小
    const char **dynstr_array;     // +12  动态字符串指针数组
    void       **dynsym_array;     // +16  动态符号指针数组
    uint32_t     reserved[2];      // +20  (未见使用，应是预留)
} LdBindingCreator;                // +28

const char *ldGetErrorMessage(LdContext *context);
void       *ldModuleGetEntryPoint(LdModule *mod);

uint32_t ldModuleSymHash(const uint8_t *name);
int      ldModuleAllocAndLoadImage(LdModule *mod, const char *path, const char *err_msg);
void     ldModuleDestroyImage(LdModule *mod);
int      ldModuleProtectImage(LdModule *mod);
int      ldModuleRelocate(LdModule *mod);
int      ldModuleInitialize(LdModule *mod);
int      ldModuleDeinitialize(LdModule *mod);

extern uintptr_t (*ldModuleGetSymbol)(LdModule *in_mod, const char *sym_name, uint32_t sym_hash, int type,
                                      const char *ver_name, uint32_t ver_hash, LdModule **out_mod);
extern uintptr_t (*ldModuleLookupSymbol)(LdModule *mod, const char *sym_name, uint32_t sym_hash, int visibility,
                                         const char *ver_name, uint32_t ver_hash);

so_hook __ldModuleHookAddr(LdModule *mod, uintptr_t vaddr, uintptr_t dst);
so_hook __ldModuleHookSymbol(LdModule *mod, const char *symbol, uintptr_t dst);

#endif