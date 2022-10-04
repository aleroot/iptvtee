//
//  analyzer.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 30/01/22.
//

#include "analyzer.hpp"
#include "evaluator.hpp"
#include <thread>
#include <algorithm>

Analyzer::Analyzer(Playlist list, std::chrono::seconds eval_max_time, int max_concurrent) : items(list) {
    if(max_concurrent <= 0)
        max_concurrent = std::thread::hardware_concurrency();
    max_concurrent_analisys = max_concurrent;
    if(max_concurrent_analisys <= 0)
        max_concurrent_analisys = 1;
    
    //start a producer thread that fill in the array of futures
    work_producer = std::async(std::launch::async,
        [&,eval_max_time]() {
        int i_playlist = 0;
        int size = (int)items.size();
        
        while(work_result.size() < size) {
            //start processing of list in batches
            const int len = std::min(max_concurrent_analisys, size - i_playlist);
            std::future<Rank> batch[len];
            
            for(int i = 0; i < len; i++, i_playlist++) {
                const std::string url = items[i_playlist].url;
                batch[i] = std::async(std::launch::async, &Evaluator::evaluate, Evaluator(url, eval_max_time));
            }
            
            int count = 0;
            std::vector<bool> completed(len);
            do {
                for(int i = 0; i < len; i++) {
                    if(!completed[i]) {
                        std::future_status status = batch[i].wait_for(std::chrono::milliseconds(100));
                        if(status == std::future_status::ready) {
                            const Rank rank = batch[i].get();
                            work_result.push_back(rank);
                            completed[i] = true;
                            count++;
                        }
                    }
                }
            } while(count < len);
        }
        
        return calc_total();
        }
    );
}

Rank Analyzer::calc_total() {
    static const int first = 0;
    Rank total;
    if(work_result.size() > first) {
        total.value = work_result[first].value;
        total.elements = work_result[first].elements;
        total.score = work_result[first].score;
        total.max_value = work_result[first].max_value;
        
        for(int i =1; i < work_result.size(); i++) {
            total += work_result[i];
        }
    }
    
    return total;
}

Rank Analyzer::evaluate() {
    if(!result_ready) {
        result = work_producer.get();
        result_ready = true;
    }
    return result;
}

float Analyzer::score() {
    evaluate();
    return result.score;
}

std::vector<Rank>::iterator Analyzer::begin() {
    evaluate();
    return work_result.begin();
}
std::vector<Rank>::iterator Analyzer::end()   { return work_result.end(); }

Report Analyzer::report(float min_score) {
    if(min_score > 0) {
        std::vector<Rank> filtered_result;
        for(Rank r : work_result) {
            if((r.score * 100) >= min_score)
                filtered_result.push_back(r);
        }
        return Report(items, filtered_result);
    }
    
    return Report(items, work_result);
}


// Define C functions for the C++ class - as python ctypes can only talk to C...
// Build the library with: g++ -std=c++20 -shared -o libanalyzer.so -fPIC analyzer.cpp
extern "C"
{
    Analyzer* Analyzer_new(const char* url) {return new Analyzer(url);}
    float Analyzer_score(Analyzer* analyzer) {return analyzer->score();}
    void Analyzer_delete(Analyzer* analyzer)
    {
        if (analyzer)
        {
            delete analyzer;
            analyzer = nullptr;
        }
    }
}
