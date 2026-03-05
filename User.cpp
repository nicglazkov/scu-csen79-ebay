#include "User.h"
#include <iostream>

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
        Listing* newListing = new Listing(name, description, startingPrice, buyOutrightPrice, sellTime, this);
        selling.push_back(newListing);
    }

    void User::placeBid(Listing* listing, double amount) {
        listing->makeBid(amount, this);
    }

    void User::buyOutright(Listing* listing){
        listing->buyOutright(this);
    }

    std::vector<Listing*>* User::getInterested(){
        return &interested;
    }

    std::vector<Listing*>* User::getPurchased(){
        return &purchased;
    }

    std::vector<Listing*>* User::getLost(){
        return &lost;
    }

    std::vector<Listing*>* User::getSelling(){
        return &selling;
    }


    void User::print() {
        std::cout << "User: " << name << std::endl;
        /*
        std::cout << "Selling Listings: " << selling.size() << std::endl;
        std::cout << "Purchased Listings: " << purchased.size() << std::endl;
        std::cout << "Interested Listings: " << interested.size() << std::endl;
        std::cout << "Lost Listings: " << lost.size() << std::endl;
        */
    }

}
