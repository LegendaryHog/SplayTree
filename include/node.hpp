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
    
    virtual node_ptr clone() const
    {
        auto new_node = new Node{};
        new_node->key_ = key_;
        return new_node;
    }
    virtual void create(key_type&& key) {key_ = std::move(key);}

    bool is_left_son()  const noexcept {return this == parent_->left_;}
    bool is_right_son() const noexcept {return this == parent_->right_;}
    
    void dump(std::fstream& file) const
    {
        file << "Node_" << this << "[fillcolor=lightgreen";    
        file << ", label = \"{<_node_>ptr:\\n " << this << "| parent:\\n " << parent_ << "| key: " << key_
        << "| {<left>left:\\n " << left_ << "| <right>right:\\n " << right_ << "}}\"];" << std::endl;
    }
};

// for sorted search tree
template<typename KeyT>
Node<KeyT>* find_min(Node<KeyT>* root) noexcept
{
    auto node = root;
    for (node = root; node->left_ != nullptr; node = node->left_) {}
    return node;
}

// for sorted search tree
template<typename KeyT>
Node<KeyT>* find_max(Node<KeyT>* root) noexcept
{
    auto node = root;
    for (node = root; node->right_ != nullptr; node = node->right_) {}
    return node;
}

} // namespace detail

template<typename Derived, typename KeyT>
concept derived_from_node = std::is_base_of<detail::Node<KeyT>, Derived>::value;

} // namespace Container
