/*
* http://code.google.com/p/cpptask/
* Copyright (c) 2012, Kirill Kolodyazhnyi
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

#ifndef _CPP_TASK_TLSKEY_H_
#define _CPP_TASK_TLSKEY_H_

#include <pthread.h>
#include <assert.h>
#include "../exception.h"

namespace cpptask { namespace internal {

class TLSKey
{
public:
    TLSKey()
    {
        int err = pthread_key_create(&key, 0);
        if (err != 0)
        {
            throw Exception("Can't create a TLS key");
        }
    }

    ~TLSKey()
    {
        if (pthread_key_delete(key) != 0)
        {
            assert(false);
        }
    }

    void* GetValue() const
    {
        return pthread_getspecific(key);
    }

    void SetValue(void* value)
    {
        if (pthread_setspecific(key, value) != 0)
        {
            throw Exception("Can't set a TLS key");
        }
    }

    TLSKey(const TLSKey&) = delete;
    const TLSKey& operator=(const TLSKey&) = delete;
private:
    pthread_key_t key;
};

}}

#endif
