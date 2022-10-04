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
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.79.1");
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
}

HTTPDownloader::~HTTPDownloader() {
    curl_easy_cleanup(curl);
}

bool HTTPDownloader::check(const std::string& url) {
    if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            /* don't write output to stdout */
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

            /* Perform the request */
            CURLcode response = curl_easy_perform(curl);

            return (response == CURLE_OK);
        }

        return false;
}

stringstream& HTTPDownloader::download(const std::string& url, stringstream& out) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    return out;
}
