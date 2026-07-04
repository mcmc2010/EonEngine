#include "AMEEScene.hpp"
#include "Entity/AMEEEntity.hpp"

namespace AMEE {

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