#pragma once
#include <vector>
#include "Facility.h"
using std::vector;


class SelectionPolicy {
    public:

        // Constructor
        virtual const FacilityType& selectFacility(const vector<FacilityType>& facilitiesOptions) = 0;

        virtual const string toString() const = 0;
        virtual SelectionPolicy* clone() const = 0;   

        //Destructor
        virtual ~SelectionPolicy() = default;

};

//Helper function to convert string to policy
SelectionPolicy* createPolicy(const std::string &policyName);

class NaiveSelection: public SelectionPolicy {
    public:
        NaiveSelection();
        const FacilityType& selectFacility(const vector<FacilityType>& facilitiesOptions) override;
        const string toString() const override;
        NaiveSelection *clone() const override;
        ~NaiveSelection() override = default;
    private:
        int lastSelectedIndex;
};

class BalancedSelection: public SelectionPolicy {
    public:
        BalancedSelection(int LifeQualityScore, int EconomyScore, int EnvironmentScore);
        const FacilityType& selectFacility(const vector<FacilityType>& facilitiesOptions) override;
        const string toString() const override;
        BalancedSelection *clone() const override;
        ~BalancedSelection() override = default;

        // Setter methods to update scores
        void setLifeQualityScore(int score);
        void setEconomyScore(int score);
        void setEnvironmentScore(int score);

        // Getter methods to retrieve scores
        int getLifeQualityScore() const;
        int getEconomyScore() const;
        int getEnvironmentScore() const;

    private:
        int LifeQualityScore;
        int EconomyScore;
        int EnvironmentScore;
};

class EconomySelection: public SelectionPolicy {
    public:
        EconomySelection();
        const FacilityType& selectFacility(const vector<FacilityType>& facilitiesOptions) override;
        const string toString() const override;
        EconomySelection *clone() const override;
        ~EconomySelection() override = default;
    private:
        int lastSelectedIndex;

};

class SustainabilitySelection: public SelectionPolicy {
    public:
        SustainabilitySelection();
        const FacilityType& selectFacility(const vector<FacilityType>& facilitiesOptions) override;
        const string toString() const override;
        SustainabilitySelection *clone() const override;
        ~SustainabilitySelection() override = default;
    private:
        int lastSelectedIndex;
};