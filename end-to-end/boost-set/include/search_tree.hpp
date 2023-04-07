#pragma once
#include <functional>
#include <fstream>
#include <string>
#include "node.hpp"
#include "search_tree_iterator.hpp"

namespace Container
{

namespace detail
{
template<typename KeyT, class Cmp, typename Node>  
class RBSearchTree
{
protected:
    using node_type      = Node;
    using node_ptr       = Node*;
    using const_node_ptr = const Node*;
    using key_type       = KeyT;
    using size_type      = typename std::size_t;
    using ConstIterator = SearchTreeIterator<key_type, Cmp, node_type>;
    using Iterator = ConstIterator;

private:
    static node_ptr null_init()
    {
        auto Null = new node_type{key_type{}, Colors::Black};
        Null->left_   = Null;
        Null->right_  = Null;
        Null->parent_ = Null;
        return Null;
    }

protected:
    node_ptr  Null_ = null_init(); // all of nullptr is replaced on Null_, for minimalize checking
    node_ptr  root_ = Null_;
    size_type size_ = 0;

    Cmp cmp {};

    bool key_less(const key_type& key1, const key_type& key2) const {return cmp(key1, key2);}
    bool key_equal(const key_type& key1, const key_type& key2) const
    {
        return !cmp(key1, key2) && !cmp(key2, key1);
    }

//----------------------------------------=| Ctors start |=---------------------------------------------
    RBSearchTree() = default;

    template<std::input_iterator InpIt>
    RBSearchTree(InpIt first, InpIt last)
    {   
        RBSearchTree tmp {};
        tmp.insert(first, last);
        swap(tmp);
    }

    RBSearchTree(std::initializer_list<key_type> initlist)
    :RBSearchTree(initlist.begin(), initlist.end())
    {}
//----------------------------------------=| Ctors end |=-----------------------------------------------

//----------------------------------------=| Size`s funcs start |=--------------------------------------
    size_type size() const noexcept {return size_;}

    bool empty() const noexcept {return (size_ == 0);} 
//----------------------------------------=| Size`s funcs end |=----------------------------------------

//----------------------------------------=| Max/min methods start |=-----------------------------------
    const key_type& maximum() const noexcept {return Null_->right_->key_;}
    const key_type& minimum() const noexcept {return Null_->left_->key_;}
//----------------------------------------=| Max/min methods end |=-------------------------------------

//----------------------------------------=| Big five start |=------------------------------------------
private:
    void swap(RBSearchTree& rhs) noexcept
    {
        std::swap(Null_, rhs.Null_);
        std::swap(root_, rhs.root_);
        std::swap(size_, rhs.size_);
    }

protected:
    RBSearchTree(RBSearchTree&& other) noexcept
    {
        swap(other);
    }
    RBSearchTree& operator=(RBSearchTree&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }

    RBSearchTree(const RBSearchTree& other): size_ {other.size_}
    {
        if (empty())
            return;

        RBSearchTree tmp {std::move(*this)};

        tmp.root_ = new node_type;
        tmp.root_->init_data(other.root_, tmp.Null_);
        auto tmp_current   = tmp.root_;
        auto other_current = other.root_;

        while (other_current != other.Null_)
            if (other_current->left_ != other.Null_ && tmp_current->left_ == tmp.Null_)
            {
                tmp_current->copy_left(new node_type, other_current->left_, tmp.Null_);
                other_current = other_current->left_;
                tmp_current   = tmp_current->left_;
            }
            else if (other_current->right_ != other.Null_ && tmp_current->right_ == tmp.Null_)
            {
                tmp_current->copy_right(new node_type, other_current->right_, tmp.Null_);
                other_current = other_current->right_;
                tmp_current   = tmp_current->right_;
            }
            else
            {
                other_current = other_current->parent_;
                tmp_current   = tmp_current->parent_;
            }

        swap(tmp);
        
        Null_->left_  = detail::find_min(root_, Null_);
        Null_->right_ = detail::find_max(root_, Null_);
    }

    RBSearchTree& operator=(const RBSearchTree& rhs)
    {
        auto rhs_cpy {rhs};
        swap(rhs_cpy);
        return *this;
    }

