#include "Listing.h"
#include "myBag.h"
#include "User.h"
#include "Bid.h"
#include <iostream>
#include <vector>

using namespace std;
namespace CSEN79{
    int main(){
        vector<string> globalLog;
        User* users[10];
        for(int i = 0; i < 10; i++){
            users[i] = new User("user"+i);
        }
        int currentUserNumber;
        cout << "What User Number are you 0-9?" << endl;
        cin >> currentUserNumber;
        Listing* garbage =  new Listing();
        garbage->setUser(users[currentUserNumber]);
        garbage->setLog(globalLog);
        delete garbage;
    }
}