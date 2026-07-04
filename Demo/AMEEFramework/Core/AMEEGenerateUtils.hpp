#ifndef __AMEE_GENERATEUTILS_H__
#define __AMEE_GENERATEUTILS_H__
#pragma once
#include <cstdint>
#include <atomic>

namespace AMEE {

enum class IDType {
    Node      = 0,
    Entity    = 1,
    Component = 2,
    //
    Resource = 7,
};

class IDGenerator {
public:
    // Generate unique ID with type prefix
    static uint64_t Generate(IDType Type = IDType::Node)
    {
        uint64_t TypePart = static_cast<uint64_t>(Type) << 60;
        uint64_t Seq = ms_Counter.fetch_add(1, std::memory_order_relaxed);
        return TypePart | (Seq & 0x0FFFFFFFFFFFFFFF);
    }

    // Extract type from ID
    static IDType GetType(uint64_t ID)
    {
        return static_cast<IDType>(ID >> 60);
    }

    // Extract sequence number from ID
    static uint64_t GetSequence(uint64_t ID)
    {
        return ID & 0x0FFFFFFFFFFFFFFF;
    }

    // Check if ID is valid (non-zero)
    static bool IsValid(uint64_t ID)
    {
        return ID != 0;
    }

    // Reset counter (for testing)
    static void Reset()
    {
        ms_Counter.store(0, std::memory_order_relaxed);
    }

private:
    static std::atomic<uint64_t> ms_Counter;
};

} // namespace AMEE

#endif // __AMEE_GENERATEUTILS_H__
