#include "Bid.h"
#include "Listing.h"
namespace CSEN79 {

    Bid::Bid() {
        amount = 0.0;
        listing = nullptr;
        bidder = nullptr;
    }

    Bid::Bid(double amount, Listing* listing, User* bidder) {
        this->amount = amount;
        this->listing = listing;
        this->bidder = bidder;
    }

    double Bid::getAmount() const {
        return amount;
    }

    Listing* Bid::getListing() {
        return listing;
    }

    User* Bid::getBidder() {
        return bidder;
    }

}