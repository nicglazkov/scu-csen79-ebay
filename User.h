// User.h
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <mutex>

namespace CSEN79
{
    // Forward declaration — full definition is in Listing.h
    class Listing;

    class User
    {
    public:
        User();
        User(std::string name);

        std::string getName();
        void makeListing(std::string name, std::string description, double startingPrice, double buyOutrightPrice, int sellTime);
        void placeBid(Listing *listing, double amount);
        void buyOutright(Listing *listing);
        // Ben Added Getters
        std::vector<Listing *> *getSelling();
        std::vector<Listing *> *getSold();
        std::vector<Listing *> *getPurchased();
        std::vector<Listing *> *getInterested();
        std::vector<Listing *> *getLost();
        std::mutex &getMutex();

    private:
        std::mutex userMutex;
        std::string name;
        std::vector<Listing *> selling;
        std::vector<Listing *> sold;
        std::vector<Listing *> purchased;
        std::vector<Listing *> interested;
        std::vector<Listing *> lost;
    };
}

#endif
