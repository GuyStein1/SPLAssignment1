#include "Facility.h"
#include <iostream>

//FacilityType implementation:

// Costructor for FacilityType
FacilityType :: FacilityType(const string &name, const FacilityCategory category, const int price, 
                            const int lifeQuality_score, const int economy_score, const int environment_score)
    : name(name), category(category), price(price), 
      lifeQuality_score(lifeQuality_score), economy_score(economy_score), environment_score(environment_score) {}

const string &FacilityType :: getName() const {
    return name;
    }

int FacilityType :: getCost() const {
    return price;
    }

int FacilityType :: getLifeQualityScore() const {
    return lifeQuality_score;
    }

int FacilityType :: getEnvironmentScore() const {
    return environment_score;
    }

int FacilityType :: getEconomyScore() const {
    return economy_score;
    }

FacilityCategory FacilityType :: getCategory() const {
    return category;
    }


//Facilty implementation:

Facility :: Facility(const string &name, const string &settlementName, const FacilityCategory category,
                   const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score),
      settlementName(settlementName), status(FacilityStatus::UNDER_CONSTRUCTIONS), timeLeft(price) {}

Facility :: Facility(FacilityType &type, const string &settlementName)
    : FacilityType(type), settlementName(settlementName), status(FacilityStatus::UNDER_CONSTRUCTIONS), timeLeft(price) {}


const string &Facility::getSettlementName() const {
    return settlementName;
}

const FacilityStatus &Facility::getStatus() const {
    return status;
}

const int Facility::getTimeLeft() const {
    return timeLeft;
}

void Facility::setStatus(FacilityStatus status){
    this->status = status;
}

FacilityStatus Facility::step() {
    if (timeLeft > 0) {
        timeLeft--;
        if (timeLeft == 0) {
            status = FacilityStatus::OPERATIONAL;
        }
    }
    return status;
}

const string Facility::toString() const {
    string statusStr = (status == FacilityStatus::UNDER_CONSTRUCTIONS) ? "Under Construction" : "Operational";
    return "Facility(Name: " + getName() + ", Settlement: " + settlementName + ", Status: " + statusStr + ")";
}

//Helper function to convert int to FacilityCategory
FacilityCategory createFacilityCategory(int value) {
    switch (value) {
        case 0: return FacilityCategory::LIFE_QUALITY;
        case 1: return FacilityCategory::ECONOMY;
        case 2: return FacilityCategory::ENVIRONMENT;
        default:
            throw std::invalid_argument("Invalid value for FacilityCategory");
    }
}