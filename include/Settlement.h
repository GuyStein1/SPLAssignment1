#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;

class Facility;

enum class SettlementType
{
    VILLAGE,
    CITY,
    METROPOLIS,
};

// Convert int to SettlementType
SettlementType createSettlementType(int value);

class Settlement
{
public:
    Settlement(const string &name, SettlementType type);
    const string &getName() const;
    SettlementType getType() const;
    const string toString() const;

    //Helper function to check if 2 settlements are the same
    bool isEqual(const Settlement& other) const;
    
private:
    const string name;
    SettlementType type;
};