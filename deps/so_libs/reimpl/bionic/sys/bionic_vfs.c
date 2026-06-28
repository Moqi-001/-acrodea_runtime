#include <sys/statvfs.h>

#include <so_common/so_logger.h>

#include "bionic_vfs.h"

#define SC_INLINE static inline __attribute__((always_inline))

SC_INLINE void statvfs_to_bionic_statfs(const struct statvfs *src, struct statfs_bionic *dst) {
    dst->f_type        = 0xEF53;  // EXT4
    dst->f_bsize       = src->f_bsize;
    dst->f_blocks      = src->f_blocks;
    dst->f_bfree       = src->f_bfree;
    dst->f_bavail      = src->f_bavail;
    dst->f_files       = src->f_files;
    dst->f_ffree       = src->f_ffree;
    uint64_t fsid      = src->f_fsid;
    dst->f_fsid.val[0] = (fsid & 0xFFFFFFFF);
    dst->f_fsid.val[1] = ((fsid >> 32) & 0xFFFFFFFF);
    dst->f_namelen     = src->f_namemax;
    dst->f_frsize      = src->f_frsize;
    dst->f_flags       = src->f_flag;
}

int fstatfs_soloader(int fd, struct statfs_bionic *st) {
    struct statvfs buf;
    int         result = fstatvfs(fd, &buf);
    if (result == 0)
        statvfs_to_bionic_statfs(&buf, st);

    so_logd("fstatfs(%d,%p): %d", fd, st, result);
    return result;
}

int statfs_soloader(const char *path, struct statfs_bionic *st) {
    struct statvfs buf;
    int         result = statvfs(path, &buf);
    if (result == 0)
        statvfs_to_bionic_statfs(&buf, st);

    so_logd("statfs(%s,%p): %d", path, st, result);
    return result;
}
