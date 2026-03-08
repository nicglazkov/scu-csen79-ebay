// Listing Class Implemented by Benjamin Castillo III
#ifndef LISTING_H
#define LISTING_H

#include <string>
#include <vector>
#include <ctime>
#include <mutex>

using namespace std;
namespace CSEN79
{
    // Forward declarations — full definitions are in their respective .h files
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
        vector<Bid *> bids;
        int sellTime;
        time_t startTime;
        User *seller;
        static vector<string> *log;
        static Listings *listings;
        static mutex logMutex;
        mutex entryMutex;

    public:
        Listing();
        Listing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime, User *seller);
        ~Listing();

        string getName();
        string getDescription();
        double getPrice();
        double getBuyOutrightPrice();
        User *getSeller();
        void buyOutright(User *buyer);
        double checkTime();
        void makeBid(double bidPrice, User *userBidding);
        static void setLog(vector<string> *newLog);
        static void setListings(Listings *newListings);
        static void addLog(const string &msg);
        int getSellTime();
        Bid *getHighestBid();
        void losers(User *winner);
        const vector<Bid *> &getBids();
        void checkCloseAuction();
        void sell();
    };
};

#endif
