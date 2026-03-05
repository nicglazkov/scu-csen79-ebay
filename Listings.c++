#include "Listing.h"

using namespace std;

namespace CSEN79{

    void Listings::addListing(Listing* newListing){
        allListings[allListings.size()] = newListing;
    }

    void Listings::removeListing(Listing* delListing){
        for(int i = 0; i < allListings.size(); i++){
            if(allListings[i] == delListing){
                log[log.size()] = allListings[i]->getName() + " has been sold to " ;
                allListings.erase(allListings.begin()+i);
            }
        }
    }

    int Listings::getNumListings(){
        return allListings.size();
    }

    void Listings::setLog(vector<string> &newLog){
        log = newLog;
    }

    void Listings::setUser(User* newUser){
        user = newUser;
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