
#include <so_common/so_logger.h>

#include "ld.h"

static int sub_52860(int *a1) {  // 某种验证，返回0为无错误
    int res = 0;
    so_logd("sub_52860(%p): %d", a1, res);
    return res;
}

int ld_patch_bootstrap(LdModule *mod) {
    so_logd("ld_patch_bootstrap(%p)", mod);

    __ldModuleHookAddr(mod, 0x52860 + 1, (uintptr_t)&sub_52860, NULL);

    return 0;
}
