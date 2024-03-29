//
//  playlist.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 28/01/22.
//

#include "playlist.hpp"
#include "StringUtils.hpp"
#include "http/downloader.hpp"
#include <map>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <filesystem>


class PlaylistLine {
    std::string line;
    std::string desc;
    std::map<std::string, std::string> descriptors;
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

std::vector<Playlist> Playlist::extractM3U(std::string url,std::function<bool(const std::string&, const int count)> filter) {
    std::vector<Playlist> result;
    
    std::stringstream page;
    if(std::filesystem::exists(std::filesystem::path{ url })) {
        std::ifstream file(url);
        page << file.rdbuf();
        file.close();
    } else {
        HTTPDownloader downloader;
        downloader.download(url, page);
    }
    
    std::regex url_regex ("\"(?:https?://[^\"]+)?(https?://[^\"]+m3u8?)\"");
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

Playlist Playlist::fromM3U(std::string url,std::function<bool(const std::string&, const int count)> filter) {
    StringUtils::trim(url);
    if(url.size() > 0) {
        HTTPDownloader downloader;
        if(downloader.check(url)) {
            std::stringstream content;
            try {
                Playlist playlist = fromM3U(downloader.download(url, content), filter);
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

template<class _CharT, class _Traits> Playlist Playlist::fromM3U(std::basic_istream<_CharT, _Traits> &is,std::function<bool(const std::string&, const int count)> filter) {
    std::vector<PlaylistItem> list;
    bool parsingM3U = false;
    std::string line;
    while (std::getline(is, line)) {
        StringUtils::trim(line);
        if(line.starts_with("#EXTM3U"))
            parsingM3U = true;
        else if(line.starts_with("#EXTINF")){
            PlaylistLine pl_line(line);
            PlaylistItem item { .id = StringUtils::toInt(pl_line.get("tvg-id")),
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
                item.url = urlLine;
            }
            if(filter(item.text, (int)list.size()))
                list.push_back(item);
        } else if (line.size() > 3 && !line.starts_with("#") && !parsingM3U) {
            //It is not a valid M3U --> check if it's a URL or file...
            Playlist pl = fromM3U(line,filter);
            if(pl.size() > 0)
                list.insert(list.end(), pl.entries.begin(), pl.entries.end());
        }
    }
    
    return list.empty() ? Playlist() : Playlist(list);
}

Playlist::Playlist(std::vector<PlaylistItem> items) : entries(items) {}

size_t Playlist::size() {
    return entries.size();
}

bool Playlist::empty() {
    return entries.empty();
}

void Playlist::load(Playlist other) {
    entries.insert(entries.end(), other.entries.begin(), other.entries.end());
}

PlaylistItem& Playlist::operator[](int idx)
{
    return entries[idx];
}
