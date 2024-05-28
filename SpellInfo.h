#pragma once
#include <string>

struct SpellInfo {
public:
    std::string name;
    std::string nameAlt;
    std::string icon;
    int flags;
    float delay;
    float castRange;
    float castRadius;
    float width;
    float height;
    float speed;
    float travelTime;
    std::string type;
    bool projectDestination;
};