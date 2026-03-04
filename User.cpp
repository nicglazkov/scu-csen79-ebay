#include "User.h"

namespace CSEN79 {

    User::User() {
        name = "No Name";
    }

    User::User(std::string name) {
        this->name = name;
    }

    std::string User::getName() {
        return name;
    }

    void User::makeListing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime) {
        Listing* newListing = new Listing(name, description, startingPrice, buyOutrightPrice, sellTime);
        selling.push_back(newListing);
    }

    void User::placeBid(Listing* listing, double amount) {
        // Implementation for placing a bid on a listing
    }

}
