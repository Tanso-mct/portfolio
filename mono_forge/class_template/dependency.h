#pragma once

#include <vector>
#include <algorithm>
#include <cassert>

namespace utility_header
{

template <typename T>
class Dependency
{
public:
    Dependency() = default;
    virtual ~Dependency() = default;

    // Get dependencies
    const std::vector<T>& GetDependencies() const { return dependencies_; }

    // Clear all dependencies
    void ClearDependencies() { dependencies_.clear(); }

    // Add a dependency
    void AddDependency(const T& dependency)
    {
        // Ensure no duplicate dependencies
        assert(std::find(dependencies_.begin(), dependencies_.end(), dependency) == dependencies_.end());

        // Add the dependency
        dependencies_.push_back(dependency);
    }

    // Get the number of dependencies
    size_t GetDependencyCount() const { return dependencies_.size(); }

private:
    std::vector<T> dependencies_;
};

} // namespace utility_header