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
         // This action never results in an error so always completed
        << "COMPLETED";
    return oss.str();
}

SimulateStep *SimulateStep::clone() const {
    return new SimulateStep(*this);
}


// ---------- AddPlan Implementation ----------

AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy)
    : settlementName(settlementName), selectionPolicy(selectionPolicy) {}

// Helper function to make sure policy is valid
bool AddPlan::isValidPolicy(const string &policyName) {
    return (policyName == "nve" || policyName == "bal" || policyName == "eco" || policyName == "env");
}

void AddPlan::act(Simulation &simulation) {
    // Make sure settlement name exists in the simulation and the selection policy string is valid
    if (!simulation.isSettlementExists(settlementName) || !isValidPolicy(selectionPolicy)) {
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
    oss << "planStatus " 
        << planId << " " 
        << (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
    return oss.str();
}

PrintPlanStatus* PrintPlanStatus::clone() const {
    return new PrintPlanStatus(*this); 
}


// ---------- ChangePlanPolicy Implementation ----------
ChangePlanPolicy::ChangePlanPolicy(const int planId, const string &newPolicy) 
    : planId(planId), newPolicy(newPolicy) {}

void ChangePlanPolicy::act(Simulation &simulation) {
    try {
        // Retrieve the plan using the plan ID
        Plan &plan = simulation.getPlan(planId);

        // Validate and create the new selection policy
        SelectionPolicy *policy = createPolicy(newPolicy);

        // Check if the new policy is the same as the current policy
        if (plan.getSelectionPolicy()->toString() == newPolicy) {
            error("Cannot change selection policy");
            simulation.addAction(this->clone());
            return;
        }

        // If the policy is BalancedSelection, update its scores based on the plan's data
        if (auto* balancedPolicy = dynamic_cast<BalancedSelection*>(policy)) {
            // Update scores using plan's current scores
            balancedPolicy->setLifeQualityScore(plan.getlifeQualityScore());
            balancedPolicy->setEconomyScore(plan.getEconomyScore());
            balancedPolicy->setEnvironmentScore(plan.getEnvironmentScore());

            // Add contributions from facilities under construction
            for (const Facility* facility : plan.getFacilitiesUnderConstruction()) {
                balancedPolicy->setLifeQualityScore(balancedPolicy->getLifeQualityScore() + facility->getLifeQualityScore());
                balancedPolicy->setEconomyScore(balancedPolicy->getEconomyScore() + facility->getEconomyScore());
                balancedPolicy->setEnvironmentScore(balancedPolicy->getEnvironmentScore() + facility->getEnvironmentScore());
            }
        }

        // Set the new selection policy in the plan
        plan.setSelectionPolicy(policy);

        // Mark the action as completed
        complete();
    } catch (const std::exception &e) {
        // Exception will occur if plan id isnt found in getPlan
        error("Cannot change selection policy");
    }

    // Log a snapshot of the action
    simulation.addAction(this->clone());
}

const string ChangePlanPolicy::toString() const {
    std::ostringstream oss;
    oss << "changePolicy " 
        << planId << " " 
        << newPolicy << " " 
        << (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
    return oss.str();
}

ChangePlanPolicy* ChangePlanPolicy::clone() const {
    return new ChangePlanPolicy(*this); 
}


// ---------- PrintActionsLog Implementation ----------

// Empty constructor
PrintActionsLog::PrintActionsLog() = default;

void PrintActionsLog::act(Simulation &simulation) {
    // Print each action in the actions log
    for (const auto *action : simulation.getActionsLog()) {
        std::cout << action->toString() << std::endl;
    }
    // Mark the action as completed
    complete();

    // Log the action in the actions log
    simulation.addAction(this->clone());
}

 PrintActionsLog* PrintActionsLog::clone() const {
    return new PrintActionsLog(*this); // Deep copy using the copy constructor
}

const string PrintActionsLog::toString() const {
    // This action never results in an error so always completed
    return "log COMPLETED";
 }

// ---------- Close Implementation ----------

Close::Close() = default;


void Close::act(Simulation &simulation) {
    // Call the simulation's close method to handle cleanup and closure
    simulation.close();

    // Mark the action as completed
    complete();

    // Log the action in the actions log
    simulation.addAction(this->clone());
}

Close* Close::clone() const {
    return new Close(*this);
}

const std::string Close::toString() const {
     // This action never results in an error so always completed
    return "close COMPLETED";
}

// ---------- BackupSimulation Implementation ----------

// Declare the global backup variable
extern Simulation* backup;

BackupSimulation::BackupSimulation() = default;

void BackupSimulation::act(Simulation &simulation) {
    // Delete any existing backup
    if (backup != nullptr) {
        delete backup;
        backup = nullptr;
    }

    // Create a new backup as a deep copy of the current simulation
    backup = new Simulation(simulation);

    // Mark the action as completed
    complete();

    // Log the action in the actions log
    simulation.addAction(this->clone());
}

BackupSimulation* BackupSimulation::clone() const {
    return new BackupSimulation(*this);
}

const std::string BackupSimulation::toString() const {
    // This action never results in an error so always completed
    return "backup COMPLETED";
}



// ---------- RestoreSimulation Implementation ----------

RestoreSimulation::RestoreSimulation() = default;

void RestoreSimulation::act(Simulation &simulation) {
    // Check if a backup exists
    if (backup == nullptr) {
        error("No backup available");
        simulation.addAction(this->clone());
        return;
    }

    // Overwrite the current simulation with the backup
    // Use copy assingment operator
    simulation = *backup;

    // Mark the action as completed
    complete();

    // Call simulation.open() to ensure the simulation is marked as running
    // and to print the "Simulation has been reopened" message
    simulation.open();

    // Log the action in the actions log
    simulation.addAction(this->clone());
}

RestoreSimulation* RestoreSimulation::clone() const {
    return new RestoreSimulation(*this);
}

const std::string RestoreSimulation::toString() const {
    std::ostringstream oss;
    oss << "restore "
        << (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
    return oss.str();
}


