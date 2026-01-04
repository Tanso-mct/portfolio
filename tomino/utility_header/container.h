#pragma once

#include <cassert>
#include <vector>
#include <memory>

#include "class_template/non_copy.h"
#include "class_template/thread_safer.h"
#include "utility_header/id.h"
#include "utility_header/locked_value.h"

namespace utility_header
{

// Container that manages elements by GenerationID
// Internally manage element generations and ensure validity of IDs
template <typename Element>
class Container : 
    public class_template::NonCopyable,
    public class_template::WithLocker<Container<Element>>
{
public:
    Container() = default;
    virtual ~Container() = default;

    virtual void Create(size_t count)
    {
        assert(this->IsUniqueLock()); // Ensure Create is called with a unique lock
        assert(elements_.size() == 0); // Ensure Create is only called when the container is empty

        elements_.resize(count); // Initialize the vector with nullptrs
        generations_.resize(count, DEFAULT_GENERATION); // Initialize generations to default
    }

    std::unique_ptr<Element> Release(const GenerationID &id)
    {
        assert(this->IsUniqueLock()); // Ensure Release is called with a unique lock
        assert(id.IsValid()); // Ensure the GenerationID is valid
        assert(id.GetIndex() < elements_.size()); // Ensure the GenerationID index is within bounds
        assert(elements_[id.GetIndex()] != nullptr); // Ensure there is an object to release
        assert(id.GetGeneration() == generations_[id.GetIndex()]); // Ensure the GenerationID generation matches

        // Update the generation for the GenerationID
        generations_[id.GetIndex()]++;

        return std::move(elements_[id.GetIndex()]); // Move the object out of the container
    }

    GenerationID Add(std::unique_ptr<Element> object)
    {
        assert(object != nullptr); // Ensure the new object is not null
        assert(this->IsUniqueLock()); // Ensure Add is called with a unique lock

        if (free_indices_.empty())
        {
            // Store the index before adding the new element
            size_t id = elements_.size();

            // Add the new object and initialize its generation
            elements_.emplace_back(std::move(object));
            generations_.emplace_back(DEFAULT_GENERATION);

            // Return the GenerationID of the newly added object
            return GenerationID(id, generations_[id]);
        }
        else // Reuse an index from free_indices_
        {
            // Get the last free index
            size_t id = free_indices_.back();
            free_indices_.pop_back();

            // Place the new object at the reused index and reset its generation
            elements_[id] = std::move(object);
            generations_[id] = DEFAULT_GENERATION;

            // Return the GenerationID of the newly added object
            return GenerationID(id, generations_[id]);
        }

    }

    std::unique_ptr<Element> Erase(const GenerationID &id)
    {
        assert(this->IsUniqueLock()); // Ensure Erase is called with a unique lock
        assert(id.IsValid()); // Ensure the GenerationID is valid
        assert(id.GetIndex() < elements_.size()); // Ensure the GenerationID index is within bounds
        assert(elements_[id.GetIndex()] != nullptr); // Ensure there is an object to erase
        assert(id.GetGeneration() == generations_[id.GetIndex()]); // Ensure the GenerationID generation matches

        // Move the will be erased object to return it
        std::unique_ptr<Element> object = std::move(elements_[id.GetIndex()]);

        // Reset the object
        elements_[id.GetIndex()] = nullptr;

        // Update the generation for the GenerationID
        generations_[id.GetIndex()]++;

        // Store the index in free_indices_ for potential reuse
        free_indices_.push_back(id.GetIndex());

        return object;
    }

    Element& Get(const GenerationID &id)
    {
        assert(this->IsUniqueLock()); // Ensure Ref is called with a unique lock
        assert(id.IsValid()); // Ensure the GenerationID is valid
        assert(id.GetIndex() < elements_.size()); // Ensure the GenerationID index is within bounds
        assert(elements_[id.GetIndex()] != nullptr); // Ensure there is an object to get
        assert(id.GetGeneration() == generations_[id.GetIndex()]); // Ensure the GenerationID generation matches

        return *(elements_[id.GetIndex()]);
    }

    std::unique_ptr<Element> Set(const GenerationID &id, std::unique_ptr<Element> object)
    {
        assert(this->IsUniqueLock()); // Ensure Set is called with a unique lock
        assert(id.IsValid()); // Ensure the GenerationID is valid
        assert(object != nullptr); // Ensure the new object is not null
        assert(id.GetIndex() < elements_.size()); // Ensure the GenerationID index is within bounds
        assert(id.GetGeneration() == generations_[id.GetIndex()]); // Ensure the GenerationID generation matches

        // Take the old object to return it
        std::unique_ptr<Element> oldObject = std::move(elements_[id.GetIndex()]);
        
        // Set the new object
        elements_[id.GetIndex()] = std::move(object);

        return oldObject; // Return the old object
    }

    size_t GetGeneration(size_t index) const
    {
        assert(this->IsSharedLock() || this->IsUniqueLock()); // Ensure GetGeneration is called with a lock
        assert(index < generations_.size()); // Ensure the index is within bounds

        return generations_[index]; // Return the generation for the given index
    }

    bool Contains(const GenerationID &id) const
    {
        assert(this->IsSharedLock() || this->IsUniqueLock()); // Ensure Contains is called with a lock
        assert(id.IsValid()); // Ensure the GenerationID is valid
        assert(id.GetIndex() < elements_.size()); // Ensure the GenerationID index is within bounds

        return elements_[id.GetIndex()] != nullptr && id.GetGeneration() == generations_[id.GetIndex()];
    }

    size_t GetCount() const
    {
        assert(this->IsSharedLock() || this->IsUniqueLock()); // Ensure GetCount is called with a lock
        return elements_.size();
    }

    void Clear()
    {
        assert(this->IsUniqueLock()); // Ensure Clear is called with a unique lock

        elements_.clear();
        generations_.clear();
        free_indices_.clear();
    }

private:
    std::vector<std::unique_ptr<Element>> elements_;
    std::vector<size_t> generations_;
    std::vector<size_t> free_indices_;
};

} // namespace utility_header