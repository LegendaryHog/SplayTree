#pragma once
#include "search_tree.hpp"

namespace Container
{

namespace detail
{
struct SplayNode final : public Node
{
    using node_ptr = SplayNode*;
    std::size_t size_ = 0;

    node_ptr clone() const override
    {
        auto new_node = new SplayNode{base::key_};
        new_node->size_ = size_;
        return new_node;
    }
};
} // namespace detail
template<typename KeyT, class Cmp = std::less<KeyT>>
class SplayTree final : public SearchTree<KeyT, Cmp, SplayNode>
{
    using base = SearchTree<KeyT, Cmp, SplayNode>;
public:
    using typename base::node_type;
    using typename base::node_ptr;
    using typename base::const_node_ptr;
    using typename base::key_type;
    using typename base::size_type;
    
    using typename base::ConstIterator;
    using typename base::Iterator;
private:
    using base::root_;
    using base::size_;
    using base::max_;

public:
    SplayTree() = default;

    template<std::input_iterator InpIt>
    SplayTree(InpIt first, InpIt last)
    {
        SplayTree tmp {};
        while (first != last) 
        {
            auto cpy = *first++;
            tmp.insert_impl(std::move(cpy));
        }
        std::swap(*this, tmp);
    }

    SplayTree(std::initializer_list<key_type> ilist)
    :SplayTree(ilist.begin(), ilist.end())
    {}

    using base::insert;
    using base::erase;
    using base::find;

public:
    std::pair<ConstIterator, bool> insert(key_type&& key) noexcept override
    {
        return insert_impl(std::move(key));
    }

    ConstIterator find(const key_type& key) const override
    {
        auto itr = base::find_key(key);
        splay(itr.base());
        return itr;
    }

    using base::end;

    ConstIterator erase(ConstIterator itr) override
    {
        if (itr == end())
            return end();
        auto itr_next = std::next(itr);
        auto node = itr.base();
        splay(base::erase_from_tree(node));
        delete node;
        return itr_next;
    }

    ConstIterator lower_bound(const key_type& key) const override
    {
        auto lower_bound = base::lower_bound_ptr(key);
        splay(lower_bound);
        return ConstIterator{lower_bound, max_};
    }
    ConstIterator upper_bound(const key_type& key) const override 
    {
        auto upper_bound = base::upper_bound_ptr(key);
        splay(upper_bound);
        return ConstIterator{upper_bound, max_};
    }

    using base::key_less;
    using base::key_equal;

    size_type number_less_than(const key_type& key) const noexcept
    {
        size_type number = 0;
        node_ptr current = root_;
        while (current != nullptr)
            if (key_less(current->key_, key))
            {
                number += current->left_->size_ + 1;
                current = current->right_;
            }
            else
                current = current->left_;
        return number;
    }

    size_type number_not_greater_than(const key_type& key) const noexcept
    {
        size_type number = 0;
        node_ptr current = root_;
        while (current != nullptr)
            if (key_less(current->key_, key) || key_equal(current->key_, key))
            {
                number += current->left_->size_ + 1;
                current = current->right_;
            }
            else
                current = current->left_;
        return number;
    }
private:
    std::pair<ConstIterator, bool> insert_impl(key_type&& key)
    {
        auto ret = base::insert_key(std::move(key));
        if (ret.second)
            splay(ret.first.base());
        return ret;
    }

