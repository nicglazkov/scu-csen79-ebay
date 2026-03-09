// Listings Class Implemented by Benjamin Castillo III
#include "Listing.h"
#include "Listings.h"
#include "Bid.h"
#include "User.h"

using namespace std;

namespace CSEN79
{
    vector<string> *Listings::log = nullptr;

    Listings::Listings()
    {
        log = nullptr;
    }

    Listings::~Listings()
    {
        lock_guard<mutex> lock(listMutex);

        for (auto &[key, vec] : allListings)
            for (Listing *L : vec)
                delete L;
        allListings.clear();

        for (Listing *L : sold)
            delete L;
        sold.clear();
    }

    /**
    Adds a new listing to the map, keyed by its expiry time.
    */
    void Listings::addListing(Listing *newListing)
    {
        lock_guard<mutex> lock(listMutex);
        allListings[newListing->getExpiryTime()].push_back(newListing);
    }

    /**
    Moves a listing from the map to the sold vector.
    */
    void Listings::sellListing(Listing *soldListing)
    {
        if (!soldListing || !log)
            return;

        lock_guard<mutex> lock(listMutex);
        for (auto &[key, vec] : allListings)
        {
            for (auto it = vec.begin(); it != vec.end(); ++it)
            {
                if (*it == soldListing)
                {
                    Listing::addLog((*it)->getName() + " has been sold by " + (*it)->getSeller()->getName());
                    sold.push_back(soldListing);
                    vec.erase(it);
                    if (vec.empty())
                        allListings.erase(key);
                    return;
                }
            }
        }
    }

    int Listings::getNumListings()
    {
        lock_guard<mutex> lock(listMutex);
        int count = 0;
        for (auto &[key, vec] : allListings)
            count += vec.size();
        return count;
    }

    int Listings::getNumSoldListings()
    {
        lock_guard<mutex> lock(listMutex);
        return sold.size();
    }

    void Listings::setLog(vector<string> *newLog)
    {
        log = newLog;
    }

    Listing *Listings::getListing(string name)
    {
        lock_guard<mutex> lock(listMutex);
        for (auto &[key, vec] : allListings)
            for (Listing *L : vec)
                if (L->getName() == name)
                    return L;
        return nullptr;
    }

    vector<Listing *> Listings::getSoldSnapshot()
    {
        lock_guard<mutex> lock(listMutex);
        return sold;
    }

    Listing *Listings::getSoldListing(string name)
    {
        lock_guard<mutex> lock(listMutex);
        for (Listing *L : sold)
            if (L->getName() == name)
                return L;
        return nullptr;
    }

    // Private helper: writes listings JSON to disk in the given order
    void Listings::writeJson(const vector<Listing *> &listings)
    {
        ofstream outFile("data/listings.json", ios::trunc);
        if (!outFile)
        {
            if (log)
                log->push_back("Error: Could Not Open Backup File");
            return;
        }

        outFile << "[\n";
        for (int i = 0; i < (int)listings.size(); i++)
        {
            Listing *t = listings[i];
            outFile << "  {\n";
            outFile << "    \"name\": \"" << t->getName() << "\",\n";
            outFile << "    \"description\": \"" << t->getDescription() << "\",\n";
            outFile << "    \"currentPrice\": " << fixed << setprecision(2) << t->getPrice() << ",\n";
            outFile << "    \"buyOutrightPrice\": " << fixed << setprecision(2) << t->getBuyOutrightPrice() << ",\n";
            outFile << "    \"seller\": \"" << t->getSeller()->getName() << "\",\n";
            outFile << "    \"timeLeft\": " << fixed << setprecision(2) << (t->getSellTime() - t->checkTime()) << ",\n";
            outFile << "    \"bids\": [\n";

            const vector<Bid *> &bids = t->getBids();
            for (int j = 0; j < (int)bids.size(); j++)
            {
                outFile << "      { \"amount\": " << fixed << setprecision(2) << bids[j]->getAmount();
                outFile << ", \"bidder\": \"" << bids[j]->getBidder()->getName() << "\" }";
                if (j < (int)bids.size() - 1)
                    outFile << ",";
                outFile << "\n";
            }
            outFile << "    ]\n";
            outFile << "  }";
            if (i < (int)listings.size() - 1)
                outFile << ",";
            outFile << "\n";
        }
        outFile << "]\n";
        outFile.close();
    }

    /**
    Saves the active listings to disk in expiry-time order (map's natural order).
    */
    void Listings::saveToFile()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        writeJson(flat);
    }

    // Sort helpers: collect flat vector, sort, write JSON

    void Listings::sortAlpha()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        sort(flat.begin(), flat.end(), [](Listing *a, Listing *b)
             { return a->getName() < b->getName(); });
        writeJson(flat);
    }

    void Listings::sortAlphaRev()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        sort(flat.begin(), flat.end(), [](Listing *a, Listing *b)
             { return a->getName() > b->getName(); });
        writeJson(flat);
    }

    void Listings::sortCurrPrice()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        sort(flat.begin(), flat.end(), [](Listing *a, Listing *b)
             { return a->getPrice() > b->getPrice(); });
        writeJson(flat);
    }

    void Listings::sortCurrPriceRev()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        sort(flat.begin(), flat.end(), [](Listing *a, Listing *b)
             { return a->getPrice() < b->getPrice(); });
        writeJson(flat);
    }

    void Listings::sortBuyOutright()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        sort(flat.begin(), flat.end(), [](Listing *a, Listing *b)
             { return a->getBuyOutrightPrice() > b->getBuyOutrightPrice(); });
        writeJson(flat);
    }

    void Listings::sortBuyOutrightRev()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        sort(flat.begin(), flat.end(), [](Listing *a, Listing *b)
             { return a->getBuyOutrightPrice() < b->getBuyOutrightPrice(); });
        writeJson(flat);
    }

    /**
    sortTimeLeft / sortTimeLeftRev use the map's natural expiry-time ordering
    directly — no re-sort needed, just flatten in map order (or reverse).
    */
    void Listings::sortTimeLeft()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        // map iterates earliest-expiry first → least time left first
        // "sortTimeLeft" means most time left first, so reverse
        reverse(flat.begin(), flat.end());
        writeJson(flat);
    }

    void Listings::sortTimeLeftRev()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec)
                    flat.push_back(L);
        }
        // map iterates earliest-expiry first → least time left first (ascending)
        writeJson(flat);
    }

    /**
    Uses the map's range query to find all expired listings in O(log n + k)
    instead of scanning the full vector.
    */
    void Listings::checkCloseAuction()
    {
        vector<Listing *> snapshot;
        {
            lock_guard<mutex> lock(listMutex);
            // All entries with key <= now are expired
            auto end = allListings.upper_bound(time(nullptr));
            for (auto it = allListings.begin(); it != end; ++it)
                for (Listing *L : it->second)
                    snapshot.push_back(L);
        }
        for (Listing *L : snapshot)
            L->checkCloseAuction();
    }
}
