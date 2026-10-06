#ifndef PRINTABLE_H
#define PRINTABLE_H

#include <string>

// Abstract base class: Trade, Holding, NewsItem derive from this for polymorphic dispatch in Dashboard
class Printable {
public:
    virtual ~Printable() = default;
    virtual std::string toString() const = 0;
};

#endif
