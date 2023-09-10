#pragma once

#include <iterator>
#include <list>
#include <unordered_map>
#include <iostream>

#define ERR_TEST(str)                 \
{                                      \
    printf(str);                        \
    printf(", Found -%c-\n", buff[pos]); \
    delete[] buff;                        \
    return 1;                              \
}

int slow_get_page_int(int key);

namespace caches {

template <typename T, typename KeyT = int>
struct cache_t
{
    size_t sz_;
    size_t min_fr_;
    std::list<std::tuple<KeyT, T, size_t>> cache_;

    using ListIt = typename std::list<std::tuple<KeyT, T, size_t>>::iterator;
    std::unordered_map<KeyT, ListIt> hash_;

    cache_t(size_t sz) : sz_(sz), min_fr_(1) {}

    bool full() const { return (cache_.size() == sz_); }

    void find_min_fr(ListIt beg, ListIt end)
    {
        min_fr_ = std::get<2>(*beg);
        for (ListIt it = ++beg; it != end; it++)
            if (std::get<2>(*it) < min_fr_) min_fr_ = std::get<2>(*it);
    }

    void print_list(ListIt beg, ListIt end)
    {
        for (ListIt it = beg; it != end; it++)
            printf("%d-%lu ", std::get<1>(*it), std::get<2>(*it));
        printf("\n");
    }

    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page)
    {
        //print_list(cache_.begin(), cache_.end());
        auto hit = hash_.find(key);
        if (hit == hash_.end())
        {
            if (full())
            {
                ListIt it = --cache_.end();
                for (; it != --cache_.begin(); it--)
                {
                    if (std::get<2>(*it) == min_fr_) break;
                }

                hash_.erase(std::get<0>(*it));
                cache_.erase(it);
            }
            cache_.emplace_front(key, slow_get_page(key), 1);
            hash_.emplace(key, cache_.begin());
            find_min_fr(cache_.begin(), cache_.end());

            //print_list(cache_.begin(), cache_.end());
            //printf("\n");

            return false;
        }

        auto eltit = hit->second;
        std::get<2>(*eltit)++;
        find_min_fr(cache_.begin(), cache_.end());

        //print_list(cache_.begin(), cache_.end());
        //printf("\n");

        return true;
    }


    std::list<std::pair<KeyT, T>> ideal_cache_;
    using IdListIt = typename std::list<std::pair<KeyT, T>>::iterator;
    std::unordered_map<KeyT, IdListIt> ideal_hash_;

    bool id_full() const { return (ideal_cache_.size() == sz_+1); }

    void ideal_print_list(IdListIt beg, IdListIt end)
    {
        for (IdListIt it = beg; it != end; it++)
            printf("-%d-", std::get<1>(*it));
        printf("\n");
    }

    template <typename F>
    bool ideal_lookup_update(KeyT key, F slow_get_page, T* arr, size_t len, int pos)
    {
        //ideal_print_list(ideal_cache_.begin(), ideal_cache_.end());
        auto hit = ideal_hash_.find(key);
        if (hit == ideal_hash_.end())
        {
            ideal_cache_.emplace_front(key, slow_get_page(key));
            ideal_hash_.emplace(key, ideal_cache_.begin());

            //ideal_print_list(ideal_cache_.begin(), ideal_cache_.end());

            if (id_full())
            {
                //printf("full\n");
                IdListIt it = ideal_cache_.begin();
                IdListIt last_elem = it;
                int last_pos = pos;

                for (; it != ideal_cache_.end(); it++)
                {
                    int flag = 0;
                    for (int i = pos; i < (int)len; i++)
                    {
                        if (std::get<1>(*it) == arr[i])
                        {
                            flag++;
                            if (i > last_pos)
                            {
                                last_elem = it;
                                last_pos = i;
                            }
                            break;
                        }
                    }
                    if (!flag)
                    {
                        last_pos = (int)len;
                        last_elem = it;
                        //printf("no elem in the future - %d\n", std::get<1>(*it));
                        break;
                    }
                }

                ideal_hash_.erase(std::get<0>(*last_elem));
                ideal_cache_.erase(last_elem);
                //printf("\n");
                return false;
            }

            //printf("\n");
            return false;
        }

        //printf("hit\n");
        //ideal_print_list(ideal_cache_.begin(), ideal_cache_.end());
        //printf("\n");

        return true;
    }
};

}