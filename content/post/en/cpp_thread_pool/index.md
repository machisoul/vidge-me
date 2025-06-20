---
author: "Vidge Wong"
title: "Thread Pool"
date: "2020-02-08"
description: "SIMPLE demo of thread pool implementation using CPP"
tags: ["c/cpp"]
categories: ["Design Pattern"]
image: "img/cover/design.png"
ShowToc: true
TocOpen: true
---

## Implementation

```cpp
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(int size) : stop_{false} {
    for (size_t i = 1; i < size; i++) {
      pool_.emplace_back(&ThreadPool::work, this);
    }
  }
  ~ThreadPool() {
    stop_ = true;
    cv_.notify_all();

    for (auto& trd : pool_) {
      if (trd.joinable()) trd.join();
    }
  }

  template <typename F, typename... Args>
  auto addJob(F&& f, Args&&... args) {
    using returnType = decltype(f(args...));

    auto task_pkg_ptr = std::make_shared<std::packaged_task<returnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    {
      std::lock_guard<std::mutex> lock(tasks_mtx);
      tasks_.emplace([task_pkg_ptr] { (*task_pkg_ptr)(); });
    }
    cv_.notify_one();

    return task_pkg_ptr->get_future();
  }

 private:
  void work() {
    while (true) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        cv_.wait(lock, [&] { return stop_ || !tasks_.empty(); });

        if (stop_) return;

        task = std::move(tasks_.front());
        tasks_.pop();
        lock.unlock();
      }
      task();
    }
  }

  std::atomic<bool> stop_;
  std::vector<std::thread> pool_;

  std::mutex tasks_mtx;
  std::queue<std::function<void()>> tasks_;

  std::condition_variable cv_;
};

int main() {
  ThreadPool pool(5);

  auto result = pool.addJob([](int x = 3, int y = 5) -> auto { return x + y; });
  auto result1 = pool.addJob([](int x = 67, float y = 5.1234) -> auto {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return x + y;
  });

  std::cout << result.get() << std::endl;
  std::cout << result1.get() << std::endl;

  return 0;
}
```