#include "ideal.hpp"

int slow_get_page_int(int key) { return key; }

int main()
{
    int cache_l = 0, elem_num = 0;
    if(!(std::cin >> cache_l && std::cin >> elem_num))
    {
        std::cerr << "ERROR in getting cache length and elements number\n";
        return 1;
    }

    if (cache_l < 0 || elem_num < 0)
    {
        std::cerr << "ERROR in getting cache length and elements number. They can't be below zero\n";
        return 1;
    }

    caches::ideal_cache_t<int> csh{cache_l};
    std::vector<int> data_arr;
    data_arr.reserve(elem_num);

    for (int i = 0; i < elem_num; i++)
        std::cin >> data_arr[i];

    csh.get_data(data_arr, slow_get_page_int);

    int hit_cnt = 0;

    const std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < elem_num; i++)
        if (csh.lookup_update(i, data_arr[i])) hit_cnt++;

    const auto end = std::chrono::high_resolution_clock::now();

    const auto mk_s = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "time in microseconds - " << mk_s << std::endl;

    std::cout << "answer - " << hit_cnt;

    return 0;
}