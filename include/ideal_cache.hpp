#pragma once

#include <iterator>
#include <list>
#include <unordered_map>
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>


int slow_get_page_int(int key);


namespace ideal_caches {

template <typename T, typename KeyT = int>
class ideal_cache_t
{
    size_t sz_;
    size_t data_num_;
    size_t elem_amount_ = 0;

    struct data_node_t
    {
        KeyT key;
        T    data;
        int  pos;
    };

    using cache_it = typename std::map<int, data_node_t, std::greater<size_t>>::iterator;
    std::vector<T>                                   data_arr_;
    std::unordered_multimap<KeyT, data_node_t>       hash_;
    std::map<int, data_node_t, std::greater<size_t>> cache_;

    bool full() const { return (elem_amount_ == sz_+1); }

    void print_cache() const
    {
        std::cout << "elem_num=" << elem_amount_ << "\n";
        std::cout << "cache: \n";
        for (auto it = cache_.begin(); it != cache_.end(); it++)
            std::cout << it->second.key << "-" << it->second.pos << "-nh=" << it->first << "\n";

        std::cout << "\n\n";
    }

    void print_hash() const
    {
        for ( unsigned i = 0; i < hash_.bucket_count(); ++i)
        {
            std::cout << i << " contains:";
            for ( auto local_it = hash_.begin(i); local_it!= hash_.end(i); ++local_it )
                std::cout << " " << local_it->first << ":" << local_it->second.pos;
            std::cout << std::endl;
        }
        std::cout << "\n\n";
    }


    bool hit_handle(auto next_hit, auto elem, int index)
    {
        if (next_hit != hash_.end())
        {
            cache_.erase(index);
            cache_.insert({next_hit->second.pos, {elem, next_hit->second.data, index}});
            return true;
        }
        cache_.erase(index);
        elem_amount_--;
        return true;
    }


    void delete_latest_elem()
    {
        cache_.erase(cache_.begin());
        elem_amount_--;
    }


    public:
    ideal_cache_t(size_t sz, size_t data_num) : sz_(sz), data_num_(data_num) {}


    template <typename F>
    bool lookup_update(int index, F get_slow_page)
    {
        T elem = data_arr_[index];

        auto hit = cache_.find(index);
        if (hit == cache_.end())
        {
            if (hash_.find(elem) == hash_.end())
                return false;

            hash_.erase(hash_.find(elem));
            auto next_hit = hash_.find(elem);

            if (next_hit == hash_.end())
                return false;

            cache_.insert({next_hit->second.pos, {elem, next_hit->second.data, index}});
            elem_amount_++;

            if (full()) delete_latest_elem();

            return false;
        }

        hash_.erase(hash_.find(elem));
        auto next_hit = hash_.find(elem);

        return hit_handle(next_hit, elem, index);
    }


    bool get_data(std::vector<T> &arr, int elem_num)
    {
        if (elem_num != (int) data_num_) return false;
        data_arr_.reserve(data_num_);

        for (int i = 0; i < (int) data_num_; i++)
            data_arr_[i] = arr[i];

        return true;
    }

    template <typename F>
    void ideal_data_init(F get_slow_page)
    {
        for (int i = data_num_-1; i > -1; i--)
        {
            T elem = data_arr_[i];
            hash_.insert({elem, {elem, get_slow_page(elem), i}});
        }
    }
};

}