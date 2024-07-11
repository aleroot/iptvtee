//
//  parameter.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 02/07/22.
//

#ifndef parameter_hpp
#define parameter_hpp
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <map>
#include "playlist.hpp"

class Parameters final {
public:
    static Parameters from(int argc, const char * argv[]);
    static Parameters from(const std::multimap<std::string, std::string>& map);
    Parameters* load(const std::multimap<std::string, std::string>& map);
    std::string& operator[](std::string p);
    const std::string get(std::string param, std::string defVal = "");
    const std::unordered_map<std::string, std::string> map(std::string param);
    const std::string pop(std::string param);
    bool contains(std::string param);
    std::function<bool(const PlaylistItem&, const int count)> filter();
    std::vector<std::string> files;
    void operator=(Parameters const&) = delete;
private:
    std::unordered_map<std::string, std::string> params;
};

#endif /* parameter_hpp */
