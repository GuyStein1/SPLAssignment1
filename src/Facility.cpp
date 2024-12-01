#include "Facility.h"
#include <iostream>
#include <string>  // For std::string
#include <sstream> // For std::ostringstream


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

Facility :: Facility(const FacilityType &type, const string &settlementName)
    // The default copy constructor of FacilityType is safe as it has no dynamic memory, preventing leaks or double deletions.
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




//test!!!!!!!!!!!!!!!!!!!!!!!!

// using namespace std;

// int main() {
//     try {
//         // Test FacilityType Constructor
//         FacilityType ft("Hospital", FacilityCategory::LIFE_QUALITY, 5, 10, 8, 7);
//         cout << "FacilityType created successfully!" << endl;
//         cout << "Name: " << ft.getName() << endl;
//         cout << "Cost: " << ft.getCost() << endl;
//         cout << "Life Quality Score: " << ft.getLifeQualityScore() << endl;
//         cout << "Economy Score: " << ft.getEconomyScore() << endl;
//         cout << "Environment Score: " << ft.getEnvironmentScore() << endl;

//         // Test Facility Constructor
//         Facility f1("Hospital", "SettlementA", FacilityCategory::LIFE_QUALITY, 5, 10, 8, 7);
//         cout << "\nFacility created successfully using parameterized constructor!" << endl;
//         cout << f1.toString() << endl;

//         // Test Facility Constructor with FacilityType
//         Facility f2(ft, "SettlementB");
//         cout << "\nFacility created successfully using FacilityType!" << endl;
//         cout << f2.toString() << endl;

//         // Test Facility Step Method
//         cout << "\nTesting step() for Facility f1:" << endl;
//         while (f1.getTimeLeft() > 0) {
//             f1.step();
//             cout << f1.toString() << endl;
//         }
//         cout << "Final Status: " << f1.toString() << endl;

//         // Test setStatus Method
//         f1.setStatus(FacilityStatus::OPERATIONAL);
//         cout << "\nUpdated Status of f1: " << f1.toString() << endl;

//         // Test createFacilityCategory
//         cout << "\nTesting createFacilityCategory:" << endl;
//         cout << "Category for 0: " << static_cast<int>(createFacilityCategory(0)) << endl;
//         cout << "Category for 1: " << static_cast<int>(createFacilityCategory(1)) << endl;
//         cout << "Category for 2: " << static_cast<int>(createFacilityCategory(2)) << endl;

//         // Test invalid category
//         cout << "Testing invalid category: ";
//         cout << static_cast<int>(createFacilityCategory(3)) << endl; // Should throw an exception
//     } catch (const exception &e) {
//         cout << "Exception: " << e.what() << endl;
//     }

//     return 0;
// }