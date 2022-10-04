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

class Parameters final {
public:
    static Parameters from(int argc, const char * argv[]);
    std::string& operator[](std::string p);
    const std::string get(std::string param, std::string defVal = "");
    const std::string pop(std::string param);
    bool contains(std::string param);
    std::function<bool(const std::string&)> filter();
    std::vector<std::string> files;
    void operator=(Parameters const&) = delete;
private:
    std::unordered_map<std::string, std::string> params;
};

#endif /* parameter_hpp */
