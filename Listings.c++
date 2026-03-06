//Listings Class Implemented by Benjamin Castillo III
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

    int Listings::getNumSoldListings(){
        return sold.size();
    }

    void Listings::setLog(vector<string>* newLog){
        log = newLog;
    }

    void Listings::sortAlpha(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int max = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->getName() > allListings[max]->getName()){
                    max = j;
                }
            }
            if(max != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[max];
                allListings[max] = temp;
            }
        }
    }

    void Listings::sortBuyOutright(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int max = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->getBuyOutrightPrice() > allListings[max]->getBuyOutrightPrice()){
                    max = j;
                }
            }
            if(max != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[max];
                allListings[max] = temp;
            }
        }
    }

    void Listings::sortCurrPrice(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int max = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->getPrice() > allListings[max]->getPrice()){
                    max = j;
                }
            }
            if(max != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[max];
                allListings[max] = temp;
            }
        }
    }

    void Listings::sortTimeLeft(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int max = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->checkTime() > allListings[max]->checkTime()){
                    max = j;
                }
            }
            if(max != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[max];
                allListings[max] = temp;
            }
        }
    }

    void Listings::sortAlphaRev(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int min = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->getName() < allListings[min]->getName()){
                    min = j;
                }
            }
            if(min != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[min];
                allListings[min] = temp;
            }
        }
    }

    void Listings::sortBuyOutrightRev(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int min = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->getBuyOutrightPrice() < allListings[min]->getBuyOutrightPrice()){
                    min = j;
                }
            }
            if(min != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[min];
                allListings[min] = temp;
            }
        }
    }

    void Listings::sortCurrPriceRev(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int min = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->getPrice() < allListings[min]->getPrice()){
                    min = j;
                }
            }
            if(min != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[min];
                allListings[min] = temp;
            }
        }
    }

    void Listings::sortTimeLeftRev(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int min = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->checkTime() < allListings[min]->checkTime()){
                    min = j;
                }
            }
            if(min != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[min];
                allListings[min] = temp;
            }
        }
    }

    void Listings::checkCloseAuction(){
        for(int i = 0; i < allListings.size(); i++){
            allListings[i]->checkCloseAuction();
        }
    }

    void Listings::saveToFile(){

    }
}