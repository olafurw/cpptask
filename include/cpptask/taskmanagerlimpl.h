/*
* https://github.com/Kolkir/cpptask/
* Copyright (c) 2012, Kyrylo Kolodiazhnyi
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

#ifndef _CPP_TASK_TASKMANAGERIMPL_H_
#define _CPP_TASK_TASKMANAGERIMPL_H_

#include "taskmanager.h"
#include "threadpool.h"

#include <assert.h>

namespace cpptask { namespace internal {

inline TaskManager::TaskManager(TaskThreadPool& threadPool, EventManager& eventManager, TaskThread* parentThread)
    : parentThread(parentThread)
    , threadPool(threadPool)
    , eventManager(eventManager)
{
}

inline TaskManager::~TaskManager()
{
}

inline size_t TaskManager::GetThreadsNum() const
{
    return threadPool.GetThreadsNum();
}

inline void TaskManager::AddTask(Task& task)
{
    taskQueue.Enqueue(&task);
    eventManager.notify(EventId::NewTaskEvent);
}

inline Task* TaskManager::GetOwnTask()
{
    Task* res = nullptr;
    std::unique_lock<std::mutex> lock(getGuard, std::try_to_lock);
    if (lock)
    {
        if (!taskQueue.Dequeue(res))
        {
            res = nullptr;
        }
    }
    return res;
}

inline Task* TaskManager::GetTask()
{
    Task* res = GetOwnTask();
    if (res == nullptr)
    {
        res = threadPool.GetTaskManager().GetOwnTask();
        if (res == nullptr)
        {
            for (size_t i = 0; i < threadPool.GetThreadsNum(); ++i)
            {
                TaskThread* thread = threadPool.GetThread(i);
                assert(thread != nullptr);
                if (thread != parentThread)
                {
                    res = thread->GetTaskManager().GetOwnTask();
                    if (res != nullptr)
                    {
                        break;
                    }
                }
            }
        }
    }
    return res;
}

inline TaskManager& TaskManager::GetCurrent()
{
    void* pvalue = GetManagerKey().GetValue();
    if (pvalue != nullptr)
    {
        return *reinterpret_cast<TaskManager*>(pvalue);
    }
    else
    {
        throw exception("Can't acquire current task manager");
    }
}

inline EventManager& TaskManager::GetEventManager()
{
    return eventManager;
}

inline void TaskManager::RegisterInTLS()
{
    GetManagerKey().SetValue(this);
}

inline void TaskManager::RemoveFromTLS()
{
    GetManagerKey().SetValue(nullptr);
}

inline void TaskManager::DoOneTask()
{
    Task* task = GetTask();
    if (task != nullptr)
    {
        DoTask(*task);
    }
}

inline void TaskManager::DoTask(Task& task)
{
    task.Run(eventManager);
}

inline void TaskManager::WaitTask(Task& waitTask)
{
    bool done {false};
    while (!done)
    {
        Task* task = GetTask();
        if (task == nullptr)
        {            
            EventId eventId = EventId::NoneEvent;
            eventManager.wait([&](EventId id)
            {
                eventId = id;
                return id == EventId::NewTaskEvent ||
                       (id == EventId::TaskFinishedEvent && waitTask.IsFinished());
            });

            if (eventId == EventId::NewTaskEvent)
            {
                task = GetTask();
            }
            else if(eventId == EventId::TaskFinishedEvent)
            {
                done = true;
                break;
            }
            else
            {
                assert(false);
            }
        }

        if (task != nullptr)
        {
            task->Run(eventManager);
        }
    }
}

}}
#endif
