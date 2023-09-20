FLAGS = -std=c++20 -O2 -DNDEBUG

DEBUG_FLAGS = -std=c++20 -D DEBUG -O0 -g3 -Wall -Wextra -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LFU_FILES = LFU_cache/cache.cpp
ID_FILES  = ideal_cache/ideal_cache.cpp

main: $(LFU_FILES) include/cache.hpp
	g++ $(FLAGS) $(LFU_FILES) -o cache

main_debug: $(LFU_FILES) include/cache.hpp
	g++ $(DEBUG_FLAGS) $(LFU_FILES) -o cache

ideal: $(ID_FILES) include/ideal_cache.hpp
	g++ $(FLAGS) $(ID_FILES) -o ideal

ideal_debug: $(ID_FILES) include/ideal_cache.hpp
	g++ $(DEBUG_FLAGS) $(ID_FILES) -o ideal