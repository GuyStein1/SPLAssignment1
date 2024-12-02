#include "Plan.h"
#include <iostream>
#include <stdexcept>
#include <algorithm> // For std::find
#include <sstream> // For std::ostringstream

// Constructor
Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId),
      settlement(settlement),
      selectionPolicy(selectionPolicy),
      status(PlanStatus::AVALIABLE),
      facilities(),
      underConstruction(),
      facilityOptions(facilityOptions),
      life_quality_score(0),
      economy_score(0),
      environment_score(0) {
}

//Helper function to delete heap allocated pointers
void Plan::clean() {
    //Note: No need to check for nullptr before delete, since in modern C++ delete does nothing if the pointer is null
    // Delete the selection policy
    delete selectionPolicy;
    selectionPolicy = nullptr; // Nullify the pointer to avoid accidental reuse

    // Free dynamically allocated memory
    for (Facility *facility : facilities)
    {
        delete facility;
    }
    facilities.clear(); //Remove all elements from the vector, avoiding dangling pointers to freed memory

    // Free dynamically allocated memory
    for (Facility *facility : underConstruction)
    {
        delete facility; 
    }
    underConstruction.clear(); //Remove all elements from the vector, avoiding dangling pointers to freed memory
}

// Copy Constructor
Plan::Plan(const Plan &other)
    // Create a new object as a copy of an existing object
    : plan_id(other.plan_id),
      settlement(other.settlement),
      selectionPolicy(other.selectionPolicy->clone()),
      status(other.status),
      facilities(),
      underConstruction(),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score) {

    // Deep copy facilities and underConstruction.
    for (Facility *facility : other.facilities)
    {
        facilities.push_back(new Facility(*facility));
    }
    for (Facility *facility : other.underConstruction)
    {
        underConstruction.push_back(new Facility(*facility));
    }
}

// Move Constructor
Plan::Plan(Plan &&other)
    // Transfer ownership of resources from the source object (other) to this new instance.
    : plan_id(other.plan_id),
      settlement(other.settlement),           // Transfer the pointer
      selectionPolicy(other.selectionPolicy), // Transfer ownership
      status(other.status),
      // Use std::move for efficient ownership transfer, avoiding deep copying.
      facilities(std::move(other.facilities)),
      underConstruction(std::move(other.underConstruction)),
      facilityOptions(other.facilityOptions), // Reference, no need to reassign
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score)
{

    other.selectionPolicy = nullptr;      // Nullify pointer to prevent double deletion
    other.facilities.clear();             // Optional: leave other in a valid empty state
    other.underConstruction.clear();      // Optional: leave other in a valid empty state
    other.status = PlanStatus::AVALIABLE; // Reset status to a default state
}

// Destructor
Plan::~Plan() {
    clean();
}

// Getter methods
const int Plan::getlifeQualityScore() const {
    return life_quality_score;
}

const int Plan::getEconomyScore() const {
    return economy_score;
}

const int Plan::getEnvironmentScore() const {
    return environment_score;
}

const vector<Facility*> &Plan::getFacilities() const {
    return facilities;
}

// Getter for the plan's unique ID
int Plan::getID() const {
    return plan_id;
}

// Getter for settlment
const Settlement& Plan::getSettlement() const {
    return settlement;
}

// Getter for selection policy
const SelectionPolicy* Plan::getSelectionPolicy() const {
    return selectionPolicy;
}

// Getter for underConstruction vector
const vector<Facility*>& Plan::getFacilitiesUnderConstruction() const {
    return underConstruction;
}

// Set selection policy
void Plan::setSelectionPolicy(SelectionPolicy *newPolicy)
{
    if (selectionPolicy)
    {
        delete selectionPolicy; // Clean up the existing policy
    }
    selectionPolicy = newPolicy; // Assign the new policy
}

