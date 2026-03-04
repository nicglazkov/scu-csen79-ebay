// item.js — Loads a single item's details and handles bidding actions

// Wait for the page to fully load before running our code
document.addEventListener("DOMContentLoaded", async function () {
  // Step 1: Get the item ID from the URL (e.g., ?id=3 means item 3)
  var params = new URLSearchParams(window.location.search);
  var itemId = params.get("id");

  // Step 2: Fetch all listings and find the one that matches our ID
  var response = await fetch("../data/listings.json");
  var listings = await response.json();
  var item = findItemById(listings, itemId);

  // Step 3: If the item doesn't exist, show an error and stop
  if (!item) {
    document.getElementById("item-detail").innerHTML = "<p>Item not found.</p>";
    return;
  }

  // Step 4: Fill the page with the item's details
  fillItemDetails(item);

  // Step 5: Set up the bidding buttons
  var minNextBid = item.currentBid + item.bidIncrement;
  setupBidButtons(item, minNextBid);
});


// --- Helper Functions ---


// Searches the listings array for an item with the matching ID
function findItemById(listings, id) {
  for (var i = 0; i < listings.length; i++) {
    if (listings[i].id === Number(id)) {
      return listings[i];
    }
  }
  return null;
}


// Puts all the item info into the HTML elements on the page
function fillItemDetails(item) {
  var minNextBid = item.currentBid + item.bidIncrement;
  var bidWord = item.bids !== 1 ? "bids" : "bid";

  // Update the page title in the browser tab
  document.title = "eBay Lite - " + item.title;

  // Fill in the item details
  document.getElementById("item-title").textContent = item.title;
  document.getElementById("item-condition").textContent = item.condition;
  document.getElementById("current-bid").textContent = "$" + item.currentBid.toFixed(2);
  document.getElementById("bid-count").textContent = item.bids + " " + bidWord;
  document.getElementById("time-left").textContent = "Time left: " + item.timeLeft;
  document.getElementById("buyout-price").textContent = "$" + item.buyout.toFixed(2);
  document.getElementById("seller-name").textContent = item.seller;
  document.getElementById("item-description").textContent = item.description;

  // Set up the bid input with the minimum allowed bid
  document.getElementById("bid-increment-btn").textContent = "Bid $" + minNextBid.toFixed(2);
  document.getElementById("custom-bid-input").min = minNextBid.toFixed(2);
  document.getElementById("custom-bid-input").placeholder = "$" + minNextBid.toFixed(2) + " or more";
}


// Attaches click handlers to the three bid/buy buttons
function setupBidButtons(item, minNextBid) {
  // "Bid +minimum" button — places a bid at the lowest allowed amount
  var incrementBtn = document.getElementById("bid-increment-btn");
  incrementBtn.addEventListener("click", function () {
    alert("Bid placed: $" + minNextBid.toFixed(2));
  });

  // "Place Bid" button — validates and places a custom bid amount
  var customBidBtn = document.getElementById("custom-bid-btn");
  customBidBtn.addEventListener("click", function () {
    var input = document.getElementById("custom-bid-input");
    var bidAmount = parseFloat(input.value);

    // Check that the bid is a valid number and meets the minimum
    if (isNaN(bidAmount) || bidAmount < minNextBid) {
      alert("Bid must be at least $" + minNextBid.toFixed(2));
      return;
    }

    alert("Bid placed: $" + bidAmount.toFixed(2));
  });

  // "Buy It Now" button — instantly purchases the item
  var buyoutBtn = document.getElementById("buyout-btn");
  buyoutBtn.addEventListener("click", function () {
    alert('Purchased "' + item.title + '" for $' + item.buyout.toFixed(2));
  });
}
