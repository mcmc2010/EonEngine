#ifndef __AMEE_SCENE_H__
#define __AMEE_SCENE_H__
#pragma once
#include "AMEENode.hpp"
#include "AMEEObject.hpp"

namespace AMEE {

class Entity;

class Scene : public Node, public Object {
public:
    Scene() = default;
    virtual ~Scene() = default;

    void Update(float DeltaTime);
};

} // namespace AMEE

#endif // __AMEE_SCENE_H__