//
//  evaluator.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 23/05/21.
//

#include "evaluator.hpp"
#include "Utils.hpp"
#include "http/downloader.hpp"
#include "vlcpp/vlc.hpp"

#include <thread>

Rank Evaluator::evaluate() {
    const std::chrono::milliseconds timeout = std::chrono::duration_cast<std::chrono::milliseconds>(evaluation_timeout);
    auto max = timeout.count();
    if(max <= 0)
        return Rank { .elements = 1, .score = 1.0f, .value = 0, .max_value = max};
    if (!HTTPDownloader::isUrl(url))
        return Rank { .elements = 1, .score = 0.0f, .value = 0, .max_value = 1 };
    const std::chrono::milliseconds start_delay = std::chrono::duration_cast<std::chrono::milliseconds>(initial_wait);
    static const auto vlc_instance = VLC::Instance(sizeof(vlc_minimal_args)/sizeof(*vlc_minimal_args), vlc_minimal_args);
    auto media = VLC::Media(vlc_instance, url, VLC::Media::FromLocation);
    // Parse the media to get track information
    media.parseWithOptions(VLC::Media::ParseFlags::Local | VLC::Media::ParseFlags::Network,static_cast<int>((start_delay + timeout).count()));
    //eventually set additional options from command line
    for (const auto& [key, value] : options)
        media.addOption(":" + key + "=" + value);
    
    auto mp = VLC::MediaPlayer(media);
    mp.setMute(true);
    mp.play();
    
    
    waitFor(start_delay, [&] () -> bool { return (!mp.isPlaying()); });
    auto waited = waitFor(timeout, [&] () -> bool { return mp.isPlaying(); });
    
    Quality quality;
    if(media.parsedStatus() == VLC::Media::ParsedStatus::Done) {
        auto tracks = media.tracks();
        for (const auto& track : tracks) {
            if (track.type() == VLC::MediaTrack::Type::Video) {
                quality.width = track.width();
                quality.height = track.height();
                quality.bitrate = track.bitrate();
                if(track.codec() > 0)
                    quality.codec = CodecUtils::fourccToString(track.codec());
                if (track.fpsDen() > 0) {
                    quality.fps = static_cast<float>(track.fpsNum()) / track.fpsDen();
                }
                break;
            }
        }
    }
    
    mp.stop();
    
    return Rank { .elements = 1, .score = (waited / static_cast<float>(max)), .value = waited, .max_value = max, .quality = quality};
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
