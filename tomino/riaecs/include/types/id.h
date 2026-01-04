#pragma once

#include <utility>

namespace riaecs
{
    constexpr size_t ID_DEFAULT_GENERATION = 0;

    class ID
    {
    private:
        size_t index_ = 0;
        size_t generation_ = ID_DEFAULT_GENERATION;
        bool isValid_ = false;

    public:
        ID(size_t index, size_t generation) : index_(index), generation_(generation), isValid_(true) {}
        ID() = default;
        ~ID() = default;

        size_t GetIndex() const { return index_; }
        size_t GetGeneration() const { return generation_; }
        bool IsValid() const { return isValid_; }

        bool operator==(const ID &other) const
        {
            return index_ == other.index_ && GetGeneration() == other.GetGeneration();
        }

        bool operator!=(const ID &other) const
        {
            return !(*this == other);
        }

        bool operator<(const ID& other) const
        {
            if (generation_ < other.generation_) return true;
            if (other.generation_ < generation_) return false;
            return index_ < other.index_;
        }
    };

} // namespace riaecs

namespace std 
{
    template <>
    struct hash<riaecs::ID> 
    {
        std::size_t operator()(const riaecs::ID &id) const noexcept 
        {
            std::size_t h1 = std::hash<size_t>()(id.GetIndex());
            std::size_t h2 = std::hash<size_t>()(id.GetGeneration());

            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
}