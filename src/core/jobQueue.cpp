// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "core/jobQueue.h"
#include "features/logger/logger.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace wne;

JobQueue::JobQueue()
{
    const uint32_t numThreads = std::thread::hardware_concurrency();
    Logger::log << "Num job thread " << numThreads << endl;
    for (uint32_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(std::thread(&JobQueue::threadLoop, this));
    }
}

void JobQueue::queueJob(const std::function<void()> &job)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    }
    mutex_condition.notify_one();
}

void JobQueue::waitJobs()
{
    while (isBusy())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

bool JobQueue::isBusy()
{
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        poolbusy = !jobs.empty() || inProgress != 0;
    }
    return poolbusy;
}

void JobQueue::stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &active_thread : threads)
    {
        active_thread.join();
    }
    threads.clear();
}

void JobQueue::threadLoop()
{
    while (true)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this]
                                 { return !jobs.empty() || should_terminate; });
            if (should_terminate)
            {
                return;
            }
            inProgress++;
            job = jobs.front();
            jobs.pop();
        }
        job();
        inProgress--;
    }
}