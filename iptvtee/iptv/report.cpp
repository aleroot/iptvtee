//
//  report.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 05/02/22.
//

#include "report.hpp"
#include "StringUtils.hpp"
#include <sstream>
#include <unordered_map>

Report::Report(std::string name, Playlist items, std::vector<Rank> ranks) : title(name){
    for(int i = 0; i < ranks.size(); i++) {
        ExportableItem entry = { .item = items[i], .rank = ranks[i] };
        entries.push_back(entry);
    }
}

template<typename T>
concept Writer = requires(T t, std::ostream& os, const std::string& title, const ExportableItem& item) {
    { T(os, title) };  // Require constructor with stream and title
    { t.addRow(item) } -> std::convertible_to<bool>;
};

template<Writer W>
bool exportWithWriter(std::ostream& outFile, const std::vector<ExportableItem>& entries, const std::string& title) {
    W writer(outFile, title);
    for (const auto& item : entries) {
        if (!writer.addRow(item)) {
            return false;
        }
    }
    return true;
}

bool Report::exportTo(std::ostream& outFile, Format fmt) const {
    // Define a map of format to writer type
    static const std::unordered_map<Format, std::function<bool(std::ostream&, const std::vector<ExportableItem>&, const std::string&)>> exporters = {
        {Format::M3U, exportWithWriter<Exporters::M3UWriter>},
        {Format::ENIGMA, exportWithWriter<Exporters::EnigmaWriter>},
        {Format::URL, exportWithWriter<Exporters::URLWriter>},
        {Format::CSV, [](std::ostream& outFile, const std::vector<ExportableItem>& entries, const std::string& title) {
            Exporters::CSVWriter csv(outFile);
            for(ExportableItem item : entries) {
                std::stringstream score;
                score << std::fixed << std::setprecision(2) << (item.rank.score * 100) << "%";
                std::vector<std::string> row = { item.item.name, item.item.text, item.item.group, item.item.logo, item.item.url, score.str()};
                csv.addRow(row);
            }
            
            return csv.count() == entries.size();
        }},
        {Format::JSON, [](std::ostream& outFile, const std::vector<ExportableItem>& entries, const std::string& title) {
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
        }}
    };

    // Use the map to get the correct exporter, or use TXTWriter as default
    auto exporter = exporters.find(fmt);
    if (exporter != exporters.end())
        return exporter->second(outFile, entries, title);
    else
        return exportWithWriter<Exporters::TXTWriter>(outFile, entries, title);
    
}

std::string Report::mime(Format fmt) {
    switch(fmt) {
        case Format::TXT: return "text/plain";
        case Format::URL: return "text/uri-list";
        case Format::CSV: return "text/csv";
        case Format::JSON: return "application/json";
        case Format::M3U: return "application/vnd.apple.mpegurl";
        case Format::ENIGMA: return "application/x-enigma2-bouquet";
        default: return "application/octet-stream";
    }
}
