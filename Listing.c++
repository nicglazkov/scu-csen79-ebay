// Listing Class Implemented by Benjamin Castillo III
#include "Listing.h"
#include "Bid.h"
#include "User.h"
#include "Listings.h"

using namespace std;
namespace CSEN79
{
    bool BidComparator::operator()(const Bid *a, const Bid *b) const
    {
        return a->getAmount() < b->getAmount(); // max-heap: higher amount = higher priority
    }

    vector<string> *Listing::log = nullptr;
    Listings *Listing::listings = nullptr;
    mutex Listing::logMutex;

    void Listing::addLog(const string &msg)
    {
        if (!log) return;
        lock_guard<mutex> lock(logMutex);
        log->push_back(msg);
    }

    Listing::Listing()
    {
        name = "";
        description = "";
        startingPrice = 0.0;
        buyOutrightPrice = 0.0;
        currentPrice = startingPrice;
        sellTime = 0;
        startTime = time(nullptr);
        seller = nullptr;
    }

    Listing::Listing(string name, string description, double startingPrice, double buyOutrightPrice, int sellTime, User *seller)
    {
        this->name = name;
        this->description = description;
        this->startingPrice = startingPrice;
        this->buyOutrightPrice = buyOutrightPrice;
        currentPrice = startingPrice;
        this->sellTime = sellTime;
        startTime = time(nullptr);
        addLog("New Listing Created by " + seller->getName() + " for item: " + name);
        this->seller = seller;
    }

    Listing::~Listing()
    {
        lock_guard<mutex> lock(entryMutex);
        for (int i = 0; i < bids.size(); i++)
        {
            delete bids[i];
        }
        bids.clear();
    }

    void Listing::setLog(vector<string> *newLog)
    {
        log = newLog;
    }

    void Listing::setListings(Listings *newListings)
    {
        listings = newListings;
    }

    int Listing::getSellTime()
    {
        return sellTime;
    }

    time_t Listing::getExpiryTime()
    {
        return startTime + sellTime;
    }

    string Listing::getName()
    {
        return name;
    }

    string Listing::getDescription()
    {
        return description;
    }

    double Listing::getPrice()
    {
        return currentPrice;
    }

    double Listing::getBuyOutrightPrice()
    {
        return buyOutrightPrice;
    }

    double Listing::checkTime()
    {
        return difftime(time(nullptr), startTime);
    }

    User *Listing::getSeller()
    {
        return seller;
    }

    Bid *Listing::getHighestBid()
    {
        if (bidHeap.empty()) return nullptr;
        return bidHeap.top(); // O(1)
    }

    const vector<Bid *> &Listing::getBids()
    {
        return bids;
    };

    /*
    Sells the listing to the winning bidder. The listing is added to the seller's sold vector and removed from their selling vector.
    */
    void Listing::sell()
    {
        lock_guard<mutex> lock(seller->getMutex());
        seller->getSold()->push_back(this);
        for (int i = 0; i < seller->getSelling()->size(); i++)
        {
            if ((*(seller->getSelling()))[i] == this)
            {
                seller->getSelling()->erase(seller->getSelling()->begin() + i);
                break;
            }
        }
    }

    /*
    Move the listing from the interested vector to the won or lost vector based on if the user won or lost the auction
    */
    void Listing::losers(User *winner)
    {
        // track which users we've already processed to avoid duplicates
        // (a user may have placed multiple bids on the same item)
        vector<User *> processed;
        for (int i = 0; i < bids.size(); i++)
        {
            User *bidder = bids[i]->getBidder();
            if (bidder == winner) continue;

            bool alreadyDone = false;
            for (int k = 0; k < processed.size(); k++)
                if (processed[k] == bidder) { alreadyDone = true; break; }
            if (alreadyDone) continue;

            processed.push_back(bidder);
            lock_guard<mutex> lock(bidder->getMutex());
            bidder->getLost()->push_back(this);
            for (int j = 0; j < bidder->getInterested()->size(); j++)
            {
                if ((*(bidder->getInterested()))[j] == this)
                {
                    bidder->getInterested()->erase(bidder->getInterested()->begin() + j);
                    break;
                }
            }
        }
    }

