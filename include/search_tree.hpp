#pragma once
#include <algorithm>
#include <fstream>
#include <cassert>
#include "tree.hpp"
#include "search_tree_iterator.hpp"

namespace Container
{

template<typename KeyT, class Cmp = std::less<KeyT>, derived_from_node<KeyT> SearchNode = detail::Node<KeyT>> 
class SearchTree : public Tree<KeyT, SearchNode>
{
    using base = Tree<KeyT, SearchNode>;
public:
    using typename base::node_type;
    using typename base::node_ptr;
    using typename base::const_node_ptr;
    using typename base::key_type;
    using typename base::size_type;
    
    using ConstIterator = detail::SearchTreeIterator<key_type, Cmp, SearchNode>;
    using Iterator      = ConstIterator;

protected:
    using base::root_;
    using base::size_;
    node_ptr min_  = nullptr, max_  = nullptr; 

    Cmp cmp {};

    bool key_less(const key_type& key1, const key_type& key2) const {return cmp(key1, key2);}
    bool key_equal(const key_type& key1, const key_type& key2) const
    {
        return !cmp(key1, key2) && !cmp(key2, key1);
    }

    using base::cast;
//----------------------------------------=| Ctors start |=---------------------------------------------
public:
    SearchTree() = default;

    template<std::input_iterator InpIt>
    SearchTree(InpIt first, InpIt last)
    {   
        SearchTree tmp {};
        while (first != last)
        {
            auto cpy = *first++;
            tmp.insert_impl(std::move(cpy));
        }
        std::swap(*this, tmp);
    }

    SearchTree(std::initializer_list<key_type> initlist)
    :SearchTree(initlist.begin(), initlist.end())
    {}
//----------------------------------------=| Ctors end |=-----------------------------------------------

//----------------------------------------=| Max/min methods start |=-----------------------------------
    const key_type& maximum() const noexcept {return max_->key_;}
    const key_type& minimum() const noexcept {return min_->key_;}
//----------------------------------------=| Max/min methods end |=-------------------------------------

//----------------------------------------=| Big five start |=------------------------------------------
    SearchTree(SearchTree&&) = default;
    SearchTree(const SearchTree& other)
    :base::Tree(other), min_ {cast(detail::find_min(root_))}, max_ {cast(detail::find_max(root_))}
    {}
    SearchTree& operator=(const SearchTree&) = default;
    SearchTree& operator=(SearchTree&&) = default;
    virtual ~SearchTree() = default;
//----------------------------------------=| Big five end |=--------------------------------------------

//----------------------------------------=| begin/end start |=-----------------------------------------
    ConstIterator begin() const noexcept {return ConstIterator{min_, max_};}
    ConstIterator end()   const noexcept {return ConstIterator{nullptr, max_};}

    ConstIterator cbegin() const noexcept {return ConstIterator{min_, max_};}
    ConstIterator cend()   const noexcept {return ConstIterator{nullptr, max_};}
//----------------------------------------=| begin/end end |=-------------------------------------------

//----------------------------------------=| Find start |=----------------------------------------------
protected:
    ConstIterator find_key(const key_type& key) const
    {
        node_ptr node = root_;
        while (node)
            if (key_less(key, node->key_))
                node = cast(node->left_);
            else if (key_less(node->key_, key))
                node = cast(node->right_);
            else
                return ConstIterator{node, max_};
        return end();
    }
public:
    virtual ConstIterator find(const key_type& key) const 
    {
        return find_key(key);
    }
//----------------------------------------=| Find end |=------------------------------------------------

//----------------------------------------=| Insert start |=--------------------------------------------
protected:
    std::pair<ConstIterator, bool> insert_key(key_type&& key)
    {
        auto parent = find_parent(key);

        // if key is alredy in tree
        if (parent && key_equal(parent->key_, key))
            return std::pair{ConstIterator{parent, max_}, false};

        auto new_node = new node_type(std::move(key));
        new_node->parent_ = parent;
        insert_in_place(new_node);
        return std::pair{ConstIterator{new_node, max_}, true};
    }

private:
    node_ptr find_parent(const key_type& key) const
    {
        node_ptr x = root_;
        node_ptr y = nullptr;

        while (x)
        {
            // save pointer on x before turn
            y = x;
            // if key less x.key turn left
            if (key_less(key, x->key_))
                x = cast(x->left_);
            // else turn right
            else if (key_equal(key, x->key_))
                return x;
            else
                x = cast(x->right_);
        }
        return y;
    }

