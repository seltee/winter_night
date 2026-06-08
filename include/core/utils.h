#pragma once
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <string>

namespace wne
{
    inline int getFileByteSize(std::string path)
    {
        struct stat stat_buf;
        int rc = stat(path.c_str(), &stat_buf);
        return rc == 0 ? stat_buf.st_size : -1;
    }

    inline unsigned int nextPowerOfTwo(unsigned int n)
    {
        if (n == 0)
            return 1;
        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        return n + 1;
    }
}