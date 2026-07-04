#include "AMEEObject.hpp"

namespace AMEE {

uint64_t Object::ms_NextID = 1;

Object::Object()
    : m_ID(ms_NextID++)
{
}

} // namespace AMEE
