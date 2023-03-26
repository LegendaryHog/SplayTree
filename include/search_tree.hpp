#pragma once
#include <algorithm>
#include "node.hpp"
#include "search_tree_iterator.hpp"

namespace Container
{

template<typename KeyT, class Cmp>  
class ISearchTree : public Tree
{
public:
    using node_type      = typename detail::Node<KeyT>;
    using node_ptr       = node_type*;
    using const_node_ptr = const node_type*;
    using key_type       = KeyT;
    using size_type      = typename std::size_t;
    using ConstIterator = SearchTreeIterator<key_type, Cmp, node_type>;
    using Iterator      = ConstIterator;

protected:
    node_ptr  root_ = nullptr;
    size_type size_ = 0;
    node_ptr  min_  = nullptr, max_  = nullptr; 

    Cmp cmp {};

    bool key_less(const key_type& key1, const key_type& key2) const {return cmp(key1, key2);}
    bool key_equal(const key_type& key1, const key_type& key2) const
    {
        return !cmp(key1, key2) && !cmp(key2, key1);
    }

//----------------------------------------=| Ctors start |=---------------------------------------------
    ISearchTree() = default;

    template<std::input_iterator InpIt>
    ISearchTree(InpIt first, InpIt last)
    {   
        ISearchTree tmp {};
        tmp.insert(first, last);
        swap(tmp);
    }

    ISearchTree(std::initializer_list<key_type> initlist)
    :ISearchTree(initlist.begin(), initlist.end())
    {}
//----------------------------------------=| Ctors end |=-----------------------------------------------

//----------------------------------------=| Size`s funcs start |=--------------------------------------
    size_type size() const {return size_;}

    bool empty() const {return (size_ == 0);} 
//----------------------------------------=| Size`s funcs end |=----------------------------------------

//----------------------------------------=| Max/min methods start |=-----------------------------------
    const key_type& maximum() const {return max_->key_;}
    const key_type& minimum() const {return min_->key_;}
//----------------------------------------=| Max/min methods end |=-------------------------------------

//----------------------------------------=| Big five start |=------------------------------------------
private:
    void swap(ISearchTree& rhs) noexcept
    {
        std::swap(root_, rhs.root_);
        std::swap(size_, rhs.size_);
    }

public:
    ISearchTree(ISearchTree&& other)
    {
        swap(other);
    }
    ISearchTree& operator=(ISearchTree&& rhs)
    {
        swap(rhs);
        return *this;
    }

    ISearchTree(const ISearchTree& other): size_ {other.size_}
    {
        if (empty())
            return;

        ISearchTree tmp {std::move(*this)};

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
        
        min_  = detail::find_min(root_);
        max_  = detail::find_max(root_);
    }

    ISearchTree& operator=(const ISearchTree& rhs)
    {
        auto rhs_cpy {rhs};
        swap(rhs_cpy);
        return *this;
    }

    virtual ~ISearchTree() 
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
//----------------------------------------=| Big five end |=--------------------------------------------

//----------------------------------------=| begin/end start |=-----------------------------------------
public:
    ConstIterator begin() const {return ConstIterator{min_, max_};}
    ConstIterator end()   const {return ConstIterator{nullptr, max_};}

    ConstIterator cbegin() const {return ConstIterator{min_, max_};}
    ConstIterator cend()   const {return ConstIterator{nullptr, max_};}
//----------------------------------------=| begin/end end |=-------------------------------------------

//----------------------------------------=| Find start |=----------------------------------------------
private:
    ConstIterator find_key(const key_type& key)
    {
        node_ptr node = root_;
        while (node != nullptr)
            if (key_less(key, node->key_))
                node = node->left_;
            else if (key_less(node->key_, key))
                node = node->right_;
            else
                return ConstIterator{node, max_};
        return end();
    }
public:
    virtual ConstIterator find(const key_type& key)
    {
        return find_key(key);
    }
//----------------------------------------=| Find end |=------------------------------------------------

//----------------------------------------=| Insert start |=--------------------------------------------
private:
    node_ptr find_parent(const key_type& key) const noexcept
    {
        auto x = root_;
        auto y = nullptr;

        while (x != nullptr)
        {
            // save pointer on x before turn
            y = x;
            // if key less x.key turn left
            if (key_less(key, x->key_))
                x = x->left_;
            // else turn right
            else if (key_equal(key, x->key_))
                return x;
            else
                x = x->right_;
        }
        return y;
    }

    node_ptr insert_key(key_type&& key)
    {
        auto parent = find_parent(key);

        // if key is alredy in tree
        if (parent != nullptr && key_equal(parent->key_, key))
            return std::pair{ConstIterator{parent, nullptr}, false};

        node_ptr new_node = new node_type{std::move(key), parent};
        insert_in_place(new_node);
        return new_node;
    }

