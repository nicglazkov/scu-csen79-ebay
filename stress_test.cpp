#include "Listing.h"
#include "Listings.h"
#include "User.h"
#include "Bid.h"
#include "HashTable.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // Same setup as main.c++ so Listing/Listings statics are set
    vector<string> *globalLog = new vector<string>;
    CSEN79::Listings *allListings = new CSEN79::Listings();
    CSEN79::Listing::setLog(globalLog);
    CSEN79::Listing::setListings(allListings);
    allListings->setLog(globalLog);

    CSEN79::HashTable users;
    vector<string> userNames;
    for (int i = 0; i < 10; i++)
    {
        string name = "user" + to_string(i);
        users.insert(name, new CSEN79::User(name));
        userNames.push_back(name);
    }

    vector<CSEN79::Listing *> listings;
    listings.reserve(1000000);

    const int NUM_ITEMS = 1000000;
    const int BIDS_PER_ITEM = 2;

    auto tStart = chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_ITEMS; i++)
    {
        string name = "StressItem_" + to_string(i);
        string desc = "Description for stress item " + to_string(i);
        double startPrice = 5.0 + (rand() % 950) / 10.0;           // 5.0–99.5
        double buyout = startPrice * (1.5 + (rand() % 50) / 50.0); // 1.5x–2.5x
        int sellTime = 3600 + (rand() % 86400);
        CSEN79::User *seller = users.find(userNames[i % 10]);
        seller->makeListing(name, desc, startPrice, buyout, sellTime);
        CSEN79::Listing *L = seller->getSelling()->back();
        allListings->addListing(L);
        listings.push_back(L);
    }

    // 2. Add a couple of bids to each item (2 different users, not the seller)
    for (CSEN79::Listing *L : listings)
    {
        string sellerName = L->getSeller()->getName();
        double curr = L->getPrice();
        // First bid: pick a non-seller user
        string u1;
        do
        {
            u1 = userNames[rand() % 10];
        } while (u1 == sellerName);
        users.find(u1)->placeBid(L, curr + 1.0);
        curr = L->getPrice();
        // Second bid: pick another non-seller, non-u1 user
        string u2;
        do
        {
            u2 = userNames[rand() % 10];
        } while (u2 == sellerName || u2 == u1);
        users.find(u2)->placeBid(L, curr + 1.0);
    }

    auto tEnd = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = tEnd - tStart;

    int totalOps = NUM_ITEMS + NUM_ITEMS * BIDS_PER_ITEM;
    double throughput = totalOps / elapsed.count();

    cout << " Stress Test Results " << endl;
    cout << "Items created: " << NUM_ITEMS << endl;
    cout << "Bids per item: " << BIDS_PER_ITEM << endl;
    cout << "Total operations: " << totalOps << endl;
    cout << "Time (s): " << elapsed.count() << endl;
    cout << "Throughput (ops/s): " << throughput << endl;
    cout << "Listings in system: " << allListings->getNumListings() << endl;

    delete allListings;
    delete globalLog;
    for (int i = 0; i < CSEN79::HashTable::SIZE; i++)
        if (users.isOccupied(i))
            delete users.getValue(i);

    return 0;
}
