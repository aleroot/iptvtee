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
#include <sstream>
#include <charconv>
#include <chrono>
#include "StringUtils.hpp"
#include "playlist.hpp"
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

namespace TimeUtils {

    /**
     * Parses a time string into a vector of std::chrono::seconds.
     * Correctly handles formats like:
     * - "5" → [5s]
     * - "5,4,3" → [5s, 4s, 3s]
     * - "5:3,0" → [5s, 5s, 5s, 0s] (3 elements of 5s followed by 0s)
     * - "5:3,4:2,1" → [5s,5s,5s,4s,4s,1s]
     *
     * @param timeArg The time string to parse.
     * @return A vector of std::chrono::seconds with timeouts.
     */
    inline const std::vector<std::chrono::seconds> get_timeouts(const std::string& timeArg) noexcept {
        std::vector<std::chrono::seconds> timeouts;
        
        if (timeArg.empty()) {
            timeouts.push_back(std::chrono::seconds(60));
            return timeouts;
        }

        std::istringstream ss(timeArg);
        std::string segment;
        bool parse_error = false;

        auto parse_int = [&parse_error](const std::string& s) -> int {
            int value = 0;
            auto result = std::from_chars(s.data(), s.data() + s.size(), value);
            if (result.ec != std::errc() || result.ptr != s.data() + s.size()) {
                parse_error = true;
                return 0;
            }
            return value;
        };

        while (std::getline(ss, segment, ',')) {
            size_t colon_pos = segment.find(':');
            
            if (colon_pos != std::string::npos) {
                // Handle count:value format (e.g., "5:3" → three 5s)
                int value = parse_int(segment.substr(0, colon_pos));
                int count = parse_int(segment.substr(colon_pos + 1));
                
                if (parse_error || count < 0) {
                    break;
                }
                
                timeouts.insert(timeouts.end(), count, std::chrono::seconds(value));
            } else {
                // Handle single value (either in list or as default)
                int val = parse_int(segment);
                if (parse_error) {
                    break;
                }
                
                // Check if this is the last segment and we have previous elements
                if (ss.eof() && !timeouts.empty()) {
                    // This is the default value for remaining elements
                    if (!timeouts.empty()) {
                        timeouts.push_back(std::chrono::seconds(val));
                    }
                } else {
                    // Regular single value
                    timeouts.push_back(std::chrono::seconds(val));
                }
            }
        }

        if (parse_error || timeouts.empty()) {
            return {std::chrono::seconds(60)};
        }

        return timeouts;
    }
}

namespace FilterUtils
{
    template <typename K, typename V>
    std::function<bool(const PlaylistItem&, const int count)> extractFilter(const std::unordered_map<K,V>& params) {
        int max = -1;
        typename std::unordered_map<K,V>::const_iterator it = params.find("max");
        if (it != params.end()) {
            max = std::stoi((std::string)it->second);
        }
        
        it = params.find("filter");
        if (it == params.end()) {
            return [max] (PlaylistItem x, int count) -> bool { return (max < 0 || count < max); };
        }

        std::string strI = (std::string)it->second;
        StringUtils::trim(strI);
        auto filter = [strI, max] (PlaylistItem item, int count) -> bool {
            if (max >= 0 && count >= max)
                return false;
            if (strI.empty())
                return true;
            std::string strF = strI;
            
            std::string prefix;
            bool hasPrefix = StringUtils::extractPrefixAndStrip(strF, prefix);
            const std::string x = item.by(hasPrefix ? prefix : "");
            const std::size_t pos = strF.find("|");
            const std::vector<std::string> andParts = StringUtils::split(strF.substr(0, pos), std::regex{"[\\&\\s]+"});
            bool result = !andParts.empty();
            for (std::string a : andParts) {
                StringUtils::trim(a);
                std::string fieldPrefix;
                bool hasFieldPrefix = StringUtils::extractPrefixAndStrip(a, fieldPrefix);
                const std::string ax = hasFieldPrefix ? item.by(fieldPrefix) : x;
                result &= StringUtils::contains(ax, a);
            }
            
            if (pos != std::string::npos) {
                for (std::string a : StringUtils::split(strF.substr(pos), std::regex{"[\\|\\s]+"})) {
                    StringUtils::trim(a);
                    std::string fieldPrefix;
                    bool hasFieldPrefix = StringUtils::extractPrefixAndStrip(a, fieldPrefix);
                    const std::string ax = hasFieldPrefix ? item.by(fieldPrefix) : x;
                    if (!a.empty())
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
            {"M3U", Format::M3U},
            {"ENIGMA", Format::ENIGMA}};
        
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

#include <string>
namespace UrlUtils
{
    inline std::string url_encode(const std::string &value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
            std::string::value_type c = (*i);

            // Keep alphanumeric and other accepted characters intact
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '/') {
                escaped << c;
                continue;
            }

            // Any other characters are percent-encoded
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char) c);
            escaped << std::nouppercase;
        }

        return escaped.str();
    }
}

namespace CodecUtils
{
    inline std::string fourccToString(uint32_t codec) {
        char fourcc[5] = {
            static_cast<char>(codec & 0xFF),
            static_cast<char>((codec >> 8) & 0xFF),
            static_cast<char>((codec >> 16) & 0xFF),
            static_cast<char>((codec >> 24) & 0xFF),
            '\0'
        };
        
        const bool allPrintable = std::all_of(fourcc, fourcc + 4,[](unsigned char c) { return std::isprint(c); });
        if (!allPrintable) {
            std::stringstream ss;
            ss << "0x" << std::hex << std::uppercase << codec;
            return ss.str();
        }
        
        return std::string(fourcc);
    }
}

namespace AppUtils
{
    inline int usage() {
        std::cerr << "iptvtee v1.0" << std::endl << "usage:" <<std::endl;
        std::cerr << "iptvtee [--format=txt|json|csv|m3u|enigma|url --jobs=15 --time=60 --runs=3 --score=1 --filter=term --max=1 --page=example.org --server] file.m3u" << std::endl;
        return 1;
    }
}

#endif /* Utils_hpp */
