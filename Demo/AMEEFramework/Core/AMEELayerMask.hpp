#ifndef __AMEE_LAYERMASK_H__
#define __AMEE_LAYERMASK_H__
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace AMEE {

// Layer names (0-31, each bit is a layer)
enum class Layer : uint8_t {
    Default     = 0,
    Transparent = 1,
    IgnoreRaycast = 2,
    Water       = 4,
    UI          = 5,
    // User layers: 8-31
};

class LayerMask {
public:
    LayerMask() : m_Mask(0) {}
    LayerMask(uint32_t mask) : m_Mask(mask) {}
    LayerMask(Layer layer) : m_Mask(1 << static_cast<uint32_t>(layer)) {}

    // Static factory methods
    static LayerMask Default() { return LayerMask(1 << 0); }
    static LayerMask Everything() { return LayerMask(0xFFFFFFFF); }
    static LayerMask Nothing() { return LayerMask(0); }

    // Layer operations
    void SetLayer(Layer layer, bool enabled) {
        if (enabled)
            m_Mask |= (1 << static_cast<uint32_t>(layer));
        else
            m_Mask &= ~(1 << static_cast<uint32_t>(layer));
    }

    bool HasLayer(Layer layer) const {
        return (m_Mask & (1 << static_cast<uint32_t>(layer))) != 0;
    }

    void AddLayer(Layer layer) {
        m_Mask |= (1 << static_cast<uint32_t>(layer));
    }

    void RemoveLayer(Layer layer) {
        m_Mask &= ~(1 << static_cast<uint32_t>(layer));
    }

    void ToggleLayer(Layer layer) {
        m_Mask ^= (1 << static_cast<uint32_t>(layer));
    }

    // Bitmask operations
    uint32_t GetMask() const { return m_Mask; }
    void SetMask(uint32_t mask) { m_Mask = mask; }

    // Operators
    LayerMask operator|(LayerMask other) const { return LayerMask(m_Mask | other.m_Mask); }
    LayerMask operator&(LayerMask other) const { return LayerMask(m_Mask & other.m_Mask); }
    LayerMask operator~() const { return LayerMask(~m_Mask); }
    bool operator==(LayerMask other) const { return m_Mask == other.m_Mask; }
    bool operator!=(LayerMask other) const { return m_Mask != other.m_Mask; }

    // Check if any layer matches
    bool ContainsAny(LayerMask other) const {
        return (m_Mask & other.m_Mask) != 0;
    }

    // Check if all layers match
    bool ContainsAll(LayerMask other) const {
        return (m_Mask & other.m_Mask) == other.m_Mask;
    }

private:
    uint32_t m_Mask;
};

} // namespace AMEE

#endif // __AMEE_LAYERMASK_H__