    std::pair<ConstIterator, bool> insert_impl(key_type&& key)
    {
        return insert_key(std::move(key));
    }

    void insert_in_place(node_ptr node)
    {
        assert(node);
        // increment size
        size_++;
        // if tree is empty
        if (!node->parent_)
        {
            // root is node
            root_ = node;
            // upadte min and max
            min_  = root_;
            max_ = root_;
            return;
        }
        
        // insert new node in right place
        if (key_less(node->key_, node->parent_->key_))
            node->parent_->left_ = node;
        else
            node->parent_->right_ = node;
        insert_fix_min_max(node);
    }

    void insert_fix_min_max(node_ptr node)
    {
        if (key_less(max_->key_, node->key_))
            max_ = node;
        if (key_less(node->key_, min_->key_))
            min_ = node;
    }

public:
    std::pair<ConstIterator, bool> insert(const key_type& key)
    {
        key_type key_cpy {key};
        return insert(std::move(key_cpy));
    }

    virtual std::pair<ConstIterator, bool> insert(key_type&& key)
    {
        return insert_impl(std::move(key));
    }

    template<std::input_iterator InpIt>
    void insert(InpIt first, InpIt last)
    {
        for (auto itr = first; itr != last; ++itr)
            insert(*itr);
    }

    void insert(std::initializer_list<key_type> initlist)
    {
        insert(initlist.begin(), initlist.end());
    }
//----------------------------------------=| Insert end |=----------------------------------------------

//----------------------------------------=| Erase start |=---------------------------------------------
    // replace subtree with root u with subtree with root v
    void transplant(node_ptr u, node_ptr v) noexcept
    {
        if (u == root_)
            root_ = v;
        else if (u->is_left_son())
            u->parent_->left_ = v;
        else
            u->parent_->right_ = v;
        if (v)
            v->parent_ = u->parent_;
    }

    void delete_fix_min_max(node_ptr node) noexcept
    {
        if (node == min_)
            min_ = cast(detail::find_min(root_));
        if (node == max_)
            max_ = cast(detail::find_max(root_));
    }
    
protected:
    // delete z from tree with saving all invariants
    node_ptr erase_from_tree(node_ptr z) noexcept
    {
        assert(z);
        // decrement size
        size_--;
        // declare two pointer, y - replacment for z in else case
        // x - root of subtree, where we need fix invarinats 
        node_ptr y = nullptr, x = nullptr;
        // if z has only right subtree
        /*\_____________________________
        |*     |                  |     |
        |*     |                  |     |
        |*     z    --------->    zr    |
        |*      \                /  \   |
        |*       \                      |
        |*        zr                    |
        |*       /  \                   |
        |* _____________________________|
        \*/
        if (!z->left_)
        {
            // save root os subtree that will be replace
            x = cast(z->right_);
            // replace right subtree of z with z
            transplant(z, cast(z->right_));
        }
        // if z has only left subtree
        /*\_____________________________
        |*     |                  |     |
        |*     |                  |     |
        |*     z    --------->    zl    |
        |*    /                  /  \   |
        |*   /                          |
        |*  zl                          |
        |* /  \                         |
        |* _____________________________|
        \*/
        else if (!z->right_)
        {
            x = cast(z->left_);
            transplant(z, cast(z->left_));
        }
        /*\____________________________________________________
        |*      z                z                     y       |
        |*     / \              /                     / \      | 
        |*    /   \            /    y                /   \     |
        |*   zl   zr          zl     \              zl   zr    |
        |*  / \   / \        /  \     \                  / \   |
        |*       ~       1             zr      2        ~      |
        |*      /      ---->          /  \   ---->     /       |
        |*     y                     ~                yr       | 
        |*      \                   /                /  \      |
        |*       \                 yr                          |
        |*        yr              /  \                         |
        |*       /  \                                          | 
        |* ____________________________________________________|
        \*/
        else
        {
            // find y in right subtree of x
            // y most left of z->right than y->left_ == nullptr
            y = cast(detail::find_min(z->right_));
            // save root of subtree that we maybe need to fix
            x = cast(y->right_);
            
            //first arrow on picture
            // if y right son of z
            if (y->parent_ == z)
                // only set parent of x as y
                x->parent_ = y;
            else
            {
                // replace y with right subtree of y
                // after this x->parent_ will have correct value
                transplant(y, cast(y->right_));
                // connect right son of z and y
                y->right_ = z->right_;
                y->right_->parent_ = y;
            }
            // second arrow on picture
            // replace z with y
            transplant(z, y);
            // connect left son of z with y
            y->left_ = z->left_;
            y->left_->parent_ = y;
        }

        delete_fix_min_max(z);
        return y;
    }

public:
    virtual ConstIterator erase(ConstIterator itr)
    {
        if (itr == end())
            return end();
        auto itr_next = std::next(itr);
        auto node = itr.base();
        erase_from_tree(node);
        delete node;
        return itr_next;
    }