    void splay(node_ptr node) const
    {
        if (!node)
            return;

        while (node != root_)
            // zig case
            // pic for node->is_left_son()
            /*\_____________________________________
            |*      parent               node
            |*      /    \               /  \
            |*     /      c  ------>    a    \
            |*   node                       parent     
            |*   /  \                       /    \
            |*  a    b                     b      c
            |* _______________________________________                        
            \*/
            if (node->parent_ == root_)
                if (node->is_left_son())
                    right_rotate(node->parent_);
                else
                    left_rotate(node->parent_);
            else
            // zig-zig case
            // pic for case in first if, else case is symmetric
            /*\_____________________________________________________
            |*             gr_par                node         
            |*             /    \                /  \
            |*            /      d              a    \
            |*         parent                       parent
            |*         /    \        ------>        /   \
            |*        /      c                     b     \
            |*      node                                gr_par
            |*      /  \                                /    \
            |*     a    b                              c      d
            |* _______________________________________________________
            \*/
                if (node->is_left_son() && node->parent_->is_left_son())
                {
                    right_rotate(node->parent_->parent_);
                    right_rotate(node->parent_);
                }
                else if (node->is_right_son() && node->parent_->is_right_son())
                {
                    left_rotate(node->parent_->parent_);
                    left_rotate(node->parent_);
                }
            // zig-zag case
            /*\________________________________________________________
            |*          gr_par                    node
            |*          /    \                   /    \
            |*         /      c                 /      \
            |*      parent                  parent    gr_par 
            |*      /   \       ------>     /   \      /   \      
            |*     a     \                 a     b    c     d
            |*          node
            |*          /  \
            |*         b    c 
            |* ________________________________________________________
            \*/
                else if (node->is_right_son() && node->parent_->is_left_son())
                {
                    left_rotate(node->parent_);
                    right_rotate(node->parent_);
                }
                else // if (node->is_left_son() && node->parent_->is_right_son())
                {
                    right_rotate(node->parent_);
                    left_rotate(node->parent_);
                }
    }

    /*\_________________________________________________
    |*                                                  |
    |*        x                             y           |
    |*       / \                           / \          |
    |*      /   \     left_rotate(x)      /   \         |
    |*     l     y    ------------->     x     yr       |
    |*          / \                     / \             |
    |*         /   \                   /   \            |
    |*       yl     yr                l     yl          |
    |*__________________________________________________|
    \*/
    void left_rotate(node_ptr x) const noexcept
    {
        if (x->right_ == nullptr)
            return;
        // declare y as right son of x
        node_ptr y = x->right_;
        // new right son of x is yl 
        x->right_ = y->left_;

        // if yl exist (not nullptr)
        // replace parent of yl from y to x
        if (y->left_ != nullptr)      
            y->left_->parent_ = x;

        // parent of y will be parent of x
        y->parent_ = x->parent_;

        // if x were root,then y is new root
        if (x == root_)
            root_ = y;
        // if x were left son of parent,
        // then y is new left son
        else if (x->is_left_son())
            x->parent_->left_ = y;
        // else x were right son of parent,
        // then y is new right son
        else
            x->parent_->right_ = y;

        // x is y left son now
        y->left_ = x;
        // parent of x is y now
        x->parent_ = y;

        x->size_ = x->left_->size_ + x->right_->size_ + 1;
        y->size_ = y->left->size_ + y->right_->size_ + 1;
    }

    /*\_________________________________________________
    |*                                                  |
    |*        x                             y           |
    |*       / \                           / \          |
    |*      /   \     right_rotate(x)     /   \         |
    |*     y     r    -------------->    yl    x        |
    |*    / \                                 / \       |
    |*   /   \                               /   \      |
    |* yl     yr                           yr      r    |
    |* _________________________________________________|
    \*/
    void right_rotate(node_ptr x) const noexcept
    {
        if (x->left_ == nullptr)
            return;
        node_ptr y = x->left_;
        x->left_ = y->right_;

        if (y->right_ != nullptr)
            y->right_->parent_ = x;

        y->parent_ = x->parent_;
        
        if (x == root_)
            root_ = y;
        else if (x == x->parent_->left_)
            x->parent_->left_  = y;
        else
            x->parent_->right_ = y;

        y->right_  = x;
        x->parent_ = y;

        x->size_ = x->left_->size_ + x->right_->size_ + 1;
        y->size_ = y->left->size_ + y->right_->size_ + 1;
    }
};
} // namespace Container