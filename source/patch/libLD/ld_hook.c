#include <string.h>

#include <so_common/so_logger.h>
#include <so_linker/so_linker.h>

#include "ld.h"

void *_find_import_slot(LdModule *mod, const char *symbol) {
    if (!mod || !mod->dynamic_phdr || !mod->dynsym || !mod->dynstr) return NULL;

    uintptr_t  load_bias = (uintptr_t)mod->load_base + mod->load_offset;
    Elf32_Dyn *dynamic   = (Elf32_Dyn *)(load_bias + mod->dynamic_phdr->p_vaddr);
    int        dyn_num   = mod->dynamic_phdr->p_filesz / sizeof(Elf32_Dyn);

    Elf32_Rel *rel_plt     = NULL;
    uint32_t   num_rel_plt = 0;
    for (int i = 0; i < dyn_num; i++) {
        Elf32_Dyn *dyn = &dynamic[i];
        switch (dyn->d_tag) {
            case DT_JMPREL:
                rel_plt = (Elf32_Rel *)(load_bias + dyn->d_un.d_ptr);
                break;
            case DT_PLTRELSZ:
                num_rel_plt = dyn->d_un.d_val / sizeof(Elf32_Rel);
                break;
        }
        if (rel_plt && num_rel_plt > 0) {
            break;
        }
    }
    if (!rel_plt || num_rel_plt == 0) return NULL;

    for (uint32_t i = 0; i < num_rel_plt; i++) {
        Elf32_Rel *rel     = &rel_plt[i];
        uint32_t   sym_idx = ELF32_R_SYM(rel->r_info);
        if (sym_idx == 0) continue;

        Elf32_Sym *sym = &mod->dynsym[sym_idx];
        if (sym->st_shndx == SHN_UNDEF && strcmp(mod->dynstr + sym->st_name, symbol) == 0) {
            return (void *)(load_bias + rel->r_offset);  // GOT 槽地址
        }
    }

    return NULL;
}

// 找到 symbol 链接器已绑定的真实地址（保留 Thumb 位），用于直接调用。
// 例如模块 import 的 fopen（游戏自带 libc.so，带资源路径转换）。
void *__ldModuleFindImport(LdModule *mod, const char *symbol) {
    void *slot = _find_import_slot(mod, symbol);
    return slot ? *(void **)slot : NULL;
}

void *__ldModuleFindExport(LdModule *mod, const char *symbol) {
    uint32_t hash = ldModuleSymHash(symbol);
    return ldModuleLookupSymbol(mod, symbol, hash, STV_DEFAULT, NULL, 0);
}

int __ldModuleHookImport(LdModule *mod, const char *symbol, void *replacement, so_hook_ref *p_hook) {
    // 必须传槽地址：so_hook_import_function 把 replacement 写进槽里完成重定向，
    // 传函数地址会把 replacement 当指令写进原函数代码，直接损坏 libc
    void *slot = _find_import_slot(mod, symbol);
    if (!slot) {
        so_loge("__ldModuleHookImport('%s'): import slot not found!", symbol);
        return -1;
    }

    // 重定位前槽值为 0（ELF 文件里的初始值），此时写入的 hook 会被链接器随后的
    // R_ARM_JUMP_SLOT 重定位覆盖，只会白做一次，所以必须等绑定完成后（ld_patch 阶段）再写。
    // 这里做运行时校验，避免将来误在 preinit 阶段调用。
    if (*(void **)slot == NULL) {
        so_loge("__ldModuleHookImport('%s'): slot %p not relocated yet, hook skipped!", symbol, slot);
        return -1;
    }

    return so_hook_import_function(slot, (void *)replacement, p_hook);
}

int __ldModuleHookExport(LdModule *mod, const char *symbol, uintptr_t dst, so_hook_ref *p_hook) {
    void *target = __ldModuleFindExport(mod, symbol);
    return so_hook_export_function(target, (void *)dst, p_hook);
}

int __ldModuleHookAddr(LdModule *mod, uintptr_t vaddr, uintptr_t dst, so_hook_ref *p_hook) {
    void *target = (char *)mod->load_base + mod->load_offset + vaddr;
    return so_hook_export_function(target, (void *)dst, p_hook);
}
