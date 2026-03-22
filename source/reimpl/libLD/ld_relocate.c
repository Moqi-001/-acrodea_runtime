#include <stddef.h>
#include <stdint.h>

#include <kubridge.h>
#include <so_util/elf.h>

#include "reimpl/libLD/ld.h"
#include "utils/logger.h"

static int relocate_rel_table(LdModule *mod, Elf32_Rel *rel_tab, size_t rel_size) {
    char      error_msg[256];
    uintptr_t load_base = (uintptr_t)mod->load_mem + mod->load_offset;
    size_t    rel_num   = rel_size / sizeof(Elf32_Rel);

    for (size_t i = 0; i < rel_num; i++) {
        Elf32_Rel *rel        = &rel_tab[i];
        uint32_t   rel_offset = rel->r_offset;
        uint32_t   rel_sym    = ELF32_R_SYM(rel->r_info);
        uint8_t    rel_type   = ELF32_R_TYPE(rel->r_info);
        uintptr_t *rel_addr   = (uintptr_t *)(load_base + rel->r_offset);

        // 相对重定位不需要符号，提前判断
        if (rel_type == R_ARM_RELATIVE) {
            *rel_addr = load_base + *rel_addr;
            // l_debug("  R_ARM_RELATIVE: %s -> 0x%x", sym_name, *rel_addr);
            continue;
        }

        // ===== 获取版本信息 =====
        const char *ver_name = NULL;
        uint32_t    ver_hash = 0;

        if (mod->versym) {
            Elf32_Verneed *verneed    = mod->verneed;
            uint32_t       verneednum = mod->verneednum;
            Elf32_Versym   ver        = mod->versym[rel_sym] & 0x7FFF;
            if (ver > 1 && verneed) {
                for (int j = 0; j < verneednum; j++) {
                    Elf32_Vernaux *aux = (Elf32_Vernaux *)((uintptr_t)verneed + verneed->vn_aux);
                    for (int k = 0; k < verneed->vn_cnt; k++) {
                        if (aux->vna_other == ver) {
                            ver_name = mod->strtab + aux->vna_name;
                            ver_hash = aux->vna_hash;
                            break;
                        }
                        aux = (Elf32_Vernaux *)((uintptr_t)aux + aux->vna_next);
                    }
                    if (ver_name) break;
                    verneed = (Elf32_Verneed *)((uintptr_t)verneed + verneed->vn_next);
                }
            }
        }

        // ===== 查找符号地址 =====
        uintptr_t   sym_addr = 0;
        LdModule   *sym_mod  = NULL;
        const char *sym_name = mod->strtab + mod->symtab[rel_sym].st_name;

        if (rel_sym) {
            uint32_t sym_hash = ldModuleSymHash(sym_name);
            if (rel_type == R_ARM_COPY)
                sym_addr = ldModuleGetSymbol(mod, sym_name, sym_hash, 2, ver_name, ver_hash, &sym_mod);
            else
                sym_addr = ldModuleGetSymbol(mod->context->loaded_mods->mod, sym_name, sym_hash, 1, ver_name, ver_hash, &sym_mod);

            // 尝试从default_dynlib中查找
            // if (!sym_addr)
            //    sym_addr = soloader_find_symbol_from_default_dynlib(sym_name);

            if (!sym_addr) {
                if (ELF32_ST_BIND(mod->symtab[rel_sym].st_info) == STB_WEAK) {
                    l_debug("relocate_rel_table: weak symbol '%s' not found, skipping", sym_name);
                    continue;
                } else {
                    if (ver_name) {
                        l_error("relocate_rel_table: symbol '%s' version '%s' not found",
                                sym_name, ver_name);
                    } else {
                        l_error("relocate_rel_table: symbol '%s' not found", sym_name);
                    }
                    return -1;
                }
            }
        }

        // ===== 执行重定位 =====
        uintptr_t val;
        size_t    len;

        switch (rel_type) {
            case R_ARM_NONE:  // 0
                break;
            case R_ARM_ABS32:  // 2
                val = sym_addr + *rel_addr;
                len = sizeof(uintptr_t);
                kuKernelCpuUnrestrictedMemcpy(rel_addr, &val, len);
                // l_debug("  R_ARM_ABS32: %s -> 0x%x", sym_name, *rel_addr);
                break;
            case R_ARM_REL32:                                           // 3
                val = sym_addr + *rel_addr - (load_base + rel_offset);  // val = sym_addr + *rel_addr - rel_offset;
                len = sizeof(uintptr_t);
                kuKernelCpuUnrestrictedMemcpy(rel_addr, &val, len);
                // l_debug("  R_ARM_REL32: %s -> 0x%x", sym_name, *rel_addr);
                break;
            case R_ARM_TLS_DTPMOD32:  // 17
                val = (uintptr_t)(sym_addr ? sym_mod : mod);
                len = sizeof(uintptr_t);
                kuKernelCpuUnrestrictedMemcpy(rel_addr, &val, len);
                // l_debug("  R_ARM_TLS_DTPMOD32: %s -> 0x%x", sym_name, *rel_addr);
                break;
            case R_ARM_TLS_DTPOFF32:  // 18
                if (sym_mod) {
                    val = (sym_addr - (uintptr_t)sym_mod->load_mem) + *rel_addr;
                    len = sizeof(uintptr_t);
                    kuKernelCpuUnrestrictedMemcpy(rel_addr, &val, len);
                    // l_debug("  R_ARM_TLS_DTPOFF32: %s -> 0x%x", sym_name, *rel_addr);
                }
                break;
            case R_ARM_COPY:  // 20
                if (sym_addr) {
                    val = sym_addr;
                    len = mod->symtab[rel_sym].st_size;
                    kuKernelCpuUnrestrictedMemcpy((void *)rel_addr, (void *)val, len);
                    // l_debug("  R_ARM_COPY: %s size=%zu", sym_name, len);
                }
                break;
            case R_ARM_GLOB_DAT:   // 21
            case R_ARM_JUMP_SLOT:  // 22
                val = sym_addr;
                len = sizeof(uintptr_t);
                kuKernelCpuUnrestrictedMemcpy(rel_addr, &val, len);
                //  l_debug("  %s: %s -> 0x%x",
                //          rel_type == R_ARM_GLOB_DAT ? "R_ARM_GLOB_DAT" : "R_ARM_JUMP_SLOT",
                //          sym_name, *rel_addr);
                break;
            /*
            case R_ARM_RELATIVE:  // 23
                val = load_base + *rel_addr;
                len = sizeof(uintptr_t);
                kuKernelCpuUnrestrictedMemcpy(rel_addr, &val, len);
                // l_debug("  R_ARM_RELATIVE: %s -> 0x%x", sym_name, *rel_addr);
                break;
            */
            default:
                l_error("relocate_rel_table: unsupported relocation type %d for symbol '%s'",
                        rel_type, sym_name);
                return -1;
        }
    }

    // l_debug("relocate_rel_table(%p): %s success!", mod, mod->so_name);
    return 0;
}

