---
author: "Vidge Wong"
title: "Condition Variable QA"
date: "2023-11-03"
description: "Common questions and answers about condition variable"
tags: ["c/cpp"]
categories: ["Design Pattern"]
image: img/cover/math.png
hiddenInList: true
ShowToc: true
TocOpen: true
---


Q&A: C++ std::condition_variable — can a notify be missed if a worker starts wait after the notifier released the lock but before calling notify_all()?

---

## Question
If in C++ a notifier thread releases the mutex and then hasn't yet called notify_all(), and a worker thread acquires the mutex and begins executing wait() but hasn't finished entering the blocked state yet, while the notifier then calls notify_all() and the worker's wait() only later actually blocks — can the worker miss that notification, even though there is a lock involved?

## Short Answer
Yes — a notification can be missed unless you use a protected predicate (shared state) together with wait (the standard pattern). Condition variable notifications are not remembered for threads that start waiting after the notification happened.

## Explanation (concise)
. cv.wait(lock) performs an atomic operation that (conceptually) checks the predicate, releases the mutex, and blocks the thread. However, there is a race: if a notify_one()/notify_all() happens before the thread actually entered the blocking state, that notification will not be stored for later — the wait will block until the next notification.

. Therefore, a notifier that sets a shared condition (a boolean or other state) and then calls notify_*() is the correct approach. The waiting side must check that shared condition and only block if the condition is not already satisfied.

. Use cv.wait(lock, predicate) or a loop that checks the predicate to avoid missed notifications and to handle spurious wakeups.

## Recommended safe pattern (C++11+)

```cpp
#include <condition_variable>
#include <mutex>

std::mutex m;
std::condition_variable cv;
bool ready = false; // protected shared predicate

// worker
{
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [&]{ return ready; }); // checks predicate first; won't block if ready==true
    // At this point: ready == true and lock is held
}

// notifier
{
    {
        std::lock_guard<std::mutex> lg(m);
        ready = true; // modify protected shared state
    } // unlock (optional: you can notify while still holding lock)
    cv.notify_all(); // notify waiting threads
}

```
Notes about this pattern:

. cv.wait(lk, pred) first checks pred() while holding the lock; if pred() is true, wait returns immediately without blocking — so any prior notify that flipped ready to true is not needed for the worker to proceed.

. The predicate-based wait also defends against spurious wakeups (it re-checks pred() after any wakeup).

. You may call notify_*() either while holding the lock or after releasing it. The canonical safe sequence is: modify protected state under the lock, then call notify. Calling notify after unlocking often avoids immediate contention on the mutex.

## Additional points

notify_all() wakes all threads that are currently blocked in wait() on that condition variable. Threads that have not yet reached the blocked state (they're still acquiring the lock or about to call wait) will not be woken by that earlier notification.

This is why relying solely on notify without a shared predicate is fragile and leads to lost-notification races.

Always protect the condition (predicate) with the same mutex used with the condition variable.

## Conclusion

Yes — your described timing can cause a missed notification. The robust, idiomatic fix is to use a shared, mutex-protected predicate and cv.wait(lock, predicate) (or a while (!predicate) cv.wait(lock); loop). This guarantees correctness even if notify and wait interleave in arbitrary ways and also handles spurious wakeups.