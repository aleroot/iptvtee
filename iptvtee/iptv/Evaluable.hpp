//
//  Evaluable.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 23/05/21.
//

#ifndef Evaluable_hpp
#define Evaluable_hpp

#include <stdio.h>
#include <string>
#include <algorithm>

struct Quality {
    int width = 0;
    int height = 0;
    int bitrate = 0;  // in bits per second
    float fps = 0.0f;
    std::string codec;
    bool empty() const {
        return width == 0 && height == 0 && bitrate == 0;
    }
};

struct Rank {
    int elements = 0;
    float score = 0;
    long long value = 0;
    long long max_value = 0;
    Quality quality;
    
    Rank& operator+(const Rank& rhs) {
        elements = ((elements < 1 ? 1 : elements) + ((rhs.elements < 1) ? 1 : rhs.elements));
        max_value = max_value + ((rhs.max_value - max_value) / elements);
        value = value + ((rhs.value - value) / elements);
        score = score + ((rhs.score - score) / elements);
        quality.width = std::max(quality.width, rhs.quality.width);
        quality.height = std::max(quality.height, rhs.quality.height);
        quality.bitrate = std::max(quality.bitrate, rhs.quality.bitrate);
        quality.fps = std::max(quality.fps, rhs.quality.fps);
        if (quality.codec.empty() && !rhs.quality.codec.empty())
            quality.codec = rhs.quality.codec;
        
        return *this;
    }
    
    bool empty() const {
        return elements <= 0 && score <= 0 && value == 0 && max_value == 0;
    }
};

#include <ostream>
#include <iomanip>

inline std::ostream& operator<<(std::ostream& os, const Quality& quality) {
    os << "{ resolution = " << quality.width << "x" << quality.height;
    if (quality.bitrate > 0)
        os << "@" << (quality.bitrate / 1000) << "kbps";
    if (quality.fps > 0)
        os << ", fps = " << std::fixed << std::setprecision(0) << quality.fps;
    if (!quality.codec.empty())
           os << ", codec = " << quality.codec;
    os << "}";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const Rank &rank) {
    os << "Rank{ score = " << std::fixed << std::setprecision(2) << (rank.score * 100) << "%, value = " << rank.value << ", max = " << rank.max_value << ", count = " << rank.elements;
    if(!rank.quality.empty())
        os << ", quality = " << rank.quality;
    os << " }";
  return os;
}

inline Rank& operator+=(Rank& lhs, Rank& rhs) {
    return (lhs + rhs);
}

// Define the Evaluable interface.
class Evaluable {
protected:
     // virtual destructor is required if the object may
     // be deleted through a pointer to Evaluable
    virtual ~Evaluable() {}
public:
    virtual Rank evaluate() = 0;
};

#endif /* Evaluable_hpp */
