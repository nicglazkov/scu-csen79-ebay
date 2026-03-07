#include "Listing.h"
#include "Listings.h"
#include "User.h"
#include "Bid.h"
#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std; 

namespace CSEN79{
    atomic<bool> keepRunning(true);

    void backgroundAuctionMonitor(Listings* listings){
        int counter = 0;
        while(keepRunning){
            listings->checkCloseAuction();

            if(counter >= 7){
                listings->saveToFile();
                counter = 0;
            }
            this_thread::sleep_for(chrono::seconds(1));
            counter++;
        }
    }

    int main(){
        vector<string>* globalLog = new vector<string>;
        User* users[10];
        int currentUserNumber;
        Listings* allListings = new Listings();
        Listing* temp =  new Listing();
        temp->setLog(globalLog);
        allListings->setLog(globalLog);
        temp->setListings(allListings);
        delete temp;
        for(int i = 0; i < 10; i++){
            users[i] = new User("user"+i);
        }
        thread auctionThread(backgroundAuctionMonitor, allListings);
        cout << "Auction system is running. Press Enter to quit: " << endl;
        cin.get();


        keepRunning = false;
        auctionThread.join();
        delete allListings;
        delete globalLog;
        return 0;
    }
}