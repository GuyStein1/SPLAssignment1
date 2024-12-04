#include "Simulation.h"
#include "Auxiliary.h"
#include "Action.h"
#include <fstream>        // For file input/output operations ( reading the configuration file).
#include <sstream>        // For tokenizing user input using istringstream.
#include <stdexcept>      // For throwing and handling runtime errors.
#include <iostream>       // For console I/O operations (logging messages with cout).

// ---------- Simulation Implementation ----------

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
        throw std::runtime_error("Failed to open configuration file."); // Ensure the file is accessible
    }

    std::string line;
    while (std::getline(configFile, line))
    {
        // Ignore comments (lines starting with '#') and blank lines
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        // Tokenize the line into arguments
        std::vector<std::string> args = Auxiliary::parseArguments(line);
        if (args.empty())
        {
            continue; // Skip invalid or empty lines
        }

        // Handle configuration based on the first token (entry type)
        if (args[0] == "settlement")
        {
            // Validate the format for settlements
            if (args.size() != 3)
            {
                throw std::runtime_error("Invalid settlement format in config file.");
            }
            std::string settlementName = args[1];
            SettlementType type = createSettlementType(std::stoi(args[2])); // Convert type from integer
            Settlement *settlement = new Settlement(settlementName, type);  // Allocate settlement dynamically
            settlements.push_back(settlement);                              // Add to the settlements vector
        }

        else if (args[0] == "facility")
        {
            // Validate the format for facilities
            if (args.size() != 7)
            {
                throw std::runtime_error("Invalid facility format in config file.");
            }
            std::string facilityName = args[1];
            FacilityCategory category = createFacilityCategory(std::stoi(args[2])); // Convert category from integer
            int price = std::stoi(args[3]);
            int lifeQualityImpact = std::stoi(args[4]);
            int economyImpact = std::stoi(args[5]);
            int environmentImpact = std::stoi(args[6]);

            // Create and add the facility to the list of options
            FacilityType facility(facilityName, category, price, lifeQualityImpact, economyImpact, environmentImpact);
            facilitiesOptions.push_back(facility);
        }

        else if (args[0] == "plan")
        {
            // Validate the format for plans
            if (args.size() != 3)
            {
                throw std::runtime_error("Invalid plan format in config file.");
            }
            std::string settlementName = args[1];
            std::string selectionPolicy = args[2];

            SelectionPolicy *policy = createPolicy(selectionPolicy); // Dynamically allocate the selection policy

            Settlement *p = settlements[0]; // Default to the first settlement (to avoid null pointer)
            for (size_t i = 0; i < settlements.size(); i++)
            {
                if (settlements[i]->getName() == settlementName)
                {
                    p = settlements[i]; // Match the settlement name
                    break;
                }
                else if (i == settlements.size() - 1)
                {
                    throw std::runtime_error("Settlement not found for plan: " + settlementName);
                }
            }

            // Create a new plan associated with the matched settlement
            Plan plan(planCounter++, *p, policy, facilitiesOptions);
            plans.push_back(plan);
        }
        else
        {
            throw std::runtime_error("Unknown configuration entry type: " + args[0]); // Handle unknown entries
        }
    }

    configFile.close(); // Ensure the file is closed after processing
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
    // Deep copy of actionsLog: Clone each BaseAction to ensure unique ownership.
    for (BaseAction* action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }

    // Deep copy of settlements: Dynamically allocate and copy each Settlement.
    for (Settlement* settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement));
    }

    // Deep copy of plans:
    // Each Plan references a Settlement. When copying, the Settlement pointers must be updated to point to the newly created copies in `settlements`.
    for (Plan plan : other.plans) {
        // Retrieve the name of the Settlement associated with the plan.
        std::string name = plan.getSettlement().getName();
        // Find the corresponding Settlement in the copied settlements vector.
        Settlement* s = &getSettlement(name); 
        // Create a new Plan using the copy constructor with settlement.
        plans.push_back(Plan(plan, *s)); 
    }

    // Copy of facilitiesOptions: Since FacilityType has no dynamic members, use its copy constructor.
    for (FacilityType facility : other.facilitiesOptions) {
        facilitiesOptions.push_back(FacilityType(facility));
    }
}

