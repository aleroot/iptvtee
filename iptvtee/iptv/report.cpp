//
//  report.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 05/02/22.
//

#include "report.hpp"
#include "StringUtils.hpp"
#include <sstream>

Report::Report(Playlist items, std::vector<Rank> ranks) {
    for(int i = 0; i < ranks.size(); i++) {
        ExportableItem entry = { .item = items[i], .rank = ranks[i] };
        entries.push_back(entry);
    }
}

bool Report::exportTo(std::ostream& outFile, Format fmt) const {
    switch (fmt) {
        case Format::CSV:
        {
            Exporters::CSVWriter csv(outFile);
            for(ExportableItem item : entries) {
                std::stringstream score;
                score << std::fixed << std::setprecision(2) << (item.rank.score * 100) << "%";
                std::vector<std::string> row = { item.item.name, item.item.text, item.item.group, item.item.logo, item.item.url, score.str()};
                csv.addRow(row);
            }
            
            return csv.count() == entries.size();
        }
        case Format::JSON:
        {
            auto length = entries.size() -1;
            std::stringstream json;
            json << "[" << std::endl;
            
            int i = 0;
            for(; i <= length; i++) {
                ExportableItem item = entries[i];
                json << "\t{" << std::endl;
                json << "\t\t\"item\": {" << std::endl;
                json << "\t\t\t\"id\": " << item.item.id << "," << std::endl;
                json << "\t\t\t" << StringUtils::quote("name") << ": " << StringUtils::quote(item.item.name) << "," << std::endl;
                json << "\t\t\t" << StringUtils::quote("text") << ": " << StringUtils::quote(item.item.text) << "," << std::endl;
                json << "\t\t\t" << StringUtils::quote("group") << ": " << StringUtils::quote(item.item.group) << "," << std::endl;
                json << "\t\t\t" << StringUtils::quote("logo") << ": " << StringUtils::quote(item.item.logo) << "," << std::endl;
                json << "\t\t\t" << StringUtils::quote("url") << ": " << StringUtils::quote(item.item.url) << std::endl;
                json << "\t\t}," << std::endl;
                json << "\t\t\"rank\": {" << std::endl;
                json << "\t\t\t" << StringUtils::quote("elements") << ": " << item.rank.elements << "," << std::endl;
                json << "\t\t\t" << StringUtils::quote("value") << ": " << item.rank.value << "," << std::endl;
                json << "\t\t\t" << StringUtils::quote("max_value") << ": " << item.rank.max_value << "," << std::endl;
                json << "\t\t\t" << StringUtils::quote("score") << ": " << std::fixed << std::setprecision(2) << (item.rank.score * 100) << std::endl;
                json << "\t\t}" << std::endl;
                json << "\t}" << ((i < length) ? "," : "") << std::endl;
            }
            json << "]";
            outFile << json.str();
            return i == length;
        }
        case Format::M3U:
        {
            Exporters::M3UWriter m3u(outFile);
            for(ExportableItem item : entries)
                if(!m3u.addRow(item))
                    return false;
            return true;
        }
    }
    
    return false;
}
