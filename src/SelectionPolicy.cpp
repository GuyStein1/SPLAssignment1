#include <stdexcept>
#include "SelectionPolicy.h"
#include <climits>
#include <algorithm> // For std::max and std::min

//NaiveSelection implementation
NaiveSelection::NaiveSelection() : lastSelectedIndex(-1) {}

const FacilityType& NaiveSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    // Check for empty facilities list or if all facilities have already been selected
    if (facilitiesOptions.empty()) {
        throw std::runtime_error("No facilities available for selection.");
    }
    if (lastSelectedIndex + 1 >= facilitiesOptions.size()) {
        throw std::runtime_error("All facilities have been selected.");
    }

    // Increment index and select the next facility
    lastSelectedIndex++;
    return facilitiesOptions[lastSelectedIndex];
}

const string NaiveSelection::toString() const {
    return "NaiveSelection Policy";
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

const string BalancedSelection::toString() const {
    return "BalancedSelection Policy";
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
    //Iterate over all facilities to find the first facility in the ECONOMY category
    for (size_t i = lastSelectedIndex + 1; i < facilitiesOptions.size(); ++i) {
        if (facilitiesOptions[i].getCategory() == FacilityCategory::ECONOMY) {
            lastSelectedIndex = i;
            return facilitiesOptions[i];
        }
    }

    throw std::runtime_error("No economy facilities available.");
}

const string EconomySelection::toString() const {
    return "EconomySelection Policy";
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
    //Iterate over all facilities to find the first facility in the ENVIRONMENT category
    for (size_t i = lastSelectedIndex + 1; i < facilitiesOptions.size(); ++i) {
        if (facilitiesOptions[i].getCategory() == FacilityCategory::ENVIRONMENT) {
            lastSelectedIndex = i;
            return facilitiesOptions[i];
        }
    }

    throw std::runtime_error("No sustainability facilities available.");
}

const string SustainabilitySelection::toString() const {
    return "SustainabilitySelection Policy";
}

SustainabilitySelection* SustainabilitySelection::clone() const {
    return new SustainabilitySelection(*this);
}


//Helper function to convert string to policy
SelectionPolicy* createPolicy(const std::string &policyName) {
    if (policyName == "nve") {
        return new NaiveSelection();
    } else if (policyName == "bal") {
        return new BalancedSelection(0, 0, 0); // Initialize scores to 0
    } else if (policyName == "eco") {
        return new EconomySelection();
    } else if (policyName == "env") {
        return new SustainabilitySelection();
    } else {
        throw std::invalid_argument("Invalid selection policy: " + policyName);
    }
}