static int relocate_rela_table(LdModule *mod, Elf32_Rela *rel_tab, size_t rela_size) {
    size_t rela_num = rela_size / sizeof(Elf32_Rela);
    l_debug("relocate_rela_table(%p): %s unsupported!", mod, mod->so_name);
    // Todo
    return 0;
}

static int ldModuleRelocate_real(LdModule *mod) {
}

int ldModuleRelocate(LdModule *mod) {
    if (!mod) {
        l_error("ldModuleRelocate(%p): null ptr", mod, mod->so_name);
        return -1;
    }

    if (mod->status > LD_MODULE_STATUS_NONE) {
        l_debug("ldModuleRelocate(%p): '%s' is already relocated", mod, mod->so_name);
        return 0;
    }

    Elf32_Phdr *dynamic_phdr = mod->dynamic_phdr;
    if (!mod->load_mem || !dynamic_phdr) {
        l_debug("ldModuleRelocate(%p): '%s' no need to relocate", mod, mod->so_name);
        return 0;
    }

    // 遍历重定位依赖库
    for (int i = 0; i < mod->needed_num; i++) {
        if (ldModuleRelocate(mod->needed_mods[i]) != 0) {
            return -1;
        }
    }

    // 重定位
    l_debug("ldModuleRelocate(%p) '%s' ...", mod, mod->so_name);

    uintptr_t   load_mem    = (uintptr_t)mod->load_mem + mod->load_offset;
    Elf32_Dyn  *dynamic     = (Elf32_Dyn *)(load_mem + dynamic_phdr->p_vaddr);
    size_t      dynamic_num = dynamic_phdr->p_filesz / sizeof(Elf32_Dyn);
    Elf32_Rel  *rel_tab     = NULL;
    size_t      rel_size    = 0;
    Elf32_Rela *rela_tab    = NULL;
    size_t      rela_size   = 0;
    uint32_t    plt_type    = 0;
    void       *plt_tab     = NULL;
    size_t      plt_size    = 0;

    for (int i = 0; i < dynamic_num; i++) {
        Elf32_Dyn *dyn = &dynamic[i];
        switch (dyn->d_tag) {
            case DT_REL:
                rel_tab = (Elf32_Rel *)(load_mem + dyn->d_un.d_ptr);
                break;
            case DT_RELA:
                rela_tab = (Elf32_Rela *)(load_mem + dyn->d_un.d_ptr);
                break;
            case DT_RELSZ:
                rel_size = dyn->d_un.d_val;
                break;
            case DT_RELASZ:
                rela_size = dyn->d_un.d_val;
                break;
            case DT_PLTREL:
                plt_type = dyn->d_un.d_val;
                break;
            case DT_JMPREL:
                plt_tab = (void *)(load_mem + dyn->d_un.d_ptr);
                break;
            case DT_PLTRELSZ:
                plt_size = dyn->d_un.d_val;
                break;
        }
    }

    if (rel_tab && rel_size > 0) {
        if (relocate_rel_table(mod, rel_tab, rel_size) != 0) {
            l_error("ldModuleRelocate(%p): '%s' failed to process rel table", mod, mod->so_name);
            return -1;
        }
    }
    if (rela_tab && rela_size > 0) {
        if (relocate_rela_table(mod, rela_tab, rela_size) != 0) {
            l_error("ldModuleRelocate(%p): '%s' failed to process rela table", mod, mod->so_name);
            return -1;
        }
    }

    if (plt_tab && plt_size > 0) {
        if (plt_type == DT_REL) {
            if (relocate_rel_table(mod, (Elf32_Rel *)plt_tab, plt_size) != 0) {
                l_error("ldModuleRelocate(%p): '%s' failed to process plt rel table", mod, mod->so_name);
                return -1;
            }
        } else if (plt_type == DT_RELA) {
            if (relocate_rela_table(mod, (Elf32_Rela *)plt_tab, plt_size) != 0) {
                l_error("ldModuleRelocate(%p): '%s' failed to process plt rela table", mod, mod->so_name);
                return -1;
            }
        }
    }

    l_debug("ldModuleRelocate(%p): '%s' success!", mod, mod->so_name);
    mod->status = LD_MODULE_STATUS_RELOCATED;
    return 0;
}
