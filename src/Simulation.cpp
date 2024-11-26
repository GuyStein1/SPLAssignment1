#include "Simulation.h"
#include "Auxiliary.h"
#include "SelectionPolicy.h"
#include "Action.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

Simulation::Simulation(const string &configFilePath)
    : isRunning(true),    // Simulation starts as running
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
            Plan plan(planCounter++, *p, policy, facilitiesOptions); // Assumes Plan has this constructor
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
      facilitiesOptions(other.facilitiesOptions) // Shallow copy of facilityOptions (sufficient since FacilityType has no dynamic resources)
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
}

// Helper function to delete heap allocated pointers
void Simulation::cleanSim() {
    // Free dynamically allocated memory
    for (BaseAction* action : actionsLog)
    {
        delete action;
    }
    actionsLog.clear(); //Remove all elements from the vector, avoiding dangling pointers to freed memory

    // Free dynamically allocated memory
    for (Settlement* settlement : settlements)
    {
        delete settlement; 
    }
    settlements.clear(); //Remove all elements from the vector, avoiding dangling pointers to freed memory
}


//Copy Assignment Operator
Simulation &Simulation::operator=(const Simulation &other) {
    //Check for self-assignment to avoid unnecessary work and potential issues.
    if (this == &other)
    {
        return *this;
    }

    // Clean up existing resources to prepare for the new data
    cleanSim();

    // Copy primitive and value-based members
    isRunning = other.isRunning;
    planCounter = other.planCounter;

    facilitiesOptions = other.facilitiesOptions; // Shallow copy (sufficient for FacilityType)

    // Deep copy dynamically allocated members
    for (BaseAction* action : other.actionsLog)
    {
        actionsLog.push_back(action->clone());
    }
    for (Plan plan : other.plans)
    {
        plans.push_back(Plan(plan)); // Copy each Plan using its copy constructor
    }
    for (Settlement* settlement : other.settlements)
    {
        settlements.push_back(new Settlement(*settlement));
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
