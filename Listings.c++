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
        if (!soldListing)
            return;

        lock_guard<mutex> lock(listMutex);
        for (auto &[key, vec] : allListings)
        {
            for (auto it = vec.begin(); it != vec.end(); ++it)
            {
                if (*it == soldListing)
                {
                    if (log) Listing::addLog((*it)->getName() + " has been sold by " + (*it)->getSeller()->getName());
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

    // --- Heap sort implementation (O(n log n)) ---
    // cmp(a, b) returns true if a should come BEFORE b in the final output.
    // The heap is a max-heap where "max" means "should come last."

    template <typename Cmp>
    static void heapify(vector<Listing *> &arr, int n, int i, Cmp cmp)
    {
        int root = i;
        int left  = 2 * i + 1;
        int right = 2 * i + 2;
        // if child should come after current root, child is the new root
        if (left  < n && cmp(arr[root], arr[left]))  root = left;
        if (right < n && cmp(arr[root], arr[right])) root = right;
        if (root != i)
        {
            swap(arr[i], arr[root]);
            heapify(arr, n, root, cmp);
        }
    }

    template <typename Cmp>
    static void heapSort(vector<Listing *> &arr, Cmp cmp)
    {
        int n = arr.size();
        // build max-heap
        for (int i = n / 2 - 1; i >= 0; i--)
            heapify(arr, n, i, cmp);
        // repeatedly move the max (last in sort order) to the end
        for (int i = n - 1; i > 0; i--)
        {
            swap(arr[0], arr[i]);
            heapify(arr, i, 0, cmp);
        }
    }

    // --- Sort functions ---

    void Listings::sortAlpha()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec) flat.push_back(L);
        }
        heapSort(flat, [](Listing *a, Listing *b) { return a->getName() < b->getName(); });
        writeJson(flat);
    }

    void Listings::sortAlphaRev()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec) flat.push_back(L);
        }
        heapSort(flat, [](Listing *a, Listing *b) { return a->getName() > b->getName(); });
        writeJson(flat);
    }

    void Listings::sortCurrPrice()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec) flat.push_back(L);
        }
        heapSort(flat, [](Listing *a, Listing *b) { return a->getPrice() > b->getPrice(); });
        writeJson(flat);
    }

    void Listings::sortCurrPriceRev()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec) flat.push_back(L);
        }
        heapSort(flat, [](Listing *a, Listing *b) { return a->getPrice() < b->getPrice(); });
        writeJson(flat);
    }

    void Listings::sortBuyOutright()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec) flat.push_back(L);
        }
        heapSort(flat, [](Listing *a, Listing *b) { return a->getBuyOutrightPrice() > b->getBuyOutrightPrice(); });
        writeJson(flat);
    }

    void Listings::sortBuyOutrightRev()
    {
        vector<Listing *> flat;
        {
            lock_guard<mutex> lock(listMutex);
            for (auto &[key, vec] : allListings)
                for (Listing *L : vec) flat.push_back(L);
        }
        heapSort(flat, [](Listing *a, Listing *b) { return a->getBuyOutrightPrice() < b->getBuyOutrightPrice(); });
        writeJson(flat);
    }

    /**
    The map iterates in ascending expiry order (earliest expiry = least time left first).
    sortTimeLeft  → Most to Least: reverse the map order.
    sortTimeLeftRev → Least to Most: use the map order directly, no sort needed.
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
