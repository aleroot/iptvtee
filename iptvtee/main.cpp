//
//  main.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 21/05/21.
//
#include "iptv/playlist.hpp"
#include "iptv/analyzer.hpp"
#include "iptv/parameter.hpp"
#include "utils/Utils.hpp"
#include "iptv/http/server.hpp"
#include <fcntl.h>
#include <iostream>

/**
 * iptvtee
 * Note: to launch the program always set the VLC_PLUGIN_PATH environment variable.
 * VLC_PLUGIN_PATH=/Applications/VLC.app/Contents/MacOS/plugins iptvtee file.m3u
 */
int main(int argc, const char * argv[]) {
    std::vector<Playlist> iptv_lists;

    //Parse command line parameters
    Parameters params = Parameters::from(argc, argv);
    for(const std::string& file : params.files) 
        iptv_lists.push_back(Playlist::fromM3U(file, params.filter()));
    
    const int max_jobs = std::stoi(params.get("jobs", "-1"));
    const int max_secs = std::stoi(params.get("time", "60"));
    const int totalRuns = std::stoi(params.get("runs", "1"));
    const float min_score = std::stof(params.get("score", "0"));
    const Format format = ReportUtils::toFormat(params.get("format"));
    
    //Read optional input from stdin(in non-blocking mode)
    const int flags = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, flags | O_NONBLOCK);
    Playlist stdinPlaylist = Playlist::fromM3U(std::cin, params.filter());
    if(stdinPlaylist.size() > 0)
        iptv_lists.push_back(stdinPlaylist);
    fcntl(0, F_SETFL, flags);
    
    //Extract all links from an HTML page, if page parameter is set
    if(params.contains("page")) {
        std::vector<Playlist> extracted = Playlist::extractM3U(params.get("page"), params.filter());
        if(extracted.empty()) {
            std::cerr << "Can't extract M3U links from page: " << params.get("page") << "!" << std::endl;
            if(iptv_lists.empty())
                return 1;
        } else
            iptv_lists.insert(iptv_lists.end(), extracted.begin(), extracted.end());
    }
    
    //Show usage in case there is no input
    if(iptv_lists.empty())
        return AppUtils::usage();

    //Processing evaluation...
    int run = 1;
    Rank totalRank;
    int return_code = 0;
    const std::unordered_map<std::string, std::string> vlcOptions = params.map("vlc-parameters");
    do {
        for(int r = 0; r < iptv_lists.size(); r++) {
            Analyzer playlistEvaluator(iptv_lists[r],std::chrono::seconds(max_secs), max_jobs, vlcOptions);
            Rank rank = playlistEvaluator.evaluate();
            if((rank.score * 100) >= min_score) {
                totalRank = (run > 1 || r > 0) ? totalRank + rank : rank;
                Report report = playlistEvaluator.report(min_score);
                if(params.contains("format"))
                    report.exportTo(std::cout, format);
                std::cerr << "Report: " << report << std::endl;
            } else
                return_code = 1;
        }
    } while (totalRuns < run++);
    
    std::cerr << "Total Rank: " << totalRank << std::endl;
    
    if(params.contains("server")) {
        HTTPServer server(params);
        server.run();
        return 0; // Server mode should exit cleanly on shutdown
    }
    
    return (return_code <= 0 && ((totalRank.score * 100) >= min_score)) ? 0 : return_code+1;
}
