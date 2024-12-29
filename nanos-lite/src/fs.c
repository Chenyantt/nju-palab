#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

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
  FD_EVENT,
  FD_FB,
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
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
    [FD_EVENT] = {"/dev/events", 0, 0, events_read, invalid_write},
    [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
    {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
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
  // Log("size: %d open_offset: %d len: %d off: %d\n",file_table[fd].size, file_table[fd].open_offset, len, file_table[fd].disk_offset + file_table[fd].open_offset);
  if (file_table[fd].read)
  {
    return file_table[fd].read(buf, 0, len);
  }
  else
  {
    printf("fd=%d,doff=%d,openoff=%d,len=%d,sz=%d\n",fd, file_table[fd].disk_offset, file_table[fd].open_offset, len,file_table[fd].size);
    if (file_table[fd].open_offset + len > file_table[fd].size)
      len = file_table[fd].size - file_table[fd].open_offset;
    size_t sz = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    assert(sz >= 0);
    file_table[fd].open_offset += sz;
    return sz;
  }
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  if (file_table[fd].write)
  {
    return file_table[fd].write(buf, 0, len);
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
  AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
  int screen_w = ev.width;
  int screen_h = ev.height;
  file_table[FD_FB].size = screen_w * screen_h * sizeof(uint32_t);
}
