#include <string.h>

#include <so_common/so_logger.h>
#include <so_linker/so_linker.h>

#include "ld.h"

static int get_rel_plt(LdModule *mod, Elf32_Rel **rel_plt, uint32_t *num_rel_plt) {
    if (!mod || !mod->dynamic_phdr) {
        return -1;
    }

    Elf32_Phdr *dynamic_phdr = mod->dynamic_phdr;
    uintptr_t   load_bias    = (uintptr_t)mod->load_base + mod->load_offset;
    Elf32_Dyn  *dynamic      = (Elf32_Dyn *)(load_bias + dynamic_phdr->p_vaddr);
    int         dynamic_num  = dynamic_phdr->p_filesz / sizeof(Elf32_Dyn);

    *rel_plt     = NULL;
    *num_rel_plt = 0;

    for (int i = 0; i < dynamic_num; i++) {
        Elf32_Dyn *dyn = &dynamic[i];
        switch (dyn->d_tag) {
            case DT_JMPREL:
                *rel_plt = (Elf32_Rel *)(load_bias + dyn->d_un.d_ptr);
                break;
            case DT_PLTRELSZ:
                *num_rel_plt = dyn->d_un.d_val / sizeof(Elf32_Rel);
                break;
        }
        if (*rel_plt && *num_rel_plt > 0) {
            break;
        }
    }

    if (!*rel_plt && *num_rel_plt == 0) {
        return -1;
    }

    return 0;
}

int __ldModuleHookImport(LdModule *mod, const char *symbol, void *replacement) {
    if (!mod || !mod->dynsym || !mod->dynstr || !symbol) {
        return -1;
    }

    Elf32_Rel *rel_plt;
    uint32_t   num_rel_plt;
    if (get_rel_plt(mod, &rel_plt, &num_rel_plt) != 0) {
        so_logd("__ldModuleHookImport: '%s', failed to get rel_plt!", symbol);
        return -1;
    }

    uintptr_t load_bias = (uintptr_t)mod->load_base + mod->load_offset;

    for (int i = 0; i < num_rel_plt; i++) {
        Elf32_Rel *rel     = &rel_plt[i];
        uint32_t   sym_idx = ELF32_R_SYM(rel->r_info);
        if (sym_idx == 0) continue;

        Elf32_Sym  *sym  = &mod->dynsym[sym_idx];
        const char *name = mod->dynstr + sym->st_name;

        if (strcmp(name, symbol) == 0) {
            if (sym->st_shndx == SHN_UNDEF) {
                void *target = (void *)(load_bias + rel->r_offset);
                so_memcpy(target, &replacement, sizeof(void *));
                so_cacheflush(target, sizeof(void *));
                so_logd("__ldModuleHookImport: '%s' ->%p", symbol, replacement);
                return 0;
            }
        }
    }

    return -1;
}

int __ldModuleHookExport(LdModule *mod, const char *symbol, uintptr_t dst, so_hook_ref *p_hook) {
    uint32_t hash = ldModuleSymHash(symbol);
    void    *addr = ldModuleLookupSymbol(mod, symbol, hash, STV_DEFAULT, NULL, 0);
    return so_hook_function(addr, (void *)dst, p_hook);
}

int __ldModuleHookAddr(LdModule *mod, uintptr_t vaddr, uintptr_t dst, so_hook_ref *p_hook) {
    void *addr = (char *)mod->load_base + mod->load_offset + vaddr;
    return so_hook_function(addr, (void *)dst, p_hook);
}
