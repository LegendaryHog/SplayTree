#pragma once
#include "colors.hpp"

namespace Container
{
namespace detail
{
template<typename KeyT = int>
struct RBNode
{
    using key_type = KeyT;
    using node_ptr = RBNode<KeyT>*; 

    key_type  key_ {};
    Colors color_ = Colors::Red;
    node_ptr parent_ = nullptr, left_ = nullptr, right_ = nullptr;

    bool is_left_son()  const noexcept {return this == parent_->left_;}
    bool is_right_son() const noexcept {return this == parent_->right_;}
    void copy_left(node_ptr new_node, node_ptr other, node_ptr Null)
    {
        left_ = new_node;
        left_->parent_ = this;

        left_->key_   = other->key_;
        left_->color_ = other->color_;
        left_->left_  = Null;
        left_->right_ = Null;
    }
    void copy_right(node_ptr new_node, node_ptr other, node_ptr Null)
    {
        right_ = new_node;
        right_->parent_ = this;

        right_->key_   = other->key_;
        right_->color_ = other->color_;
        right_->left_  = Null;
        right_->right_ = Null;
    }
    void init_data(node_ptr other, node_ptr Null)
    {
        key_   = other->key_;
        color_ = other->color_;

        parent_ = Null;
        left_   = Null;
        right_  = Null;
    }

    void dump(std::fstream& file) const
    {
        file << "Node_" << this << "[fillcolor=";
        if (color_ == Colors::Red)    
            file << "red, fontcolor=black";
        else
            file << "black, color=red";
            
        file << ", label = \"{<_node_>ptr:\\n " << this << "| parent:\\n " << parent_ << "| key: " << key_
        << "| {<left>left:\\n " << left_ << "| <right>right:\\n " << right_ << "}}\"];" << std::endl;
    }

    void action_after_left_rotate(node_ptr Null) {}

    void action_after_right_rotate(node_ptr Null) {}

    void action_before_insert(node_ptr Null) {}
};

// for sorted search tree
template<typename Node>
Node* find_min(Node* root, const Node* Null) noexcept
{
    auto node = root;
    for (node = root; node->left_ != Null; node = node->left_) {}
    return node;
}

// for sorted search tree
template<typename Node>
Node* find_max(Node* root, const Node* Null) noexcept
{
    auto node = root;
    for (node = root; node->right_ != Null; node = node->right_) {}
    return node;
}

} // namespace detail
} // namespace Container