#include "AMEEObject.hpp"

namespace AMEE {

Object::Object()
    : m_ID(IDGenerator::Generate(IDType::Node))
{
}

} // namespace AMEE