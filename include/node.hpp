#pragma once
#include <iostream>
#include <concepts>
#include <type_traits>

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

    explicit Node(key_type&& key): key_ {std::move(key)} {}
    explicit Node(const key_type& key): key_ {key} {}

    Node(const Node& node): key_ {node.key_} {}

    bool is_left_son()  const noexcept {return this == parent_->left_;}
    bool is_right_son() const noexcept {return this == parent_->right_;}
    
    void dump(std::fstream& file) const
    {
        file << "Node_" << this << "[fillcolor=lightgreen";    
        file << ", label = \"{<_node_>ptr:\\n " << this << "| parent:\\n " << parent_ << "| key: " << key_
        << "| {<left>left:\\n " << left_ << "| <right>right:\\n " << right_ << "}}\"];" << std::endl;
    }

    virtual ~Node() = default;
};

// for sorted search tree
template<typename KeyT>
Node<KeyT>* find_min(Node<KeyT>* root) noexcept
{
    auto node = root;
    if (!node)
        return nullptr;
    for (node = root; node->left_ != nullptr; node = node->left_) {}
    return node;
}

// for sorted search tree
template<typename KeyT>
Node<KeyT>* find_max(Node<KeyT>* root) noexcept
{
    auto node = root;
    if (node)
        for (node = root; node->right_ != nullptr; node = node->right_) {}
    return node;
}

} // namespace detail

template<typename Derived, typename KeyT>
concept derived_from_node = std::is_base_of<detail::Node<KeyT>, Derived>::value;

} // namespace Container
