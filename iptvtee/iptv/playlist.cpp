//
//  playlist.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 28/01/22.
//

#include "playlist.hpp"
#include "StringUtils.hpp"
#include "http/downloader.hpp"
#include <unordered_map>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <filesystem>


class PlaylistLine {
    std::string line;
    std::string desc;
    std::unordered_map<std::string, std::string> descriptors;
    void parse() {
        //parse #EXTINF line into descriptors if possible
        int lastComma = 0;
        for(int i = 0; i < line.length(); i++) {
            char c = line.at(i);
            if(c == '=') {
                std::string name = StringUtils::wordBefore(line, i);
                std::string desc = StringUtils::phraseAfter(line, i);
                descriptors.insert({ name, desc });
            }
            if(c == ',')
                lastComma = i;
        }
        if(lastComma > 0) {
            lastComma = lastComma + 1;
            desc = line.substr(lastComma, line.length() - lastComma);
        }
    }
public:
    PlaylistLine(std::string &l) : line(l){
        parse();
    };
    std::string text() {
        return line;
    }
    std::string description() {
        return desc;
    }
    std::string get(std::string id) {
        try {
            return descriptors.at(id);
        } catch (const std::out_of_range& oor) {
            for (const auto& [name, desc] : descriptors) {
                if(name.ends_with(id) || name.starts_with(id))
                    return desc;
            }
        }
        return std::string();
    }
};

std::vector<Playlist> Playlist::extractM3U(std::string url,std::function<bool(const PlaylistItem&, const int count)> filter) {
    std::vector<Playlist> result;
    
    std::stringstream page;
    if(std::filesystem::exists(std::filesystem::path{ url })) {
        std::ifstream file(url);
        page << file.rdbuf();
        file.close();
    } else {
        HTTPDownloader downloader;
        auto downloaded = downloader.download(url, page);
        if(downloaded.has_value())
            page.str(downloaded.value().str());
    }
    
    static const std::regex url_regex ("\"(?:https?://[^\"]+)?(https?://[^\"]+m3u8?)\"");
    std::string line;
    while (std::getline(page, line)) {
        std::smatch m;
        std::regex_search(line, m, url_regex);
        if(m.size()>1) {
            try {
            result.push_back(fromM3U(m.str(1), filter));
            } catch (std::invalid_argument &e) { }
        }
    }
    return result;
}

Playlist Playlist::fromM3U(std::string url,std::function<bool(const PlaylistItem&, const int count)> filter) {
    StringUtils::trim(url);
    if(url.size() > 0) {
        HTTPDownloader downloader;
        auto downloaded = downloader.download(url);
        if(downloaded) {
            try {
                Playlist playlist = fromM3U(downloaded.value(), filter);
                if(playlist.size() < 1)
                    playlist.entries.push_back(PlaylistItem{.url = url});
                return playlist;
            } catch(const std::invalid_argument& e) {
                return Playlist(); //The url is a valid page, but it is not really an M3U, so empty playlist
            }
        } else if(std::filesystem::is_directory(url)){
            Playlist playlist;
            for (auto &entry : std::filesystem::directory_iterator(url)) {
                std::ifstream file(entry.path());
                playlist.load(fromM3U(file, filter));
            }
            return playlist;
        } else if(std::filesystem::exists(std::filesystem::path{ url })){
            std::ifstream file(url);
            return fromM3U(file, filter);
        }
    }
    throw std::invalid_argument("Given url:" + url + " not parsable!");
}

template<class _CharT, class _Traits> Playlist Playlist::fromM3U(std::basic_istream<_CharT, _Traits> &is,std::function<bool(const PlaylistItem&, const int count)> filter) {
    std::vector<PlaylistItem> list;
    bool parsingM3U = false;
    std::string line;
    while (std::getline(is, line)) {
        StringUtils::trim(line);
        if(line.starts_with("#EXTM3U"))
            parsingM3U = true;
        else if(line.starts_with("#EXTINF")){
            PlaylistLine pl_line(line);
            std::string id = pl_line.get("tvg-id");
            std::erase_if(id, [](char const &c) {
                return !std::isdigit(c);
            });
            PlaylistItem item { .id = StringUtils::toNumber(id),
                                .name = pl_line.get("tvg-name"),
                                .text = pl_line.description(),
                                .group =  pl_line.get("group-title"),
                                .logo = pl_line.get("tvg-logo")
                              };
            std::string urlLine;
            if(std::getline(is, urlLine)) {
                StringUtils::trim(urlLine);
                if(urlLine.length() < 5 || urlLine.starts_with("#"))
                    continue; //something wrong here, discard the item.
                item.url = std::move(urlLine);
            }
            if(filter(item, static_cast<int>(list.size())))
                list.push_back(item);
        } else if (line.size() > 3 && !line.starts_with("#") && !parsingM3U) {
            //It is not a valid M3U --> check if it's a URL or file...
            Playlist pl = fromM3U(line, filter);
            if (!pl.empty())
                list.insert(list.end(), std::make_move_iterator(pl.entries.begin()), std::make_move_iterator(pl.entries.end()));
        }
    }
    
    return list.empty() ? Playlist() : Playlist(list);
}

Playlist::Playlist(const std::vector<PlaylistItem> items) : entries(std::move(items)) {}

size_t Playlist::size() {
    return entries.size();
}

bool Playlist::empty() {
    return entries.empty();
}

void Playlist::load(Playlist other) {
    entries.insert(entries.end(), other.entries.begin(), other.entries.end());
}

PlaylistItem& Playlist::operator[](int idx) {
    if (idx < 0 || idx >= static_cast<int>(entries.size())) {
        throw std::out_of_range("Index out of bounds");
    }
    return entries[idx];
}

