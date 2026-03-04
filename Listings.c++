#include "Listing.h"

using namespace std;

namespace CSEN79{
    void Listings::addListing(Listing* newListing){
        allListings[allListings.size()] = newListing;
    }

    void Listings::removeListing(Listing* delListing){
        for(int i = 0; i < allListings.size(); i++){
            if(allListings[i] == delListing){
                allListings.erase(allListings.begin()+i);
                log[log.size()] = "";
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
}