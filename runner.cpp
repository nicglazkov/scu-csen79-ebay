// Glue entrypoint to call the existing CSEN79::main in main.c++
// Does not modify any existing source files.

#include "User.h"
#include "Bid.h"
#include "Listing.h"
#include "Listings.h"

namespace CSEN79 {
    int main();
}

int main() {
    return CSEN79::main();
}

