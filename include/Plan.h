#pragma once
#include <vector>
#include "Facility.h"
#include "Settlement.h"
#include "SelectionPolicy.h"
using std::vector;

enum class PlanStatus {
    AVALIABLE,
    BUSY,
};

class Plan {
    public:
        Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions);
        
        //Rule of 5
        Plan(const Plan &other); // Copy constructor
        Plan &operator=(const Plan &other); // Copy assignment operator
        Plan(Plan &&other); // Move constructor
        Plan &operator=(Plan &&other); // Move assignment operator
        ~Plan(); // Destructor

        // Copy Constructor with settlement: Allows copying a plan while associating it with a new settlement refrence.
        Plan(const Plan &other, const Settlement &settlement);

        //Getter for plan_id
        int getID() const;

        //Getter for settlment
        const Settlement& getSettlement() const;

        //Getter for selection policy
        const SelectionPolicy* getSelectionPolicy() const;

        //Getter for underConstruction
        const vector<Facility*>& getFacilitiesUnderConstruction() const;
        
        const int getlifeQualityScore() const;
        const int getEconomyScore() const;
        const int getEnvironmentScore() const;
        void setSelectionPolicy(SelectionPolicy *selectionPolicy);
        void step();
        void printStatus();
        const vector<Facility*> &getFacilities() const;
        void addFacility(Facility* facility);
        const string toString() const;

    private:
        int plan_id;
        const Settlement &settlement;
        SelectionPolicy *selectionPolicy; //What happens if we change this to a reference?
        PlanStatus status;
        vector<Facility*> facilities;
        vector<Facility*> underConstruction;
        const vector<FacilityType> &facilityOptions;
        int life_quality_score, economy_score, environment_score;
};