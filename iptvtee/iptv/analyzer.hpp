//
//  analyzer.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 30/01/22.
//

#ifndef analyzer_hpp
#define analyzer_hpp

#include "Evaluable.hpp"
#include "playlist.hpp"
#include "report.hpp"
#include <chrono>
#include <future>

using namespace std::chrono_literals;

class Analyzer : public virtual Evaluable {
    int max_concurrent_analisys = 1;
    Playlist items;
    Rank result;
    std::vector<Rank> work_result;
    std::future<Rank> work_producer;
    Rank calc_total();
    std::atomic<bool> result_ready = false;
public:
    explicit Analyzer(Playlist list, std::chrono::seconds eval_max_time = 30s, int max_concurrent = -1);
    explicit Analyzer(std::string url) : Analyzer(Playlist::fromM3U(url)) {};
    virtual ~Analyzer() {};
    virtual Rank evaluate() override;
    std::vector<Rank>::iterator begin();
    std::vector<Rank>::iterator end();
    Report report(float min_score = -1);
    float score();
};

#endif /* analyzer_hpp */
