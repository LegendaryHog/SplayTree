#pragma once
#include "colors.hpp"
#include <fstream>
namespace Container
{
namespace detail
{
template<typename KeyT>
struct RBSubTreeRoot
{
    using key_type  = KeyT;
    using node_ptr  = RBSubTreeRoot<KeyT>*;
    using size_type = typename std::size_t; 

    key_type  key_ {};
    Colors color_ = Colors::Red;
    node_ptr parent_ = nullptr, left_ = nullptr, right_ = nullptr;
    size_type size_ = 0;

    bool is_left_son()  const noexcept {return this == parent_->left_;}
    bool is_right_son() const noexcept {return this == parent_->right_;}

private:
    void copy_data(node_ptr other)
    {
        key_   = other->key_;
        color_ = other->color_;
        size_  = other->size_;
    }

public:
    void copy_left(node_ptr new_node, node_ptr other, node_ptr Null)
    {
        left_ = new_node;
        left_->parent_ = this;
        left_->copy_data(other);
        left_->left_  = Null;
        left_->right_ = Null;
    }

    void copy_right(node_ptr new_node, node_ptr other, node_ptr Null)
    {
        right_ = new_node;
        new_node->parent_ = this;
        right_->copy_data(other);
        new_node->left_ = Null;
        new_node->right_ = Null;
    }

    void init_data(node_ptr other, node_ptr Null)
    {
        copy_data(other);
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
        << "| size: " << size_ << "| {<left>left:\\n " << left_
        << "| <right>right:\\n " << right_ << "}}\"];" << std::endl;
    }

private:
    void recalc_size() noexcept
    {
        size_ = left_->size_ + right_->size_ + 1;
    }
public:
    void action_after_left_rotate(node_ptr Null) noexcept
    {
        left_->recalc_size();
        recalc_size();
    }

    void action_after_right_rotate(node_ptr Null) noexcept
    {
        right_->recalc_size();
        recalc_size();
    }

    void action_before_insert(node_ptr Null) noexcept
    {
        for (auto node = this; node != Null; node = node->parent_)
            node->size_++;
    }
};
} // namespace detail
} // namespace Container