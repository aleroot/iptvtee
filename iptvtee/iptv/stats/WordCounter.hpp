//
//  WordCounter.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 27/11/24.
//

#ifndef WordCounter_hpp
#define WordCounter_hpp

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/// A utility class that processes text to count and analyze word frequencies.
///
/// Use WordCounter to process text and retrieve statistics about word frequencies. It normalizes words
/// by converting them to uppercase and removing punctuation.
///
class WordCounter {
public:
    explicit WordCounter(size_t minLength = 1) : minWordLength(minLength) {}
    void process(std::string_view text);
    
    /// Returns the top `wordCount` most frequent words from the word counter, filtered by a relative frequency threshold.
    ///
    /// ### Algorithm:
    /// 1. **Maximum Frequency**: The method first calculates the frequency of the most frequent word in the dataset (the word with the highest count).
    /// 2. **Threshold Calculation**:
    ///    - The threshold is computed as a percentage of the most frequent word's count.
    ///    - A word's frequency must be greater than or equal to `threshold%` of the maximum frequency to be included in the result.
    ///    - For example, with a threshold of 50%, any word with a frequency greater than or equal to half the most frequent word's count will be included.
    /// 3. **Filtering**: Words that have a frequency less than the threshold are excluded from the result.
    /// 4. **Sorting**: The remaining words are sorted by frequency (in descending order). In the case of ties, words are sorted alphabetically in ascending order.
    /// 5. **Return the result**: The method returns the top `wordCount` words that meet the threshold, up to the number of words that match. If fewer words meet the threshold, the method returns as many as possible. If no words meet the threshold, the result will be an empty string.
    ///
    /// - Parameters:
    ///     - Parameter wordCount: The maximum number of words to return. The method will return up to `wordCount` words that meet the threshold. If fewer words meet the threshold, it returns as many as possible.
    ///     - Parameter threshold: A percentage threshold (from 0 to 100) representing the minimum relative frequency for words to be included in the result. For example, a threshold of 50% means words must have at least 50% of the frequency of the most frequent word to be included.
    ///
    /// - Returns: A string containing the top words that meet the relative frequency threshold, separated by spaces. If no words meet the threshold, an empty string is returned.
    std::string top(int wordCount, double threshold = 0.0) const;

    void clear();
    size_t getTotalWords() const noexcept;
    size_t getUniqueWords() const noexcept;
private:
    std::unordered_map<std::string, size_t> wordCounts;
    size_t totalWords{0};
    size_t minWordLength;
    static void normalizeWord(std::string& word);
};

#endif /* WordCounter_hpp */
