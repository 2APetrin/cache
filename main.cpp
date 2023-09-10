#include "include/cache.hpp"
#include <fstream>

int slow_get_page_int(int key) { return key; }
int do_tests(void);

int main()
{
    #ifdef DEBUG
    if (do_tests())
    {
        printf("ERROR in tests\n");
        return 1;
    }
    #endif
    #ifndef DEBUG

    int cache_l, elem_num;
    if(scanf("%d %d", &cache_l, &elem_num) != 2)
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
        scanf("%d", &cache_buff[i]);

    int hit_cnt = 0;
    for (int i = 0; i < elem_num; i++)
        if (csh.lookup_update(cache_buff[i], slow_get_page_int)) hit_cnt++;
    printf("%d\n", hit_cnt);

    delete[] cache_buff;
    #endif
    return 0;
}


int do_tests(void)
{
    std::ifstream in_stream("tests.txt");
    if (!in_stream.is_open())
    {
        std::cout << "file is not opened\n";
        return 1;
    }

    in_stream.seekg(0, in_stream.end);
    long int length = in_stream.tellg();
    in_stream.seekg(0, in_stream.beg);

    char* buff = new char [length + 1];
    in_stream.read(buff, length);
    in_stream.close();

    buff[length] = 0;

    int pos    = 0;
    int it_cnt = 0;

    while (buff[pos])
    {
        if (it_cnt) pos++;

        int cache_cp = 0;
        int elem_num = 0;
        int scan_len = 0;

        if (sscanf(buff + pos++, "%d%d%n", &cache_cp, &elem_num, &scan_len) != 2) ERR_TEST("ERROR. Expected hash length");
        pos += scan_len;

        caches::cache_t <int> csh{(size_t) cache_cp};

        int* cache_buff = new int [elem_num];
        for (int i = 0; i < elem_num; i++)
        {
            if (!sscanf(buff + pos++, "%d%n", &cache_buff[i], &scan_len)) ERR_TEST("ERROR. Expected number");
            pos += scan_len;
        }

        int hit_cnt = 0;
        for (int i = 0; i < elem_num; i++)
            if (csh.lookup_update(cache_buff[i], slow_get_page_int)) hit_cnt++;

        int expected_hits = 0;
        if (!sscanf(buff + pos++, "%d%n", &expected_hits, &scan_len)) ERR_TEST("ERROR. Expected number of hits");
        pos += scan_len;

        pos--;
        std::cout << ++it_cnt << ") hit count = " << hit_cnt;
        if (hit_cnt == expected_hits) std::cout << " passed\n";
        else std::cout << " NOT passed\n";

        int ideal_hits = 0;
        for (int i = 0; i < elem_num; i++)
            if (csh.ideal_lookup_update(cache_buff[i], slow_get_page_int, cache_buff, (size_t)elem_num, i+1)) ideal_hits++;
        printf("ideal - %d\n", ideal_hits);

        delete[] cache_buff;
    }

    delete[] buff;
    return 0;
}