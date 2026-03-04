#include <string>
#include <vector>
#include <ctime>
#include "Bid.h"
#include "User.h"

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
            bool sold;

        public:
        Listing(){};
        
        Listing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime){};

        string getName(){};

        string getDescription(){};

        double getPrice(){};

        double getBuyOutrightPrice(){};

        void buyOutright(){};

        double checkTime(){};

        void makeBid(double bidPrice){};
    };

};