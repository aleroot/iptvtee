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
     * @return The download result
     */
    std::stringstream& download(const std::string& url, std::stringstream& out);
    bool check(const std::string& url);
private:
    void* curl;
};

#endif  /* HTTPDOWNLOADER_HPP */
