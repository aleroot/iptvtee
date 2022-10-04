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
    std::function<bool(const std::string&)> extractFilter(const  std::unordered_map <K,V> & params) {
        typename std::unordered_map<K,V>::const_iterator it = params.find( "filter" );
        if ( it == params.end() ) {
            return [] (std::string x) -> bool { return true; };
        }

        std::string strF = (std::string)it->second;
        StringUtils::trim(strF);
        auto filter = [strF] (std::string x) -> bool {
            if(strF.empty())
                return true;
            const std::size_t pos = strF.find("|");
            const std::vector<std::string> andParts = StringUtils::split(strF.substr(0, pos), std::regex{"[\\&\\s]+"});
            bool result = !andParts.empty();
            for(std::string a : andParts) {
                StringUtils::trim(a);
                result &= StringUtils::contains(x, a);
            }
            
            if(pos != std::string::npos) {
                for(std::string a : StringUtils::split(strF.substr(pos), std::regex{"[\\|\\s]+"})) {
                    StringUtils::trim(a);
                    if(!a.empty())
                        result |= StringUtils::contains(x, a);
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
        std::map<std::string, Format> mappings = {{"CSV", Format::CSV},
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
        std::cerr << "iptvtee [--format=json|csv|m3u --jobs=15 --time=60 --runs=3 --score=1 --filter=term] file.m3u" << std::endl;
        return 1;
    }
}

#endif /* Utils_hpp */
