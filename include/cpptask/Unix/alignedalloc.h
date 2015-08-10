/*
* http://code.google.com/p/cpptask/
* Copyright (c) 2011, Kirill Kolodyazhnyi
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _ALIGNED_ALLOC_H_
#define _ALIGNED_ALLOC_H_

#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

namespace cpptask
{

inline size_t GetCacheLineSize()
{
    static unsigned int  cacheLineSize = 0;
    if (cacheLineSize == 0)
    {
        FILE * p = 0;
        p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
        if (p)
        {
            fscanf(p, "%d", &cacheLineSize);
            fclose(p);
        }
        else
        {
            cacheLineSize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
        }
    }
    assert(cacheLineSize != 0);
    return cacheLineSize;
}

inline void* AlignedAlloc(size_t size, size_t alignment)
{
    void* ret = 0;
    if (posix_memalign(&ret, alignment, size) != 0)
    {
        ret = 0;
        throw std::bad_alloc();
    }
    return ret;
}

inline void AlignedFree(void* ptr)
{
    free(ptr);
}

}

#endif