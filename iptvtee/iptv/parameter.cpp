//
//  parameter.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 02/07/22.
//

#include "parameter.hpp"
#include "FileUtils.hpp"
#include "Utils.hpp"

Parameters Parameters::from(const std::multimap<std::string, std::string>& map) {
    Parameters params;
    params.load(map);
    return params;
}

Parameters Parameters::from(int argc, const char * argv[]) {
    Parameters params;
    for (int i = 1; i < argc; ++i) {
        std::string par = argv[i];
        if (par.rfind("-", 0) == 0) {
            par = par.substr(par.find_first_not_of('-') , par.length());
            size_t pos;
            if((pos = par.find("=")) != std::string::npos) {
                params[par.substr(0, pos)] = par.substr(pos+1, par.length());
            } else
                params[par] = "";
            continue;
        }
        
        for(std::string file : FileUtils::glob(par))
            params.files.push_back(file);
    }
    return params;
}

Parameters* Parameters::load(const std::multimap<std::string, std::string>& map) {
    for (const auto& [key, value] : map) {
        if (key == "file" || key == "url") {
            files.push_back(value);
        } else {
            params[key] = value;
        }
    }
    return this;
}

const std::string Parameters::get(std::string param, std::string defVal) {
    return MapExtension::GetOrDefault(params, param, defVal);
}

const std::string Parameters::pop(std::string param) {
    std::string result{};
    auto got = params.find (param);
    if ( got != params.end() ) {
        result = got->second;
        params.erase(got);
    }
    return result;
}

std::function<bool(const PlaylistItem&, const int count)> Parameters::filter() {
    static std::function<bool (const PlaylistItem &, const int count)> result = FilterUtils::extractFilter(params);
    return result;
}

bool Parameters::contains(std::string param) {
    return params.contains(param);
}

std::string& Parameters::operator[](std::string p) {
    return params[p];
}
