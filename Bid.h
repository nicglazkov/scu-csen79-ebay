// Bid.h, implemented by Lucas Woodman
// Represents a single bid placed by a user on a listing
#ifndef BID_H
#define BID_H

namespace CSEN79
{
    // Forward declarations, full definitions are in their respective .h files
    class Listing;
    class User;

    class Bid
    {
    public:
        Bid();
        // constructs a bid with the given amount, target listing, and bidding user
        Bid(double amount, Listing *listing, User *bidder);

        // returns the bid amount
        double getAmount() const;
        // returns a pointer to the listing this bid was placed on
        Listing *getListing();
        // returns a pointer to the user who placed this bid
        User *getBidder();

    private:
        double amount;
        Listing *listing;
        User *bidder;
    };
}

#endif
