#include "Listing.h"
#include "Listings.h"
#include "User.h"
#include "Bid.h"
#include "third_party/httplib/httplib.h"
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <chrono>
#include "HashTable.h"

using namespace std;
using namespace httplib;
using namespace CSEN79;

atomic<bool> keepRunning(true);

// Runs in the background. Checks if any auctions have expired every second,
// and saves the current state to listings.json every 7 seconds.
void backgroundAuctionMonitor(Listings *listings)
{
    int counter = 0;
    while (keepRunning)
    {
        listings->checkCloseAuction();
        if (counter >= 7)
        {
            listings->saveToFile();
            counter = 0;
        }
        this_thread::sleep_for(chrono::seconds(1));
        counter++;
    }
}

// Reads listings.json and returns it as a string to send to the browser.
string readListingsFile()
{
    ifstream f("data/listings.json");
    if (!f)
        return "[]";
    string contents((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    return contents;
}

int main()
{
    // Setup
    const int NUM_USERS = 10;
    vector<string> *globalLog = new vector<string>;
    HashTable users;
    Listings *allListings = new Listings();

    Listing::setLog(globalLog);
    Listing::setListings(allListings);
    allListings->setLog(globalLog);

    for (int i = 0; i < NUM_USERS; i++)
    {
        string name = "user" + to_string(i);
        users.insert(name, new User(name));
    }

    // Seed Listings
    users.find("user0")->makeListing("Vintage Camera", "A classic 35mm film camera in great condition.", 25.00, 120.00, 300);
    users.find("user1")->makeListing("Gaming Chair", "Ergonomic chair with lumbar support.", 80.00, 350.00, 300);
    users.find("user2")->makeListing("Electric Keyboard", "61-key beginner keyboard with built-in sounds.", 40.00, 180.00, 300);
    users.find("user3")->makeListing("Old Laptop", "Used but functional. Good for basic tasks.", 50.00, 200.00, 300);
    users.find("user4")->makeListing("Desk Lamp", "Adjustable LED lamp, barely used.", 8.00, 35.00, 300);

    for (int i = 0; i < 5; i++)
        allListings->addListing(users.find("user" + to_string(i))->getSelling()->back());

    allListings->saveToFile();

    // Start the background auction monitor on a separate thread
    thread auctionThread(backgroundAuctionMonitor, allListings);

    // Web Server
    // svr handles requests from the browser.
    Server svr;

    // When the browser asks for the list of listings (optionally sorted),
    // save the current state and send back listings.json.
    svr.Get("/listings", [&](const Request &req, Response &res)
            {
        if (req.has_param("sort"))
        {
            string sort = req.get_param_value("sort");
            if      (sort == "sortAlpha")          allListings->sortAlpha();
            else if (sort == "sortAlphaRev")       allListings->sortAlphaRev();
            else if (sort == "sortCurrPrice")      allListings->sortCurrPrice();
            else if (sort == "sortCurrPriceRev")   allListings->sortCurrPriceRev();
            else if (sort == "sortBuyOutright")    allListings->sortBuyOutright();
            else if (sort == "sortBuyOutrightRev") allListings->sortBuyOutrightRev();
            else if (sort == "sortTimeLeft")       allListings->sortTimeLeft();
            else if (sort == "sortTimeLeftRev")    allListings->sortTimeLeftRev();
        }
        else
        {
            allListings->saveToFile();
        }
        res.set_content(readListingsFile(), "application/json"); });

    // When the browser submits a bid (item name + username + bid amount),
    // find the listing and user, place the bid, then save.
    svr.Post("/bid", [&](const Request &req, Response &res)
             {
        string listingName = req.get_param_value("name");
        string userName    = req.get_param_value("user");

        double amount;
        try { amount = stod(req.get_param_value("amount")); }
        catch (...) { res.status = 400; return; }

        Listing *listing = allListings->getListing(listingName);
        User *user = users.find(userName);

        if (!listing || !user) { res.status = 404; return; }

        user->placeBid(listing, amount);
        allListings->saveToFile();
        res.set_content("ok", "text/plain"); });

    // When the browser submits a buy-outright request (item name + username),
    // find the listing and user and complete the purchase.
    svr.Post("/buyout", [&](const Request &req, Response &res)
             {
        string listingName = req.get_param_value("name");
        string userName    = req.get_param_value("user");

        Listing *listing = allListings->getListing(listingName);
        User *user = users.find(userName);

        if (!listing || !user) { res.status = 404; return; }

        user->buyOutright(listing);
        allListings->saveToFile();
        res.set_content("ok", "text/plain"); });

    // Returns all sold listings as JSON so the dashboard can show them greyed out at the bottom.
    svr.Get("/sold-listings", [&](const Request &req, Response &res)
            {
        vector<Listing *> soldItems = allListings->getSoldSnapshot();
        string json = "[\n";
        for (int i = 0; i < (int)soldItems.size(); i++)
        {
            Listing *t = soldItems[i];
            json += "  {\n";
            json += "    \"name\": \""        + t->getName()              + "\",\n";
            json += "    \"seller\": \""      + t->getSeller()->getName() + "\",\n";
            json += "    \"currentPrice\": "  + to_string(t->getPrice())  + "\n";
            json += "  }";
            if (i < (int)soldItems.size() - 1) json += ",";
            json += "\n";
        }
        json += "]";
        res.set_content(json, "application/json"); });

    // When the browser checks if a specific item has been sold, return "true" or "false".
    svr.Get("/sold", [&](const Request &req, Response &res)
            {
        string name = req.get_param_value("name");
        Listing *listing = allListings->getSoldListing(name);
        res.set_content(listing ? "true" : "false", "text/plain"); });

    // When the browser asks for a user's profile data (selling, purchased, bids, lost),
    // find the user and return their info as JSON.
    svr.Get("/user", [&](const Request &req, Response &res)
            {
        string userName = req.get_param_value("name");
        User *user = users.find(userName);
        if (!user) { res.status = 404; return; }

        // Helper that turns a vector of Listing pointers into a JSON array of names
        auto listingsToJson = [](vector<Listing *> *lst) {
            string out = "[";
            for (int i = 0; i < (int)lst->size(); i++) {
                out += "\"" + (*lst)[i]->getName() + "\"";
                if (i < (int)lst->size() - 1) out += ", ";
            }
            out += "]";
            return out;
        };

        string json = "{\n";
        json += "  \"name\": \""      + user->getName() + "\",\n";
        json += "  \"selling\": "     + listingsToJson(user->getSelling())   + ",\n";
        json += "  \"purchased\": "   + listingsToJson(user->getPurchased()) + ",\n";
        json += "  \"interested\": "  + listingsToJson(user->getInterested()) + ",\n";
        json += "  \"lost\": "        + listingsToJson(user->getLost())      + "\n";
        json += "}";

        res.set_content(json, "application/json"); });

    // Creates a new listing for the given user and adds it to the active listings.
    svr.Post("/add-listing", [&](const Request &req, Response &res)
             {
        string name        = req.get_param_value("name");
        string description = req.get_param_value("description");
        string userName    = req.get_param_value("user");

        double startPrice, buyoutPrice;
        int sellTime;
        try {
            startPrice  = stod(req.get_param_value("startPrice"));
            buyoutPrice = stod(req.get_param_value("buyoutPrice"));
            sellTime    = stoi(req.get_param_value("sellTime"));
        } catch (...) { res.status = 400; return; }

        User *user = users.find(userName);
        if (!user) { res.status = 404; return; }

        user->makeListing(name, description, startPrice, buyoutPrice, sellTime);
        allListings->addListing(user->getSelling()->back());
        allListings->saveToFile();
        res.set_content("ok", "text/plain"); });

    // Removes a listing. Only the seller can remove their own listing.
    svr.Post("/remove-listing", [&](const Request &req, Response &res)
             {
        string listingName = req.get_param_value("name");
        string userName    = req.get_param_value("user");

        Listing *listing = allListings->getListing(listingName);
        User    *user    = users.find(userName);

        if (!listing || !user)              { res.status = 404; return; }
        if (listing->getSeller() != user)   { res.status = 403; return; }

        allListings->removeListing(listing);

        vector<Listing *> *selling = user->getSelling();
        for (int i = 0; i < (int)selling->size(); i++)
        {
            if ((*selling)[i] == listing)
            {
                selling->erase(selling->begin() + i);
                break;
            }
        }
        delete listing;
        allListings->saveToFile();
        res.set_content("ok", "text/plain"); });

    // Runs a self-contained stress test for 3 seconds and returns timing stats.
    // Each iteration creates 1000 listings and places 2 bids on each (3000 ops).
    svr.Post("/stress-test", [](const Request &, Response &res)
             {
        srand(static_cast<unsigned>(time(nullptr)));

        vector<string> userNames;
        for (int i = 0; i < 10; i++)
            userNames.push_back("user" + to_string(i));

        int totalOps = 0;
        auto tStart = chrono::high_resolution_clock::now();

        while (chrono::duration<double>(chrono::high_resolution_clock::now() - tStart).count() < 3.0)
        {
            CSEN79::HashTable stressUsers;
            for (int i = 0; i < 10; i++)
                stressUsers.insert(userNames[i], new CSEN79::User(userNames[i]));

            CSEN79::Listings *stressListings = new CSEN79::Listings();
            vector<CSEN79::Listing *> items;

            for (int i = 0; i < 1000; i++) {
                double startPrice = 5.0 + (rand() % 950) / 10.0;
                double buyout = startPrice * (1.5 + (rand() % 50) / 50.0);
                CSEN79::User *seller = stressUsers.find(userNames[i % 10]);
                seller->makeListing("Item_" + to_string(i), "", startPrice, buyout, 3600);
                CSEN79::Listing *L = seller->getSelling()->back();
                stressListings->addListing(L);
                items.push_back(L);
            }

            for (CSEN79::Listing *L : items) {
                string sel = L->getSeller()->getName();
                double curr = L->getPrice();
                string u1;
                do { u1 = userNames[rand() % 10]; } while (u1 == sel);
                stressUsers.find(u1)->placeBid(L, curr + 1.0);
                curr = L->getPrice();
                string u2;
                do { u2 = userNames[rand() % 10]; } while (u2 == sel || u2 == u1);
                stressUsers.find(u2)->placeBid(L, curr + 1.0);
            }

            totalOps += 3000;

            for (int i = 0; i < CSEN79::HashTable::SIZE; i++)
                if (stressUsers.isOccupied(i)) delete stressUsers.getValue(i);
            delete stressListings;
        }

        double elapsed = chrono::duration<double>(chrono::high_resolution_clock::now() - tStart).count();
        double throughput = totalOps / elapsed;
        double avgMs = (elapsed / totalOps) * 1000;

        char buf[256];
        snprintf(buf, sizeof(buf),
            "{\"totalOps\":%d,\"elapsedMs\":%.0f,\"throughput\":%d,\"avgMsPerOp\":%.4f}",
            totalOps, elapsed * 1000, (int)throughput, avgMs);

        res.set_content(buf, "application/json"); });

    // Serve the frontend files (HTML, CSS, JS) from the project folder.
    // Opening http://localhost:8080 in a browser will load index.html.
    svr.set_mount_point("/", ".");

    cout << "eBay Lite running at http://localhost:8080" << endl;
    svr.listen("0.0.0.0", 8080);

    // Cleanup when the server stops
    keepRunning = false;
    auctionThread.join();
    for (int i = 0; i < HashTable::SIZE; i++)
        if (users.isOccupied(i)) delete users.getValue(i);
    delete allListings;
    delete globalLog;
    return 0;
}