    /*
    Places a new bid on the listing. The bid is added to the listing's bids vector (if not already there),
    and the current price is updated.
    */
    void Listing::makeBid(double bidAmount, User *userBidding)
    {
        lock_guard<mutex> lock(entryMutex);

        if (bidAmount <= currentPrice || !userBidding || !log || userBidding == seller)
            return;
        Bid *newBid = new Bid(bidAmount, this, userBidding);
        bidHeap.push(newBid);  // O(log n) insert into max-heap
        bids.push_back(newBid); // keep parallel list for iteration
        currentPrice = bidAmount;

        addLog("Bid on " + name + " placed by " + userBidding->getName() + " for: $" + to_string(bidAmount));

        lock_guard<mutex> userLock(userBidding->getMutex());
        bool found = false;
        for (int i = 0; i < userBidding->getInterested()->size(); i++)
        {
            if ((*(userBidding->getInterested()))[i] == this)
            {
                found = true;
            }
        }
        if (found == false)
        {
            userBidding->getInterested()->push_back(this);
        }
    }

    /*
    Buys the listing outright from the seller. The listing is added to the buyer's purchased vector and removed from the seller's selling vector.
    Updates the losers of the auction and sells the listing.
    */
    void Listing::buyOutright(User *buyer)
    {
        {
            lock_guard<mutex> lock(entryMutex);
            if (buyOutrightPrice < currentPrice)
                return;
            currentPrice = buyOutrightPrice;
        }

        addLog(name + " purchased outright by " + buyer->getName() +
                       " for: $" + to_string(buyOutrightPrice));
        listings->sellListing(this);
        {
            lock_guard<mutex> userLock(buyer->getMutex());
            bool found = false;
            for (int i = 0; i < buyer->getInterested()->size(); i++)
            {
                if ((*(buyer->getInterested()))[i] == this)
                {
                    found = true;
                    buyer->getPurchased()->push_back(this);
                    buyer->getInterested()->erase(buyer->getInterested()->begin() + i);
                }
            }
            if (found == false)
            {
                buyer->getPurchased()->push_back(this);
            }
        }
        this->losers(buyer);
        this->sell();
    }
    /**
    Checks if the auction should be closed based on the time remaining and updates the status of the listing accordingly.
    */
    void Listing::checkCloseAuction()
    {
        // Determine whether the auction has expired and who the winner is,
        // then release entryMutex before calling sellListing() to avoid a
        // deadlock: saveToFile() holds listMutex and calls getBids() which
        // tries to acquire entryMutex, while this path held entryMutex and
        // called sellListing() which tries to acquire listMutex.
        bool shouldClose = false;
        Bid *winner = nullptr;
        {
            lock_guard<mutex> lock(entryMutex);
            if (this->checkTime() >= sellTime)
            {
                shouldClose = true;
                if (!bidHeap.empty())
                    winner = bidHeap.top();
            }
        }

        if (!shouldClose)
            return;

        listings->sellListing(this);

        if (winner == nullptr)
            return;

        addLog(winner->getBidder()->getName() +
                       " has won the auction for " + name + " at the price of: $" +
                       to_string(currentPrice));

        {
            lock_guard<mutex> userLock(winner->getBidder()->getMutex());
            for (int i = 0; i < winner->getBidder()->getInterested()->size(); i++)
            {
                if ((*(winner->getBidder()->getInterested()))[i] == this)
                {
                    winner->getBidder()->getInterested()->erase(winner->getBidder()->getInterested()->begin() + i);
                    break;
                }
            }
            winner->getBidder()->getPurchased()->push_back(this);
        }
        this->losers(winner->getBidder());
        this->sell();
    }
};
