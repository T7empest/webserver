//
// Created by Lukáš Blažek on 14.09.2025.
//

#include "thread_pool.h"

ThreadPool::ThreadPool(size_t thread_count)
{
	workers_.reserve(thread_count);
	for (size_t i = 0; i < thread_count; ++i)
	{
		workers_.emplace_back([this](std::stop_token st) { worker_loop(st); });
	}
}

ThreadPool::~ThreadPool()
{
	for (auto& t : workers_)
	{
		t.request_stop();
	}

	cv_.notify_all();
}

void ThreadPool::request_stop()
{
	{
		std::scoped_lock lock(mutex_);
		stopping_ = true;
	}
	for (auto& t : workers_) t.request_stop();
	cv_.notify_all();
}

void ThreadPool::worker_loop(std::stop_token st)
{
	for (;;)
	{
		std::function<void()> job;

		{
			std::unique_lock<std::mutex> lock(mutex_);
			// sleep until notify or predicate true
			cv_.wait(lock, st, [this]
			{
				return !tasks_.empty() || stopping_;
			});

			// if stop requested and no work left - exit
			if ((st.stop_requested() || stopping_) && tasks_.empty())
				break;

			// grab work
			job = std::move(tasks_.front());
			tasks_.pop();
		}
		// do work (outside the lock)
		job();
	}
}