// Copy Assignment Operator
Simulation &Simulation::operator=(const Simulation &other) {
    // Check for self-assignment to avoid unnecessary work and potential issues.
    if (this == &other) {
        return *this;
    }

    //----Clean the state of 'this'----

    // Clear plans (no dynamic allocation in Plan, so just clear the vector).
    plans.clear();

    // Clear the facilitiesOptions vector (shallow clear as FacilityType doesn't use dynamic memory).
    facilitiesOptions.clear();

    // Free dynamically allocated actions and clear the actions log.
    for (BaseAction* action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    // Free dynamically allocated settlements
    for (Settlement* settlement : settlements) {
        delete settlement;
    }
    settlements.clear(); 

    //----Copy 'other' to 'this'----

    // Copy primitive and value-based members.
    isRunning = other.isRunning;
    planCounter = other.planCounter;

    // Deep copy actionsLog
    for (BaseAction* action : other.actionsLog) {
        actionsLog.push_back(action->clone()); // Clone each action in the log.
    }

    // Deep copy of settlements: Dynamically allocate and copy each Settlement.
    for (Settlement* settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement));
    }

    // Deep copy of plans:
    // Each Plan references a Settlement. When copying, the Settlement pointers must be updated to point to the newly created copies in `settlements`.
    for (Plan plan : other.plans) {
        // Retrieve the name of the Settlement associated with the plan.
        std::string name = plan.getSettlement().getName();
        // Find the corresponding Settlement in the copied settlements vector.
        Settlement* s = &getSettlement(name); 
        // Create a new Plan using the copy constructor with settlement.
        plans.push_back(Plan(plan, *s)); 
    }

    // Deep copy facilitiesOptions
    for (FacilityType facility : other.facilitiesOptions) {
        facilitiesOptions.push_back(FacilityType(facility)); // Copy FacilityType objects.
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
Simulation &Simulation::operator=(Simulation &&other) {
    // Check for self-assignment to avoid unnecessary work and potential issues.
    if (this == &other) {
        return *this;
    }

    //----Clean the state of 'this'----

    // Clear plans and facilitiesOptions. No dynamic memory to free here.
    plans.clear();
    facilitiesOptions.clear();

    // Free dynamically allocated actions in `this->actionsLog`.
    for (BaseAction* action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    // Free dynamically allocated settlements
    for (Settlement* settlement : settlements) {
        delete settlement;
    }
    settlements.clear(); 

    //----Move 'other' to 'this'----

    // Copy primitive and value-based members from `other`.
    isRunning = other.isRunning;
    planCounter = other.planCounter;

    // Move other resources into `this` to transfer ownership.
    settlements = std::move(other.settlements);
    actionsLog = std::move(other.actionsLog);
    plans = std::move(other.plans);
    facilitiesOptions = std::move(other.facilitiesOptions);

    // Leave `other` in a valid empty state to ensure safe destruction.
    // This makes it clear that `other` is no longer usable after the move.
    other.settlements.clear();
    other.actionsLog.clear();
    other.plans.clear();
    other.facilitiesOptions.clear();
    other.isRunning = false;
    other.planCounter = 0;

    return *this;
}

// Destructor
Simulation::~Simulation() {

    // Free dynamically allocated actions
    for (BaseAction* action : actionsLog) {
        delete action;
    }
    actionsLog.clear(); // Ensures a clean state before destruction, though not strictly necessary.

    // Free dynamically allocated settlements
    for (Settlement* settlement : settlements) {
        delete settlement;
    }
    settlements.clear(); // Ensures a clean state before destruction, though not strictly necessary.

    // Clear plans (no dynamic allocation, just reset the vector)
    plans.clear(); // Optional but ensures explicit reset of the vector.

    // Clear facilities (no dynamic memory, just reset the vector)
    facilitiesOptions.clear(); // Keeps the state consistent, though not strictly required.
}


void Simulation::start() {
    // Log the start of the simulation
    std::cout << "The simulation has started" << std::endl;
    
    isRunning = true; // Set the simulation state to running

    while (isRunning) {
        try {
            std::cout << "> "; // Prompt the user
            std::string input;
            std::getline(std::cin, input); // Read the full user input as a single line

            std::istringstream iss(input); // Parse the input into tokens
            std::string command;
            iss >> command; // Extract the first token as the command

            if (command == "step") {
                int numOfSteps;
                iss >> numOfSteps; // Attempt to extract the number of steps
                if (iss.fail() || numOfSteps <= 0) {
                    throw std::runtime_error("Invalid input for step");
                }
                SimulateStep action(numOfSteps); // Create an action for simulating steps
                action.act(*this);
            } else if (command == "plan") {
                std::string settlementName, selectionPolicy;
                iss >> settlementName >> selectionPolicy; // Extract settlement and policy
                if (settlementName.empty() || selectionPolicy.empty()) {
                    throw std::runtime_error("Invalid input for plan");
                }
                AddPlan action(settlementName, selectionPolicy); // Add a plan
                action.act(*this);
            } else if (command == "settlement") {
                std::string settlementName;
                int settlementTypeInt;
                iss >> settlementName >> settlementTypeInt; // Extract settlement name and type as an int
                if (settlementName.empty() || iss.fail() || settlementTypeInt < 0 || settlementTypeInt > 2) {
                    throw std::runtime_error("Invalid input for settlement");
                }

                // Convert integer to SettlementType using static_cast
                SettlementType settlementType = static_cast<SettlementType>(settlementTypeInt);

                AddSettlement action(settlementName, settlementType); // Add a settlement
                action.act(*this);
            } else if (command == "facility") {
                std::string facilityName;
                int category, price, lifeQ, economy, environment;
                iss >> facilityName >> category >> price >> lifeQ >> economy >> environment; // Extract facility details
                if (facilityName.empty() || iss.fail() || category < 0 || category > 2 || price < 0 || lifeQ < 0 || economy < 0 || environment < 0) {
                    throw std::runtime_error("Invalid input for facility");
                }

                // Convert integer to FacilityCategory using static_cast
                FacilityCategory facilityCategory = static_cast<FacilityCategory>(category);

                AddFacility action(facilityName, facilityCategory, price, lifeQ, economy, environment); // Add a facility
                action.act(*this);
            } else if (command == "planStatus") {
                int planId;
                iss >> planId; // Extract plan ID
                if (iss.fail()) {
                    throw std::runtime_error("Invalid input for planStatus");
                }
                PrintPlanStatus action(planId); // Print the status of a specific plan
                action.act(*this);
            } else if (command == "changePolicy") {
                int planId;
                std::string newPolicy;
                iss >> planId >> newPolicy; // Extract plan ID and new policy
                if (iss.fail() || newPolicy.empty()) {
                    throw std::runtime_error("Invalid input for changePolicy");
                }
                ChangePlanPolicy action(planId, newPolicy); // Change the policy of a specific plan
                action.act(*this);
            } else if (command == "log") {
                PrintActionsLog action; // Log all actions taken
                action.act(*this);
            } else if (command == "backup") {
                BackupSimulation action; // Backup the current simulation state
                action.act(*this);
            } else if (command == "restore") {
                RestoreSimulation action; // Restore the simulation from backup
                action.act(*this);
            } else if (command == "close") {
                Close action; // Close the simulation
                action.act(*this);
            } else {
                throw std::runtime_error("Unknown command"); // Handle invalid commands
            }
        } catch (const std::exception &e) {
            // Print the error message and continue the loop
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
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
    for (BaseAction* action : actionsLog) {
        delete action;
    }
    actionsLog.clear(); // Ensures a clean state before destruction, though not strictly necessary.

    // Free dynamically allocated settlements
    for (Settlement* settlement : settlements) {
        delete settlement;
    }
    settlements.clear(); // Prevents dangling pointers, though the destructor handles it.

    // Clear plans (no dynamic allocation, just reset the vector)
    plans.clear(); // Optional but ensures explicit reset of the vector.

    // Clear facilities (no dynamic memory, just reset the vector)
    facilitiesOptions.clear(); // Keeps the state consistent, though not strictly required.

    // Reset planCounter
    planCounter = 0;


    std::cout << "Simulation closed successfully." << std::endl;
}

void Simulation::open() {
    // Reinitialize key members
    isRunning = true;
}







