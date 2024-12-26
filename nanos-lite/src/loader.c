#include <proc.h>
#include <elf.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename)
{
  Elf_Ehdr ehdr;
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  uint16_t ph_nr = ehdr.e_phnum;
  uint16_t ph_ensz = ehdr.e_phentsize;
  for (int i = 0; i < ph_nr; ++i)
  {
    Elf_Phdr phdr;
    ramdisk_read(&phdr, ehdr.e_phoff + i * ph_nr, ph_ensz);
    if (phdr.p_type == PT_LOAD)
    {
      printf("%d\n", phdr.p_type);
      Elf32_Off off = phdr.p_offset;
      Elf32_Addr vaddr = phdr.p_vaddr;
      uint32_t mem_sz = phdr.p_memsz;
      uint32_t file_sz = phdr.p_filesz;
      uint8_t *p = (uint8_t*)vaddr;
      ramdisk_read(p, off, file_sz);
      memset(p + file_sz, 0, mem_sz - file_sz);
    }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
