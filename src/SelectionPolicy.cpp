#include <stdexcept>
#include "SelectionPolicy.h"
#include <climits>
#include <algorithm> // For std::max and std::min
#include <string>
#include <sstream>

//NaiveSelection implementation
NaiveSelection::NaiveSelection() : lastSelectedIndex(-1) {}

const FacilityType& NaiveSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    // Check for empty facilities list or if all facilities have already been selected
    if (facilitiesOptions.empty()) {
        throw std::runtime_error("No facilities available for selection.");
    }

    // Increment index and select the next facility
    lastSelectedIndex =(lastSelectedIndex + 1) % facilitiesOptions.size();
    return facilitiesOptions[lastSelectedIndex];
}

const string NaiveSelection::toString() const {
    return "nve";
}

NaiveSelection* NaiveSelection::clone() const {
    return new NaiveSelection(*this);
}

//BalancedSelection implementation
BalancedSelection::BalancedSelection(int lifeQuality, int economy, int environment)
    : LifeQualityScore(lifeQuality), EconomyScore(economy), EnvironmentScore(environment) {}

const FacilityType& BalancedSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw std::runtime_error("No facilities available for selection.");
    }

    int minDifference = INT_MAX; //Store the min difference found
    const FacilityType* selectedFacility = nullptr;

    //Iterate over all facilities to find the best option for BalancedSelection
    for (const FacilityType& facility : facilitiesOptions) {
        //Store current score changes for each facility
        int lifeScore = LifeQualityScore + facility.getLifeQualityScore();
        int economyScore = EconomyScore + facility.getEconomyScore();
        int environmentScore = EnvironmentScore + facility.getEnvironmentScore();

        //Find difference if current facility was chosen
        int maxScore = std::max({lifeScore, economyScore, environmentScore});
        int minScore = std::min({lifeScore, economyScore, environmentScore});
        int difference = maxScore - minScore;

        //If current difference is smaller choose facilty
        if (difference < minDifference) {
            minDifference = difference;
            selectedFacility = &facility;
        }
    }

    return *selectedFacility;
}

// Getter methods to retrieve scores
int BalancedSelection::getLifeQualityScore() const {
    return LifeQualityScore;
}

int BalancedSelection::getEconomyScore() const {
    return EconomyScore;
}

int BalancedSelection::getEnvironmentScore() const {
    return EnvironmentScore;
}

const std::string BalancedSelection::toString() const {
    return "bal";
}

BalancedSelection* BalancedSelection::clone() const {
    return new BalancedSelection(*this);
}

// Setter methods to update scores
void BalancedSelection::setLifeQualityScore(int score) { LifeQualityScore = score; }
void BalancedSelection::setEconomyScore(int score) { EconomyScore = score; }
void BalancedSelection::setEnvironmentScore(int score) { EnvironmentScore = score; }


//EconomySelection implementation
EconomySelection::EconomySelection() : lastSelectedIndex(-1) {}

const FacilityType& EconomySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw std::runtime_error("No facilities available for selection.");
    }

    size_t i = (lastSelectedIndex + 1) % facilitiesOptions.size();

    // Iterate until we find the next facility in the ECONOMY category.
    while (facilitiesOptions[i].getCategory() != FacilityCategory::ECONOMY) {
        i = (i + 1) % facilitiesOptions.size();
    }

    lastSelectedIndex = i; // Update the index for the next selection.
    return facilitiesOptions[i];
}

const string EconomySelection::toString() const {
    return "eco";
}

EconomySelection* EconomySelection::clone() const {
    return new EconomySelection(*this);
}


//SustainabilitySelection implementation
SustainabilitySelection::SustainabilitySelection() : lastSelectedIndex(-1) {}

const FacilityType& SustainabilitySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw std::runtime_error("No facilities available for selection.");
    }
    
    size_t i = (lastSelectedIndex + 1) % facilitiesOptions.size();

    // Iterate until we find the next facility in the ENVIRONMENT category.
    while (facilitiesOptions[i].getCategory() != FacilityCategory::ENVIRONMENT) {
        i = (i + 1) % facilitiesOptions.size();
    }

    lastSelectedIndex = i; // Update the index for the next selection.
    return facilitiesOptions[i];
}

const string SustainabilitySelection::toString() const {
    return "env";
}

SustainabilitySelection* SustainabilitySelection::clone() const {
    return new SustainabilitySelection(*this);
}

//Helper function to convert string to policy
SelectionPolicy* createPolicy(const std::string &policyName) {
    if (policyName == "nve") {
        return new NaiveSelection();
    } else if (policyName == "bal") {
        return new BalancedSelection(0, 0, 0); // Generic initialization to 0's.
    } else if (policyName == "eco") {
        return new EconomySelection();
    } else if (policyName == "env") {
        return new SustainabilitySelection();
    } else {
        throw std::invalid_argument("Invalid selection policy: " + policyName);
    }
}
