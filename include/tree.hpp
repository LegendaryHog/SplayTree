#pragma once
#include "node.hpp"

namespace Container
{
template<typename KeyT>
class Tree
{
public:
    using node_type      = typename detail::Node<KeyT>;
    using node_ptr       = node_type*;
    using const_node_ptr = const node_type*;
    using key_type       = KeyT;
    using size_type      = typename std::size_t;

protected:
    node_ptr  root_ = nullptr;
    size_type size_ = 0;

public:
    Tree() = default;

    size_type size() const {return size_;}

    bool empty() const {return (size_ == 0);}

private:
    void swap(Tree& rhs) noexcept
    {
        std::swap(root_, rhs.root_);
        std::swap(size_, rhs.size_);
    }

public:
    Tree(Tree&& other)
    {
        swap(other);
    }
    Tree& operator=(Tree&& rhs)
    {
        swap(rhs);
        return *this;
    }

    Tree(const Tree& other): size_ {other.size_}
    {
        if (empty())
            return;

        Tree tmp {std::move(*this)};

        tmp.root_ = new node_type{other.root_->key_};
        auto tmp_current   = tmp.root_;
        auto other_current = other.root_;

        while (other_current != nullptr)
            if (other_current->left_ != nullptr && tmp_current->left_ == nullptr)
            {
                tmp_current->left_ = new node_type{other_current->left_->key_, tmp_current};
                other_current = other_current->left_;
                tmp_current   = tmp_current->left_;
            }
            else if (other_current->right_ != nullptr && tmp_current->right_ == nullptr)
            {
                tmp_current->right_ = new node_type{other_current->right_->key_, tmp_current};
                other_current = other_current->right_;
                tmp_current   = tmp_current->right_;
            }
            else
            {
                other_current = other_current->parent_;
                tmp_current   = tmp_current->parent_;
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
        while (current != nullptr)
            if (current->left_ != nullptr)
                current = current->left_;
            else if (current->right_ != nullptr)
                current = current->right_;
            else
            {
                auto parent = current->parent_;

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