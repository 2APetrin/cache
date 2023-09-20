#pragma once

#include <iterator>
#include <list>
#include <unordered_map>
#include <iostream>

// ---------------- DSL ----------------
#define FIRST_DATA_LIST frequency_list_.begin()->data_list
// ---------------- DSL ----------------


int slow_get_page_int(int key);


namespace caches {

template <typename T, typename KeyT = int>
class cache_t
{
    size_t sz_;
    size_t elem_amount_;

    struct freq_node_t;
    using  freq_list_it = typename std::list<freq_node_t>::iterator;

    struct data_node_t
    {
        KeyT         key;
        T            data;
        freq_list_it freq_it;
    };

    using data_list_it = typename std::list<data_node_t>::iterator;

    struct freq_node_t
    {
        size_t                 freq;
        std::list<data_node_t> data_list;
    };

    std::list<freq_node_t> frequency_list_;
    std::unordered_map<KeyT, data_list_it> hash_;

    bool full() const { return (elem_amount_ == sz_); }

    void print_list(freq_list_it beg, freq_list_it end)
    {
        for (auto it = beg; it != end; it++)
        {
            std::cout << "\n" << it->freq << ") ";
            for (auto dit = it->data_list.begin(); dit != it->data_list.end(); dit++)
                std::cout << dit->data << "-";
        }
        printf("\n");
    }

    void delete_min_elem()
    {
        elem_amount_--;
        hash_.erase(FIRST_DATA_LIST.begin()->key);
        FIRST_DATA_LIST.erase(FIRST_DATA_LIST.begin());

        if (FIRST_DATA_LIST.empty())
            frequency_list_.erase(frequency_list_.begin());
    }

    void push_new_elem(KeyT key, T data)
    {
        if (frequency_list_.begin()->freq != 1)
            frequency_list_.emplace_front(1);

        elem_amount_++;
        FIRST_DATA_LIST.emplace_back(key, data, frequency_list_.begin());
        hash_.emplace(key, std::prev(FIRST_DATA_LIST.end()));
    }

    bool elem_to_next_frequency(data_list_it elem)
    {
        if (elem->freq_it == std::prev(frequency_list_.end()))
        {
            frequency_list_.emplace_back(elem->freq_it->freq + 1);
            elem->freq_it->data_list.splice(std::prev(frequency_list_.end())->data_list.begin(), std::prev(frequency_list_.end())->data_list, elem);
            elem->freq_it = std::next(elem->freq_it);

            return true;
        }

        if (elem->freq_it->freq + 1 != std::next(elem->freq_it)->freq)
            frequency_list_.emplace(std::next(elem->freq_it), elem->freq_it->freq + 1);

        std::next(elem->freq_it)->data_list.splice(std::next(elem->freq_it)->data_list.end(), elem->freq_it->data_list, elem);
        elem->freq_it = std::next(elem->freq_it);

        if (FIRST_DATA_LIST.empty())
            frequency_list_.erase(frequency_list_.begin());

        return true;
    }

    public:
    cache_t(size_t sz) : sz_(sz), elem_amount_(0) {}

    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page)
    {
        //print_list(frequency_list_.begin(), frequency_list_.end());
        auto hit = hash_.find(key);

        if (hit == hash_.end())
        {
            if (full()) delete_min_elem();

            push_new_elem(key, slow_get_page(key));

            return false;
        }

        return elem_to_next_frequency(hit->second);
    }
};

}