    void insert_in_place(node_ptr node) noexcept
    {
        // increment size
        size_++;
        // if tree is empty
        if (node->parent_ == nullptr)
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

protected:
    std::pair<ConstIterator, bool> insert(const key_type& key)
    {
        key_type key_cpy {key};
        return insert(std::move(key_cpy));
    }

    std::pair<ConstIterator, bool> insert(key_type&& key) noexcept
    {
        return std::pair{ConstIterator{insert_key(std::move(key)), nullptr}, true};
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

private:
    void insert_fix_min_max(node_ptr node) noexcept
    {
        if (key_less(max_->key_, node->key_))
            max_ = node;
        if (key_less(node->key_, min_->key_))
            min_ = node;
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
        if ( v != nullptr)
            v->parent_ = u->parent_;
    }

    // delete z from tree with saving all invariants
    void erase_from_tree(node_ptr z)
    {
        // decrement size
        size_--;
        // declare two pointer, y - replacment for z in else case
        // x - root of subtree, where we need fix invarinats 
        node_ptr y = z, x = nullptr;
        auto y_original_color = y->color_;
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
        if (z->left_ == nullptr)
        {
            // save root os subtree that will be replace
            x = z->right_;
            // replace right subtree of z with z
            transplant(z, z->right_);
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
        else if (z->right_ == nullptr)
        {
            x = z->left_;
            transplant(z, z->left_);
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
            y = detail::find_min(z->right_);
            // save root of subtree that we maybe need to fix
            x = y->right_;
            
            //first arrow on picture
            // if y right son of z
            if (y->parent_ == z)
                // only set parent of x as y
                x->parent_ = y;
            else
            {
                // replace y with right subtree of y
                // after this x->parent_ will have correct value
                transplant(y, y->right_);
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
    }

private:
    void delete_fix_min_max(node_ptr node)
    {
        if (node == min_)
            min_ = detail::find_min(root_, nullptr);
        if (node == max_)
            max_ = detail::find_max(root_, nullptr);
    }

protected:
    ConstIterator erase(ConstIterator itr)
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
private:
    node_ptr lower_bound_ptr(const key_type& key) const
    {
        node_ptr result = nullptr, current = root_;
        while (current != nullptr)
            if (!key_less(current->key_, key))
            {
                result = current;
                current = current->left_;
            }
            else
                current = current->right_;
        return result;
    }

    node_ptr upper_bound_ptr(const key_type& key) const 
    {
        node_ptr result = nullptr, current = root_;
        while (current != nullptr)
            if (key_less(key, current->key_))
            {
                result = current;
                current = current->left_;
            }
            else
                current = current->right_;
        return result;
    }

protected:
    ConstIterator lower_bound(const key_type& key) const {return ConstIterator{lower_bound_ptr(key), nullptr};}
    ConstIterator upper_bound(const key_type& key) const {return ConstIterator{upper_bound_ptr(key), nullptr};}
//----------------------------------------=| Bounds end |=----------------------------------------------

//----------------------------------------=| Graph dump start |=----------------------------------------  
    void debug_graph_dump(const std::string& filename) const
    {   
        std::fstream file {filename + ".dot", std::ofstream::out | std::ofstream::trunc};

        file << "digraph G {" << std::endl;
        file << "\trankdir=\"TB\"" << std::endl;
        file << "\tnode[shape=record, penwidth=3.0, style=filled, color=black, fontcolor=white];" << std::endl;
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
        file << "\tTree [fillcolor=purple, label = \"{ ISearchTree\\ndescriptor| size: " << size() << "| <root> root:\\n " << root_
        << "| min key: " << minimum() << "| max key: " << maximum() << "| <null> Null:\\n " << nullptr << "}\"];" << std::endl;
    }

    void nullptrdump(std::fstream& file) const
    {
        file << "nullptr" << "[fillcolor=navy, label = \"{Null node | ptr:\\n " << nullptr << "| {min:\\n " << min_ <<
        "| max:\\n " << max_ << "}}\"];" << std::endl;
    }
    
    void tree_dump(std::fstream& file) const
    {
        if (empty())
            return;

        nullptrdump(file);

        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
            itr.base()->dump(file);

        file << "edge[penwidth=3, color=black];" << std::endl;
        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
        {
            node_ptr node = itr.base();
            if (node->left_ != nullptr)
                file << "Node_" << node << ":left:s -> Node_" << node->left_ << ":_node_:n;" << std::endl;
            if (node->right_ != nullptr)
                file << "Node_" << node << ":right:s -> Node_" << node->right_ << ":_node_:n;" << std::endl;
        }

        file << "Tree:root:e -> Node_" << root_ << ":_node_:n;" << std::endl;
        file << "Tree:null:w -> nullptr:n;" << std::endl;
    }
//----------------------------------------=| Graph dump end |=------------------------------------------

//----------------------------------------=| equal_to start |=------------------------------------------
protected:
    bool equal_to(const ISearchTree& other) const
    {
        if (size() != other.size())
            return false;
        
        for (auto itr = cbegin(), other_itr = other.cbegin(), end = cend(); itr != end; ++itr, ++other_itr)
            if (!key_equal(*itr, *other_itr))
                return false;
        return true;
    }
//----------------------------------------=| equal_to end |=--------------------------------------------
}; // class ISearchTree

} // namespace Container