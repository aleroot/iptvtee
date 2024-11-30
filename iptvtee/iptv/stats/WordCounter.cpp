//
//  WordCounter.cpp
//  iptvtee
//
//  Created by Alessio Pollero on 27/11/24.
//

#include "WordCounter.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <cctype>
#include <cmath>

void WordCounter::process(std::string_view text) {
    std::istringstream stream{std::string(text)};
    std::string word;
    
    while (stream >> word) {
        // Normalize word: convert to lowercase and trim punctuation
        normalizeWord(word);
        if (word.length() > minWordLength) {
            ++wordCounts[word];
            ++totalWords;
        }
    }
}

std::string WordCounter::top(int wordCount, double threshold) const {
    if (wordCount <= 0 || wordCounts.empty() || totalWords == 0) {
        return {};
    }
    
    // Create vector of word-count pairs for sorting
    std::vector<std::pair<std::string, size_t>> words;
    words.reserve(wordCounts.size());
    
    // Find maximum word count (the most frequent word)
    size_t maxCount = 0;
    for (const auto& [word, count] : wordCounts) {
        maxCount = std::max(maxCount, count);
    }
    
    if (maxCount == 0) {
        return {};
    }
    
    // Calculate the threshold based on the maximum word count and the desired percentage difference
    const size_t minCount = static_cast<size_t>(std::ceil(threshold * maxCount / 100.0));
    
    // Collect words that meet the threshold based on relative frequency
    for (const auto& [word, count] : wordCounts) {
        if (count >= minCount) {
            words.emplace_back(word, count);
        }
    }
    
    // If no words meet the threshold, return empty result
    if (words.empty()) {
        return {};
    }
    
    // Sort words by frequency (descending), and alphabetically for ties
    std::sort(words.begin(), words.end(),
        [](const auto& a, const auto& b) {
            return (a.second > b.second) ||
                   (a.second == b.second && a.first < b.first);
        });
    
    // Calculate the number of words to return (up to wordCount)
    const size_t resultSize = std::min(static_cast<size_t>(wordCount), words.size());
    
    std::string result;
    result.reserve(resultSize * 10); // Approximate space for efficiency
    result += words[0].first;
    
    for (size_t i = 1; i < resultSize; ++i) {
        result += ' ' + words[i].first;
    }
    
    return result;
}

void WordCounter::clear() {
    wordCounts.clear();
    totalWords = 0;
}

size_t WordCounter::getTotalWords() const noexcept {
    return totalWords;
}

size_t WordCounter::getUniqueWords() const noexcept {
    return wordCounts.size();
}

void WordCounter::normalizeWord(std::string& word) {
    // Convert to uppercase
    std::transform(word.begin(), word.end(), word.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    
    // Trim punctuation from both ends
    const auto isNotPunct = [](unsigned char c) {
        return !std::ispunct(c);
    };
    
    const auto first = std::find_if(word.begin(), word.end(), isNotPunct);
    const auto last = std::find_if(word.rbegin(), word.rend(), isNotPunct).base();
    
    if (first >= last) {
        word.clear();
    } else {
        word = std::string(first, last);
    }
}
