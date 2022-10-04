//
//  FileUtils.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 11/06/22.
//

#ifndef FileUtils_hpp
#define FileUtils_hpp

#include <glob.h> // glob(), globfree()
#include <string.h> // memset()
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>

namespace FileUtils
{
    std::vector<std::string> glob(const std::string& pattern) {
        using namespace std;
        vector<string> filenames;
        // glob struct resides on the stack
        glob_t glob_result;
        memset(&glob_result, 0, sizeof(glob_result));

        // do the glob operation
        int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
        if(return_value != 0) {
            globfree(&glob_result);
            filenames.push_back(pattern);
            return filenames;
        }
        
        for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
            filenames.push_back(string(glob_result.gl_pathv[i]));
        }

        // cleanup
        globfree(&glob_result);

        // done
        return filenames;
    }

    bool getline_async(std::istream& is, std::string& str, char delim = '\n') {
        std::ios_base::sync_with_stdio(false);
        static std::string lineSoFar;
        char inChar;
        long charsRead = 0;
        bool lineRead = false;
        str = "";

        do {
            charsRead = is.readsome(&inChar, 1);
            if (charsRead == 1) {
                // if the delimiter is read then return the string so far
                if (inChar == delim) {
                    str = lineSoFar;
                    lineSoFar = "";
                    lineRead = true;
                } else {  // otherwise add it to the string so far
                    lineSoFar.append(1, inChar);
                }
            }
        } while (charsRead != 0 && !lineRead);

        return lineRead;
    }

}

#endif /* FileUtils_hpp */
