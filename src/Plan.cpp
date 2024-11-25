#include "Plan.h"
#include <iostream>
#include <stdexcept>
#include <algorithm> // For std::find
#include <string>    // For std::to_string
#include <sstream> // For std::ostringstream

// Constructor
Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId),
      settlement(&settlement),
      selectionPolicy(selectionPolicy),
      facilityOptions(facilityOptions),
      status(PlanStatus::AVALIABLE),
      life_quality_score(0),
      economy_score(0),
      environment_score(0),
      facilities(),
      underConstruction() {
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
      status(other.status),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score),
      facilities(),
      underConstruction(),
      // Deep copy of selection policy, using clone():
      // The clone() method is implemented in each derived class of SelectionPolicy to create a new instance of the same class and return a pointer to it.
      // This ensures the correct derived type is duplicated, preserving polymorphism and copying all unique attributes of the specific SelectionPolicy.
      // It is an elegant solution for copying polymorphic objects safely and correctly.
      selectionPolicy(other.selectionPolicy->clone())
{
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

// Copy Assignment Operator
Plan &Plan::operator=(const Plan &other) {
    //Check for self-assignment to avoid unnecessary work and potential issues.
    if (this == &other)
    {
        return *this;
    }
    // Make sure settlements are the same
    if (settlement != other.settlement)
    {
        throw std::invalid_argument("Cannot assign plans to different settlements.");
    }
    // Make sure facility options are the same
    if (&facilityOptions != &other.facilityOptions)
    {
        throw std::invalid_argument("Cannot assign plans with different facility options.");
    }

    //Delete dynamically allocated memory associated with the current object (this), preventing memory leaks.
    clean();

    plan_id = other.plan_id;
    status = other.status;
    life_quality_score = other.life_quality_score;
    economy_score = other.economy_score;
    environment_score = other.environment_score;

    // Deep copy the selection policy using clone()
    if (other.selectionPolicy) {
        selectionPolicy = other.selectionPolicy->clone(); // Use clone() to avoid slicing
    } else {
        selectionPolicy = nullptr; // Handle case where the other policy is null
    }

    // Deep copy facilities and underConstruction
    for (Facility *facility : other.facilities)
    {
        facilities.push_back(new Facility(*facility));
    }
    for (Facility *facility : other.underConstruction)
    {
        underConstruction.push_back(new Facility(*facility));
    }

    return *this;
}

// Move Constructor
Plan::Plan(Plan &&other)
    // Transfer ownership of resources from the source object (other) to this new instance.
    : plan_id(other.plan_id),
      settlement(other.settlement), //Transfer the pointer
      selectionPolicy(other.selectionPolicy), // Transfer ownership
      status(other.status),
      facilityOptions(other.facilityOptions), // Reference, no need to reassign
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score),
      //Use std::move for efficient ownership transfer, avoiding deep copying.
      facilities(std::move(other.facilities)),              // Move vector
      underConstruction(std::move(other.underConstruction)) // Move vector
{

    other.selectionPolicy = nullptr; //Nullify pointer to prevent double deletion
    other.facilities.clear(); // Optional: leave other in a valid empty state
    other.underConstruction.clear(); // Optional: leave other in a valid empty state
    other.status = PlanStatus::AVALIABLE; // Reset status to a default state
}

// Move Assignment Operator
Plan &Plan::operator=(Plan &&other) {
    //Check for self-assignment to avoid unnecessary work and potential issues.
    if (this == &other) 
    {
        return *this;
    }
    // Make sure settlements are equal
    if (settlement != other.settlement)
    {
        throw std::invalid_argument("Cannot assign plans to different settlements.");
    }
    // Make sure facility options are the same
    if (&facilityOptions != &other.facilityOptions)
    {
        throw std::invalid_argument("Cannot assign plans with different facility options.");
    }

    //Release any dynamically allocated memory owned by the current object (this) to prevent memory leaks.
    clean();

    //Adopt the resources from the source object (other), efficiently transferring ownership.
    plan_id = other.plan_id;
    status = other.status;
    life_quality_score = other.life_quality_score;
    economy_score = other.economy_score;
    environment_score = other.environment_score;

    facilities = std::move(other.facilities);// Move vector
    underConstruction = std::move(other.underConstruction); // Move vector

    // Transfer ownership of selectionPolicy
    selectionPolicy = other.selectionPolicy;
    other.selectionPolicy = nullptr; // Nullify other’s pointer to prevent double deletion

    return *this;
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
        int maxConstruction = static_cast<int>(settlement->getType()) + 1; // Maximum allowed facilities under construction

        while (underConstruction.size() < maxConstruction) {
            // Select a facility according to the selection policy
            FacilityType chosenType = selectionPolicy->selectFacility(facilityOptions);

            // Dynamically create a new Facility instance based on the selected type
            Facility *newFacility = new Facility(
                chosenType.getName(),
                settlement->getName(),
                chosenType.getCategory(),
                chosenType.getCost(),
                chosenType.getLifeQualityScore(),
                chosenType.getEconomyScore(),
                chosenType.getEnvironmentScore()
            );
            // Add the newly created facility to the underConstruction list
            underConstruction.push_back(newFacility);

            // Update the scores based on the facility's attributes
            life_quality_score += newFacility->getLifeQualityScore();
            economy_score += newFacility->getEconomyScore();
            environment_score += newFacility->getEnvironmentScore();

            // Check if the current selection policy is of type BalancedSelection using dynamic_cast.
            // If the cast succeeds, update the BalancedSelection scores with the plan's updated scores.
            BalancedSelection *balancedPolicy = dynamic_cast<BalancedSelection *>(selectionPolicy);
            if (balancedPolicy) {
                balancedPolicy->setLifeQualityScore(life_quality_score);
                balancedPolicy->setEconomyScore(economy_score);
                balancedPolicy->setEnvironmentScore(environment_score);
            }
        }
    }
    // Stage 3: Process facilities under construction
    for (int i = underConstruction.size() - 1; i >= 0; i--) {
        Facility *facility = underConstruction[i];

        // Update the facility's construction progress
        FacilityStatus facilityStatus = facility->step();

        // If the facility is now operational, move it to the facilities list
        if (facilityStatus == FacilityStatus::OPERATIONAL) {
            facilities.push_back(facility); // Add to the list of operational facilities
            underConstruction.erase(underConstruction.begin() + i); // Remove from underConstruction
        }
    }

    // Stage 4: Update the plan's status based on the number of facilities under construction
    status = (underConstruction.size() >= static_cast<int>(settlement->getType()) + 1) ? 
             PlanStatus::BUSY : 
             PlanStatus::AVALIABLE;
}

