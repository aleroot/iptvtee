//
//  server.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 10/07/2024.
//

#ifndef server_hpp
#define server_hpp

#include "httplib/httplib.h"
#include "parameter.hpp"
#include <string>
#include <unordered_map>

class HTTPServer {
private:
    httplib::Server svr;
    Parameters params;
    std::unordered_map<std::string, std::string> users;

    [[nodiscard]] bool authenticate(std::string_view username, std::string_view password) const;
public:
    HTTPServer(Parameters p);
    void run(const std::string& host, int port);
    void run();
};

#endif /* server_hpp */

