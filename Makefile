all:
	g++ -std=c++17 main.c++ User.cpp Listing.c++ Listings.c++ Bid.cpp -pthread -o auction_app

clean:
	rm -f auction_app auction_app.exe
