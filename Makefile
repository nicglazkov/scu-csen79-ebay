all:
	g++ -std=c++17 main.c++ User.cpp Listing.c++ Listings.c++ Bid.cpp -pthread -o auction_app
	mkdir -p data

clean:
	rm -f auction_app auction_app.exe
