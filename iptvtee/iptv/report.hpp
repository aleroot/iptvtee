//
//  report.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 05/02/22.
//

#ifndef report_hpp
#define report_hpp

#include "exporter.hpp"

enum class Format{ TXT, URL, CSV, JSON, M3U, ENIGMA };

class Report {
    std::vector<ExportableItem> entries;
    std::string title;
public:
    Report(std::string name, Playlist items, std::vector<Rank> ranks);
    static std::string mime(Format fmt);
    bool exportTo(std::ostream& outFile, Format fmt = Format::M3U) const;
    friend std::ostream& operator<< (std::ostream& os, const Report& r)  {
        os << std::endl;
        for(ExportableItem item : r.entries) {
            os << item << std::endl;
        }
        return os;
    }
};
#endif /* report_hpp */
