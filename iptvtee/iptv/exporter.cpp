//
//  exporter.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 17/06/22.
//

#include "exporter.hpp"
#include "Utils.hpp"
#include <sstream>

Exporters::TXTWriter::TXTWriter(std::ostream& file) : file(file) {
    // No header needed for this simple format
}

bool Exporters::TXTWriter::addRow(const ExportableItem& row) {
    if (row.item.url.empty()) {
        return false;
    }
    std::string description = row.item.name.empty() ? row.item.text : row.item.name;
    if(description.empty())
        return false;
    file << row.item.url << " -> " << description << std::endl;
    return true;
}

Exporters::URLWriter::URLWriter(std::ostream& file) : file(file) { }

bool Exporters::URLWriter::addRow(const ExportableItem& row) {
    if (row.item.url.empty()) {
        return false;
    }
    
    file << row.item.url << std::endl;
    return true;
}

int Exporters::CSVWriter::count() {
    return linesCount;
}

bool Exporters::CSVWriter::addRow(std::vector<std::string> row) {
    int count = linesCount;
    enquoteIfNecessary(row);
    addRow(row.begin(), row.end());
    return linesCount == ++count;
}

void Exporters::CSVWriter::enquoteIfNecessary(std::vector<std::string> &row) {
    for(std::string& elem : row) {
        if (elem.find(delimeter) != std::string::npos)
            elem = "\"" + elem + "\"";
    }
}

/*
 * This Function accepts a range and appends all the elements in the range
 * to the last row, seperated by delimeter (Default is comma)
 */
 template<typename T>
 void Exporters::CSVWriter::addRow(T first, T last)
{
    // Iterate over the range and add each lement to file seperated by delimeter.
    while (first != last)
    {
        file << *first;
        if (++first != last)
            file << delimeter;
    }
    file << std::endl;
    linesCount++;
}

Exporters::M3UWriter::M3UWriter(std::ostream& file) : file(file) {
    file << "#EXTM3U" << std::endl;
}

bool Exporters::M3UWriter::addRow(ExportableItem row) {
    if(row.item.url.empty())
        return false;
    
    file << "#EXTINF:-1";
    if(row.item.id > 0)
        file << " tvg-id=\"" << row.item.id << "\"";
    if(!row.item.name.empty())
        file << " tvg-name=\"" << row.item.name << "\"";
    if(!row.item.logo.empty())
        file << " tvg-logo=\"" << row.item.logo << "\"";
    if(!row.item.group.empty())
        file << " group-title=\"" << row.item.group << "\"";
    file << "," << row.item.text << "(Rank:" << std::fixed << std::setprecision(0) << (row.rank.score * 100) << "%)"  << std::endl;
    file << row.item.url << std::endl;
    return true;
}

Exporters::EnigmaWriter::EnigmaWriter(std::ostream& file, const std::string& name)
    : file(file), bouquetName(name) {
        file << "#NAME " << bouquetName << std::endl;
}

bool Exporters::EnigmaWriter::addRow(const ExportableItem& row) {
    if (row.item.url.empty()) {
        return false;
    }

    // Convert URL to Enigma 2 service reference format
    std::stringstream serviceRef;
    std::string format_prefix = "1:0:1:";  // Default to .ts format
    if (row.item.url.ends_with(".m3u") || row.item.url.ends_with(".m3u8")) {
        format_prefix = "4097:0:1:";  // Non-.ts format (e.g., m3u8)
    }
    serviceRef << format_prefix << std::hex << ++number
               << ":62a8:4f82:0:0:0:0:" << UrlUtils::url_encode(row.item.url);

    file << "#SERVICE " << serviceRef.str() << ":" << std::endl;
    
    // Write description
    std::string description = row.item.name.empty() ? row.item.text : row.item.name;
    if (!description.empty()) {
        file << "#DESCRIPTION " << description << std::endl;
    }

    return true;
}
