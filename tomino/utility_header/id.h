#pragma once

#include <utility>
#include <atomic>

namespace utility_header
{

using ID = size_t;

// Default generation for a new ID
constexpr size_t DEFAULT_GENERATION = 0;

// Class representing a unique identifier for objects
// An ID consists of an index and a generation
class GenerationID
{
public:
    GenerationID(size_t index, size_t generation)
    {
        Set(index, generation, true);
    }

    GenerationID()
    {
        Set(0, DEFAULT_GENERATION, false);
    }

    // Copy constructor
    GenerationID(const GenerationID& other)
    {
        Bits b = other.bits_.load(std::memory_order_acquire);
        bits_.store(b, std::memory_order_release);
    }

    ~GenerationID() = default;

    // Get index from the bits
    size_t GetIndex() const
    {
        Bits b = bits_.load(std::memory_order_acquire);
        return static_cast<size_t>(UnpackIndex(b));
    }

    // Get generation from the bits
    size_t GetGeneration() const
    {
        Bits b = bits_.load(std::memory_order_acquire);
        return static_cast<size_t>(UnpackGeneration(b));
    }

    // Check if the ID is valid
    bool IsValid() const
    {
        Bits b = bits_.load(std::memory_order_acquire);
        return UnpackValid(b);
    }

    // Equality operator
    bool operator==(const GenerationID &other) const
    {
        Bits a = bits_.load(std::memory_order_acquire);
        Bits b = other.bits_.load(std::memory_order_acquire);
        return UnpackIndex(a) == UnpackIndex(b) && UnpackGeneration(a) == UnpackGeneration(b);
    }

    // Inequality operator
    bool operator!=(const GenerationID &other) const
    {
        return !(*this == other);
    }

    // Less-than operator for ordering
    bool operator<(const GenerationID &other) const
    {
        Bits a = bits_.load(std::memory_order_acquire);
        Bits b = other.bits_.load(std::memory_order_acquire);

        auto ga = UnpackGeneration(a);
        auto gb = UnpackGeneration(b);
        if (ga < gb) return true;
        if (ga > gb) return false;

        return UnpackIndex(a) < UnpackIndex(b);
    }

    // Assignment operator
    GenerationID& operator=(const GenerationID& other)
    {
        if (this != &other)
        {
            Bits b = other.bits_.load(std::memory_order_acquire);
            bits_.store(b, std::memory_order_release);
        }
        return *this;
    }

private:
    // Internal representation of the ID using bits
    using Bits = std::uint64_t;

    // Bit layout:
    // [ valid (1 bit) | index (31 bits) | generation (32 bits) ]
    static constexpr unsigned VALID_SHIFT = 0;
    static constexpr unsigned INDEX_SHIFT = 1;
    static constexpr unsigned GEN_SHIFT   = 32;

    // Masks for extracting fields
    static constexpr Bits VALID_MASK = Bits{1} << VALID_SHIFT;
    static constexpr Bits INDEX_MASK = (Bits{1} << 31) - 1; // 31bit
    static constexpr Bits GEN_MASK = (Bits{1} << 32) - 1; // 32bit

    // Atomic bits storing the ID information
    std::atomic<Bits> bits_{0};

    // Set the bits from index, generation, and validity
    void Set(size_t index, size_t generation, bool valid)
    {
        bits_.store(MakeBits(index, generation, valid), std::memory_order_release);
    }

    // Pack index, generation, and validity into bits
    static constexpr Bits MakeBits(size_t index, size_t generation, bool valid)
    {
        Bits b = 0;
        Bits v = valid ? Bits{1} : Bits{0};
        b |= (v & Bits{1}) << VALID_SHIFT;
        b |= (Bits(index) & INDEX_MASK) << INDEX_SHIFT;
        b |= (Bits(generation) & GEN_MASK) << GEN_SHIFT;
        return b;
    }

    // Unpack fields from bits
    static bool UnpackValid(Bits b)
    {
        return (b & VALID_MASK) != 0;
    }

    // Unpack index from bits
    static std::uint32_t UnpackIndex(Bits b)
    {
        return static_cast<std::uint32_t>((b >> INDEX_SHIFT) & INDEX_MASK);
    }

    // Unpack generation from bits
    static std::uint32_t UnpackGeneration(Bits b)
    {
        return static_cast<std::uint32_t>((b >> GEN_SHIFT) & GEN_MASK);
    }
};

class IDGenerator
{
public:
    IDGenerator() = default;
    virtual ~IDGenerator() = default;

    // Generate a new unique ID
    ID Generate()
    {
        return next_id_++;
    }

private:
    ID next_id_ = 0; // The next ID to be issued
};

} // namespace utility_header

namespace std 
{

// Specialization of std::hash for utility_header::GenerationID
// This allows utility_header::GenerationID to be used as a key in unordered containers
template <>
struct hash<utility_header::GenerationID> 
{
    std::size_t operator()(const utility_header::GenerationID &id) const noexcept {
        return std::hash<size_t>()(id.GetIndex()) ^ (std::hash<size_t>()(id.GetGeneration()) << 1); }
};

} // namespace std