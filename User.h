// User.h
#pragma once

#include <string>
#include <vector>

class Listing;

class User {
public:
    User();
    User(std::string name);

private:
    std::string name;
    std::vector<Listing*> selling;
    std::vector<Listing*> purchased;
    std::vector<Listing*> interested;
    std::vector<Listing*> lost;

    std::string getName();
    void makeListing(User* owner);
    void placeBid(Listing* listing, double amount);

};
