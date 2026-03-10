// Listings.h, implemented by Benjamin Castillo III
// Bag class that manages all active and sold listings, keyed by expiry time for efficient auction closing
#ifndef LISTINGS_H
#define LISTINGS_H

#include <vector>
#include <map>
#include <ctime>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;
namespace CSEN79
{
    // Forward declaration, full definition is in Listing.h
    class Listing;

    class Listings
    {
    private:
        map<time_t, vector<Listing *>> allListings;  // sorted by expiry time, earliest entry is always checked first
        vector<Listing *> sold;
        static vector<string> *log;
        mutex listMutex;
        // formats and writes the given listings vector to the JSON persistence file
        void writeJson(const vector<Listing *> &listings);

    public:
        Listings();
        ~Listings();
        // inserts a listing into the map keyed by its expiry timestamp, O(log n)
        void addListing(Listing *newListing);
        // moves a listing from active map to the sold vector
        void sellListing(Listing *soldListing);
        // removes an active listing without selling it, used when a seller cancels
        void removeListing(Listing *listing);
        // returns the total count of all active listings across all expiry buckets
        int getNumListings();
        int getNumSoldListings();
        // sets the shared log pointer, called once at startup
        void setLog(vector<string> *newLog);
        // searches active listings by name, returns nullptr if not found
        Listing *getListing(string name);
        // searches sold listings by name, returns nullptr if not found
        Listing *getSoldListing(string name);
        // returns a copy of the sold vector for safe iteration by the HTTP handler
        vector<Listing *> getSoldSnapshot();

        // sorting functions, each rebuilds a flat sorted vector using heap sort, O(n log n)
        void sortAlpha();
        void sortBuyOutright();
        void sortCurrPrice();
        void sortTimeLeft();
        void sortAlphaRev();
        void sortBuyOutrightRev();
        void sortCurrPriceRev();
        void sortTimeLeftRev();

        // iterates from the front of the map and closes any listings whose expiry has passed
        void checkCloseAuction();
        // persists active and sold listings to data/listings.json
        void saveToFile();
    };
}

#endif
