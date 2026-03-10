// Listing.h, implemented by Benjamin Castillo III
// Represents a single auction listing, tracking its bids, seller, prices, and expiry time
#ifndef LISTING_H
#define LISTING_H

#include <string>
#include <vector>
#include <ctime>
#include <mutex>

using namespace std;
namespace CSEN79
{
    // Forward declarations, full definitions are in their respective .h files
    class Bid;
    class User;
    class Listings;

    class Listing
    {
    private:
        string name;
        string description;
        double startingPrice;
        double buyOutrightPrice;
        double currentPrice;
        vector<Bid *> bids;  // sorted in ascending order, highest bid is always last
        int sellTime;        // auction duration in seconds
        time_t startTime;
        User *seller;
        static vector<string> *log;      // shared across all listings, set once at startup
        static Listings *listings;       // shared bag, used to trigger auction close
        static mutex logMutex;           // guards concurrent writes to the shared log
        mutex entryMutex;                // guards per-listing state during bids and close

    public:
        Listing();
        // constructs a listing with name, description, prices, duration in seconds, and seller
        Listing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime, User *seller);
        ~Listing();

        string getName();
        string getDescription();
        // returns the current highest bid price, or starting price if no bids
        double getPrice();
        double getBuyOutrightPrice();
        User *getSeller();
        // immediately sells to buyer at the buy outright price, precondition: listing is still active
        void buyOutright(User *buyer);
        // returns seconds remaining until the auction closes, negative if already expired
        double checkTime();
        // places a bid, precondition: bidPrice must exceed current highest bid
        void makeBid(double bidPrice, User *userBidding);
        // sets the shared log vector for all Listing instances, called once at startup
        static void setLog(vector<string> *newLog);
        // sets the shared Listings bag for all Listing instances, called once at startup
        static void setListings(Listings *newListings);
        // appends a message to the shared log, thread safe via logMutex
        static void addLog(const string &msg);
        static mutex &getLogMutex();
        // returns the auction duration in seconds as set at construction
        int getSellTime();
        // returns the absolute expiry timestamp, computed as startTime + sellTime
        time_t getExpiryTime();
        // returns the highest bid, which is the last element of the sorted bids vector
        Bid *getHighestBid();
        const vector<Bid *> &getBids();
        // moves all bidders except winner into their lost lists
        void losers(User *winner);
        // closes the auction if expiry time has passed, no-op otherwise
        void checkCloseAuction();
        // finalizes the sale, awards listing to the highest bidder and notifies losers
        void sell();
    };
};

#endif
