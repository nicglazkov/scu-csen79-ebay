// Bid.h
#pragma once

#include <string>

namespace CSEN79 {
    class Bid {
    public:
        Bid();
        Bid(double amount, Listing* listing, User* bidder);

        double getAmount();
        Listing* getListing();
        User* getBidder();

    private:
        double amount;
        Listing* listing;
        User* bidder;
    };

}