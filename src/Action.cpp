#include "Action.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

// ---------- BaseAction Implementation ----------
BaseAction::BaseAction() : status(ActionStatus::COMPLETED), errorMsg("") {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = ActionStatus::COMPLETED;
    errorMsg = "";
}

void BaseAction::error(string errorMsg) {
    status = ActionStatus::ERROR;
    this->errorMsg = std::move(errorMsg); // Use std::move for efficiency
    std::cout << "Error: " << this->errorMsg << std::endl;
}

const string &BaseAction::getErrorMsg() const {
    return errorMsg;
}


// ---------- SimulateStep Implementation ----------

// No need to explicitly call the BaseAction constructor; it is automatically invoked to initialize status and errorMsg.
SimulateStep::SimulateStep(const int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(Simulation &simulation) {
    for (int i = 0; i < numOfSteps; i++) {
        simulation.step();
    }

    complete();

    // Log a snapshot of the action
    simulation.addAction(this->clone());
}

const string SimulateStep::toString() const {
    std::ostringstream oss;
    oss << "step " 
        << numOfSteps << " " 
        << (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
    return oss.str();
}

SimulateStep *SimulateStep::clone() const {
    return new SimulateStep(*this);
}


// ---------- AddPlan Implementation ----------

AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy)
    : settlementName(settlementName), selectionPolicy(selectionPolicy) {}

void AddPlan::act(Simulation &simulation) {
    // Make sure settlement name exists in the simulation and the selection policy string is valid
    if (!simulation.isSettlementExists(settlementName) || !isValidPolicy) {
        error("Cannot create this plan");
        // Log a snapshot of the action 
        simulation.addAction(this->clone());
        return;
    }

    // Create selection policy
    SelectionPolicy *policy = createPolicy(selectionPolicy); 

    // Add the plan, automaticlly sets plan to available
    simulation.addPlan(simulation.getSettlement(settlementName), policy); 

    complete();

    // Log a snapshot of the action
    simulation.addAction(this->clone());                                  
}

const string AddPlan::toString() const {
    std::ostringstream oss;
    oss << "plan " 
        << settlementName << " " 
        << selectionPolicy << " " 
        << (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
    return oss.str();
}

AddPlan *AddPlan::clone() const {
    return new AddPlan(*this);
}

// ---------- AddSettlement Implementation ----------
AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType)
    : settlementName(settlementName), settlementType(settlementType) {}

void AddSettlement::act(Simulation &simulation) {

// Create a new settlement
Settlement *settlement = new Settlement(settlementName, settlementType); 

// Attempt to add the settlement to the simulation
if (!simulation.addSettlement(settlement)) {
    // If the settlement already exists, clean up and throw an error
    delete settlement; 
    error("Settlement already exists");
    // Log a snapshot of the action 
    simulation.addAction(this->clone());
    return;
}

// If didn't enter the if it means settlement was added;
complete();

// Log a snapshot of the action 
simulation.addAction(this->clone());
}

const string AddSettlement::toString() const {
    std::ostringstream oss;
    oss << "settlement " 
        << settlementName << " " 
        << static_cast<int>(settlementType) << " " 
        << (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR"); 
    return oss.str();
}

AddSettlement *AddSettlement::clone() const {
    return new AddSettlement(*this);
}

// ---------- AddFacility Implementation ----------
AddFacility::AddFacility(const string &facilityName,
                         const FacilityCategory facilityCategory,
                         const int price,
                         const int lifeQualityScore,
                         const int economyScore,
                         const int environmentScore)
    : facilityName(facilityName),
      facilityCategory(facilityCategory),
      price(price),
      lifeQualityScore(lifeQualityScore),
      economyScore(economyScore),
      environmentScore(environmentScore) {}

void AddFacility::act(Simulation &simulation) {

    // Create a new FacilityType object
    FacilityType facility(facilityName, facilityCategory, price, lifeQualityScore, economyScore, environmentScore);

    // Add the facility to the simulation
    if (!simulation.addFacility(facility)) {
        // If the settlement already exists, clean up and throw an error
        error("Facility already exists");
        // Log a snapshot of the action
        simulation.addAction(this->clone());
        return;
    }

    // Mark the action as completed
    complete();

    // Log a snapshot of the action
    simulation.addAction(this->clone());
}

const string AddFacility::toString() const {
    std::ostringstream oss;
    oss << "facility " 
        << facilityName << " "
        << static_cast<int>(facilityCategory) << " "
        << price << " " 
        << lifeQualityScore << " "
        << economyScore << " "
        << environmentScore << " "
        << (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR"); 
    return oss.str();
}

AddFacility *AddFacility::clone() const {
    return new AddFacility(*this);
}


// ---------- PrintPlanStatus Implementation ----------
PrintPlanStatus::PrintPlanStatus(int planId) : planId(planId) {}

void PrintPlanStatus::act(Simulation &simulation) {
    try {
        // Retrieve the plan using the plan ID
        Plan &plan = simulation.getPlan(planId);

        // Delegate the printing to the Plan class's printStatus method
        plan.printStatus();

        // Mark the action as completed
        complete();
    } catch (const std::exception &e) {
        // Handle a case where plan not found
        error(e.what());
    }

    // Log a snapshot of the action
    simulation.addAction(this->clone());
}

const string PrintPlanStatus::toString() const {
    std::ostringstream oss;
    oss << "plan_status " 
        << planId << " " 
        << (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
    return oss.str();
}

PrintPlanStatus* PrintPlanStatus::clone() const {
    return new PrintPlanStatus(*this); 
}

// ---------- ChangePlanPolicy Implementation ----------
