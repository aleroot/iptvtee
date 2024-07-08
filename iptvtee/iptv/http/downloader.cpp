//
//  downloader.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 29/01/22.
//

#include "downloader.hpp"
using namespace std;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    string data((const char*) ptr, (size_t) size * nmemb);
    *((stringstream*) stream) << data << endl;
    return size * nmemb;
}

HTTPDownloader::HTTPDownloader() {
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/8.6.0");
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
}

HTTPDownloader::~HTTPDownloader() {
    curl_easy_cleanup(curl);
}

std::optional<std::stringstream> HTTPDownloader::download(const std::string& url, std::stringstream& out) {
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0); // Ensure it performs the download
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

        /* Perform the request */
        CURLcode res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            return std::nullopt; // Download failed
        }
        return std::optional<std::stringstream>(std::move(out)); // Download succeeded
    }
    return std::nullopt;
}

std::optional<std::stringstream> HTTPDownloader::download(const std::string& url) {
    std::stringstream ss;
    return download(url, ss);
}
