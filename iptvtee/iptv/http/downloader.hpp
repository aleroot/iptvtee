//
//  downloader.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 29/01/22.
//

#ifndef HTTPDOWNLOADER_HPP
#define HTTPDOWNLOADER_HPP

#include <string>
#include <curl/curl.h>
#include <curl/easy.h>

#include <sstream>
#include <iostream>
#include <optional>
/**
 * A non-threadsafe simple libcURL-easy based HTTP downloader
 */
class HTTPDownloader {
public:
    HTTPDownloader();
    ~HTTPDownloader();
    /**
     * Download a file using HTTP GET and store in in a std::string
     * @param url The URL to download
     * @return an optinal downloaded content, if download succeeded 
     */
    std::optional<std::stringstream> download(const std::string& url);
    std::optional<std::stringstream> download(const std::string& url, std::stringstream& out);
private:
    void* curl;
};

#endif  /* HTTPDOWNLOADER_HPP */
