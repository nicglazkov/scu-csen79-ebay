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

// Searches the users array for a user with the given name.
User *findUser(User *users[], int numUsers, const string &name)
{
    for (int i = 0; i < numUsers; i++)
    {
        if (users[i]->getName() == name)
            return users[i];
    }
    return nullptr;
}

// Reads listings.json and returns it as a string to send to the browser.
string readListingsFile()
{
    ifstream f("data/listings.json");
    if (!f) return "[]";
    string contents((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    return contents;
}

int main()
{
    // --- Setup ---
    const int NUM_USERS = 10;
    vector<string> *globalLog = new vector<string>;
    User *users[NUM_USERS];
    Listings *allListings = new Listings();

    Listing::setLog(globalLog);
    Listing::setListings(allListings);
    allListings->setLog(globalLog);

    for (int i = 0; i < NUM_USERS; i++)
        users[i] = new User("user" + to_string(i));

    // --- Seed Listings ---
    users[0]->makeListing("Vintage Camera",    "A classic 35mm film camera in great condition.",  25.00,  120.00, 300);
    users[1]->makeListing("Gaming Chair",      "Ergonomic chair with lumbar support.",            80.00,  350.00, 300);
    users[2]->makeListing("Electric Keyboard", "61-key beginner keyboard with built-in sounds.",  40.00,  180.00, 300);
    users[3]->makeListing("Old Laptop",        "Used but functional. Good for basic tasks.",      50.00,  200.00, 300);
    users[4]->makeListing("Desk Lamp",         "Adjustable LED lamp, barely used.",                8.00,   35.00, 300);

    for (int i = 0; i < 5; i++)
        allListings->addListing(users[i]->getSelling()->back());

    allListings->saveToFile();

    // Start the background auction monitor on a separate thread
    thread auctionThread(backgroundAuctionMonitor, allListings);

    // --- Web Server ---
    // svr handles requests from the browser.
    // Think of each svr.Get / svr.Post as "when the browser asks for X, do Y".
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
        res.set_content(readListingsFile(), "application/json");
    });

    // When the browser submits a bid (item name + username + bid amount),
    // find the listing and user, place the bid, then save.
    svr.Post("/bid", [&](const Request &req, Response &res)
    {
        string listingName = req.get_param_value("name");
        string userName    = req.get_param_value("user");
        double amount      = stod(req.get_param_value("amount"));

        Listing *listing = allListings->getListing(listingName);
        User *user = findUser(users, NUM_USERS, userName);

        if (!listing || !user) { res.status = 404; return; }

        user->placeBid(listing, amount);
        allListings->saveToFile();
        res.set_content("ok", "text/plain");
    });

    // When the browser submits a buy-outright request (item name + username),
    // find the listing and user and complete the purchase.
    svr.Post("/buyout", [&](const Request &req, Response &res)
    {
        string listingName = req.get_param_value("name");
        string userName    = req.get_param_value("user");

        Listing *listing = allListings->getListing(listingName);
        User *user = findUser(users, NUM_USERS, userName);

        if (!listing || !user) { res.status = 404; return; }

        user->buyOutright(listing);
        allListings->saveToFile();
        res.set_content("ok", "text/plain");
    });

    // Serve the frontend files (HTML, CSS, JS) from the project folder.
    // Opening http://localhost:8080 in a browser will load index.html.
    svr.set_mount_point("/", ".");

    cout << "eBay Lite running at http://localhost:8080" << endl;
    svr.listen("0.0.0.0", 8080);

    // Cleanup when the server stops
    keepRunning = false;
    auctionThread.join();
    for (int i = 0; i < NUM_USERS; i++) delete users[i];
    delete allListings;
    delete globalLog;
    return 0;
}
