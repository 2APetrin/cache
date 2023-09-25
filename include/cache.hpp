#pragma once

#include <iterator>
#include <list>
#include <unordered_map>
#include <iostream>


int slow_get_page_int(int key);


namespace caches {

template <typename T, typename KeyT = int>
class cache_t
{
    size_t sz_;
    size_t elem_amount_ = 0;

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


    struct frequency_list_t
    {
        std::list<freq_node_t> list;

        std::list<data_node_t> &first_data() { return list.begin()->data_list; }
        std::list<data_node_t> &last_data()  { return std::prev(list.end())->data_list; }

        freq_list_it         first_node_it() { return list.begin(); };
        freq_list_it         last_node_it()  { return std::prev(list.end()); }
    };

    frequency_list_t freq_list_;
    std::unordered_map<KeyT, data_list_it> hash_;

    bool full() const { return (elem_amount_ == sz_); }


    void print_list(freq_list_it beg, freq_list_it end) const
    {
        for (auto it = beg; it != end; it++)
        {
            std::cout << "\n" << it->freq << ") ";
            for (auto dit = it->data_list.begin(); dit != it->data_list.end(); dit++)
                std::cout << dit->data << "-";
        }
        std::cout << std::endl;
    }


    void delete_min_elem()
    {
        elem_amount_--;
        hash_.erase(freq_list_.first_data().begin()->key);
        freq_list_.first_data().erase(freq_list_.first_data().begin());

        if (freq_list_.first_data().empty())
            freq_list_.list.erase(freq_list_.first_node_it());
    }


    void push_new_elem(KeyT key, T data)
    {
        if (freq_list_.first_node_it()->freq != 1)
            freq_list_.list.emplace_front(1);

        elem_amount_++;
        freq_list_.first_data().emplace_back(key, data, freq_list_.first_node_it());
        hash_.emplace(key, std::prev(freq_list_.first_data().end()));
    }


    bool elem_to_next_frequency(data_list_it elem)
    {
        if (elem->freq_it == freq_list_.last_node_it())
        {
            freq_list_.list.emplace_back(elem->freq_it->freq + 1);
            elem->freq_it->data_list.splice(freq_list_.last_node_it()->data_list.begin(), freq_list_.last_node_it()->data_list, elem);
            elem->freq_it = std::next(elem->freq_it);

            return true;
        }

        if (elem->freq_it->freq + 1 != std::next(elem->freq_it)->freq)
            freq_list_.list.emplace(std::next(elem->freq_it), elem->freq_it->freq + 1);

        std::next(elem->freq_it)->data_list.splice(std::next(elem->freq_it)->data_list.end(), elem->freq_it->data_list, elem);
        elem->freq_it = std::next(elem->freq_it);

        if (freq_list_.first_node_it()->data_list.empty())
            freq_list_.list.erase(freq_list_.first_node_it());

        return true;
    }


    public:
    cache_t(size_t sz) : sz_(sz), freq_list_{} {}

    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page)
    {
        //print_list(freq_list_.list.begin(), freq_list_.list.end());
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