#include <stdexcept>
#include "SelectionPolicy.h"

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