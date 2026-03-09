eBay Lite - CSEN79 Project

How to run

1. run `make`

2. Run `./auction_app` (Mac) `auction_app.exe` (Windows)

3. Open http://localhost:8080

press Ctrl+C to stop server

File layout

- Listing.h / Listing.c++ - a single auction listing (price, bids, timer, buy outright)
- Listings.h / Listings.c++ - collection of all active and sold listings, sort functions
- User.h / User.cpp - a user (selling, buying, bid history)
- Bid.h / Bid.cpp - a single bid (amount, listing, bidder)
- main.c++ - starts the HTTP server and seeds a few test listings
- stress_test.cpp - standalone load test (100k items), run separately: `g++ -std=c++17 stress_test.cpp User.cpp Listing.c++ Listings.c++ Bid.cpp -pthread -o stress_test && ./stress_test`
- HashTable.h - open addressing hash table used for user storage

- index.html - dashboard, shows all active listings, add/remove listings
- pages/item.html - detail page for a single item, place bids or buy outright here
- pages/profile.html - shows what the current user is selling, bidding on, won, lost
- pages/stress.html - runs the stress test and shows results

- js/dashboard.js - loads and auto-refreshes the listings grid, handles add listing form
- js/item.js - handles bidding, buyout, and remove listing on the item page
- js/main.js - shared across pages (active user selection)
- js/profile.js - loads user profile data
- js/stress.js - runs the stress test and displays stats
- css/ - styles for each page

- third_party/httplib/ - cpp-httplib, a single-header HTTP server library (not our code, need this for the frontend to work)
- data/ - listings.json is written here at runtime

Data structures

- Hash table, stores the 10 users. Used because user lookups happen on every bid and buyout, so O(1) average is better than searching a list.

- map<time_t, vector<Listing\*>> stores active listings, keyed by expiry time. The map keeps them sorted by when they expire, so checking for ended auctions is O(log n + k) instead of scanning everything.

- vector<Bid\*> stores bids on a listing. Bids are always placed in increasing order (enforced by the bid validation), so the highest bid is always the last element. O(1) to read the winner, O(1) to add a bid.

- Heap sort used when the frontend requests sorted listings (by price, name, time, etc). Implemented by hand because the assignment requires it. O(n log n).

notes:

- 5 test listings are created automatically when the server starts
- Auctions run for 300 seconds (5 minutes) by default
- The dashboard auto-refreshes every 5 seconds
- 10 users are available (user0–user9), switchable via the nav dropdown
- listings.json is saved every 7 seconds and on every bid/buyout/add/remove
