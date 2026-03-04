#include "Listing.h"
#include "Listings.h"
#include "User.h"
#include "Bid.h"
#include <iostream>
#include <vector>

using namespace std;
namespace CSEN79{
    int main(){
        vector<string> globalLog;
        User* users[10];
        int currentUserNumber;
        Listings* allListings = new Listings();
        Listing* garbage =  new Listing();
        garbage->setLog(globalLog);
        allListings->setLog(globalLog);
        garbage->setListings(allListings);
        for(int i = 0; i < 10; i++){
            users[i] = new User("user"+i);
        }
        cout << "What User Number are you 0-9?" << endl;
        cin >> currentUserNumber;
        garbage->setUser(users[currentUserNumber]);
        allListings->setUser(users[currentUserNumber]);
        delete garbage;
    }
}