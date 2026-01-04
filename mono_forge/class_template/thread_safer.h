#pragma once

#include "class_template/non_copy.h"

#include <cassert>
#include <shared_mutex>
#include <atomic>

namespace class_template
{

// A template class to provide thread safety mechanisms
class ThreadSafer
{
public:
    ThreadSafer() = default;
    virtual ~ThreadSafer() = default;

    // Lock for shared (read) access
    std::shared_lock<std::shared_mutex> LockShared() const
    {
        return std::shared_lock<std::shared_mutex>(mutex_);
    }

    // Lock for exclusive (write) access
    std::unique_lock<std::shared_mutex> LockUnique() const
    {
        return std::unique_lock<std::shared_mutex>(mutex_);
    }

private:
    mutable std::shared_mutex mutex_; // Mutex for thread-safe access
};

// RAII class to manage lock state
// Sets the lock state to true when constructed and false when destructed
class LockState {
public:
    explicit LockState(std::atomic<bool>& s) : state_(s) { state_.store(true, std::memory_order_release); }
    ~LockState() { state_.store(false, std::memory_order_release); }
private:
    std::atomic<bool>& state_;
};

template <typename Derived>
class WithLocker :
    public ThreadSafer
{
public:
    WithLocker() = default;
    virtual ~WithLocker() = default;

    // Methods to execute a function with a unique lock
    template <typename Func>
    void WithUniqueLock(Func&& func)
    {
        // Acquire unique lock
        std::unique_lock<std::shared_mutex> lock = LockUnique();

        // Set lock state to true for the duration of the function call
        unique_lock_state_ = true;
        LockState lock_state(unique_lock_state_);

        // Call the function with the derived class instance
        return func(static_cast<Derived&>(*this));
    }

    // Methods to execute a function with a shared lock
    template <typename Func>
    void WithSharedLock(Func&& func)
    {
        // Acquire shared lock
        std::shared_lock<std::shared_mutex> lock = LockShared();

        // Set lock state to true for the duration of the function call
        shared_lock_state_ = true;
        LockState lock_state(shared_lock_state_);

        // Call the function with the derived class instance
        return func(static_cast<const Derived&>(*this));
    }

    // Check if currently holding a unique lock
    bool IsUniqueLock() const
    {
        return unique_lock_state_;
    }

    // Check if currently holding a shared lock
    bool IsSharedLock() const
    {
        return shared_lock_state_;
    }

private:
    mutable std::atomic<bool> unique_lock_state_{false};
    mutable std::atomic<bool> shared_lock_state_{false};

};

} // namespace class_template