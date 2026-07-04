#include "AMEENode.hpp"
#include "Log/AMEELog.hpp"
#include <algorithm>

namespace AMEE {

void Node::AddChild(std::unique_ptr<Node> Child)
{
    if (!Child) return;
    if (Child->m_pParent) {
        AMEE_LOG_WARN("Node", "Child already has a parent");
        return;
    }
    Child->m_pParent = this;
    m_Children.push_back(std::move(Child));
}

void Node::RemoveChild(Node* Child)
{
    if (!Child) return;
    auto it = std::find_if(m_Children.begin(), m_Children.end(),
                           [Child](const std::unique_ptr<Node>& Ptr) {
                               return Ptr.get() == Child;
                           });
    if (it != m_Children.end()) {
        (*it)->m_pParent = nullptr;
        m_Children.erase(it);
    }
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
