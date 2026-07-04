#include "AMEEScene.hpp"
#include "Entity/AMEEEntity.hpp"

namespace AMEE {

void Scene::AddEntity(std::unique_ptr<Entity> Ent)
{
    AddChild(std::move(Ent));
}

void Scene::RemoveEntity(Entity* Ent)
{
    RemoveChild(Ent);
}

void Scene::Update(float DeltaTime)
{
    for (auto& Child : GetChildren()) {
        if (Child && Child->IsActive()) {
            if (Entity* Ent = dynamic_cast<Entity*>(Child.get())) {
                Ent->Update(DeltaTime);
            }
        }
    }
}

} // namespace AMEE