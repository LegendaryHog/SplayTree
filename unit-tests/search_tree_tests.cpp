#include <gtest/gtest.h>
#include "search_tree.hpp"

using namespace Container;

TEST(SearchTree, default_constructor)
{
    SearchTree<int> def_tree {};
}

TEST(SearchTree, insert_n_iterators)
{
    SearchTree<int> tree {};
    auto ret0 = tree.insert(0);
    EXPECT_TRUE(ret0.second);
    EXPECT_EQ(*ret0.first, 0);
    EXPECT_EQ(tree.minimum(), 0);
    EXPECT_EQ(tree.maximum(), 0);

    auto ret1 = tree.insert(1);
    EXPECT_TRUE(ret1.second);
    EXPECT_EQ(*ret1.first, 1);
    EXPECT_EQ(tree.minimum(), 0);
    EXPECT_EQ(tree.maximum(), 1);

    auto ret01 = tree.insert(0);
    EXPECT_FALSE(ret01.second);
    EXPECT_EQ(*ret01.first, *ret0.first);
    EXPECT_EQ(tree.minimum(), 0);
    EXPECT_EQ(tree.maximum(), 1);

    auto ret2 = tree.insert(2);
    EXPECT_TRUE(ret2.second);
    EXPECT_EQ(*ret2.first, 2);
    EXPECT_EQ(tree.minimum(), 0);
    EXPECT_EQ(tree.maximum(), 2);

    std::vector<int> vec {0, 1, 2};
    int i = 0;
    for (auto x: tree)
        EXPECT_EQ(x, vec[i++]);
}

TEST(SearchTree, ctors_n_iterators_n_equlity)
{
    std::vector<int> vec  {-1, 0, 1, 3, 5, 7, 8};
    SearchTree<int> tree0 (vec.cbegin(), vec.cend());
    SearchTree<int> tree1 {8, 7, 0, 1, 5, 3, -1};

    EXPECT_EQ(tree0, tree1);
    EXPECT_EQ(tree1, tree0);
    EXPECT_EQ(tree1, tree1);

    int i = 0;
    for (auto x: tree0)
        EXPECT_EQ(x, vec[i++]);

    SearchTree<int> tree3 {-1, -1, 0, 3, 0, 5, 7, 8, 5, -1, 1};
    EXPECT_EQ(tree3, tree0);
    EXPECT_EQ(tree0, tree3);
    
    std::vector<int> vec1 {-1, -1, 0, 3, 0, 5, 7, 8, 5, -1, 1};
    SearchTree<int> tree4 (vec1.cbegin(), vec1.cend());

    EXPECT_EQ(tree4, tree0);
    EXPECT_EQ(tree0, tree4);
}

