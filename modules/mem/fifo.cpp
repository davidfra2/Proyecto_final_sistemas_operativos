#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>

int fifo_run(const std::vector<int>& pages, int frames){
    std::queue<int> q;
    std::unordered_set<int> inmem;
    int faults=0;
    for(int p:pages){
        std::cout<<"Access "<<p<<std::endl;
        if(inmem.find(p)==inmem.end()){
            faults++;
            if((int)q.size()>=frames){
                int ev = q.front(); q.pop(); inmem.erase(ev);
            }
            q.push(p); inmem.insert(p);
        }
        std::cout<<"Faults: "<<faults<<std::endl;
    }
    return faults;
}
