#include <proc.h>
#include <elf.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
enum {SEEK_SET, SEEK_CUR, SEEK_END};

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename)
{
  int fd = fs_open(filename, 0, 0);
  if(fd < 0){
    panic("cannot open the file: %s", filename);
  }
  Elf_Ehdr ehdr;
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);

  uint16_t ph_nr = ehdr.e_phnum;
  uint16_t ph_ensz = ehdr.e_phentsize;
  fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  for (int i = 0; i < ph_nr; ++i)
  {
    Elf_Phdr phdr;
      fs_lseek(fd, ehdr.e_phoff + i * ph_ensz, SEEK_SET);
    fs_read(fd, &phdr, ph_ensz);
    if (phdr.p_type == PT_LOAD)
    {
      Elf32_Off off = phdr.p_offset;
      Elf32_Addr vaddr = phdr.p_vaddr;
      uint32_t mem_sz = phdr.p_memsz;
      uint32_t file_sz = phdr.p_filesz;
      void *p = (void*)vaddr;
      fs_lseek(fd, off, SEEK_SET);
      fs_read(fd, p, file_sz);
      memset(p + file_sz, 0, mem_sz - file_sz);
    }
  }
  fs_close(fd);
  return ehdr.e_entry;
  // // open elf
  // Elf_Ehdr ehdr;
  // ramdisk_read(&ehdr, 448451, sizeof(Elf_Ehdr));
  // assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  // Log("phoff = %p", ehdr.e_phoff);
  // Log("entry = %p", ehdr.e_entry);
  // // read phdr
  // Elf_Phdr ph[ehdr.e_phnum];
  // ramdisk_read(ph, ehdr.e_phoff + 448451, sizeof(Elf_Phdr)*ehdr.e_phnum);
  // for (int i = 0; i < ehdr.e_phnum; i++) {
  //   if (ph[i].p_type == PT_LOAD) {
  //     ramdisk_read((void *)ph[i].p_vaddr, ph[i].p_offset + 448451, ph[i].p_memsz);
  //     memset((void *)(ph[i].p_vaddr + ph[i].p_filesz), 0, ph[i].p_memsz - ph[i].p_filesz);
  //   }
  // }
  // return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
