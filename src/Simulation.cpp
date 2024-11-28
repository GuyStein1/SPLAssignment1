#include "Simulation.h"
#include "Auxiliary.h"
#include "SelectionPolicy.h"
#include "Action.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <algorithm>

Simulation::Simulation(const string &configFilePath)
    : isRunning(false),    // Simulation starts as running
      planCounter(0),     // Initialize plan counter
      actionsLog(),       // Empty action log
      plans(),            // Empty plans list
      settlements(),      // Empty settlements list
      facilitiesOptions() // Empty facility options list
{
    // Open the configuration file
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open())
    {
        throw std::runtime_error("Failed to open configuration file.");
    }

    std::string line;
    while (std::getline(configFile, line))
    {
        // Ignore comments and blank lines
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        // Parse the arguments from the line
        std::vector<std::string> args = Auxiliary::parseArguments(line);
        if (args.empty())
        {
            continue;
        }

        // Process each entry based on its type
        if (args[0] == "settlement")
        {
            if (args.size() != 3)
            {
                throw std::runtime_error("Invalid settlement format in config file.");
            }
            std::string settlementName = args[1];
            SettlementType type = createSettlementType(std::stoi(args[2]));
            Settlement *settlement = new Settlement(settlementName, type); // Dynamically allocate
            settlements.push_back(settlement);
        }

        else if (args[0] == "facility")
        {
            if (args.size() != 7)
            {
                throw std::runtime_error("Invalid facility format in config file.");
            }
            std::string facilityName = args[1];
            FacilityCategory category = createFacilityCategory(std::stoi(args[2]));
            int price = std::stoi(args[3]);
            int lifeQualityImpact = std::stoi(args[4]);
            int economyImpact = std::stoi(args[5]);
            int environmentImpact = std::stoi(args[6]);
            FacilityType facility(facilityName, category, price, lifeQualityImpact, economyImpact, environmentImpact);
            facilitiesOptions.push_back(facility);
        }

        else if (args[0] == "plan")
        {
            if (args.size() != 3)
            {
                throw std::runtime_error("Invalid plan format in config file.");
            }
            std::string settlementName = args[1];
            std::string selectionPolicy = args[2];

            SelectionPolicy* policy = createPolicy(selectionPolicy);

            Settlement *p = settlements[0];
            for (size_t i = 0; i < settlements.size(); i++)
            {
                if (settlements[i]->getName() == settlementName)
                {
                    // Settlement found
                    p = settlements[i];
                    break;
                } else if (i == settlements.size() - 1)
                {
                    throw std::runtime_error("Settlement not found for plan: " + settlementName);
                }
            }

            // Create the plan and associate it with the settlement
            Plan plan(planCounter++, *p, policy, facilitiesOptions); 
            plans.push_back(plan);
        }
        else
        {
            throw std::runtime_error("Unknown configuration entry type: " + args[0]);
        }
    }

    configFile.close();
}

