#pragma once
#include "node.hpp"

namespace Container
{

template<typename KeyT, derived_from_node<KeyT> Node>
class Tree
{
public:
    using node_type      = Node;
    using node_ptr       = node_type*;
    using const_node_ptr = const node_type*;
    using key_type       = KeyT;
    using size_type      = std::size_t;

protected:
    mutable node_ptr  root_ = nullptr;
    size_type size_ = 0;

    static node_ptr cast(detail::Node<KeyT>* node) noexcept
    {
        return static_cast<node_ptr>(node);
    }
public:
    Tree() = default;

    size_type size() const noexcept {return size_;}

    bool empty() const noexcept {return (size_ == 0);}

private:
    void swap(Tree& rhs) noexcept
    {
        std::swap(root_, rhs.root_);
        std::swap(size_, rhs.size_);
    }
public:
    Tree(Tree&& other) noexcept 
    {
        swap(other);
    }
    Tree& operator=(Tree&& rhs) noexcept 
    {
        swap(rhs);
        return *this;
    }

    Tree(const Tree& other): size_ {other.size_}
    {
        if (empty())
            return;

        Tree tmp {std::move(*this)};


        tmp.root_ = new node_type(*other.root_);
        auto tmp_current   = tmp.root_;
        auto other_current = other.root_;

        while (other_current)
            if (other_current->left_  && !tmp_current->left_)
            {
                tmp_current->left_ = new node_type(*cast(other_current->left_));
                tmp_current->left_->parent_ = tmp_current;
                other_current = cast(other_current->left_);
                tmp_current   = cast(tmp_current->left_);
            }
            else if (other_current->right_ && !tmp_current->right_)
            {
                tmp_current->right_ = new node_type(*cast(other_current->right_));
                tmp_current->right_->parent_ = tmp_current;
                other_current = cast(other_current->right_);
                tmp_current   = cast(tmp_current->right_);
            }
            else
            {
                other_current = cast(other_current->parent_);
                tmp_current   = cast(tmp_current->parent_);
            }
        swap(tmp);
    }

    Tree& operator=(const Tree& rhs)
    {
        auto rhs_cpy {rhs};
        swap(rhs_cpy);
        return *this;
    }

    virtual ~Tree()
    {
        if (empty())
            return;

        auto current = root_;
        while (current)
            if (current->left_)
                current = static_cast<node_ptr>(current->left_);
            else if (current->right_)
                current = static_cast<node_ptr>(current->right_);
            else
            {
                auto parent = static_cast<node_ptr>(current->parent_);

                if (current == root_)
                    break;
                else if (current->is_left_son())
                    parent->left_ = nullptr;
                else
                    parent->right_ = nullptr;

                delete current;
                current = parent;
            }
        delete root_;
    }
};
} // namwspace Container