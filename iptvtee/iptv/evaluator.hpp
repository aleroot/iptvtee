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
        "--quiet",                      // Suppress all console output
        "--no-video-title-show",        // Don't display the video title on video start
        "--no-osd",                     // Disable On-Screen Display (OSD)
        "--no-stats",                   // Don't collect playback statistics
        "--no-snapshot-preview",        // Disable snapshot previews
        "--no-sub-autodetect-file",     // Don't auto-detect subtitle files
        "--no-metadata-network-access", // Don't fetch metadata from the network
        "--no-audio",                   // Disable audio processing completely
        "--vout=dummy",                 // Use dummy video output (no actual display)
        "--network-caching=50",         // Set network caching to 50ms
        "--file-caching=0",             // Disable file caching
        "--live-caching=50",            // Set live caching to 50ms
        "--sout-mux-caching=0",         // Disable mux caching
        "--no-spu"                      // Disable sub-picture processing (including subtitles)
    };
};

#endif /* evaluator_hpp */
