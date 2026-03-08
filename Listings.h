// Listings Class Implemented by Benjamin Castillo III
#ifndef LISTINGS_H
#define LISTINGS_H

#include <vector>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;
namespace CSEN79
{
    // Forward declaration — full definition is in Listing.h
    class Listing;

    class Listings
    {
    private:
        vector<Listing *> allListings;
        vector<Listing *> sold;
        static vector<string> *log;
        mutex listMutex;

    public:
        Listings();
        ~Listings();
        void addListing(Listing *newListing);
        void sellListing(Listing *soldListing);
        int getNumListings();
        int getNumSoldListings();
        void setLog(vector<string> *newLog);
        Listing *getListing(string name);
        Listing *getSoldListing(string name);

        // Sorting Functions
        void sortAlpha();
        void sortBuyOutright();
        void sortCurrPrice();
        void sortTimeLeft();
        void sortAlphaRev();
        void sortBuyOutrightRev();
        void sortCurrPriceRev();
        void sortTimeLeftRev();

        void checkCloseAuction();
        void saveToFile();
    };
}

#endif
