//
//  StringUtils.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 28/01/22.
//

#ifndef StringUtils_hpp
#define StringUtils_hpp
#include <string>
#include <regex>
#include <stdexcept>
namespace StringUtils
{
    const char* const whitespaceDelimiters = " \t\n\r\f\v";
    inline void trim(std::string& str)
    {
        str.erase(str.find_last_not_of(whitespaceDelimiters) + 1);
        str.erase(0, str.find_first_not_of(whitespaceDelimiters));
    }

    inline bool extractPrefixAndStrip(std::string& str, std::string& prefix) {
        size_t pos = str.find('=');
        if (pos != std::string::npos) {
            prefix = str.substr(0, pos);
            str = str.substr(pos + 1); // Modify the original string to strip out the field
            return true;
        }
        prefix = ""; // Set an empty string if no prefix found
        return false; // Return false if no '=' is found
    }

    inline long long toNumber(std::string str)
    {
        try {
            return std::stoll(str);
        } catch (const std::invalid_argument& e) {
            return -1;
        } catch (const std::out_of_range& e) {
            return -10;
        }
    }

    inline std::string wordBefore(std::string input, int startIndex) {
        for(int i = startIndex-1; i >= 0; i--) {
            char cc = input.at(i);
            for(int y = 0; y < 6; y++) {
                const char c = whitespaceDelimiters[y];
                if(cc == c) {
                    const int s = i+1;
                    return input.substr(s,startIndex - s);
                }
            }
        }
        return input.substr(0,startIndex);
    }

    inline std::string phraseAfter(std::string input, int startIndex) {
        const int start = startIndex + 1;
        if(start < input.length()) {
            const bool inquote = input.at(start) == '"';
            for(int i = start; i < input.length(); i++) {
                char cc = input.at(i);
                if(i > start && inquote && cc == '"')
                    return input.substr(start+1, i - (start+1));
                for(int y = 0; !inquote && y < 6; y++) {
                    const char c = whitespaceDelimiters[y];
                    if(cc == c && !inquote)
                        return input.substr(start, i - start);
                }
            }
        }
        return input.substr(start, input.length() - start);
    }

    inline std::string quote(std::string input, const char quote = '"') {
        return quote + input + quote;
    }

    inline bool contains(const std::string & strHaystack, const std::string & strNeedle)
    {
      auto it = std::search(
        strHaystack.begin(), strHaystack.end(),
        strNeedle.begin(),   strNeedle.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
      );
      return (it != strHaystack.end() );
    }

    inline std::vector<std::string> split(const std::string &s, const std::regex &sep_regex = std::regex{"\\s+"}) {
      std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
      std::sregex_token_iterator end;
      return {iter, end};
    }

}

#endif /* StringUtils_hpp */
