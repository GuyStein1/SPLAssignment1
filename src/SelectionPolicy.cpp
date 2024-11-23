#include <stdexcept>
#include "SelectionPolicy.h"

//NaiveSelection implementation
NaiveSelection::NaiveSelection() : lastSelectedIndex(-1) {}

const FacilityType& NaiveSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (facilitiesOptions.empty() || lastSelectedIndex >= facilitiesOptions.size()) {
        throw std::runtime_error("No facilities available for selection.");
    }
    lastSelectedIndex = lastSelectedIndex + 1;
    return facilitiesOptions[lastSelectedIndex];
}

const string NaiveSelection::toString() const {
    return "NaiveSelection Policy";
}

NaiveSelection* NaiveSelection::clone() const {
    return new NaiveSelection(*this);
}

//BalanceSelection implementation





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