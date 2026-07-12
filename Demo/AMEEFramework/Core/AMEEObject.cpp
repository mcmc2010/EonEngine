#include "AMEEObject.hpp"

namespace AMEE {

Object::Object(ObjectType Type)
    :   m_Type(Type), 
        m_ID(IDGenerator::Generate(ToIDType(Type)))
{
}

IDType Object::ToIDType(ObjectType Type)
{
    switch (Type) {
        case ObjectType::Node:      return IDType::Node;
        case ObjectType::Entity:    return IDType::Entity;
        case ObjectType::Component: return IDType::Component;
        case ObjectType::Resource:  return IDType::Resource;
    }
    return IDType::Node;
}

} // namespace AMEE