TEST(SearchTree, big_five)
{
    SearchTree<int> origin {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SearchTree<int> cpy1 {origin};
    EXPECT_EQ(origin, cpy1);
    EXPECT_EQ(origin.minimum(), cpy1.minimum());
    EXPECT_EQ(origin.maximum(), cpy1.maximum());

    SearchTree<int> cpy2 {1, 2, 3, 4};
    cpy2 = origin;
    EXPECT_EQ(cpy2, origin);
    EXPECT_EQ(origin.minimum(), cpy2.minimum());
    EXPECT_EQ(origin.maximum(), cpy2.maximum());

    SearchTree<int> move1 {std::move(cpy1)};
    EXPECT_EQ(move1, origin);
    EXPECT_EQ(origin.minimum(), move1.minimum());
    EXPECT_EQ(origin.maximum(), move1.maximum());

    SearchTree<int> move2 {1, 2, 4};
    move2 = std::move(cpy2);
    EXPECT_EQ(move2, origin);
    EXPECT_EQ(origin.minimum(), move2.minimum());
    EXPECT_EQ(origin.maximum(), move2.maximum());
}

TEST(SearchTree, Iterators)
{
    static_assert(std::bidirectional_iterator<SearchTree<int>::Iterator>);
    static_assert(std::bidirectional_iterator<SearchTree<int>::ConstIterator>);

    std::array<int, 8> arr {-2, 1, 2, 4, 5, 6, 7, 9};

    SearchTree<int> set1 {1, 2, 4, 7, 9, -2, 5, 6};
    std::size_t i = 0;
    for (auto x: set1)
        EXPECT_EQ(x, arr[i++]);

    const SearchTree<int> cset {1, 2, 4, 7, 9, -2, 5, 6};
    i = 0;
    for (auto x: cset)
        EXPECT_EQ(x, arr[i++]);

    SearchTree<int> set {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    EXPECT_EQ(11, *std::prev(set.end()));
    EXPECT_EQ(11, *std::prev(set.cend()));

    auto itr  = set.begin();
    auto citr = set.cbegin();
    EXPECT_EQ(*itr++, 0);
    EXPECT_EQ(*citr++, 0);
    EXPECT_EQ(*itr, 1);
    EXPECT_EQ(*citr, 1);
    EXPECT_EQ(*++itr, 2);
    EXPECT_EQ(*++citr, 2);
    EXPECT_EQ(*itr--, 2);
    EXPECT_EQ(*citr--, 2);
    EXPECT_EQ(*itr, 1);
    EXPECT_EQ(*citr, 1);
    EXPECT_EQ(*--itr, 0);
    EXPECT_EQ(*--citr, 0);

    EXPECT_EQ(std::next(set.begin(), 5), std::prev(set.end(), 7));
    EXPECT_EQ(std::distance(set.begin(), set.end()), set.size());
}

TEST(SearchTree, find)
{
    SearchTree<int> tree {1, 3, 5, 8, 9, 10, 14};
    EXPECT_EQ(*tree.find(1), 1);
    EXPECT_EQ(*tree.find(3), 3);
    EXPECT_EQ(*tree.find(5), 5);
    EXPECT_EQ(*tree.find(8), 8);
    EXPECT_EQ(*tree.find(9), 9);
    EXPECT_EQ(*tree.find(10), 10);
    EXPECT_EQ(*tree.find(14), 14);

    EXPECT_EQ(tree.find(13), tree.end());
    EXPECT_EQ(tree.find(0), tree.end());
}

TEST(SearchTree, lower_n_upper_bounds)
{
    SearchTree<int> set {-5, -4, -3, 6, 8, 9, 10, 11, 15, 17};

    EXPECT_EQ(*set.upper_bound(-4), -3);
    EXPECT_EQ(*set.upper_bound(6), 8);
    EXPECT_EQ(*set.upper_bound(7), 8);
    EXPECT_EQ(*set.upper_bound(13), 15);
    EXPECT_EQ(*set.upper_bound(15), 17);

    EXPECT_EQ(*set.lower_bound(-5), -5);
    EXPECT_EQ(*set.lower_bound(0), 6);
    EXPECT_EQ(*set.lower_bound(-4), -4);
    EXPECT_EQ(*set.lower_bound(6), 6);
    EXPECT_EQ(*set.lower_bound(7), 8);
}

TEST(SearchTree, erase)
{
    SearchTree<int> set {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

    auto er1 = set.erase(1);
    EXPECT_EQ(*er1, 2);
    EXPECT_EQ(*set.erase(er1), 3);

    auto citr = set.cbegin();
    EXPECT_EQ(*set.erase(citr), 3);

    SearchTree<int> set1 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    auto itr1 = std::next(set1.begin(), 6);
    auto itr2 = std::next(set1.begin(), 12);

    EXPECT_EQ(*set1.erase(itr1, itr2), 12);

    SearchTree<int> set2 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    auto citr1 = std::next(set2.cbegin(), 6);
    auto citr2 = std::next(set2.cbegin(), 12);

    EXPECT_EQ(*set2.erase(citr1, citr2), 12);
}
