#include "AMEENode.hpp"
#include "Log/AMEELog.hpp"
#include <algorithm>

namespace AMEE {

//
bool Node::AddChild(std::unique_ptr<Node> Child)
{
    if (!Child) {
        return false;
    }
    
    if (Child->m_pParent) {
        AMEE_LOG_WARN("Node", "Child already has a parent");
        return false;
    }
    Child->m_pParent = this;
    m_Children.push_back(std::move(Child));
    return true;
}

std::unique_ptr<Node> Node::RemoveChild(Node* Child)
{
    if (!Child) {
        return nullptr;
    }
    
    //
    auto it = std::find_if(m_Children.begin(), m_Children.end(),
                           [Child](const std::unique_ptr<Node>& Ptr) {
                               return Ptr.get() == Child;
                           });
    if (it != m_Children.end()) {
        
        auto extracted = std::move(*it);
        m_Children.erase(it);
        extracted->m_pParent = nullptr;
        return extracted;
    }
    
    return nullptr;
}

bool Node::IsActiveInHierarchy() const
{
    if (!m_Active) return false;
    const Node* P = m_pParent;
    while (P) {
        if (!P->m_Active) return false;
        P = P->m_pParent;
    }
    return true;
}

} // namespace AMEE
