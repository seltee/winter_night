// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"
#include "core/api.h"
#include <mutex>
#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <math.h>
#include <string>
#include <vector>
#include <condition_variable>

namespace wne
{
    class WNE_API JobQueue
    {
    public:
        JobQueue();

        void queueJob(const std::function<void()> &job);
        void waitJobs();
        bool isBusy();

        inline int getMaxJobs() { return threads.size(); }

    private:
        void stop();
        void threadLoop();

        bool should_terminate = false;           // Tells threads to stop looking for jobs
        std::mutex queue_mutex;                  // Prevents data races to the job queue
        std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> jobs;

        std::atomic<int> inProgress = 0;
    };
};
