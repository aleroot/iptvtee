//
//  Evaluable.hpp
//  iptvtee
//
//  Created by Alessio Pollero on 23/05/21.
//

#ifndef Evaluable_hpp
#define Evaluable_hpp

#include <stdio.h>

struct Rank {
    int elements = 0;
    float score = 0;
    long long value = 0;
    long long max_value = 0;
    
    Rank& operator+(const Rank& rhs) {
        elements = ((elements < 1 ? 1 : elements) + ((rhs.elements < 1) ? 1 : rhs.elements));
        max_value = max_value + ((rhs.max_value - max_value) / elements);
        value = value + ((rhs.value - value) / elements);
        score = score + ((rhs.score - score) / elements);
        return *this;
    }
    
    bool empty() {
        return elements <= 0 && score <=0 && value == 0 && max_value == 0;
    }
};

#include <ostream>
#include <iomanip>

inline std::ostream &operator<<(std::ostream &os, const Rank &rank) {
  os << "Rank{ score = " << std::fixed << std::setprecision(2) << (rank.score * 100) << "%, value = " << rank.value << ", max = " << rank.max_value << ", count = " << rank.elements << " }";
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
