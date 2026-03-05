#include "Listing.h"

using namespace std;

namespace CSEN79{

    void Listings::addListing(Listing* newListing){
        allListings.push_back(newListing);
    }

    void Listings::sellListing(Listing* soldListing){
        if(!soldListing || !log) return;
        
        for(int i = 0; i < allListings.size(); i++){
            if(allListings[i] == soldListing){
                log->push_back(allListings[i]->getName() + " has been sold to " + allListings[i]->getSeller()->getName());
                sold.push_back(soldListing);
                allListings.erase(allListings.begin() + i);
                break;
            }
        }
    }

    int Listings::getNumListings(){
        return allListings.size();
    }

    void Listings::setLog(vector<string>* newLog){
        log = newLog;
    }

    void Listings::sortAlpha(){

    }

    void Listings::sortBuyOutright(){

    }

    void Listings::sortCurrPrice(){

    }

    void Listings::sortTimeLeft(){

    }

    void Listings::checkCloseAuction(){

    }
    void sortAlphaRev(){

    }

    void sortBuyOutrightRev(){

    }

    void sortCurrPriceRev(){

    }

    void sortTimeLeftRev(){

    }

}