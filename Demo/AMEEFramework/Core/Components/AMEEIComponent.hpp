
#ifndef __AMEE_ICOMPONENT_H__
#define __AMEE_ICOMPONENT_H__
#pragma once


namespace AMEE {


class Entity;

////
class IComponent {
public:

    virtual ~IComponent() = default;

    // Owner
    virtual Entity* GetOwner() const = 0;
    virtual void SetOwner(Entity* pOwner) = 0;
    
    //
    virtual bool IsVisible() = 0;
    virtual void SetVisible(bool value) = 0;

    // Lifecycle
    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;
    virtual void Update(float DeltaTime) = 0;

};

} // namespace AMEE

#endif // __AMEE_ICOMPONENT_H__
