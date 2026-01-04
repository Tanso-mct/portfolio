#pragma once

#include <shared_mutex>

namespace utility_header
{

// Class that holds a reference to an object along with a lock to ensure thread safety
// The lock is acquired when the LockedValue is created and released when it goes out of scope
template <typename T, typename LockType>
class LockedValue
{
public:
    LockedValue(T& ref, LockType lock) : 
        ref_(ref), lock_(std::move(lock)) 
    {
    }

    ~LockedValue() = default;

    // Access the underlying object
    T& operator()() noexcept { return ref_; }
    T& operator*() noexcept { return ref_; }
    T* operator->() noexcept { return &ref_; }

    // Access the lock
    LockType& Lock() noexcept { return lock_; }

private:
    T& ref_;
    LockType lock_;
};

// Const version of LockedValue
// Holds a const reference to an object along with a lock to ensure thread safety
// The lock is acquired when the ConstLockedValue is created and released when it goes out of
template <typename T, typename LockType>
class ConstLockedValue
{
public:
    ConstLockedValue(const T& ref, LockType lock) : 
        ref_(ref), lock_(std::move(lock)) 
    {
    }

    ~ConstLockedValue() = default;

    // Access the underlying object
    const T& operator()() noexcept { return ref_; }
    const T& operator*() noexcept { return ref_; }
    const T* operator->() noexcept { return &ref_; }

    // Access the lock
    LockType& Lock() noexcept { return lock_; }

private:
    const T& ref_;
    LockType lock_;
};

// Type alias for LockedValue with std::shared_lock
template <typename T>
using SharedLockedValue = LockedValue<T, std::shared_lock<std::shared_mutex>>;

// Type alias for ConstLockedValue with std::shared_lock
template <typename T>
using ConstSharedLockedValue = ConstLockedValue<T, std::shared_lock<std::shared_mutex>>;

// Type alias for LockedValue with std::unique_lock
template <typename T>
using UniqueLockedValue = LockedValue<T, std::unique_lock<std::shared_mutex>>;

// Type alias for ConstLockedValue with std::unique_lock
template <typename T>
using ConstUniqueLockedValue = ConstLockedValue<T, std::unique_lock<std::shared_mutex>>;

} // namespace utility_header