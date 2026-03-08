eBay Lite - CSEN79 Project

How to run

1. run `make`

2. Run `./auction_app` (Mac) `auction_app.exe` (Windows)

3. Open http://localhost:8080

press Ctrl+C to stop server

File layout

Listing.h / Listing.c++ - a single auction listing (price, bids, timer, buy outright)
Listings.h / Listings.c++ - collection of all active and sold listings, sort functions
User.h / User.cpp - a user (selling, buying, bid history)
Bid.h / Bid.cpp - a single bid (amount, listing, bidder)
main.c++ - starts the HTTP server and seeds a few test listings
stress_test.cpp - basic load test, run separately

index.html - dashboard, shows all active listings
pages/item.html - detail page for a single item, place bids here
pages/profile.html - shows what the current user is selling, bidding on, won, lost

js/dashboard.js - loads and auto-refreshes the listings grid
js/item.js - handles bidding and buyout on the item page
js/profile.js - loads user profile data
css/ - styles for each page

third_party/httplib/ - cpp-httplib, a single-header HTTP server library (not our code)
data/ - listings.json is written here at runtime

notes:

- 5 test listings are created automatically when the server starts
- Auctions run for 300 seconds (5 minutes) by default
