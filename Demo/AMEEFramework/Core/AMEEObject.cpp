#include "AMEEObject.hpp"

namespace AMEE {

Object::Object(IDType Type)
    : m_Type(Type)
    , m_ID(IDGenerator::Generate(Type))
{
}

} // namespace AMEE