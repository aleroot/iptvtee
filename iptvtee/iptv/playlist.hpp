//
//  playlist.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 28/01/22.
//

#ifndef playlist_hpp
#define playlist_hpp

#include <string>
#include <fstream>
#include <vector>
#include <functional>

struct PlaylistItem {
    long long id;
    std::string name;
    std::string text;
    std::string group;
    std::string logo;
    std::string url;
    
    std::string by(const std::string& field) const {
        if (field.empty()) {
            return text;  // Default to 'text' if empty string provided
        }

        if (field == "name") return name;
        if (field == "group") return group;
        if (field == "logo") return logo;
        if (field == "url") return url;
        return text;  // Default to 'text' if no match
    }
    
    friend std::ostream& operator << (std::ostream &os, const PlaylistItem &p) {
        os << "Item{";
        if(p.name.length() > 0)
            os <<"name = " << p.name << ",";
        if(p.group.length() > 0)
            os <<"group = " << p.group << ",";
        if(p.logo.length() > 0)
            os <<"logo = " << p.logo << ",";
        return (os << "text = \"" << p.text << "\",url = " << p.url << "}");
    }
};

class Playlist {
    std::vector<PlaylistItem> entries;
    constexpr static auto noFilter = [] (PlaylistItem x, int count) -> bool { return true; };
public:
    static std::vector<Playlist> extractM3U(std::string url,std::function<bool(const PlaylistItem&, const int count)> filter = noFilter);
    /** Parse the m3u file from an URL or local file on disk.
     *  It can be an URL or a phisycal file on disk:
     *    - fromM3U("https://www.example.com/test.m3u")
     *    - fromM3U("/Users/user/test.m3u8")
     */
    static Playlist fromM3U(std::string url,std::function<bool(const PlaylistItem&, const int count)> filter = noFilter);
    /** Load an M3U from a stream.
     */
    template<class _CharT, class _Traits> static Playlist fromM3U(std::basic_istream<_CharT, _Traits>& is, std::function<bool(const PlaylistItem&, const int count)> filter = noFilter);
    Playlist() {};
    Playlist(const std::vector<PlaylistItem> items);
    bool empty();
    size_t size();
    PlaylistItem& operator[](size_t idx);
    void load(Playlist other);
};

#endif /* playlist_hpp */
