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

HTTPDownloader::HTTPDownloader(const std::string userAgent, long timeout) {
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
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

bool HTTPDownloader::isUrl(const std::string& text) {
    if (text.empty())
        return false;
    
    CURLU *url = curl_url();
    if (!url)
        return false; // Failed to create handle

    // Attempt to parse the URL.
    CURLUcode rc = curl_url_set(url, CURLUPART_URL, text.c_str(), 0);
    curl_url_cleanup(url); // Clean up the handle
    return (rc == CURLUE_OK);
}
