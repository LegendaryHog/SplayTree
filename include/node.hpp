#pragma once
#include <iostream>

namespace Container
{
namespace detail
{
template<typename KeyT = int>
struct Node
{
    using node_ptr = Node*;
    using key_type = KeyT;

    key_type key_ {};
    node_ptr parent_ = nullptr, left_ = nullptr, right_ = nullptr;

    bool is_left_son()  const noexcept {return this == parent_->left_;}
    bool is_right_son() const noexcept {return this == parent_->right_;}
    
    void dump(std::fstream& file) const
    {
        file << "Node_" << this << "[fillcolor=";    
        file << ", label = \"{<_node_>ptr:\\n " << this << "| parent:\\n " << parent_ << "| key: " << key_
        << "| {<left>left:\\n " << left_ << "| <right>right:\\n " << right_ << "}}\"];" << std::endl;
    }
};

Node* find_min(Node* root) noexcept
{
    auto node = root;
    for (node = root; node->left_ != nullptr; node = node->left_) {}
    return node;
}

// for sorted search tree
Node* find_max(Node* root) noexcept
{
    auto node = root;
    for (node = root; node->right_ != nullptr; node = node->right_) {}
    return node;
}

} // namespace detail
} // namespace Container
