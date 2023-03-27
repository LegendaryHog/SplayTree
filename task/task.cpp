#include <iostream>
#include "splay_tree.hpp"

int main()
{
    Container::SplayTree<int> tree {};

    int num_of_keys = 0;
    std::cin >> num_of_keys;

    for (int i = 0; i < num_of_keys; i++)
    {
        int inp = 0;
        std::cin >> inp;
        tree.insert(inp);
    }
    
    int num_of_reqs = 0;
    std::cin >> num_of_reqs;
    for (int i = 0; i < num_of_reqs; i++)
    {
        int left_bound = 0, right_bound = 0;
        std::cin >> left_bound >> right_bound;
        std::cout << std::distance(tree.lower_bound(left_bound), tree.upper_bound(right_bound)) << ' ';
    }
    std::cout << std::endl;
    return 0;
}