#include <stdlib.h>
#include <string.h>

#include <so_common/so_logger.h>
#include <so_linker/so_linker.h>

#include <so_libs/libc/bionic_md5.h>
#include "ggee.h"

void ggeeLibEncodePath(char *out_encode, int out_len, const char *path) {
    struct md5    md5_ctx;
    char          md5_hex[36];
    unsigned char md5_res[16];
    size_t        path_len = strlen(path);

    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, path, path_len);
    MD5_Final(md5_res, &md5_ctx);

    // 将16字节MD5转换为32位十六进制字符串
    const char *hex_table = "0123456789abcdef";
    char       *hex_ptr   = md5_hex;
    for (int i = 0; i < sizeof(md5_res); i++) {
        hex_ptr[0] = hex_table[md5_res[i] >> 4];   // 高4位 → 十六进制字符
        hex_ptr[1] = hex_table[md5_res[i] & 0xF];  // 低4位 → 十六进制字符
        hex_ptr += 2;
    }
    md5_hex[32] = 0;

    strncpy(out_encode, md5_hex, out_len - 1);
    out_encode[out_len - 1] = 0;

    so_logd("ggeeLibEncodePath: path='%s', md5='%s'", path, out_encode);
}

int soloader_patch_libGgee(so_module *mod) {
    so_logd("soloader_patch_libGgee(%p)", mod);

    so_module_hook_symbol(mod, "ggeeLibEncodePath", &ggeeLibEncodePath, NULL);

    return 0;
}
