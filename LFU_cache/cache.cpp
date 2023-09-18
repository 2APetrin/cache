#include "../include/cache.hpp"

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

    caches::cache_t <int> csh{(size_t) cache_l};

    int* cache_buff = new int [elem_num];
    for (int i = 0; i < elem_num; i++)
        std::cin >> cache_buff[i];

    int hit_cnt = 0;
    for (int i = 0; i < elem_num; i++)
        if (csh.lookup_update(cache_buff[i], slow_get_page_int)) hit_cnt++;

    std::cout << hit_cnt;

    delete[] cache_buff;
    return 0;
}