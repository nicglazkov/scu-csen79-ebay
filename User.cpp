#include "User.h"
#include "Listing.h"
#include <iostream>
using namespace std;

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

    /*
    Creates a new listing for the user. The listing is created with the provided parameters and added to the user's selling vector.
    */
    void User::makeListing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime) {
        Listing* newListing = new Listing(name, description, startingPrice, buyOutrightPrice, sellTime, this);
        lock_guard<mutex> lock(userMutex);
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

    std::vector<Listing*>* User::getSold(){
        return &sold;
    }

    std::mutex& User::getMutex(){
        return userMutex;
    }

}
