//
//  server.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 10/07/2024.
//

#include "server.hpp"
#include <iostream>
#include "analyzer.hpp"
#include "Utils.hpp"

bool HTTPServer::authenticate(std::string_view username, std::string_view password) const {
    for (const auto& user : users) {
        if (user.first == username && user.second == password) {
            return true;
        }
    }
    return false;
}

HTTPServer::HTTPServer(Parameters p) : params(p) {
    // Initialize the user database
    if (params.contains("server")) {
        std::string serverValue = params.get("server");
        if(!serverValue.empty()) {
            size_t separatorPos = serverValue.find(':');
            if (separatorPos != std::string::npos) {
                std::string username = serverValue.substr(0, separatorPos);
                std::string password = serverValue.substr(separatorPos + 1);
                users[username] = password;
            }
        }
    }

    // Set up the routes
    svr.Post("/analyse", [this](const httplib::Request& req, httplib::Response& res) {
        if(!users.empty()) {
            auto auth_header = req.get_header_value("Authorization");
            if (auth_header.empty() || auth_header.find("Basic ") != 0) {
                res.status = 401;
                res.set_content("Unauthorized", "text/plain");
                res.set_header("WWW-Authenticate", "Basic realm=\"User Visible Realm\"");
                return;
            }
            
            std::string encoded_credentials = auth_header.substr(6); // Remove "Basic "
            std::string decoded_credentials = httplib::detail::base64_decode(encoded_credentials);
            size_t separatorPos = decoded_credentials.find(':');
            if (separatorPos == std::string::npos) {
                res.status = 401;
                res.set_content("Unauthorized", "text/plain");
                return;
            }
            
            std::string username = decoded_credentials.substr(0, separatorPos);
            std::string password = decoded_credentials.substr(separatorPos + 1);
            
            // Authenticate the user
            if (!authenticate(username, password)) {
                res.status = 401;
                res.set_content("Unauthorized", "text/plain");
                return;
            }
        }
        
        std::vector<Playlist> iptv_lists;
        params.files.clear();
        params.load(req.params);
        
        if(params.contains("page")){
            res.status = 400;
            res.set_content("Analyse endpoint cannot parse page!", "text/plain");
            return;
        }
        
        for(const std::string& file : params.files)
            iptv_lists.push_back(Playlist::fromM3U(file, params.filter()));
        
        std::stringstream response;
        const float min_score = std::stof(params.get("score", "0"));
        const int totalRuns = std::stoi(params.get("runs", "1"));
        const Format format = ReportUtils::toFormat(params.get("format"));
        //Processing evaluation...
        int run = 1;
        Rank totalRank;
        do {
            for(int r = 0; r < iptv_lists.size(); r++) {
                Analyzer playlistEvaluator(iptv_lists[r],std::chrono::seconds(std::stoi(params.get("time", "60"))), std::stoi(params.get("jobs", "-1")), params.map("vlc-parameters"));
                Rank rank = playlistEvaluator.evaluate();
                if((rank.score * 100) >= min_score) {
                    totalRank = (run > 1 || r > 0) ? totalRank + rank : rank;
                    Report report = playlistEvaluator.report(min_score);
                    report.exportTo(response, format);
                }
            }
        } while (totalRuns < run++);

        res.set_content(response.str(), Report::mime(format));
    });
}

void HTTPServer::run(const std::string& host, int port) {
    std::cerr << "Server started on http://" << host << ":" << port << std::endl;
    svr.listen(host, port);
}

void HTTPServer::run() {
    auto env_or_default = [](const char* env, const char* default_value) {
        return std::getenv(env) ? std::getenv(env) : default_value;
    };

    run(env_or_default("IPTVTEE_HOST", "0.0.0.0"),
        std::atoi(env_or_default("IPTVTEE_PORT", "8080")));
}
