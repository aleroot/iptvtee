//
//  analyzer.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 30/01/22.
//

#ifndef analyzer_hpp
#define analyzer_hpp

#include "Evaluable.hpp"
#include "stats/WordCounter.hpp"
#include "playlist.hpp"
#include "report.hpp"
#include <chrono>
#include <future>
#include <unordered_map>

using namespace std::chrono_literals;

class Analyzer : public virtual Evaluable {
    size_t max_concurrent_analisys = 1;
    Playlist items;
    Rank result;
    WordCounter nameCounter{2};
    std::vector<Rank> work_result;
    std::future<Rank> work_producer;
    Rank calc_total();
    std::atomic<bool> result_ready = false;
    std::mutex result_mutex;
    std::unordered_map<std::string, std::string> options;
public:
    explicit Analyzer(Playlist list, std::vector<std::chrono::seconds> timeouts = {std::chrono::seconds{30}}, int max_concurrent = -1, std::unordered_map<std::string, std::string> opt = {});
    explicit Analyzer(Playlist list, std::chrono::seconds timeout, int max_concurrent, std::unordered_map<std::string, std::string> opt) : Analyzer(std::move(list), std::vector<std::chrono::seconds>{timeout}, max_concurrent, std::move(opt)) {}
    explicit Analyzer(std::string url, std::vector<std::chrono::seconds> timeouts = {std::chrono::seconds{30}}) : Analyzer(Playlist::fromM3U(url) , timeouts) {};
    virtual ~Analyzer();
    virtual Rank evaluate() override;
    std::vector<Rank>::iterator begin();
    std::vector<Rank>::iterator end();
    Report report(float min_score = -1);
    float score();
};

#endif /* analyzer_hpp */