    ConstIterator erase(const key_type& key)
    {
        return erase(find(key));
    }

    ConstIterator erase(ConstIterator first, ConstIterator last)
    {
        ConstIterator ret {};
        while (first != last)
            ret = erase(first++);
        return ret;
    }
//----------------------------------------=| Erase end |=-----------------------------------------------

//----------------------------------------=| Bounds start |=--------------------------------------------
protected:
    node_ptr lower_bound_ptr(const key_type& key) const
    {
        node_ptr result = nullptr, current = root_;
        while (current)
            if (!key_less(current->key_, key))
            {
                result = current;
                current = cast(current->left_);
            }
            else
                current = cast(current->right_);
        return result;
    }

    node_ptr upper_bound_ptr(const key_type& key) const 
    {
        node_ptr result = nullptr, current = root_;
        while (current)
            if (key_less(key, current->key_))
            {
                result = current;
                current = cast(current->left_);
            }
            else
                current = cast(current->right_);
        return result;
    }

public:
    virtual ConstIterator lower_bound(const key_type& key) const {return ConstIterator{lower_bound_ptr(key), max_};}
    virtual ConstIterator upper_bound(const key_type& key) const {return ConstIterator{upper_bound_ptr(key), max_};}
//----------------------------------------=| Bounds end |=----------------------------------------------

//----------------------------------------=| Graph dump start |=----------------------------------------  
    void debug_graph_dump(const std::string& filename) const
    {   
        std::fstream file {filename + ".dot", std::ofstream::out | std::ofstream::trunc};

        file << "digraph G {" << std::endl;
        file << "\trankdir=\"TB\"" << std::endl;
        file << "\tnode[shape=record, penwidth=3.0, style=filled, color=black, fontcolor=black];" << std::endl;
        descriptor_dump(file);
        tree_dump(file);
        file << "}" << std::endl;
        file.close();

        std::system(("dot -T svg " + filename + ".dot -o " + filename + ".svg").c_str());
        std::system(("rm " + filename + ".dot").c_str());
    }

private:
    void descriptor_dump(std::fstream& file) const
    {
        file << "\tTree [fillcolor=purple, label = \"{ SearchTree\\ndescriptor| size: " << this->size() << "| <root> root:\\n " << root_;
        if (!this->empty())
            file << "| min key: " << minimum() << "| max key: " << maximum();
        file << "}\"];" << std::endl;
    }
    
    void tree_dump(std::fstream& file) const
    {
        if (this->empty())
            return;

        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
            itr.base()->dump(file);

        file << "edge[penwidth=3, color=black];" << std::endl;
        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
        {
            node_ptr node = itr.base();
            if (node->left_)
                file << "Node_" << node << ":left:s -> Node_" << node->left_ << ":_node_:n;" << std::endl;
            if (node->right_)
                file << "Node_" << node << ":right:s -> Node_" << node->right_ << ":_node_:n;" << std::endl;
        }

        file << "Tree:root:e -> Node_" << root_ << ":_node_:n;" << std::endl;
    }
//----------------------------------------=| Graph dump end |=------------------------------------------

//----------------------------------------=| equal_to start |=------------------------------------------
public:
    bool equal_to(const SearchTree& other) const
    {
        if (this->size() != other.size())
            return false;
        
        for (auto itr = cbegin(), other_itr = other.cbegin(), end = cend(); itr != end; ++itr, ++other_itr)
            if (!key_equal(*itr, *other_itr))
                return false;
        return true;
    }
//----------------------------------------=| equal_to end |=--------------------------------------------
}; // class ISearchTree

template<typename KeyT, class Cmp = std::less<KeyT>, derived_from_node<KeyT> SearchNode = detail::Node<KeyT>>
bool operator==(const SearchTree<KeyT, Cmp, SearchNode>& lhs, const SearchTree<KeyT, Cmp, SearchNode>& rhs)
{
    return lhs.equal_to(rhs);
}
} // namespace Container