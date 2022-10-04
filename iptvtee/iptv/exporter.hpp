//
//  exporter.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 17/06/22.
//

#ifndef exporter_hpp
#define exporter_hpp

#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>

#include "Evaluable.hpp"
#include "playlist.hpp"

struct ExportableItem {
    PlaylistItem item;
    Rank rank;
    friend std::ostream& operator<< (std::ostream &os, const ExportableItem &e) {
        return (os << e.item << " -> " << e.rank << std::endl);
    }
};

namespace Exporters {

    class CSVWriter
    {
        std::ostream& file;
        std::string delimeter;
        int linesCount;
        template<typename T>
        void addRow(T first, T last);
    protected:
        
    public:
        CSVWriter(std::ostream& file, std::string delm = ",") :
        file(file), delimeter(delm), linesCount(0)
        {}
        int count();
        void enquoteIfNecessary(std::vector<std::string> &row);
        bool addRow(std::vector<std::string> row);
    };

    class M3UWriter
    {
        std::ostream& file;
    public:
        explicit M3UWriter(std::ostream& file);
        bool addRow(ExportableItem row);
    };

}

#endif /* exporter_hpp */
