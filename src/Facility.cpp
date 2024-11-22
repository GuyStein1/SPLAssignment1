#include "Facility.h"
#include <iostream>

FacilityCategory createFacilityCategory(int value) {
    switch (value) {
        case 0: return FacilityCategory::LIFE_QUALITY;
        case 1: return FacilityCategory::ECONOMY;
        case 2: return FacilityCategory::ENVIRONMENT;
        default:
            throw std::invalid_argument("Invalid value for FacilityCategory");
    }
}