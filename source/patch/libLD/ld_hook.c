#include <string.h>

#include <so_common/so_logger.h>
#include <so_linker/so_linker.h>

#include "ld.h"

static void *_find_import_slot(LdModule *mod, const char *symbol) {
    if (!mod || !mod->dynamic_phdr || !mod->dynsym || !mod->dynstr) {
        return NULL;
    }

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
    if (!rel_plt || num_rel_plt == 0) {
        return NULL;
    }

    for (uint32_t i = 0; i < num_rel_plt; i++) {
        Elf32_Rel *rel     = &rel_plt[i];
        uint32_t   sym_idx = ELF32_R_SYM(rel->r_info);
        if (sym_idx == 0) {
            continue;
        }

        Elf32_Sym *sym = &mod->dynsym[sym_idx];
        if (sym->st_shndx == SHN_UNDEF && strcmp(mod->dynstr + sym->st_name, symbol) == 0) {
            return (void *)(load_bias + rel->r_offset);  // GOT 槽地址
        }
    }

    return NULL;
}

void *__ldModuleFindImport(LdModule *mod, const char *symbol) {
    void *slot = _find_import_slot(mod, symbol);
    return slot ? *(void **)slot : NULL;
}

void *__ldModuleFindExport(LdModule *mod, const char *symbol) {
    uint32_t hash = ldModuleSymHash(symbol);
    return ldModuleLookupSymbol(mod, symbol, hash, STV_DEFAULT, NULL, 0);
}

int __ldModuleHookImport(LdModule *mod, const char *symbol, void *repl, so_hook_ref *p_hook) {
    void *slot = _find_import_slot(mod, symbol);
    return so_hook_got(slot, repl, p_hook);
}

int __ldModuleHookExport(LdModule *mod, const char *symbol, uintptr_t dst, so_hook_ref *p_hook) {
    void *target = __ldModuleFindExport(mod, symbol);
    return so_hook(target, (void *)dst, p_hook);
}

int __ldModuleHookAddr(LdModule *mod, uintptr_t vaddr, uintptr_t dst, so_hook_ref *p_hook) {
    void *target = (char *)mod->load_base + mod->load_offset + vaddr;
    return so_hook(target, (void *)dst, p_hook);
}
