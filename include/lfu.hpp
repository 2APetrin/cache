#pragma once

#include <list>
#include <chrono>
#include <iostream>
#include <iterator>
#include <unordered_map>


int slow_get_page_int(int key);


namespace caches {

template <typename T, typename KeyT = int>
class cache_t
{
    size_t sz_;
    size_t elem_amount_ = 0;

    struct freq_node_t;
    using  freq_it = typename std::list<freq_node_t>::iterator;

    struct data_node_t
    {
        KeyT    key_;
        T       data_;
        freq_it freq_it_;
    };

    using data_it = typename std::list<data_node_t>::iterator;

    struct freq_node_t
    {
        size_t                 freq_;
        std::list<data_node_t> data_list_;
    };

    struct frequency_list_t
    {
        std::list<freq_node_t> list;

        std::list<data_node_t> &first_data() { return list.begin()->data_list_;          }
        std::list<data_node_t> &last_data()  { return std::prev(list.end())->data_list_; }

        freq_it first_node_it() { return list.begin();          }
        freq_it last_node_it()  { return std::prev(list.end()); }
        data_it min_elem()      { return first_data().begin();  }
    } freq_list_;

    std::unordered_map<KeyT, data_it> hash_;



    bool full() const { return (elem_amount_ == sz_); }


    void delete_min_elem()
    {
        elem_amount_--;
        hash_.erase(freq_list_.min_elem()->key_);
        freq_list_.first_data().erase(freq_list_.min_elem());

        if (freq_list_.first_data().empty())
            freq_list_.list.erase(freq_list_.first_node_it());
    }


    void push_new_elem(KeyT key, T data)
    {
        if (freq_list_.first_node_it()->freq_ != 1)
            freq_list_.list.emplace_front(1);

        elem_amount_++;

        freq_it first_freq_node = freq_list_.first_node_it();
        freq_list_.first_data().emplace_back(key, data, first_freq_node);

        data_it first_data_last_element = std::prev(freq_list_.first_data().end());
        hash_.emplace(key, first_data_last_element);
    }


    bool elem_to_next_frequency(data_it elem)
    {
        if (elem->freq_it_ == freq_list_.last_node_it())
        {
            freq_list_.list.emplace_back(get_freq(elem) + 1);

            elem->freq_it_->data_list_.splice(freq_list_.last_data().begin(), freq_list_.last_data(), elem);
            elem->freq_it_ = std::next(elem->freq_it_);

            return true;
        }

        if (get_freq(elem)+ 1 != std::next(elem->freq_it_)->freq_)
            freq_list_.list.emplace(std::next(elem->freq_it_), get_freq(elem)+ 1);

        const freq_it          &next_freq_it   = std::next(elem->freq_it_);
        std::list<data_node_t> &next_freq_data = next_freq_it->data_list_;

        next_freq_data.splice(next_freq_data.end(), elem->freq_it_->data_list_, elem);

        elem->freq_it_ = next_freq_it;

        if (freq_list_.first_data().empty())
            freq_list_.list.erase(freq_list_.first_node_it());

        return true;
    }


    size_t get_freq(const data_it it) const
    {
        return it->freq_it_->freq_;
    }



    public:

    cache_t(size_t sz) : sz_(sz) {}


    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page)
    {
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