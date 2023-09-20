#include "../include/ideal_cache.hpp"

int slow_get_page_int(int key) { return key; }

int main()
{
    int cache_l = 0, elem_num = 0;
    if(!(std::cin >> cache_l && std::cin >> elem_num))
    {
        std::cout << "ERROR in getting cache length and elements number\n";
        return 1;
    }

    if (cache_l < 0 || elem_num < 0)
    {
        std::cout << "ERROR in getting cache length and elements number. They can't be below zero\n";
        return 1;
    }

    ideal_caches::ideal_cache_t <int> csh{(size_t)cache_l, (size_t)elem_num};

    csh.get_data();
    csh.ideal_data_init(slow_get_page_int);

    int hit_cnt = 0;
    for (int i = 0; i < elem_num; i++)
        if (csh.lookup_update(i, slow_get_page_int)) hit_cnt++;

    std::cout << hit_cnt;

    return 0;
}