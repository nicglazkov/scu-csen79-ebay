// User.h
#pragma once

#include <string>
#include <vector>
#include "Listing.h"

namespace CSEN79 {
    class User {
    public:
        User();
        User(std::string name);

        std::string getName();
        void makeListing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime);
        void placeBid(Listing* listing, double amount);
        void print();
    private:
        std::string name;
        std::vector<Listing*> selling;
        std::vector<Listing*> purchased;
        std::vector<Listing*> interested;
        std::vector<Listing*> lost;
    };
}
