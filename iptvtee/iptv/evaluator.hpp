//
//  evaluator.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 23/05/21.
//

#ifndef evaluator_hpp
#define evaluator_hpp

#include <string>
#include <chrono>
#include <functional>
#include <unordered_map>
#include "Evaluable.hpp"

using namespace std::chrono_literals;

class Evaluator : public virtual Evaluable {
public:
    explicit Evaluator(std::string url, std::chrono::seconds eval_max_time, std::unordered_map<std::string, std::string> opt) : url(url), evaluation_timeout(eval_max_time), options(opt) {preSettings();};
    explicit Evaluator(std::string url, std::chrono::seconds eval_max_time) : url(url), evaluation_timeout(eval_max_time), options({}) {preSettings();};
    explicit Evaluator(std::string url) : url(url), evaluation_timeout(60s), options({}) {preSettings();};
    [[nodiscard]] virtual Rank evaluate() override;
private:
    std::chrono::seconds evaluation_timeout;
    std::unordered_map<std::string, std::string> options;
    static constexpr std::chrono::milliseconds wait_step{10ms};
    static constexpr std::chrono::seconds initial_wait{5s}; //Default maximum wait time for the stream play...
    std::string url;
    long long waitFor(std::chrono::milliseconds time, std::function<bool(void)> condition) const;
    void preSettings();
    static constexpr const char* vlc_minimal_args[] = {
            "--quiet",                // No messages
            "--no-video-title-show", // No video title
            "--no-osd",             // No on-screen display
            "--no-media-library", // No media database
            "--no-stats",        // No statistics
            "--no-plugins-cache", // Don't cache plugins
            "--no-snapshot-preview", // No snapshot previews
            "--no-sub-autodetect-file", // Don't auto-load subtitles
            "--no-metadata-network-access" // No online metadata
        };
};

#endif /* evaluator_hpp */
