/**
 * Stress test for eBay-Lite backend (Success Criteria §4).
 * - Generates 1000 random items (listings).
 * - Adds a couple of bids to each item.
 * - Measures throughput (operations per second) and reports.
 *
 * Build (same sources as auction_app; run from project root):
 *   g++ -std=c++17 stress_test.cpp User.cpp Listing.c++ Listings.c++ Bid.cpp -pthread -o stress_test
 * Run:
 *   ./stress_test
 */

#include "Listing.h"
#include "Listings.h"
#include "User.h"
#include "Bid.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    // Same setup as main.c++ so Listing/Listings statics are set
    vector<string>* globalLog = new vector<string>;
    CSEN79::Listings* allListings = new CSEN79::Listings();
    CSEN79::Listing* temp = new CSEN79::Listing();
    temp->setLog(globalLog);
    allListings->setLog(globalLog);
    temp->setListings(allListings);
    delete temp;

    CSEN79::User* users[10];
    for (int i = 0; i < 10; i++) {
        users[i] = new CSEN79::User("user" + to_string(i));
    }

    vector<CSEN79::Listing*> listings;
    listings.reserve(1000);

    const int NUM_ITEMS = 1000;
    const int BIDS_PER_ITEM = 2;

    auto tStart = chrono::high_resolution_clock::now();

    // 1. Generate 1000 random items
    for (int i = 0; i < NUM_ITEMS; i++) {
        string name = "StressItem_" + to_string(i);
        string desc = "Description for stress item " + to_string(i);
        double startPrice = 5.0 + (rand() % 950) / 10.0;   // 5.0–99.5
        double buyout = startPrice * (1.5 + (rand() % 50) / 50.0);  // 1.5x–2.5x
        int sellTime = 3600 + (rand() % 86400);
        CSEN79::User* seller = users[i % 10];
        seller->makeListing(name, desc, startPrice, buyout, sellTime);
        CSEN79::Listing* L = seller->getSelling()->back();
        allListings->addListing(L);
        listings.push_back(L);
    }

    // 2. Add a couple of bids to each item (2 different users, not the seller)
    for (CSEN79::Listing* L : listings) {
        CSEN79::User* seller = L->getSeller();
        double curr = L->getPrice();
        int sellerIdx = -1;
        for (int k = 0; k < 10; k++) {
            if (users[k] == seller) { sellerIdx = k; break; }
        }
        // First bid: pick a non-seller user
        int u1 = 0;
        while (u1 == sellerIdx) u1 = rand() % 10;
        users[u1]->placeBid(L, curr + 1.0);
        curr = L->getPrice();
        // Second bid: pick another non-seller user
        int u2 = (sellerIdx + 1) % 10;
        if (u2 == u1) u2 = (u2 + 1) % 10;
        if (u2 == sellerIdx) u2 = (u2 + 1) % 10;
        users[u2]->placeBid(L, curr + 1.0);
    }

    auto tEnd = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = tEnd - tStart;

    int totalOps = NUM_ITEMS + NUM_ITEMS * BIDS_PER_ITEM;  // 1000 + 2000 = 3000
    double throughput = totalOps / elapsed.count();

    cout << "--- Stress Test Results ---" << endl;
    cout << "Items created: " << NUM_ITEMS << endl;
    cout << "Bids per item: " << BIDS_PER_ITEM << endl;
    cout << "Total operations: " << totalOps << endl;
    cout << "Time (s): " << elapsed.count() << endl;
    cout << "Throughput (ops/s): " << throughput << endl;
    cout << "Listings in system: " << allListings->getNumListings() << endl;
    cout << "----------------------------" << endl;

    // Cleanup
    delete allListings;
    delete globalLog;
    for (int i = 0; i < 10; i++) delete users[i];

    return 0;
}
