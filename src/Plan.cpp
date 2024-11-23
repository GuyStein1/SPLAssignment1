#include "Plan.h"
#include "Settlement.h"
#include <iostream>
#include <stdexcept>
#include <algorithm> // For std::find
#include <string> // For std::to_string

// Constructor
Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId),
      settlement(settlement),
      selectionPolicy(selectionPolicy),
      status(PlanStatus::AVALIABLE),
      facilityOptions(facilityOptions),
      life_quality_score(0),
      economy_score(0),
      environment_score(0),
      facilities(),
      underConstruction() {
}

void Plan::clean() {
    // Delete the selection policy 
        delete selectionPolicy;
        selectionPolicy = nullptr; // Nullify the pointer to avoid accidental reuse

    // Delete facilities from operational list
    for (Facility *facility : facilities) {
            delete facility;
    }

    // Delete facilities from under-construction list
    for (Facility *facility : underConstruction) {
            delete facility;
    }
}

//Copy Constructor
Plan::Plan(const Plan &other)
    : plan_id(other.plan_id),
      settlement(other.settlement), // Reference, so no copying needed
      selectionPolicy(other.selectionPolicy->clone()), // Deep copy of selection policy
      status(other.status),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score),
      facilities(),
      underConstruction() {
    // Deep copy facilities, store facilities on heap to avoid their destruction after
    for (Facility *facility : other.facilities) {
        facilities.push_back(new Facility(*facility));
    }
    for (Facility *facility : other.underConstruction) {
        underConstruction.push_back(new Facility(*facility));
    }
}

//Copy Assignment Operator
Plan &Plan::operator=(const Plan &other){
    if (this == &other){
        return *this;
    }
    // Make sure settlements are equal
    if (!settlement.isEqual(other.settlement)) {
        throw std::invalid_argument("Cannot assign plans to different settlements.");
    }
    // Make sure facility options are the same
    if (&facilityOptions != &other.facilityOptions) {
        throw std::invalid_argument("Cannot assign plans with different facility options.");
    }

    //Delete heap allocated resources
    clean();

    plan_id = other.plan_id;
    status = other.status;
    life_quality_score = other.life_quality_score;
    economy_score = other.economy_score;
    environment_score = other.environment_score;

    //Deep copy facilities
    for (Facility* facility : other.facilities) {
        facilities.push_back(new Facility(*facility));
    }
    for (Facility* facility : other.underConstruction) {
        underConstruction.push_back(new Facility(*facility));
    }
    return *this;
}

//Move Constructor
Plan::Plan(Plan&& other)
    : plan_id(other.plan_id),
      settlement(other.settlement), // Reference, no need to reassign
      selectionPolicy(other.selectionPolicy), // Transfer ownership
      status(other.status),
      facilityOptions(other.facilityOptions), // Reference, no need to reassign
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score),
      facilities(std::move(other.facilities)), // Move vector
      underConstruction(std::move(other.underConstruction)) // Move vector 
      {
    // Nullify pointers to prevent double deletion
    other.selectionPolicy = nullptr;
}

//Move Assignment Operator
Plan &Plan::operator=(Plan &&other){
    if (this == &other){
        return *this;
    }
    // Make sure settlements are equal
    if (!settlement.isEqual(other.settlement)) {
        throw std::invalid_argument("Cannot assign plans to different settlements.");
    }
    // Make sure facility options are the same
    if (&facilityOptions != &other.facilityOptions) {
        throw std::invalid_argument("Cannot assign plans with different facility options.");
    }

    //Delete heap allocated resources
    clean();

    plan_id = other.plan_id;
    status = other.status;
    life_quality_score = other.life_quality_score;
    economy_score = other.economy_score;
    environment_score = other.environment_score;

    facilities = std::move(other.facilities); // Move vector
    underConstruction = std::move(other.underConstruction); // Move vector

    other.selectionPolicy = nullptr;

    return *this;
}

//Destructor
Plan::~Plan() {
    clean();
}
