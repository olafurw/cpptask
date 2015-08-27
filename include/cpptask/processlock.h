/*
* https://github.com/Kolkir/cpptask/
* Copyright (c) 2015, Kyrylo Kolodiazhnyi
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

#ifndef _PROCESS_LOCK_H_
#define _PROCESS_LOCK_H_

#include "async.h"

namespace cpptask
{

    template<class SyncPrimitive>
    class process_lock
    {
    public:
        process_lock(SyncPrimitive& _guard)
            : guardLock(_guard, std::defer_lock)
        {
            auto f = cpptask::async(std::launch::async,
                [this]() 
            {
                auto& manager = TaskManager::GetCurrent();
                while (!guardLock.try_lock()) //TODO: remove loop
                {
                    manager.DoOneTask();
                }
            });
            f.wait();
        }

        ~process_lock()
        {
            if (guardLock)
            {
                guardLock.unlock();
            }
        }

        process_lock(const process_lock&) = delete;
        process_lock& operator=(const process_lock&) = delete;
    private:
        std::unique_lock<SyncPrimitive> guardLock;
    };

}

#endif