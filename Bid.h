// Bid.h
#ifndef BID_H
#define BID_H

namespace CSEN79
{
    // Forward declarations — full definitions are in their respective .h files
    class Listing;
    class User;

    class Bid
    {
    public:
        Bid();
        Bid(double amount, Listing *listing, User *bidder);

        double getAmount() const;
        Listing *getListing();
        User *getBidder();

    private:
        double amount;
        Listing *listing;
        User *bidder;
    };
}

#endif
