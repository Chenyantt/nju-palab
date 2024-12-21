#ifndef __FTRACE_H__
#define __FTRACE_H__

#ifdef CONFIG_FTRACE
#include <elf.h>
#include <common.h>

#define SYMSTR_SIZE 65536
#define SYMTAB_SIZE 65536
#define FTRACE_SIZE 65536

typedef struct
{
    int type;
    uint32_t pc;
    char *name;
    uint32_t dnpc;
} FTraceLog;

extern char symstr[SYMSTR_SIZE];
extern Elf32_Sym sym_tab[SYMTAB_SIZE];
extern uint32_t symtab_size;
extern FTraceLog ftrace_log[FTRACE_SIZE];
extern uint32_t ftracelog_size;

static inline int init_ftrace(char *filename)
{
    Elf32_Ehdr ehdr;
    FILE *fp = fopen(filename, "rb");
    int ret;

    if (fp == NULL)
    {
        // Log("Ftrace: Cannot open the elf file!");
        return EXIT_FAILURE;
    }
    ret = fread((void *)(&ehdr), 1, sizeof(ehdr), fp);

    uint16_t shentsize = ehdr.e_shentsize, shnum = ehdr.e_shnum;

    if (fseek(fp, ehdr.e_shoff + ((uint32_t)shentsize) * ehdr.e_shstrndx, SEEK_SET) != 0)
    {

        // Log("Ftrace: Get the section header table failed!");
        fclose(fp);
        return EXIT_FAILURE;
    }

    Elf32_Shdr shdr;
    ret = fread((void *)(&shdr), 1, sizeof(shdr), fp);

    if (fseek(fp, shdr.sh_offset, SEEK_SET) != 0)
    {
        // Log("Ftrace: Get the shstr section failed!");
        fclose(fp);
        return EXIT_FAILURE;
    }

    char shstr[65536];
    ret = fread((void *)shstr, 1, shdr.sh_size, fp);

    if (fseek(fp, ehdr.e_shoff, SEEK_SET) != 0)
    {
        // Log("Ftrace: Get the section header table failed!");
        fclose(fp);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < shnum; ++i)
    {
        ret = fread((void *)(&shdr), 1, sizeof(shdr), fp);
        if (strcmp(shstr + shdr.sh_name, ".strtab") == 0)
        {
            if (fseek(fp, shdr.sh_offset, SEEK_SET) != 0)
            {
                // Log("Ftrace: Get the symstr section failed!");
                fclose(fp);
                return EXIT_FAILURE;
            }
            ret = fread((void *)symstr, 1, shdr.sh_size, fp);
            break;
        }
    }

    if (fseek(fp, ehdr.e_shoff, SEEK_SET) != 0)
    {
        // Log("Ftrace: Get the section header table failed!");
        fclose(fp);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < shnum; ++i)
    {
        ret = fread((void *)(&shdr), 1, sizeof(shdr), fp);
        if (strcmp(shstr + shdr.sh_name, ".symtab") == 0)
        {
            if (fseek(fp, shdr.sh_offset, SEEK_SET) != 0)
            {
                // Log("Ftrace: Get the symtab section failed!");
                fclose(fp);
                return EXIT_FAILURE;
            }
            uint32_t sym_num = shdr.sh_size / shdr.sh_entsize;
            Elf32_Sym sym;
            for (int j = 0; j < sym_num; ++j)
            {
                ret = fread((void *)(&sym), 1, sizeof(sym), fp);

                if (ELF32_ST_TYPE(sym.st_info) == STT_FUNC)
                {
                    memcpy(&(sym_tab[symtab_size]), &sym, sizeof(sym));
                    ++symtab_size;
                }
            }
            break;
        }
    }
    return ret;
}

static inline char *get_funcname(uint32_t pc)
{
    for (int i = 0; i < symtab_size; ++i)
    {
        if (sym_tab[i].st_value <= pc && sym_tab[i].st_value + sym_tab[i].st_size > pc)
        {
            return symstr + sym_tab[i].st_name;
        }
    }
    return "???";
}

static inline int ftrace(uint32_t src, uint32_t dest, uint32_t pc, uint32_t dnpc)
{
    if (dest == 1)
    {
        ftrace_log[ftracelog_size].type = 0;
        ftrace_log[ftracelog_size].pc = pc;
        ftrace_log[ftracelog_size].dnpc = dnpc;
        ftrace_log[ftracelog_size].name = get_funcname(dnpc);
        ++ftracelog_size;
        return 1;
    }
    else if (src == 1)
    {
        ftrace_log[ftracelog_size].type = 1;
        ftrace_log[ftracelog_size].pc = pc;
        ftrace_log[ftracelog_size].name = get_funcname(pc);
        ++ftracelog_size;
        return 2;
    }
    return 0;
}

static inline void display_ftrace()
{
    int depth = 0;
    for (int i = 0; i < ftracelog_size; ++i)
    {
        printf("0x%08x: ", ftrace_log[i].pc);
        for (int j = 0; j < depth; ++j)
            putchar(' '), putchar(' ');
        if (ftrace_log[i].type == 0)
        {
            printf("call [%s@0x%08x]\n", ftrace_log[i].name, ftrace_log[i].dnpc);
        }
        else
        {
            printf("ret  [%s]\n", ftrace_log[i].name);
        }
    }
}


#endif

#endif