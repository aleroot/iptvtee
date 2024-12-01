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
        return (os << e.item << " -> " << e.rank);
    }
};

static std::string DEFAULT_EXPORT_TITLE = "IPTVTEE - EXPORT";

namespace Exporters {

    class TXTWriter
    {
        std::ostream& file;
    public:
        explicit TXTWriter(std::ostream& file, const std::string& name = DEFAULT_EXPORT_TITLE);
        bool addRow(const ExportableItem& row);
    };

    class URLWriter
    {
        std::ostream& file;
    public:
        explicit URLWriter(std::ostream& file, const std::string& name = DEFAULT_EXPORT_TITLE);
        bool addRow(const ExportableItem& row);
    };

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
        explicit M3UWriter(std::ostream& file, const std::string& name = DEFAULT_EXPORT_TITLE);
        bool addRow(ExportableItem row);
    };

    class EnigmaWriter
    {
        std::ostream& file;
        std::string bouquetName;
        int number = 0;
    public:
        explicit EnigmaWriter(std::ostream& file, const std::string& name = DEFAULT_EXPORT_TITLE);
        bool addRow(const ExportableItem& row);
    };

}

#endif /* exporter_hpp */
