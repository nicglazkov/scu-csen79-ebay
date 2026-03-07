// Listing Class Implemented by Benjamin Castillo III
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
        seller = nullptr;
    }

    Listing::Listing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime, User* seller){
        this->name = name;
        this->description = description;
        this->startingPrice = startingPrice;
        this->buyOutrightPrice = buyOutrightPrice;
        currentPrice = startingPrice;
        this->sellTime = sellTime;
        startTime = clock();
        log->push_back("New Listing Created by " + seller->getName() + " for item: "+ name);
        this->seller = seller;
    }

    Listing::~Listing(){
        lock_guard<mutex> lock(entryMutex);
        for(int i = 0; i < bids.size(); i++){
            delete bids[i];
        }
        bids.clear();
    }

    void Listing::setLog(vector<string>* newLog){
        log = newLog;
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

    User* Listing::getSeller(){ 
        return seller;
    }

    Bid* Listing::getHighestBid(){
        return bids.back();
    }

    const vector<Bid*>& Listing::getBids(){
        lock_guard<mutex> lock(entryMutex);
        return bids;
    };

    /*
    Sells the listing to the winning bidder. The listing is added to the seller's sold vector and removed from their selling vector.
    */
    void Listing::sell(){
        seller->getSold()->push_back(this);
        for(int i = 0; i < seller->getSelling()->size(); i++){
            if((*(seller->getSelling()))[i] == this){
                seller->getSelling()->erase(seller->getSelling()->begin() + i);
            }
        }
    }

    /*
    Move the listing from the interested vector to the won or lost vector based on if the user won or lost the auction
    */
    void Listing::losers(User* winner){
        for(int i = 0; i < bids.size(); i++){
            if(bids[i]->getBidder() != winner){
                winner->getLost()->push_back(this);
                for(int j = 0; j < winner->getInterested()->size(); j++){
                    if((*(bids[i]->getBidder()->getInterested()))[i] == this){
                        bids[i]->getBidder()->getInterested()->erase(bids[i]->
                            getBidder()->getInterested()->begin() + j);
                    }
                }
            }
        }
    }

    /*
    Places a new bid on the listing. The bid is added to the listing's bids vector (if not already there),
    and the current price is updated.
    */
    void Listing::makeBid(double bidAmount, User* userBidding){
        lock_guard<mutex> lock(entryMutex);

        if(bidAmount<=currentPrice || !userBidding || !log) return;
        Bid* newBid = new Bid(bidAmount, this, userBidding);
        bids.push_back(newBid);
        currentPrice = bidAmount;

        log->push_back("Bid on " + name + " placed by " + userBidding->getName()
        + " for: $" + to_string(bidAmount));

        bool found = false;
        for(int i = 0; i<userBidding->getInterested()->size(); i++){
            if((*(userBidding->getInterested()))[i] == this){
                found = true;
            }
        }
        if(found == false){
            userBidding->getInterested()->push_back(this);
        }
    }

    /*
    Buys the listing outright from the seller. The listing is added to the buyer's purchased vector and removed from the seller's selling vector.
    Updates the losers of the auction and sells the listing.
    */
    void Listing::buyOutright(User* buyer){
        if(buyOutrightPrice > currentPrice){
            log->push_back(name + " purchased outright by " + buyer->getName() +
            " for: $" + to_string(buyOutrightPrice));
            listings->sellListing(this);
            bool found = false;
            for(int i = 0; i<buyer->getInterested()->size(); i++){
                if((*(buyer->getInterested()))[i] == this){
                    found = true;
                    buyer->getPurchased()->push_back(this);
                    buyer->getInterested()->erase(buyer->getInterested()->begin() + i);

                }
            }
            if(found == false){
                buyer->getPurchased()->push_back(this);
            }
            this->losers(buyer);
            this->sell();
        }
    }
    /**
    Checks if the auction should be closed based on the time remaining and updates the status of the listing accordingly.
    */
    void Listing::checkCloseAuction(){
        lock_guard<mutex> lock(entryMutex);

        if(this->checkTime() <= 0){
            listings->sellListing(this);
            log->push_back(this->getHighestBid()->getBidder()->getName() +
            " has won the auction for " + name + " at the price of: $" +
            to_string(currentPrice));

            this->getHighestBid()->getBidder()->getPurchased()->push_back(this);
            for(int i = 0; i < this->getHighestBid()->getBidder()->getInterested()->size(); i++){
                if((*(this->getHighestBid()->getBidder()->getInterested()))[i] == this){
                    this->getHighestBid()->getBidder()->getPurchased()->push_back(this);

                    this->getHighestBid()->getBidder()->getInterested()->
                    erase(this->getHighestBid()->getBidder()->getInterested()->begin() + i);
                }
            }
            this->losers(this->getHighestBid()->getBidder());
            this->sell();
        }
    }
};