#pragma once

#include <map>
#include <vector>
#include <iterator>
#include <iostream>
#include <unordered_map>
#include <chrono>

int slow_get_page_int(int key);

namespace caches {

template <typename T, typename KeyT = int>
class ideal_cache_t
{
    size_t sz_;
    size_t data_num_    = 0;
    size_t elem_amount_ = 0;

    struct data_node_t
    {
        KeyT key_;
        T    data_;
        int  pos_;
    };

    using cache_it = typename std::map<int, data_node_t, std::greater<size_t>>::iterator;

    std::vector<T>                                   data_arr_;
    std::unordered_multimap<KeyT, data_node_t>       hash_;
    std::map<int, data_node_t, std::greater<size_t>> cache_;


    bool full() const { return (elem_amount_ == sz_+1); }


    /* void print_cache() const
    {
        std::cout << "elem_num=" << elem_amount_ << "\n";
        std::cout << "cache: \n";

        std::copy(cache_.begin(), cache_.end(), std::ostream_iterator<int>());

        std::cout << "\n\n";
    } */


    /* void print_hash() const
    {
        for ( unsigned i = 0; i < hash_.bucket_count(); ++i)
        {
            std::cout << i << " contains:";
            for ( auto local_it = hash_.begin(i); local_it!= hash_.end(i); ++local_it )
                std::cout << " " << local_it->first << ":" << local_it->second.pos_;
            std::cout << std::endl;
        }
        std::cout << "\n\n";
    } */


    void delete_latest_elem()
    {
        cache_.erase(cache_.begin());
        elem_amount_--;
    }


    bool hit_handle(auto next_hit, int index)
    {
        if (next_hit != hash_.end())
        {
            cache_.insert({next_hit->second.pos_, cache_.find(index)->second});
            return true;
        }
        cache_.erase(index);
        elem_amount_--;
        return true;
    }


    public:

    ideal_cache_t(size_t sz) : sz_(sz) {}


    bool lookup_update(int index, T elem)
    {
        auto hit = cache_.find(index);
        if (hit == cache_.end())
        {
            if (hash_.find(elem) == hash_.end())
                return false;

            hash_.erase(hash_.find(elem));
            auto next_hit = hash_.find(elem);

            if (next_hit == hash_.end())
                return false;

            cache_.insert({next_hit->second.pos_, next_hit->second});
            elem_amount_++;

            if (full()) delete_latest_elem();

            return false;
        }

        hash_.erase(hash_.find(elem));
        auto next_hit = hash_.find(elem);

        return hit_handle(next_hit, index);
    }


    template <typename F>
    void get_data(std::vector<T> &arr, F slow_get_page)
    {
        T elem;
        int counter = arr.capacity() - 1;

        for (; counter > -1; counter--)
        {
            elem = arr[counter];
            hash_.insert({elem, {elem, slow_get_page(elem), counter}});
        }
    }
};

}
