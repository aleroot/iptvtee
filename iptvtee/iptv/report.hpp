//
//  report.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 05/02/22.
//

#ifndef report_hpp
#define report_hpp

#include "exporter.hpp"

enum class Format{ CSV, JSON, M3U };

class Report {
    std::vector<ExportableItem> entries;
public:
    Report(Playlist items, std::vector<Rank> ranks);
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
