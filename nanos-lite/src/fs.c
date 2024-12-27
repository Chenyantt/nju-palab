#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

int fs_open(const char *pathname, int flags, int mode)
{
  int file_nr = sizeof(file_table) / sizeof(Finfo);
  for (int i = 3; i < file_nr; ++i)
  {
    if (strcmp(pathname, file_table[i].name) == 0)
    {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("FileL %s not found", pathname);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  Log("size: %d open_offset: %d len: %d\n",file_table[fd].size, file_table[fd].open_offset, len);
  if (fd == 0 || fd == 1 || fd == 2)
    return 0;
  if (file_table[fd].open_offset + len > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  size_t sz = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  assert(sz >= 0);
  file_table[fd].open_offset += sz;
  return sz;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  if (fd == 0)
    return 0;
  else if (fd == 1 || fd == 2)
  {
    for (size_t i = 0; i < len; ++i)
    {
      putch(((char *)buf)[i]);
    }
    return len;
  }
  else
  {
    assert(file_table[fd].open_offset + len <= file_table[fd].size);
    size_t sz = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += sz;
    return sz;
  }
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  // Log("size: %d offset: %d whence %d\n",file_table[fd].size,offset,whence);
  if (fd == 0 || fd == 1 || fd == 2)
    return 0;
  switch (whence)
  {
  case SEEK_SET:
    file_table[fd].open_offset = offset;
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  default:
    Log("Error type of lseek");
    return -1;
    break;
  }
  assert(file_table[fd].open_offset <= file_table[fd].size);
  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
  return 0;
}

void init_fs()
{
  // TODO: initialize the size of /dev/fb
}
