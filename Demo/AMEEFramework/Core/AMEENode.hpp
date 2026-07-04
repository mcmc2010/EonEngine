#ifndef __AMEE_NODE_H__
#define __AMEE_NODE_H__
#pragma once
#include <vector>
#include <memory>
#include <cstdint>

namespace AMEE {

class Node {
public:
    Node() = default;
    virtual ~Node() = default;

    // Hierarchy
    void AddChild(std::unique_ptr<Node> Child);
    void RemoveChild(Node* Child);
    Node* GetParent() const { return m_pParent; }
    const std::vector<std::unique_ptr<Node>>& GetChildren() const { return m_Children; }
    size_t GetChildCount() const { return m_Children.size(); }

    // Active (affects entire subtree)
    void SetActive(bool Active) { m_Active = Active; }
    bool IsActive() const { return m_Active; }
    bool IsActiveInHierarchy() const;

private:
    Node* m_pParent = nullptr;
    std::vector<std::unique_ptr<Node>> m_Children;
    bool m_Active = true;
};

} // namespace AMEE

#endif // __AMEE_NODE_H__