    ~RBSearchTree()
    {
        if (empty())
        {
            delete Null_;
            return;
        }

        auto current = root_;
        while (current != Null_)
            if (current->left_ != Null_)
                current = current->left_;
            else if (current->right_ != Null_)
                current = current->right_;
            else
            {
                auto parent = current->parent_;

                if (current == root_)
                    break;
                else if (current->is_left_son())
                    parent->left_ = Null_;
                else
                    parent->right_ = Null_;

                delete current;
                current = parent;
            }
        delete root_;
        delete Null_;
    }
//----------------------------------------=| Big five end |=--------------------------------------------

//----------------------------------------=| Algorithm funcs start |=-----------------------------------
private:
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
    void left_rotate(node_ptr x) noexcept
    {
        // declare y as right son of x
        node_ptr y = x->right_;
        // new right son of x is yl 
        x->right_ = y->left_;

        // if yl exist (not nullptr)
        // replace parent of yl from y to x
        if (y->left_ != Null_)      
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
        y->action_after_left_rotate(Null_);
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
    void right_rotate(node_ptr x) noexcept
    {
        node_ptr y = x->left_;
        x->left_ = y->right_;

        if (y->right_ != Null_)
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
        y->action_after_right_rotate(Null_);
    }
//----------------------------------------=| Algorithm funcs end |=-------------------------------------

//----------------------------------------=| begin/end start |=-----------------------------------------
protected:
    ConstIterator begin() const noexcept {return ConstIterator{Null_->left_, Null_};}
    ConstIterator end()   const noexcept {return ConstIterator{Null_, Null_};}

    ConstIterator cbegin() const noexcept {return ConstIterator{Null_->left_, Null_};}
    ConstIterator cend()   const noexcept {return ConstIterator{Null_, Null_};}
//----------------------------------------=| begin/end end |=-------------------------------------------

//----------------------------------------=| Find start |=----------------------------------------------
    ConstIterator find(const key_type& key) const
    {
        node_ptr node = root_;
        while (node != Null_)
            if (key_less(key, node->key_))
                node = node->left_;
            else if (key_less(node->key_, key))
                node = node->right_;
            else
                return ConstIterator{node, Null_};
        return end();
    }
//----------------------------------------=| Find end |=------------------------------------------------

//----------------------------------------=| Insert start |=--------------------------------------------
private:
    node_ptr find_parent(const key_type& key) const
    {
        auto x = root_;
        auto y = Null_;

        while (x != Null_)
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

    void insert_by_ptr(node_ptr node)
    {
        // increment size
        size_++;

        // if tree is empty
        if (node->parent_ == Null_)
        {
            // root is node
            root_ = node;
            root_->color_ = Colors::Black;
            // upadte min and max
            Null_->left_  = root_;
            Null_->right_ = root_;
            return;
        }
        
        // insert new node in right place
        if (key_less(node->key_, node->parent_->key_))
            node->parent_->left_ = node;
        else
            node->parent_->right_ = node;
        // fix red-black properties
        rb_insert_fix(node);
    }

protected:
    std::pair<ConstIterator, bool> insert(const key_type& key)
    {
        key_type key_cpy {key};
        return insert(std::move(key_cpy));
    }

    std::pair<ConstIterator, bool> insert(key_type&& key)
    {
        auto parent = find_parent(key);

        // if key is alredy in tree
        if (parent != Null_ && key_equal(parent->key_, key))
            return std::pair{ConstIterator{parent, Null_}, false};

        node_ptr new_node = new node_type{std::move(key), Colors::Red, parent, Null_, Null_};
        new_node->action_before_insert(Null_);
        insert_by_ptr(new_node);
        return std::pair{ConstIterator{new_node, Null_}, true};
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
    void insert_fix_min_max(node_ptr node)
    {
        if (key_less(Null_->right_->key_, node->key_))
            Null_->right_ = node;
        if (key_less(node->key_, Null_->left_->key_))
            Null_->left_ = node;
    }

    void rb_insert_fix(node_ptr node)
    {
        // fix min and max pointers
        insert_fix_min_max(node);
        // if parent color is Black of all invariants holds
        while (node->parent_->color_ == Colors::Red)
            // if parent is right son of grandparent
            if (node->parent_ == node->parent_->parent_->left_)
            {
                // uncle declare
                node_ptr uncle = node->parent_->parent_->right_;
                // Case 1
                if (uncle->color_ == Colors::Red)
                {
                    /*\___________________________________________________
                    |*                                                    |
                    |*      (Black)                          (Red)        |
                    |*      gr_par                           gr_par       |
                    |*       / \                              / \         |
                    |*      /   \      after iteartion       /   \        |
                    |*   (Red) (Red)  --------------->  (Black) (Black)   |
                    |*   parent uncle                   parent   uncle    |             
                    |*    / \   / \                       / \     / \     |
                    |*   /                               /                |
                    |* (Red)                           (Red)              |
                    |* node                            node               |
                    |*____________________________________________________|
                    \*/
                    // Comment to picture: dont matter which son of parent is node (right or left)
                    node->parent_->color_          = Colors::Black;
                    uncle->color_                  = Colors::Black;
                    node->parent_->parent_->color_ = Colors::Red;
                    // new node for new iteration of cycle
                    node = node->parent_->parent_;
                    /*\
                    |* we fix invariant that told us "son of red node is black node"
                    |* but we broke invariant that told us "for all nodes simple ways from node to leaf,
                    |* that are descendants of this node, are equal", to fix that we start new iteration with new node = gr_par
                    |* but we can change color of root from balck to red, we need to fix this invarinat in the end
                    \*/
                }
                // Case 2 and Case 3
                else
                {
                    // Case 2
                    /*\__________________________________________________
                    |*                      ______                       |
                    |*       gr_par(Black) |rotate|       gr_par(Black)  |
                    |*          /   \      | left |         /    \       |
                    |*         /   delta   |around|        /    delta    |
                    |*    par(Red)         |parent|     node(Red)        |
                    |*       / \         ----------->   /  \             |
                    |*   alpha  \                      /    gamma        |
                    |*         node(Red)            par(Red)             |
                    |*          /  \                 /  \                |
                    |*       beta  gamma          alpha beta             |
                    |* __________________________________________________|
                    \*/
                    if (node == node->parent_->right_)
                    {
                        // node is pointer on parent
                        node = node->parent_;
                        // after left rotate node is par from pictre
                        left_rotate(node);
                    }
                    // Case 3
                    /*\___________________________________________________________________
                    |*                                 ______                             |
                    |*         gr_par(Black -> Red)   |rotate|         par(Black)         |
                    |*            /    \              |right |           /   \            |
                    |*           /    delta           |around|          /     \           |
                    |*       par(Red -> Black)        |gr_par|    node(Red)  gr_par(Red)  |
                    |*        /  \                  ------------>   /  \        /  \      |
                    |*       /    gamma                          alpha beta  gamma delta  |
                    |*   node(Red)                                                        |
                    |*     /  \                                                           |
                    |*  alpha beta                                                        |
                    |* ___________________________________________________________________|
                    \*/
                    node->parent_->color_          = Colors::Black;
                    node->parent_->parent_->color_ = Colors::Red;
                    right_rotate(node->parent_->parent_);
                    /*
                     * After this case tree will be finally fixed and
                     * cycle finished because parent color is Black
                     */
                }
            }
            // symmetrical about the replacement "right" on "left" in "if" part
            else
            {
                node_ptr uncle = node->parent_->parent_->left_;
                if (uncle->color_ == Colors::Red)
                {
                    node->parent_->color_          = Colors::Black;
                    uncle->color_                  = Colors::Black;
                    node->parent_->parent_->color_ = Colors::Red;
                    node = node->parent_->parent_;
                }
                else
                {
                    if (node == node->parent_->left_)
                    {
                        node = node->parent_;
                        right_rotate(node);
                    }
                    node->parent_->color_ = Colors::Black;
                    node->parent_->parent_->color_ = Colors::Red;
                    left_rotate(node->parent_->parent_);
                }
            }
        // fix invariont that is "root is black"
        root_->color_ = Colors::Black;
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
        v->parent_ = u->parent_;
    }

    // delete z from tree with saving all invariants
    void erase_from_tree(node_ptr z) noexcept
    {
        // decrement size
        size_--;
        // declare two pointer, y - replacment for z in else case
        // x - root of subtree, where we need fix invarinats 
        node_ptr y = z, x = Null_;
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
        if (z->left_ == Null_)
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
        else if (z->right_ == Null_)
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
            // y most left of z->right than y->left_ == Null_
            y = detail::find_min(z->right_, Null_);
            // save original color of y node
            // (look at the end of method to see cause)
            y_original_color = y->color_;
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
            // to save invariant everywhere except in subtree with root x
            y->color_ = z->color_;
        }

        // in first two cases ("if" and "else if")
        // if z had the Red color, then we cant broke any invarinats
        // in third case, if y was Red we cant broke any invarints too
        if (y_original_color == Colors::Black)
            rb_delete_fixup(x);
        delete_fix_min_max(z);
    }

private:
    void delete_fix_min_max(node_ptr node) noexcept
    {
        if (node == Null_->left_)
            Null_->left_ = detail::find_min(root_, Null_);
        if (node == Null_->right_)
            Null_->right_ = detail::find_max(root_, Null_);
    }

    void rb_delete_fixup(node_ptr x) noexcept
    {
        while (x != root_ && x->color_ == Colors::Black)
            if (x->is_left_son())
            {
                node_ptr w = x->parent_->right_;
                if (w->color_ == Colors::Red)
                {
                    w->color_ = Colors::Black;
                    x->parent_->color_ = Colors::Red;
                    left_rotate(x->parent_);
                    w = x->parent_->right_;
                }
                if (w->left_->color_ == Colors::Black && w->right_->color_ == Colors::Black)
                {
                    w->color_ = Colors::Red;
                    x = x->parent_;
                }
                else
                {
                    if (w->right_->color_ == Colors::Black)
                    {
                        w->left_->color_ = Colors::Black;
                        w->color_ = Colors::Red;
                        right_rotate(w);
                        w = x->parent_->right_;
                    }
                    w->color_ = x->parent_->color_;
                    x->parent_->color_ = Colors::Black;
                    w->right_->color_ = Colors::Black;
                    left_rotate(x->parent_);
                    x = root_;
                }
            }
            else
            {
                node_ptr w = x->parent_->left_;
                if (w->color_ == Colors::Red)
                {
                    w->color_ = Colors::Black;
                    x->parent_->color_ = Colors::Red;
                    right_rotate(x->parent_);
                    w = x->parent_->left_;
                }
                if (w->right_->color_ == Colors::Black && w->left_->color_ == Colors::Black)
                {
                    w->color_ = Colors::Red;
                    x = x->parent_;
                }
                else
                {
                    if (w->left_->color_ == Colors::Black)
                    {
                        w->right_->color_ = Colors::Black;
                        w->color_ = Colors::Red;
                        left_rotate(w);
                        w = x->parent_->left_;
                    }
                    w->color_ = x->parent_->color_;
                    x->parent_->color_ = Colors::Black;
                    w->left_->color_ = Colors::Black;
                    right_rotate(x->parent_);
                    x = root_;
                }
            }
        x->color_ = Colors::Black;
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
        node_ptr result = Null_, current = root_;
        while (current != Null_)
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
        node_ptr result = Null_, current = root_;
        while (current != Null_)
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
    ConstIterator lower_bound(const key_type& key) const {return ConstIterator{lower_bound_ptr(key), Null_};}
    ConstIterator upper_bound(const key_type& key) const {return ConstIterator{upper_bound_ptr(key), Null_};}
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
        file << "\tTree [fillcolor=purple, label = \"{ RBSearchTree\\ndescriptor| size: " << size() << "| <root> root:\\n " << root_
        << "| min key: " << minimum() << "| max key: " << maximum() << "| <null> Null:\\n " << Null_ << "}\"];" << std::endl;
    }

    void null_dump(std::fstream& file) const
    {
        file << "Null_" << "[fillcolor=navy, label = \"{Null node | ptr:\\n " << Null_ << "| {min:\\n " << Null_->left_ <<
        "| max:\\n " << Null_->right_ << "}}\"];" << std::endl;
    }
    
    void tree_dump(std::fstream& file) const
    {
        if (empty())
            return;

        null_dump(file);

        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
            itr.base()->dump(file);

        file << "edge[penwidth=3, color=black];" << std::endl;
        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
        {
            node_ptr node = itr.base();
            if (node->left_ != Null_)
                file << "Node_" << node << ":left:s -> Node_" << node->left_ << ":_node_:n;" << std::endl;
            if (node->right_ != Null_)
                file << "Node_" << node << ":right:s -> Node_" << node->right_ << ":_node_:n;" << std::endl;
        }

        file << "Tree:root:e -> Node_" << root_ << ":_node_:n;" << std::endl;
        file << "Tree:null:w -> Null_:n;" << std::endl;
    }
//----------------------------------------=| Graph dump end |=------------------------------------------

//----------------------------------------=| equal_to start |=------------------------------------------
protected:
    bool equal_to(const RBSearchTree& other) const
    {
        if (size() != other.size())
            return false;
        
        for (auto itr = cbegin(), other_itr = other.cbegin(), end = cend(); itr != end; ++itr, ++other_itr)
            if (!key_equal(*itr, *other_itr))
                return false;
        return true;
    }
//----------------------------------------=| equal_to end |=--------------------------------------------
}; // class RBSearchTree
} // namespace detail

} // namespace Container