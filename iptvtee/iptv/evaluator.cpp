//
//  evaluator.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 23/05/21.
//

#include "evaluator.hpp"
#include "vlcpp/vlc.hpp"

#include <thread>

Rank Evaluator::evaluate() {
    const std::chrono::milliseconds timeout = std::chrono::duration_cast<std::chrono::milliseconds>(evaluation_timeout);
    auto max = timeout.count();
    if(max <= 0)
        return Rank { .elements = 1, .score = static_cast<float>(max), .value = max, .max_value = max};
    static const auto vlc_instance = VLC::Instance(0, nullptr);
    auto media = VLC::Media(vlc_instance, url, VLC::Media::FromLocation);
    for (const auto& [key, value] : options)
        media.addOption(":" + key + "=" + value);
    
    auto mp = VLC::MediaPlayer(media);
    mp.setMute(true);
    mp.play();
    
    waitFor(std::chrono::duration_cast<std::chrono::milliseconds>(initial_wait), [&] () -> bool { return (!mp.isPlaying()); });
    
    auto waited = waitFor(timeout, [&] () -> bool { return mp.isPlaying(); });
    mp.stop();
    
    return Rank { .elements = 1, .score = (waited / static_cast<float>(max)), .value = waited, .max_value = max};
}

long long Evaluator::waitFor(std::chrono::milliseconds time, std::function<bool(void)> condition) const {
    int c = 0;
    for (;condition() && c < (time.count() / wait_step.count());c++) {
        std::this_thread::sleep_for(wait_step);
    }
    return wait_step.count() * c;
}


void Evaluator::preSettings() {
    setenv("VLC_VERBOSE","-1",0); //Remove VLC logs to stdout, does not overwrite
    //Setting the VLC_PLUGIN_PATH environment variable is needed to avoid: std::runtime_error: Wrapping a nullptr instance
#ifdef __APPLE__
    if (getenv("VLC_PLUGIN_PATH") == nullptr) {
        setenv("VLC_PLUGIN_PATH", "/Applications/VLC.app/Contents/MacOS/plugins", 0);
    }
#endif
}
