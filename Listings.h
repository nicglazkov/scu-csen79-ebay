#include <vector>

using namespace std;
namespace CSEN79{
    class Listings{
        private:
            vector<Listing*> allListings;
            static vector<string> log;
            static User* user;

        public:
            void addListing(Listing* newListing){};
            void removeListing(Listing* delListing){};
            int getNumListings(){};
            void setLog(vector<string> &newLog){};
            void setUser(User* newUser){};
            void sortAlpha(){};
            void sortBuyOutright(){};
            void sortCurrPrice(){};
            void sortTimeLeft(){};
            void checkCloseAuction(){};
    };
}