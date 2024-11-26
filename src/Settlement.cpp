#include "Settlement.h"
#include <iostream>

Settlement ::Settlement(const string &name, SettlementType type)
    : name(name), type(type) {}

const string &Settlement :: getName() const {
    return name;
}

SettlementType Settlement :: getType() const {
    return type;
}

const string Settlement :: toString() const {
    if (type == SettlementType :: VILLAGE) {
        return "Settlement(Name: " + name + ", Type: VILLAGE)";
    } else if (type == SettlementType :: CITY){
        return "Settlement(Name: " + name + ", Type: CITY)";
    } else {
        return "Settlement(Name: " + name + ", Type: METROPOLIS)";
    }
}

//Convert in to SettlementType
SettlementType createSettlementType(int value){
    switch (value)
    {
    case 0:
        return SettlementType::VILLAGE;
    case 1:
        return SettlementType::CITY;
    case 2:
        return SettlementType::METROPOLIS;
    default:
        throw std::invalid_argument("Invalid value for SettlementType");
    }
}

//Implement isEqual
bool Settlement::isEqual(const Settlement& other) const {
    return (this->getName() == other.getName() && this->getType() == other.getType());
}
