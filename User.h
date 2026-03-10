// User.h, implemented by Lucas Woodman
// Represents an auction participant, tracking all listings they are selling, bidding on, or have won or lost
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <mutex>

namespace CSEN79
{
    // Forward declaration, full definition is in Listing.h
    class Listing;

    class User
    {
    public:
        User();
        // constructs a user with the given username
        User(std::string name);

        std::string getName();
        // creates a new Listing and adds it to this user's selling vector
        void makeListing(std::string name, std::string description, double startingPrice, double buyOutrightPrice, int sellTime);
        // places a bid on a listing, adds listing to interested if not already present
        void placeBid(Listing *listing, double amount);
        // purchases a listing immediately at the buy outright price
        void buyOutright(Listing *listing);
        // returns a pointer to the vector of listings this user is currently selling
        std::vector<Listing *> *getSelling();
        // returns a pointer to the vector of listings this user has sold
        std::vector<Listing *> *getSold();
        // returns a pointer to the vector of listings this user has bought outright or won
        std::vector<Listing *> *getPurchased();
        // returns a pointer to the vector of active listings this user has bid on
        std::vector<Listing *> *getInterested();
        // returns a pointer to the vector of auctions this user bid on but did not win
        std::vector<Listing *> *getLost();
        // returns a reference to the user mutex, used by Listing to lock user state during auction close
        std::mutex &getMutex();

    private:
        std::mutex userMutex;
        std::string name;
        std::vector<Listing *> selling;    // active listings posted by this user
        std::vector<Listing *> sold;       // listings this user sold after auction close
        std::vector<Listing *> purchased;  // items won by bid or bought outright
        std::vector<Listing *> interested; // active listings this user has placed a bid on
        std::vector<Listing *> lost;       // auctions this user bid on but did not win
    };
}

#endif
