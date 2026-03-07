//Listing Class Implemented by Benjamin Castillo III
#include <string>
#include <vector>
#include <ctime>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
#include "Bid.h"
#include "User.h"
#include "Listings.h"

using namespace std;
namespace CSEN79{

    class Listing{
        private:
            string name;
            string description;
            double startingPrice;
            double buyOutrightPrice;
            double currentPrice;
            vector<Bid*> bids;
            int sellTime;
            time_t startTime;
            User* seller;
            static vector<string>* log;
            static Listings* listings;
            mutex entryMutex;

        public:
            Listing();
            Listing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime, User* seller);
            ~Listing();
            
            string getName();
            string getDescription();
            double getPrice(){};
            double getBuyOutrightPrice();
            User* getSeller();
            void buyOutright(User* buyer);
            double checkTime(){};
            void makeBid(double bidPrice, User* userBidding);
            void setLog(vector<string>* newLog);
            void setListings(Listings* newListings);
            Bid* getHighestBid();
            void losers(User* winner);
            const vector<Bid*>& getBids();
            //Multithreaded function, constantly running
            void checkCloseAuction();
            void sell();
    };

};