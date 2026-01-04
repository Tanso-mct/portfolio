#pragma once

#include <cassert>
#include <unordered_map>
#include <memory>

#include "class_template/non_copy.h"
#include "class_template/thread_safer.h"

#include "utility_header/id.h"
#include "utility_header/locked_value.h"

namespace utility_header
{

// A registry that maps unique IDs to entries of type Entry
// It provides thread-safe registration and retrieval of entries
template <typename Entry>
class Registry :
    public class_template::NonCopyable,
    public class_template::WithLocker<Registry<Entry>>
{
public:
    Registry() = default;
    virtual ~Registry() = default;

    // Move constructor
    Registry(Registry&& other)
    {
        // Move entries from the other registry
        other.WithUniqueLock([&](Registry<Entry>& other_ref)
        {
            entries_ = std::move(other_ref.entries_);
        });
    }

    void Register(size_t id, std::unique_ptr<Entry> entry)
    {
        assert(this->IsUniqueLock()); // Ensure Register is called with a unique lock
        assert(entry != nullptr); // Ensure the entry is not null
        assert(entries_.find(id) == entries_.end()); // Ensure the ID is not already registered
        
        entries_[id] = std::move(entry); // Register the entry
    }

    void Unregister(size_t id)
    {
        assert(this->IsUniqueLock()); // Ensure Unregister is called with a unique lock
        assert(entries_.find(id) != entries_.end()); // Ensure the ID is registered

        entries_.erase(id); // Unregister the entry
    }

    Entry& Get(size_t id) const
    {
        assert(this->IsUniqueLock()); // Ensure Get is called with a unique lock
        assert(entries_.find(id) != entries_.end()); // Ensure the ID is registered

        return *(entries_.at(id)); // Return the entry associated with the ID
    }

    bool Contains(size_t id) const
    {
        assert(this->IsSharedLock() || this->IsUniqueLock()); // Ensure Contains is called with a lock
        return entries_.find(id) != entries_.end(); // Check if the ID is registered
    }

    size_t GetCount() const
    {
        assert(this->IsSharedLock() || this->IsUniqueLock()); // Ensure GetCount is called with a lock
        return entries_.size(); // Return the number of registered entries
    }

    std::vector<size_t> GetRegisteredIDs() const
    {
        assert(this->IsSharedLock() || this->IsUniqueLock()); // Ensure GetRegisteredIDs is called with a lock

        // Prepare a vector to hold the IDs
        std::vector<size_t> ids;
        ids.reserve(entries_.size());

        for (const auto& pair : entries_)
            ids.push_back(pair.first); // Collect all registered IDs

        return ids; // Return the list of registered IDs
    }

    void Clear()
    {
        assert(this->IsUniqueLock()); // Ensure Clear is called with a unique lock
        entries_.clear(); // Clear all registered entries
    }

private:
    std::unordered_map<size_t, std::unique_ptr<Entry>> entries_; // The map of registered entries
};

// A helper class to register an entry to a registry upon construction
// It is useful for static registration of entries
template <typename Entry>
class Registerer
{
public:
    Registerer(Registry<Entry>& registry, size_t id, std::unique_ptr<Entry> entry)
    {
        registry.Register(id, std::move(entry)); // Register the entry upon construction
    }
};

} // namespace utility_header