#ifndef __AMEE_SCENE_H__
#define __AMEE_SCENE_H__
#pragma once
#include "AMEENode.hpp"
#include <string>

namespace AMEE {

class Entity;

class Scene : public Node {
public:
    Scene() = default;
    virtual ~Scene() = default;

    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& Name) { m_Name = Name; }

    // Add root entity to scene
    void AddEntity(std::unique_ptr<Entity> Entity);

    // Remove root entity from scene
    void RemoveEntity(Entity* Entity);

    // Update all entities in scene
    virtual void Update(float DeltaTime);

private:
    std::string m_Name;
};

} // namespace AMEE

#endif // __AMEE_SCENE_H__