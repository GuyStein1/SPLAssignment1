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

AddPlan *AddPlan::clone() const {
    return new AddPlan(*this);
}

// ---------- AddFacility Implementation ----------
