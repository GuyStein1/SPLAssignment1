#include "Settlement.h"
#include <iostream>



SettlementType createSettlementType(int value) {
    switch (value) {
        case 0: return SettlementType::VILLAGE;
        case 1: return SettlementType::CITY;
        case 2: return SettlementType::METROPOLIS;
        default:
            throw std::invalid_argument("Invalid value for SettlementType");
    }
}

