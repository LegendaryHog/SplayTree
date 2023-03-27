#pragma once
#include "node.hpp"
#include "tree.hpp"
#include <iterator>

namespace Container
{

template<typename KeyT, class Cmp, derived_from_node<KeyT> Node>
class SearchTree;

template<typename KeyT, class Cmp>
class SplayTree;

namespace detail
{
template<typename KeyT, class Cmp, derived_from_node<KeyT> Node>
class SearchTreeIterator
{
public:
    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type   = typename std::ptrdiff_t;
    using value_type        = KeyT;
    using const_pointer     = KeyT*;
    using const_reference   = KeyT&;
    using node_ptr          = Node*;
    using const_node_ptr    = const Node*;
private:
    node_ptr node_, max_;

    static node_ptr cast(detail::Node<KeyT>* node)
    {
        return static_cast<node_ptr>(node);
    }
public:
    SearchTreeIterator(node_ptr node = nullptr, node_ptr max = nullptr)
    :node_ {node}, max_ {max}
    {}

    const_reference operator*() const {return node_->key_;}

    const_pointer operator->() const {return &(node_->key_);}

    SearchTreeIterator& operator++()
    {
        if (node_->right_ != nullptr)
            node_ = cast(detail::find_min(node_->right_));
        else
        {
            auto parent = node_->parent_;

            while (parent != nullptr && node_->is_right_son())
            {
                node_ = cast(parent);
                parent = parent->parent_;
            }
            node_ = cast(parent);
        }
        return *this;
    }

    SearchTreeIterator operator++(int)
    {
        auto cpy {*this};
        ++(*this);
        return cpy;
    }

    SearchTreeIterator& operator--()
    {
        if (node_ == nullptr)
            node_ = max_;
        else if (node_->left_ != nullptr)
            node_ = cast(detail::find_max(node_->left_));
        else
        {    
            auto parent = node_->parent_;

            while (parent != nullptr && node_->is_left_son())
            {
                node_ = cast(parent);
                parent = cast(parent->parent_);
            }
            node_ = cast(parent);
        }
        return *this;
    }

    SearchTreeIterator operator--(int)
    {
        auto cpy {*this};
        --(*this);
        return cpy;
    }

    bool operator==(const SearchTreeIterator& rhs) const {return (node_ == rhs.node_ && max_ == rhs.max_);}

protected:
    node_ptr base() const {return node_;}
    
public:
    friend class SearchTree<KeyT, Cmp, Node>;
    friend class SplayTree<KeyT, Cmp>;
}; // class SearchTreeIterator
} // namespace detail
} // namespace Container