//
//  Utils.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 11/06/22.
//

#ifndef Utils_hpp
#define Utils_hpp
#include <unordered_map>
#include <map>
#include "StringUtils.hpp"
#include "Playlist.hpp"
namespace MapExtension
{
    template <typename K, typename V>
    V GetOrDefault(const  std::unordered_map <K,V> & m, const K & key, const V & defval ) {
       typename std::unordered_map<K,V>::const_iterator it = m.find( key );
       if ( it == m.end() ) {
          return defval;
       }
       else {
          return it->second;
       }
    }
}

namespace FilterUtils
{
    template <typename K, typename V>
    std::function<bool(const PlaylistItem&, const int count)> extractFilter(const  std::unordered_map <K,V> & params) {
        int max = -1;
        typename std::unordered_map<K,V>::const_iterator it = params.find( "max" );
        if ( it != params.end() ) {
            max = std::stoi((std::string)it->second);
        }
        
        it = params.find( "filter" );
        if ( it == params.end() ) {
            return [max] (PlaylistItem x, int count) -> bool { return (max < 0 || count < max); };
        }

        std::string strI = (std::string)it->second;
        StringUtils::trim(strI);
        auto filter = [strI, max] (PlaylistItem item, int count) -> bool {
            if(max >= 0 && count >= max)
                return false;
            if(strI.empty())
                return true;
            std::string strF = strI;
            
            const std::string x = item.by(StringUtils::extractPrefixAndStrip(strF));
            const std::size_t pos = strF.find("|");
            const std::vector<std::string> andParts = StringUtils::split(strF.substr(0, pos), std::regex{"[\\&\\s]+"});
            bool result = !andParts.empty();
            for(std::string a : andParts) {
                StringUtils::trim(a);
                std::optional<std::string> opt = StringUtils::extractPrefixAndStrip(a);
                const std::string ax = opt.has_value() ? item.by(opt) : x;
                result &= StringUtils::contains(ax, a);
            }
            
            if(pos != std::string::npos) {
                for(std::string a : StringUtils::split(strF.substr(pos), std::regex{"[\\|\\s]+"})) {
                    StringUtils::trim(a);
                    std::optional<std::string> opt = StringUtils::extractPrefixAndStrip(a);
                    const std::string ax = opt.has_value() ? item.by(opt) : x;
                    if(!a.empty())
                        result |= StringUtils::contains(ax, a);
                }
            }
            return result;
        };
        
        return filter;
    }
}

#include "report.hpp"
#include <sstream>
namespace ReportUtils
{

    inline Format toFormat(std::string strFormat) {
        StringUtils::trim(strFormat);
        std::map<std::string, Format> mappings = {{"TXT", Format::TXT},
                                                  {"URL", Format::URL},
                                                  {"CSV", Format::CSV},
                                                  {"JSON", Format::JSON},
                                                  {"M3U", Format::M3U}};
        
        std::stringstream upperFormat;
        for(int i = 0; i < strFormat.length(); i++) {
            char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(strFormat[i])));
            upperFormat << ch;
        }
        try {
            return mappings.at(upperFormat.str());
        }
        catch (const std::out_of_range&) {
            return Format::M3U;
        }
    }
}

namespace AppUtils
{
    inline int usage() {
        std::cerr << "iptvtee v1.0" << std::endl << "usage:" <<std::endl;
        std::cerr << "iptvtee [--format=txt|json|csv|m3u|url --jobs=15 --time=60 --runs=3 --score=1 --filter=term --max=1 --page=example.org --server] file.m3u" << std::endl;
        return 1;
    }
}

#endif /* Utils_hpp */
