/*
 * Copyright (C) 2021      Andy Nguyen
 * Copyright (C) 2022      Rinnegatamante
 * Copyright (C) 2022-2024 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include "reimpl/io.h"

#include <dirent.h>
#include <psp2/kernel/threadmgr.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "utils/logger.h"
#include "utils/utils.h"

// Includes the following inline utilities:
// int oflags_musl_to_newlib(int flags);
// dirent64_bionic * dirent_newlib_to_bionic(struct dirent* dirent_newlib);
// void stat_newlib_to_bionic(struct stat * src, stat64_bionic * dst);
#include "reimpl/bits/_struct_converters.c"

FILE *fopen_soloader(const char *filename, const char *mode) {
    // TODO: 判断文件名称, 帮他引导到 DATA_PATH/filename 下去 open
    FILE *res = fopen(filename, mode);
    if (res) {
        l_debug("fopen(%s, %s): %p", filename, mode, res);
    } else {
        l_warn("error fopen(%s, %s): %p", filename, mode, res);
    }

    return res;
}

int fseek_soloader(FILE *stream, long offset, int whence) {
    int res = fseek(stream, offset, whence);
    l_debug("fseek(%p,%ld,%d): %d", stream, offset, whence, res);
    return res;
}

long ftell_soloader(FILE *stream) {
    long res = ftell(stream);
    l_debug("ftell(%p): %ld", stream, res);
    return res;
}

size_t fread_soloader(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t res = fread(ptr, size, nmemb, stream);
    l_debug("fread(%p,%zu,%zu,%p): %zu", ptr, size, nmemb, stream, res);
    return res;
}

size_t fwrite_soloader(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t res = fwrite(ptr, size, nmemb, stream);
    l_debug("fwrite(%p,%zu,%zu,%p): %zu", ptr, size, nmemb, stream, res);
    return res;
}

int fclose_soloader(FILE *stream) {
    int ret = fclose(stream);
    l_debug("fclose(%p): %d", stream, ret);
    return ret;
}

FILE *fdopen_soloader(int fd, const char *mode) {
    FILE *res = fdopen(fd, mode);
    l_debug("fdopen(%d, %s): %p", fd, mode, res);
    return res;
}

static int fake_urandom_open() {
    int pipefd[2];
    if (pipe(pipefd) < 0)
        return -1;

    uint64_t buffer[64];
    for (int i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++) {
        buffer[i] = ((uint64_t)random() << 32) | (uint64_t)random();
    }

    write(pipefd[1], buffer, sizeof(buffer));
    close(pipefd[1]);
    return pipefd[0];
}

int open_soloader(const char *path, int oflag, ...) {
    int    res;
    mode_t mode = 0666;
    if (((oflag & BIONIC_O_CREAT) == BIONIC_O_CREAT) ||
        ((oflag & BIONIC_O_TMPFILE) == BIONIC_O_TMPFILE)) {
        va_list args;
        va_start(args, oflag);
        mode = (mode_t)(va_arg(args, int));
        va_end(args);
    }

    oflag = oflags_bionic_to_newlib(oflag);

    if (strcmp(path, "/proc/cpuinfo") == 0) {
        res = open("app0:/cpuinfo", oflag, mode);
    } else if (strcmp(path, "/proc/meminfo") == 0) {
        res = open("app0:/cpuinfo", oflag, mode);
    } else if (strcmp(path, "/dev/urandom") == 0) {
        res = fake_urandom_open();
    } else {
        res = open(path, oflag, mode);
    }

    if (res < 0) {
        l_warn("open(%s, %x): %i", path, oflag, res);
        return res;
    }

    l_debug("open(%s,0x%x): %i", path, oflag, res);
    return res;
}

int close_soloader(int fd) {
    int res = close(fd);
    l_debug("close(%i): %i", fd, res);
    return res;
}

int fstat_soloader(int fd, stat64_bionic *buf) {
    struct stat st;
    int         res = fstat(fd, &st);

    if (res == 0)
        stat_newlib_to_bionic(&st, buf);

    l_debug("fstat(%i): %i", fd, res);
    return res;
}

int stat_soloader(const char *path, stat64_bionic *buf) {
    struct stat st;
    int         res = stat(path, &st);

    if (res == 0)
        stat_newlib_to_bionic(&st, buf);

    l_debug("stat(%s): %i", path, res);
    return res;
}

int fcntl_soloader(int fd, int cmd, ...) {
    l_warn("fcntl(%i, %i, ...): not implemented", fd, cmd);
    return 0;
}

int ioctl_soloader(int fd, int request, ...) {
    l_warn("ioctl(%i, %i, ...): not implemented", fd, request);
    return 0;
}

int fsync_soloader(int fd) {
    int ret = fsync(fd);
    l_debug("fsync(%i): %i", fd, ret);
    return ret;
}

int dup_soloader(int fd) {
    l_debug("dup(fd): %d", fd, fd);
    return fd;
}

DIR *opendir_soloader(char *_pathname) {
    DIR *ret = opendir(_pathname);
    l_debug("opendir(%s): %p", _pathname, ret);
    return ret;
}

struct dirent64_bionic *readdir_soloader(DIR *dir) {
    static struct dirent64_bionic dirent_tmp;

    struct dirent *ret = readdir(dir);
    l_debug("readdir(%p): %p", dir, ret);

    if (ret) {
        dirent64_bionic *entry_tmp = dirent_newlib_to_bionic(ret);
        memcpy(&dirent_tmp, entry_tmp, sizeof(dirent64_bionic));
        free(entry_tmp);
        return &dirent_tmp;
    }

    return NULL;
}

int readdir_r_soloader(DIR *dirp, dirent64_bionic *entry, dirent64_bionic **result) {
    struct dirent  dirent_tmp;
    struct dirent *pdirent_tmp;

    int ret = readdir_r(dirp, &dirent_tmp, &pdirent_tmp);

    if (ret == 0) {
        dirent64_bionic *entry_tmp = dirent_newlib_to_bionic(&dirent_tmp);
        memcpy(entry, entry_tmp, sizeof(dirent64_bionic));
        *result = (pdirent_tmp != NULL) ? entry : NULL;
        free(entry_tmp);
    }

    l_debug("readdir_r(%p, %p, %p): %i", dirp, entry, result, ret);
    return ret;
}

int closedir_soloader(DIR *dir) {
    int ret = closedir(dir);
    l_debug("closedir(%p): %i", dir, ret);
    return ret;
}
