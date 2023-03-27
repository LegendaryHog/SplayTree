#pragma once
#include "boost_node.hpp"
#include "rbsearch_tree.hpp"

namespace Container
{
template<typename KeyT = int, class Cmp = std::less<KeyT>>
class BoostSet final : public detail::RBSearchTree<KeyT, Cmp, detail::RBSubTreeRoot<KeyT>>
{
    using base = detail::RBSearchTree<KeyT, Cmp, detail::RBSubTreeRoot<KeyT>>;
    using typename base::node_ptr;
    using typename base::node_type;
    using typename base::key_type;
    using typename base::size_type;
    using typename base::Iterator;
    using typename base::ConstIterator;
    
    using base::Null_;
    using base::root_;
    using base::size_;

public:
    BoostSet(): base::RBSearchTree() {}
    
    template<std::input_iterator InpIt>
    BoostSet(InpIt first, InpIt last): base::RBSearchTree(first, last) {}

    BoostSet(std::initializer_list<key_type> initlist): base::RBSearchTree(initlist) {}

    using base::size;
    using base::empty;
    using base::maximum;
    using base::minimum;
    using base::find;
    using base::insert;
    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;
    using base::lower_bound;
    using base::upper_bound;
    using base::debug_graph_dump;
    using base::equal_to;

    const key_type& kth_smallest(size_type index) const noexcept
    {
        node_ptr current = root_;
        while (current->left_->size_ != index)
            if (current->left_ != Null_ && index < current->left_->size_)
                current = current->left_;
            else
            {
                index  -= current->left_->size_ + 1;
                current = current->right_;
            }
        return current->key_;
    }
private:
    using base::key_less;

public:
    const key_type& operator[](size_type index) const noexcept
    {
        return kth_smallest(index);
    }

    size_type number_less_than(const key_type& key) const noexcept
    {
        size_type number = 0;
        node_ptr current = root_;
        while (current != Null_)
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
        while (current != Null_)
            if (key_less(current->key_, key) || key_equal(current->key_, key))
            {
                number += current->left_->size_ + 1;
                current = current->right_;
            }
            else
                current = current->left_;
        return number;
    }
};

template<typename KeyT, class Cmp>
bool operator==(const BoostSet<KeyT, Cmp>& lhs, const BoostSet<KeyT, Cmp>& rhs)
{
    return lhs.equal_to(rhs);
}
/*\
    Public enemy number one
    Jailbreak and a smoking gun
    You won't believe the things I've done
    And the killing is just for fun

    Public enemy number one
    A stolen car and I'm on the run
    Through the night 'til the rising sun
    And the trouble has just begun

    Roses on your grave
    I'll be on my way
    There's no time to stay
    With the enemies I've made

    I'm invincible
    You might say despicable
    Punishment's reciprocal
    Public enemy number one

    I'm unbeatable
    My mind is untreatable
    Crimes unrepeatable
    Public enemy number one
\*/
template<typename KeyT, class Cmp = std::less<KeyT>>
std::size_t operator->*(const BoostSet<KeyT, Cmp>& set, const KeyT& key)
{
    return set.number_less_than(key);
}

} // namespace Container