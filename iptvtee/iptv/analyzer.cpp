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

Analyzer::Analyzer(Playlist list, std::vector<std::chrono::seconds> timeouts, int max_concurrent, std::unordered_map<std::string, std::string> opt) : items(list),
    max_concurrent_analisys(max_concurrent > 0 ? max_concurrent : std::thread::hardware_concurrency()),
    options(opt)
{
    if(items.empty())
        throw std::runtime_error("Playlist to analyse is empty!");
        
    if(max_concurrent_analisys <= 0)
        max_concurrent_analisys = 1;

    //start a producer thread that fill in the array of futures
    work_producer = std::async(std::launch::async,
        [&,timeouts]() {
        size_t i_playlist = 0;
        size_t size = items.size();
        
        while(work_result.size() < size) {
            //start processing of list in batches
            const size_t len = std::min(max_concurrent_analisys, size - i_playlist);
            std::vector<std::future<Rank>> batch(len);
            
            for(int i = 0; i < len; i++, i_playlist++) {
                const auto eval_max_time = timeouts[std::min(i_playlist, timeouts.size()-1)];
                const std::string url = items[i_playlist].url;
                batch[i] = std::async(std::launch::async, &Evaluator::evaluate, Evaluator(url, eval_max_time, options));
                nameCounter.process(items[i_playlist].text);
            }
            
            int count = 0;
            std::vector<std::atomic<bool>> completed(len);
            do {
                for(int i = 0; i < len; i++) {
                    if(!completed[i]) {
                        std::future_status status = batch[i].wait_for(std::chrono::milliseconds(100));
                        if(status == std::future_status::ready) {
                            const Rank rank = batch[i].get();
                            {
                                std::lock_guard<std::mutex> lock(result_mutex);
                                work_result.push_back(rank);
                                completed[i] = true;
                            }
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

Analyzer::~Analyzer() {
    if (work_producer.valid()) {
        work_producer.wait();
    }
};

Rank Analyzer::calc_total() {
    static constexpr int first = 0;
    Rank total;
    std::lock_guard<std::mutex> lock(result_mutex);
    if(work_result.size() > first) {
        total = work_result[first];
        
        for(size_t i = 1; i < work_result.size(); i++)
            total = total + work_result[i];
        
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

std::vector<Rank>::iterator Analyzer::end()   {
    evaluate();
    return work_result.end();
}

Report Analyzer::report(float min_score) {
    evaluate();
    const std::string topWords = nameCounter.top(3,50);
    const std::string title = "IPTVTEE - " + ((!topWords.empty()) ? topWords : "EXPORT");
    if(min_score > 0) {
        std::vector<PlaylistItem> filtered_items;
        std::vector<Rank> filtered_ranks;

        if (work_producer.valid()) {
            work_producer.wait();
        }

        for (size_t i = 0; i < work_result.size(); ++i) {
            if ((work_result[i].score * 100) >= min_score) {
                filtered_items.push_back(items[i]);
                filtered_ranks.push_back(work_result[i]);
            }
        }
        return Report(title, Playlist(filtered_items), filtered_ranks);
    }
    
    return Report(title, items, work_result);
}


// Define C functions for the C++ class - as python ctypes can only talk to C...
// Build the library with: g++ -std=c++20 -shared -o libanalyzer.so -fPIC analyzer.cpp
extern "C"
{
    Analyzer* Analyzer_new(const char* url) {return new Analyzer(url);}
    float Analyzer_score(Analyzer* analyzer) {return analyzer->score();}
    Analyzer* Analyzer_with_timeout(const char* url, int* timeouts, int num_timeouts) {
        std::vector<std::chrono::seconds> timeout_vec;
        for (int i = 0; i < num_timeouts; ++i)
            timeout_vec.push_back(std::chrono::seconds{timeouts[i]});
        
        return new Analyzer(url, timeout_vec);
    }
    void Analyzer_delete(Analyzer* analyzer)
    {
        if (analyzer)
        {
            delete analyzer;
            analyzer = nullptr;
        }
    }
}
