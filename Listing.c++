#include "Listing.h"
#include "Bid.h"

using namespace std;
namespace CSEN79{

    Listing::Listing(){
        name = "";
        description = "";
        startingPrice = 0.0;
        buyOutrightPrice = 0.0;
        currentPrice = startingPrice;
        sellTime = 0;
        startTime = clock();
        sold = false;
    }

    Listing::Listing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime){
        this->name = name;
        this->description = description;
        this->startingPrice = startingPrice;
        this->buyOutrightPrice = buyOutrightPrice;
        currentPrice = startingPrice;
        this->sellTime = sellTime;
        startTime = clock();
        sold = false;
        log[log.size()] = "New Listing Created by " + currentUser->getName() + " for item: "+ name;
    }

    void Listing::setLog(vector<string> &newLog){
        log = newLog;
    }

    void Listing::setUser(User* newUser){
        currentUser = newUser;
    }

    void Listing::setListings(Listings* newListings){
        listings = newListings;
    }

    string Listing::getName(){
        return name;
    }

    string Listing::getDescription(){ 
        return description;
    }

    double Listing::getPrice(){ 
        return currentPrice; 
    }

    double Listing::getBuyOutrightPrice(){ 
        return buyOutrightPrice; 
    }

    double Listing::checkTime(){ 
        return (double)(clock() - startTime)/CLOCKS_PER_SEC; 
    }

    User* Listing::getUser(){ 
        return currentUser;
    }

    void Listing::makeBid(double bidAmount){
        if(bidAmount<=currentPrice) return;

        bids[bids.size()] = new Bid(bidAmount, this, getUser());
        log[log.size()] = "Bid on " + name + " placed by " + getUser()->getName(); 
    }

    void Listing::buyOutright(){
        sold = true;
    }
};