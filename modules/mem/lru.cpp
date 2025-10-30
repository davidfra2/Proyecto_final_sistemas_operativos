#include "lru.h"
#include <vector>                
#include <list>
#include <unordered_map>
#include <iostream>

LRU::LRU(int f): frames(f) {}

int LRU::run_trace(const std::vector<int>& pages){
    std::list<int> memory;
    std::unordered_map<int, std::list<int>::iterator> page_map;
    int page_faults = 0;
    for (int page : pages) {
        std::cout << "Access page " << page << std::endl;
        if (page_map.find(page) != page_map.end()) {
            // hit: move to front
            memory.erase(page_map[page]);
        } else {
            // miss
            if ((int)memory.size() >= frames) {
                int lru_page = memory.back();
                memory.pop_back();
                page_map.erase(lru_page);
                page_faults++;
            } else {
                page_faults++;
            }
        }
        memory.push_front(page);
        page_map[page] = memory.begin();
        std::cout << "Frames: ";
        for (int p : memory) std::cout << p << " ";
        std::cout << "\t Faults: " << page_faults << std::endl;
    }
    return page_faults;
}
