//Listings Class Implemented by Benjamin Castillo III
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
#include <fstream>
#include <iomanip>

using namespace std;
namespace CSEN79{
    class Listing;
    class Listings{
        private:
            vector<Listing*> allListings;
            vector<Listing*> sold;
            static vector<string>* log;
            mutex listMutex;

        public:
            Listings();
            ~Listings();
            void addListing(Listing* newListing);
            void sellListing(Listing* soldListing);
            int getNumListings();
            int getNumSoldListings();
            void setLog(vector<string>* newLog);
            Listing* getListing(string name);

            //Sorting Functions
            void sortAlpha();
            void sortBuyOutright();
            void sortCurrPrice();
            void sortTimeLeft();
            void sortAlphaRev();
            void sortBuyOutrightRev();
            void sortCurrPriceRev();
            void sortTimeLeftRev();

            //Multithreaded function, constantly running
            void checkCloseAuction();
            void saveToFile();
    };
}