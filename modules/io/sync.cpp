#include "sync.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <chrono>

static std::mutex mtx;
static std::condition_variable cv;
static std::queue<int> buffer;
static const int BUFFER_MAX = 5;

SyncDemo::SyncDemo(){}

void producer(int n){
    for(int i=0;i<n;i++){
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk,[]{return (int)buffer.size()<BUFFER_MAX;});
        buffer.push(i);
        std::cout<<"Produced item "<<i<<", buffer size "<<buffer.size()<<std::endl;
        lk.unlock();
        cv.notify_all();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(int n){
    for(int i=0;i<n;i++){
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk,[]{return !buffer.empty();});
        int v = buffer.front(); buffer.pop();
        std::cout<<"Consumed item "<<v<<", buffer size "<<buffer.size()<<std::endl;
        lk.unlock();
        cv.notify_all();
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

void SyncDemo::produce_n(int n){
    std::thread t(producer, n);
    t.join();
}
void SyncDemo::consume_n(int n){
    std::thread t(consumer, n);
    t.join();
}
