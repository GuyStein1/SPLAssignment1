#include "Facility.h"
#include <iostream>
#include <sstream> // For std::ostringstream


//-----------FacilityType implementation-----------

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


//-----------Facilty implementation-----------

// Constructors
Facility :: Facility(const string &name, const string &settlementName, const FacilityCategory category,
                   const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score),
      settlementName(settlementName), status(FacilityStatus::UNDER_CONSTRUCTIONS), timeLeft(price) {}

Facility :: Facility(const FacilityType &type, const string &settlementName)
    // The default copy constructor of FacilityType is safe as it has no dynamic memory, preventing leaks or double deletions.
    : FacilityType(type), settlementName(settlementName), status(FacilityStatus::UNDER_CONSTRUCTIONS), timeLeft(price) {}

// Getter methods
const string &Facility::getSettlementName() const {
    return settlementName;
}

const FacilityStatus &Facility::getStatus() const {
    return status;
}

const int Facility::getTimeLeft() const {
    return timeLeft;
}

// Set status method
void Facility::setStatus(FacilityStatus status){
    this->status = status;
}

FacilityStatus Facility::step() {
    if (timeLeft > 0) {
        timeLeft--;
    }
    if (timeLeft == 0) {
        status = FacilityStatus::OPERATIONAL;
    }
    return status;
}

const string Facility::toString() const {
    std::ostringstream output;

    // Add facility details
    string statusStr = (status == FacilityStatus::UNDER_CONSTRUCTIONS) ? "Under Construction" : "Operational";
    string categoryStr = 
        (category == FacilityCategory::LIFE_QUALITY) ? "Life Quality" :
        (category == FacilityCategory::ECONOMY) ? "Economy" : "Environment";

    output << "Facility(Name: " << getName()
           << ", Settlement: " << settlementName
           << ", Category: " << categoryStr
           << ", Cost: " << getCost()
           << ", Life Quality Score: " << getLifeQualityScore()
           << ", Economy Score: " << getEconomyScore()
           << ", Environment Score: " << getEnvironmentScore()
           << ", Time Left: " << timeLeft
           << ", Status: " << statusStr << ")";

    return output.str();
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
