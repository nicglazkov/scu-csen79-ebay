#include <iostream>
#include "User.h"
#include "Bid.h"
#include "Listing.h"
#include "Listings.h"

namespace CSEN79 {

    

    int main() {
        std::cout << "start" << std::endl;

        std::vector<std::string> log;
        User* users[10];
        for (int i = 0; i < 10; i++) {
            char c = 'a' + i;
            users[i] = new User(std::string(1, c));
        }

        for (auto user : users) {
            user->print();
        }

        std::cout << "end" << std::endl;
        return 0;
    }

}