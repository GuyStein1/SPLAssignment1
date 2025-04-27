
# Reconstruction Simulation - SPL Course Project (Fall 2024)

## Overview

This project was completed as part of the SPL course at **Ben Gurion University**. 
It simulates the reconstruction of settlements in the fictional land of SPLand after a long war, by managing settlements, facilities, and reconstruction plans according to different strategies.  

The project is written in **C++**, following strict OOP principles and the **Rule of 5**, with careful manual memory management.

---

## Project Structure

- **main.cpp** – Initializes the simulation from a config file and starts the interactive command loop.
- **Simulation.cpp / Simulation.h** – Core simulation engine: manages settlements, plans, facilities, and user actions.
- **Settlement.cpp / Settlement.h** – Defines a settlement and its properties.
- **Facility.cpp / Facility.h** – Defines facility types and facility instances under construction or operational.
- **Plan.cpp / Plan.h** – Manages a reconstruction plan associated with a settlement and a selection policy.
- **SelectionPolicy.cpp / SelectionPolicy.h** – Implements different facility selection strategies:  
  - Naive (nve)
  - Balanced (bal)
  - Economy-focused (eco)
  - Sustainability-focused (env)
- **Action.cpp / Action.h** – Defines the user actions (e.g., add plans, simulate steps, print status, backup, etc.)
- **Auxiliary.cpp / Auxiliary.h** – Utility for parsing command inputs and config lines.
- **config_file.txt** – Example configuration file used to initialize the simulation.

---

## How to Compile and Run

**Compile:**
```bash
make


**Run:**
```bash
./simulation config_file.txt
```
Where `config_file.txt` is the path to your configuration file that defines initial settlements, facilities, and plans.

---

## Simulation Workflow

1. **Initialization:**  
   - The program reads the config file provided as a command-line argument.
   - Initializes settlements, facilities, and initial plans.
   
2. **Interactive Loop:**  
   After startup (`The simulation has started`), users can input commands such as:
   - `step <num_steps>` — Progresses the simulation.
   - `plan <settlement_name> <selection_policy>` — Adds a new reconstruction plan.
   - `settlement <name> <type>` — Adds a new settlement.
   - `facility <name> <category> <price> <lifeQ> <eco> <env>` — Adds a new facility type.
   - `planStatus <plan_id>` — Displays the current status of a plan.
   - `changePolicy <plan_id> <new_policy>` — Changes the policy of a plan.
   - `log` — Prints the history of actions performed.
   - `backup` — Saves a snapshot of the current simulation.
   - `restore` — Restores the last backup.
   - `close` — Ends the simulation and prints the final report.

---

## Key Concepts and Features

- **Rule of 5:**  
  Deep copy constructors, destructors, copy/move assignment operators were implemented carefully in all relevant classes (`Simulation`, `Plan`, `Facility`, etc.)

- **Memory Management:**  
  Manual allocation and deallocation using `new` and `delete` were carefully handled to prevent memory leaks.

- **Selection Policies:**  
  Facilities are selected for construction based on the attached plan's policy:
  - **NaiveSelection:** Round-robin over available facilities.
  - **BalancedSelection:** Chooses a facility minimizing the score differences (life quality, economy, environment).
  - **EconomySelection:** Prioritizes economy-enhancing facilities.
  - **SustainabilitySelection:** Prioritizes environmentally friendly facilities.

- **Actions System:**  
  All user commands are implemented as derived classes from an abstract `BaseAction` class, allowing clean logging and error management.

- **Backup and Restore:**  
  The entire simulation state (including settlements, plans, facilities, and actions log) can be backed up and restored at any time.

---

## Example `config_file.txt`

```plaintext
# settlement <settlement_name> <settlement_type>
settlement KfarSPL 0
settlement KiryatSPL 2
settlement BeitSPL 1

# facility <facility_name> <category> <price> <lifeq_impact> <eco_impact> <env_impact>
facility Hospital 0 5 5 3 2
facility Factory 1 5 2 5 1
facility SolarFarm 2 4 2 2 4

# plan <settlement_name> <selection_policy>
plan KfarSPL eco
plan KiryatSPL bal
```

---

## Notes

- **Development Environment:**  
  - Project was implemented and tested to work on CS Lab UNIX machines.
  - Compiled using `g++` with C++11 standard.

- **Project Objectives:**  
  Gain experience with:
  - Object-Oriented Design (OOD) in C++
  - Deep copy / Move semantics
  - Manual memory management
  - Simulation design and user interaction

---

## Authors
- Guy Stein
- Guy Zilberstein

