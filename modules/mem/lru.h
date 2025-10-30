#ifndef LRU_H
#define LRU_H

#include <vector>

class LRU {
private:
    int frames;
public:
    LRU(int f);
    int run_trace(const std::vector<int>& pages);
};

#endif
