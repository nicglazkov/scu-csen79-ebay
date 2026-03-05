#include <vector>

using namespace std;
namespace CSEN79{
    class Listings{
        private:
            vector<Listing*> allListings;
            vector<Listing*> sold;
            static vector<string>* log;

        public:
            Listings(){
                log = nullptr;
            }
            void addListing(Listing* newListing){};
            void sellListing(Listing* soldListing){};
            int getNumListings(){};
            int getNumSoldListings(){};
            void setLog(vector<string>* newLog){};

            //Sorting Functions
            void sortAlpha(){};
            void sortBuyOutright(){};
            void sortCurrPrice(){};
            void sortTimeLeft(){};
            void sortAlphaRev(){};
            void sortBuyOutrightRev(){};
            void sortCurrPriceRev(){};
            void sortTimeLeftRev(){};

            //Multithreaded function, constantly running
            void checkCloseAuction(){};
    };
}