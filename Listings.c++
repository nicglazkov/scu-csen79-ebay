//Listings Class Implemented by Benjamin Castillo III
#include "Listing.h"

using namespace std;

namespace CSEN79{
    
    Listings::Listings(){
        log = nullptr;
    }

    Listings::~Listings(){
        lock_guard<mutex> lock(listMutex);

        for(int i = 0; i < allListings.size(); i++){
            delete allListings[i];
        }
        allListings.clear();
        for(int i = 0; i < sold.size(); i++){
            delete sold[i];
        }
        sold.clear();
    }
    void Listings::addListing(Listing* newListing){
        lock_guard<mutex> lock(listMutex);
        allListings.push_back(newListing);
    }

    void Listings::sellListing(Listing* soldListing){
        if(!soldListing || !log) return;

        lock_guard<mutex> lock(listMutex);
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
                if(allListings[j]->getName() < allListings[max]->getName()){
                    max = j;
                }
            }
            if(max != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[max];
                allListings[max] = temp;
            }
        }
        this->saveToFile();
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
        this->saveToFile();
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
        this->saveToFile();
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
        this->saveToFile();
    }

    void Listings::sortAlphaRev(){
        int size = allListings.size();
        for(int i = 0; i < size-1; i++){
            int min = i;
            for(int j = i+1; j < size; j++){
                if(allListings[j]->getName() > allListings[min]->getName()){
                    min = j;
                }
            }
            if(min != i){
                Listing* temp = allListings[i];
                allListings[i] = allListings[min];
                allListings[min] = temp;
            }
        }
        this->saveToFile();
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
        this->saveToFile();
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
        this->saveToFile();
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
        this->saveToFile();
    }

    void Listings::checkCloseAuction(){
        lock_guard<mutex> lock(listMutex);
        for(int i = 0; i < allListings.size(); i++){
            allListings[i]->checkCloseAuction();
        }
    }

    void Listings::saveToFile(){
        lock_guard<mutex> lock(listMutex);

        ofstream outFile("listings_backup.txt", ios::trunc);
        if(!outFile){
            if(log){
                log->push_back("Error: Could Not Open Backup File");
            }
        }
        outFile << "Active Listings" << endl << endl;
        
        for(int i = 0; i < allListings.size(); i++){
            Listing* traversal = allListings[i];

            outFile << "Item: " << traversal->getName() << endl; 
            outFile << "Description: " << traversal->getDescription() << endl; 
            outFile << "Current Price: $" << fixed << setprecision(2) << traversal->getPrice() << endl; 
            outFile << "Buy Outright Price: $" << fixed << setprecision(2) << traversal->getBuyOutrightPrice() << endl; 
            outFile << "Time Left: " << traversal->getName() << " seconds" << endl;
            outFile << "Bid History:" << endl;
            for(auto bid : traversal->getBids()){
                outFile << "$" << bid->getAmount() << " by " << bid->getBidder()->getName() << endl;
            }

            outFile << "---------------------------------" << endl;
        }
        outFile.close();
    }
}