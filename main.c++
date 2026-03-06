#include "Listing.h"
#include "Listings.h"
#include "User.h"
#include "Bid.h"
#include <iostream>
#include <vector>

using namespace std;

namespace CSEN79{
    int main(){
        vector<string>* globalLog = new vector<string>;
        User* users[10];
        int currentUserNumber;
        Listings* allListings = new Listings();
        Listing* temp =  new Listing();
        temp->setLog(globalLog);
        allListings->setLog(globalLog);
        temp->setListings(allListings);
        delete temp;
        for(int i = 0; i < 10; i++){
            users[i] = new User("user"+i);
        }
    }
}