// Copy Constructor
Simulation::Simulation(const Simulation &other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(),
      plans(),
      settlements(),
      facilitiesOptions() 
{
    // Deep copy other vectors
    for (BaseAction* action : other.actionsLog)
    {
        actionsLog.push_back(action->clone());
    }
    for (Plan plan : other.plans)
    {
        plans.push_back(Plan(plan)); //Copy each Plan using its copy constructor
    }
    for (Settlement* settlement : other.settlements)
    {
        settlements.push_back(new Settlement(*settlement));
    }
    for (FacilityType facility : other.facilitiesOptions)
    {
        facilitiesOptions.push_back(FacilityType(facility));
    }
}

// Helper function to delete all heap allocated pointers
void Simulation::cleanSim() {
    // Clear plans (no dynamic allocation in Plan, so just clear the vector)
    plans.clear();

    // Free dynamically allocated actions
    for (BaseAction* action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    // Free dynamically allocated settlements
    for (Settlement* settlement : settlements) {
        delete settlement; // Properly delete settlements
    }
    settlements.clear();

    // Clear facilities (no dynamic memory)
    facilitiesOptions.clear();
}


//Copy Assignment Operator
Simulation &Simulation::operator=(const Simulation &other) {
    //Check for self-assignment to avoid unnecessary work and potential issues.
    if (this == &other)
    {
        return *this;
    }

    // Clear plans (no dynamic allocation in Plan, so just clear the vector)
    plans.clear();

    // Free dynamically allocated actions
    for (BaseAction* action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    // Temporary vector to hold settlements to retain
    vector<Settlement*> retainedSettlements;

    // We want to delete all heap allocated settlements that aren't in use anymore.
    for (Settlement* settlement : settlements) {
        // Check if the settlement exists in `other.settlements`
        auto it = std::find_if(
            other.settlements.begin(),
            other.settlements.end(),
            [&settlement](Settlement* s) { return s->getName() == settlement->getName(); }
        );

        if (it != other.settlements.end()) {
            // Settlement exists in `other`, retain it
            retainedSettlements.push_back(settlement);
        } else {
            // Settlement does not exist in `other`, delete it
            delete settlement;
        }
    }

    // Assign retained settlements back to this->settlements
    settlements = retainedSettlements;

    // Clear facilities (no dynamic memory)
    facilitiesOptions.clear();

    // Copy primitive and value-based members
    isRunning = other.isRunning;
    planCounter = other.planCounter;

    // Deep copy dynamically allocated members
    for (BaseAction* action : other.actionsLog)
    {
        actionsLog.push_back(action->clone());
    }
    for (Plan plan : other.plans)
    {
        plans.push_back(Plan(plan)); // Copy each Plan using its copy constructor
    }
    for (FacilityType facility : other.facilitiesOptions)
    {
        facilitiesOptions.push_back(FacilityType(facility));
    }

    return *this;
}

// Move Constructor
Simulation::Simulation(Simulation &&other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),

      // Use std::move for efficient ownership transfer, avoiding deep copying.
      actionsLog(std::move(other.actionsLog)),   
      plans(std::move(other.plans)),             
      settlements(std::move(other.settlements)),
      facilitiesOptions(std::move(other.facilitiesOptions))
{
      // After std::move, the vectors in 'other' are in a valid but unspecified state.
      // This is sufficient for the move constructor, as the destructor of 'other' will handle cleanup.
}

// Move Assignment Operator
Simulation &Simulation::operator=(Simulation &&other)
{
    // Check for self-assignment to avoid unnecessary work and potential issues.
    if (this == &other)
    {
        return *this;
    }

    // Clean up existing resources to prepare the current object for new ownership
    cleanSim();

    isRunning = other.isRunning;
    planCounter = other.planCounter;

    // Use std::move for efficient ownership transfer, avoiding deep copying.
    actionsLog = std::move(other.actionsLog);
    plans = std::move(other.plans);
    settlements = std::move(other.settlements);
    facilitiesOptions = std::move(other.facilitiesOptions);

    return *this;
}

// Destructor
Simulation::~Simulation()
{
    // Call cleanSim() to free all dynamically allocated resources
    // This ensures proper cleanup of BaseAction and Settlement objects
    cleanSim();
}


void Simulation::start() {
    // Log the start of the simulation
    std::cout << "The simulation has started" << std::endl;
    isRunning = true; // Set the simulation state to running

    runCommandLoop();
}

void Simulation::addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy) {
    // Create a new plan with a unique ID, using the provided settlement and selection policy
    Plan newPlan(planCounter++, settlement, selectionPolicy, facilitiesOptions);
    
    plans.push_back(newPlan); 
}

void Simulation::addAction(BaseAction *action) {
    // Add the provided action to the actions log
    actionsLog.push_back(action);
}

bool Simulation::addSettlement(Settlement *settlement) {
    // Check if the settlement already exists
    if (isSettlementExists(settlement->getName())) {
        return false; // Settlement already exists, return false
    }
    // Add the new settlement to the vector
    settlements.push_back(settlement);
    return true; // Successfully added the settlement
}

bool Simulation::addFacility(FacilityType facility) {
    // Check if a facility with the same name already exists
    for (const auto &existingFacility : facilitiesOptions) {
        if (existingFacility.getName() == facility.getName()) {
            return false; // Facility already exists, return false
        }
    }

    // Add the new facility type to the vector
    facilitiesOptions.push_back(std::move(facility));
    return true; // Successfully added the facility
}

bool Simulation::isSettlementExists(const string &settlementName) {
    // Search for a settlement with the given name
    for (const auto &settlement : settlements) {
        if (settlement->getName() == settlementName) {
            return true; // Settlement exists
        }
    }
    return false; // Settlement not found
}

Settlement &Simulation::getSettlement(const string &settlementName) {
    for (auto &settlement : settlements) {
        if (settlement->getName() == settlementName) {
            return *settlement; // Return a reference to the found settlement
        }
    }

    throw std::runtime_error("Settlement not found: " + settlementName); // Throw an exception if not found
}

Plan &Simulation::getPlan(const int planID) {
    for (auto &plan : plans) {
        if (plan.getID() == planID) { // Assuming Plan has a getID() method
            return plan; // Return a reference to the found plan
        }
    }

    throw std::runtime_error("Plan doesn't exist"); // Throw an exception if not found
}

const vector<BaseAction*> &Simulation::getActionsLog() const {
    return actionsLog;
}


void Simulation::step() {
    // Iterate through all plans and execute their step function
    for (auto &plan : plans) {
        plan.step();
    }
}

void Simulation::close() {
    // Print the summary of all plans
    for (const auto &plan : plans) {
        std::cout << "PlanID: " << plan.getID() << std::endl;
        std::cout << "SettlementName: " << plan.getSettlement().getName() << std::endl; // Assuming Plan provides a way to get Settlement
        std::cout << "LifeQualityScore: " << plan.getlifeQualityScore() << std::endl;
        std::cout << "EconomyScore: " << plan.getEconomyScore() << std::endl;
        std::cout << "EnvironmentScore: " << plan.getEnvironmentScore() << std::endl;
    }

    // Mark the simulation as not running
    isRunning = false;

    // Free allocated memory
    cleanSim();

    // Reset planCounter
    planCounter = 0;


    std::cout << "Simulation closed successfully." << std::endl;
}

void Simulation::open() {
    // Reinitialize key members
    isRunning = true;
    std::cout << "Simulation has been reopened." << std::endl;

    // Run the command loop to accept new actions
    runCommandLoop();
}


// ---------- User Interface Implementation ----------

void Simulation::runCommandLoop() {
    while (true) { // Infinite loop to allow reopening
        if (!isRunning) {
            std::cout << "Simulation is closed. Type 'open' to restart or 'exit' to quit." << std::endl;
        }

        std::cout << "> "; // Prompt the user
        std::string input;
        std::getline(std::cin, input);

        std::istringstream iss(input);
        std::string command;
        iss >> command;

        try {
            if (command == "step") {
                int numOfSteps;
                iss >> numOfSteps;
                if (iss.fail() || numOfSteps <= 0) {
                    throw std::runtime_error("Invalid input for step");
                }
                SimulateStep action(numOfSteps);
                action.act(*this);
            } else if (command == "plan") {
                std::string settlementName, selectionPolicy;
                iss >> settlementName >> selectionPolicy;
                if (settlementName.empty() || selectionPolicy.empty()) {
                    throw std::runtime_error("Invalid input for plan");
                }
                AddPlan action(settlementName, selectionPolicy);
                action.act(*this);
            } else if (command == "settlement") {
                std::string settlementName;
                int settlementTypeInt;
                iss >> settlementName >> settlementTypeInt;
                if (settlementName.empty() || iss.fail() || settlementTypeInt < 0 || settlementTypeInt > 2) {
                    throw std::runtime_error("Invalid input for settlement");
                }
                SettlementType settlementType = static_cast<SettlementType>(settlementTypeInt);
                AddSettlement action(settlementName, settlementType);
                action.act(*this);
            } else if (command == "facility") {
                std::string facilityName;
                int category, price, lifeQ, economy, environment;
                iss >> facilityName >> category >> price >> lifeQ >> economy >> environment;
                if (facilityName.empty() || iss.fail() || category < 0 || category > 2 || price < 0 || lifeQ < 0 || economy < 0 || environment < 0) {
                    throw std::runtime_error("Invalid input for facility");
                }
                FacilityCategory facilityCategory = static_cast<FacilityCategory>(category);
                AddFacility action(facilityName, facilityCategory, price, lifeQ, economy, environment);
                action.act(*this);
            } else if (command == "planStatus") {
                int planId;
                iss >> planId;
                if (iss.fail()) {
                    throw std::runtime_error("Invalid input for planStatus");
                }
                PrintPlanStatus action(planId);
                action.act(*this);
            } else if (command == "changePolicy") {
                int planId;
                std::string newPolicy;
                iss >> planId >> newPolicy;
                if (iss.fail() || newPolicy.empty()) {
                    throw std::runtime_error("Invalid input for changePolicy");
                }
                ChangePlanPolicy action(planId, newPolicy);
                action.act(*this);
            } else if (command == "log") {
                PrintActionsLog action;
                action.act(*this);
            } else if (command == "backup") {
                BackupSimulation action;
                action.act(*this);
            } else if (command == "restore") {
                RestoreSimulation action;
                action.act(*this);
            } else if (command == "close") {
                Close action;
                action.act(*this);
                isRunning = false; // Mark the simulation as closed
            } else if (command == "open") {
                if (!isRunning) {
                    open();
                } else {
                    std::cout << "Simulation is already running." << std::endl;
                }
            } else if (command == "exit") {
                break; // Exit the loop and terminate the program
            } else {
                throw std::runtime_error("Unknown command");
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}