void Plan::step() {
    // Stage 1: Check if the plan is available to proceed with construction
    if (status == PlanStatus::AVALIABLE) {
        // Stage 2: Ensure the number of facilities under construction meets the settlement's type limit
        size_t maxConstruction = static_cast<size_t>(settlement.getType()) + 1; // Convert enum to size_t value

        while (underConstruction.size() < maxConstruction) {
            // Select a facility according to the selection policy
            FacilityType chosenType = selectionPolicy->selectFacility(facilityOptions);

            // Dynamically create a new Facility instance based on the selected type
            Facility* newFacility = new Facility(chosenType, settlement.getName());

            // Use addFacility method
            addFacility(newFacility);
        }
    }
    // Stage 3: Process facilities under construction
    for (int i = static_cast<int>(underConstruction.size()) - 1; i >= 0; i--) {
        Facility *facility = underConstruction[i];

        // Update the facility's construction progress
        FacilityStatus facilityStatus = facility->step();

        // If the facility is now operational, move it to the facilities list
        if (facilityStatus == FacilityStatus::OPERATIONAL) {
            facilities.push_back(facility); // Add to the list of operational facilities
            underConstruction.erase(underConstruction.begin() + i); // Remove from underConstruction

            // Update the scores based on the facility's attributes
            life_quality_score += facility->getLifeQualityScore();
            economy_score += facility->getEconomyScore();
            environment_score += facility->getEnvironmentScore();
        }
    }

    // Stage 4: Update the plan's status based on the number of facilities under construction
    status = (underConstruction.size() >= static_cast<size_t>(settlement.getType()) + 1) ? 
             PlanStatus::BUSY : 
             PlanStatus::AVALIABLE;
}

void Plan::printStatus() {
    // Print the plan ID
    std::cout << "PlanID: " << plan_id << "\n";

    // Print the settlement name
    std::cout << "SettlementName: " << settlement.getName() << "\n";

    // Print the plan status
    std::cout << "PlanStatus: " << (status == PlanStatus::AVALIABLE ? "AVALIABLE" : "BUSY") << "\n";

    // Print the selection policy
    if (selectionPolicy) {
        std::cout << "SelectionPolicy: " << selectionPolicy->toString() << "\n";
    } else {
        std::cout << "SelectionPolicy: None\n";
    }

    // Print the scores
    std::cout << "LifeQualityScore: " << life_quality_score << "\n";
    std::cout << "EconomyScore: " << economy_score << "\n";
    std::cout << "EnvironmentScore: " << environment_score << "\n";

    // Print facilities under construction
    for (const Facility* facility : underConstruction) {
        std::cout << "FacilityName: " << facility->getName() << "\n";
        std::cout << "FacilityStatus: UNDER_CONSTRUCTION\n";
    }

    // Print operational facilities
    for (const Facility* facility : facilities) {
        std::cout << "FacilityName: " << facility->getName() << "\n";
        std::cout << "FacilityStatus: OPERATIONAL\n";
    }
}

void Plan::addFacility(Facility *facility) {
    // Add the newly created facility to the underConstruction list
    underConstruction.push_back(facility);
}

const string Plan::toString() const {
    std::ostringstream output;

    // Basic plan details
    output << "Plan ID: " << plan_id << "\n";
    output << "Status: " << (status == PlanStatus::AVALIABLE ? "Available" : "Busy") << "\n";
    output << "Settlement: " << settlement.getName() << "\n";

    // Facility options 
    output << "Facility Options: (" << facilityOptions.size() << " total)\n";
    for (const FacilityType& facilityType : facilityOptions) {
        output << "  - " << facilityType.getName() << "\n";
    }

    // Facilities under construction
    output << "Facilities Under Construction (" << underConstruction.size() << "):\n";
    for (const Facility* facility : underConstruction) {
        if (facility) {
            output << "  - " << facility->toString() << "\n";
        }
    }

    // Operational facilities
    output << "Operational Facilities (" << facilities.size() << "):\n";
    for (const Facility* facility : facilities) {
        if (facility) {
            output << "  - " << facility->toString() << "\n";
        }
    }

    // Scores
    output << "Life Quality Score: " << life_quality_score << "\n";
    output << "Economy Score: " << economy_score << "\n";
    output << "Environment Score: " << environment_score << "\n";

    return output.str();
}