void Plan::printStatus() {
    // Print the basic status of the plan
    std::cout << "Plan ID: " << plan_id << "\n";
    std::cout << "Status: " << (status == PlanStatus::AVALIABLE ? "Available" : "Busy") << "\n";

    // Print details of operational facilities
    std::cout << "Operational Facilities (" << facilities.size() << "):\n";
    for (const Facility* facility : facilities) {
        std::cout << "- " << facility->toString() << "\n";
    }

    // Print details of facilities under construction
    std::cout << "Facilities Under Construction (" << underConstruction.size() << "):\n";
    for (const Facility* facility : underConstruction) {
        std::cout << "- " << facility->toString() << "\n";
    }
}

void Plan::addFacility(Facility* facility) {
    //Ensure the settlement has room for more facilities
    int maxFacilities = static_cast<int>(settlement->getType()) + 1;
    if (facilities.size() + underConstruction.size() >= maxFacilities) {
        throw std::runtime_error("No room for more facilities in this settlement.");
    }

    // Add the facility to the operational facilities list
    facilities.push_back(facility);

    // Update the scores based on the facility's attributes
    life_quality_score += facility->getLifeQualityScore();
    economy_score += facility->getEconomyScore();
    environment_score += facility->getEnvironmentScore();

    // Check if the current selection policy is of type BalancedSelection using dynamic_cast.
    // If the cast succeeds, update the BalancedSelection scores with the plan's updated scores.
    BalancedSelection *balancedPolicy = dynamic_cast<BalancedSelection *>(selectionPolicy);
    if (balancedPolicy) {
        balancedPolicy->setLifeQualityScore(life_quality_score);
        balancedPolicy->setEconomyScore(economy_score);
        balancedPolicy->setEnvironmentScore(environment_score);
    }
}

const string Plan::toString() const {
    std::ostringstream output;

    // Basic plan details
    output << "Plan ID: " << plan_id << "\n";
    output << "Status: " << (status == PlanStatus::AVALIABLE ? "Available" : "Busy") << "\n";
    output << "Settlement: " << (settlement ? settlement->getName() : "None") << "\n";

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





//Test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <cassert>

void testPlanWithMultipleFacilities() {
    // Create a Settlement
    Settlement settlement("TestVillage", SettlementType::VILLAGE);

    // Create Facility Options
    FacilityType facility1("Park", FacilityCategory::LIFE_QUALITY, 10, 5, 3, 2);
    FacilityType facility2("Mall", FacilityCategory::ECONOMY, 15, 2, 10, 1);
    FacilityType facility3("Solar Plant", FacilityCategory::ENVIRONMENT, 20, 3, 4, 10);
    FacilityType facility4("School", FacilityCategory::LIFE_QUALITY, 12, 8, 2, 1);
    std::vector<FacilityType> facilitiesOptions = {facility1, facility2, facility3, facility4};

    // Create a Naive Selection Policy
    SelectionPolicy* policy = new NaiveSelection();

    // Create a Plan
    Plan plan(1, settlement, policy, facilitiesOptions);

    // Print Initial State
    std::cout << "Initial Plan: \n" << plan.toString() << std::endl;

    // Perform Step 1
    plan.step();
    std::cout << "After Step 1: \n" << plan.toString() << std::endl;

    // Perform Step 2
    plan.step();
    std::cout << "After Step 2: \n" << plan.toString() << std::endl;

    // Perform Step 3
    plan.step();
    std::cout << "After Step 3: \n" << plan.toString() << std::endl;

    // Verify all facilities are added and operational eventually
    while (plan.getFacilities().size() < facilitiesOptions.size()) {
        plan.step();
        std::cout << "After Another Step: \n" << plan.toString() << std::endl;
    }

    std::cout << "Test Completed Successfully!" << std::endl;
}

int main() {
    testPlanWithMultipleFacilities();
    return 0;
}
