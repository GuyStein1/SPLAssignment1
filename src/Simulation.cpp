#include "Simulation.h"
#include "Auxiliary.h"
#include "Settlement.h"
#include "Plan.h"
#include "Facility.h"
#include "SelectionPolicy.h"
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
