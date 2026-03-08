// item.js — Loads a single item's details and handles bidding actions

document.addEventListener("DOMContentLoaded", async function () {
  // Get the item name from the URL (e.g., ?name=Widget)
  var params = new URLSearchParams(window.location.search);
  var itemName = params.get("name");

  // Fetch all listings and find the one that matches
  var response = await fetch("http://localhost:8080/listings");
  var listings = await response.json();
  var item = findItemByName(listings, itemName);

  if (!item) {
    document.getElementById("item-detail").innerHTML = "<p>Item not found.</p>";
    return;
  }

  fillItemDetails(item);
  setupBidButtons(item);
});

// --- Helper Functions ---

function findItemByName(listings, name) {
  for (var i = 0; i < listings.length; i++) {
    if (listings[i].name === name) {
      return listings[i];
    }
  }
  return null;
}

function fillItemDetails(item) {
  var bidCount = item.bids ? item.bids.length : 0;
  var bidWord = bidCount !== 1 ? "bids" : "bid";

  document.title = "eBay Lite - " + item.name;

  document.getElementById("item-title").textContent = item.name;
  document.getElementById("item-condition").textContent = "";
  document.getElementById("current-bid").textContent =
    "$" + item.currentPrice.toFixed(2);
  document.getElementById("bid-count").textContent = bidCount + " " + bidWord;
  document.getElementById("time-left").textContent =
    "Time left: " + item.timeLeft.toFixed(0) + "s";
  document.getElementById("buyout-price").textContent =
    "$" + item.buyOutrightPrice.toFixed(2);
  document.getElementById("seller-name").textContent = item.seller;
  document.getElementById("item-description").textContent = item.description;

  var minNextBid = item.currentPrice + 0.01;
  document.getElementById("bid-increment-btn").textContent =
    "Bid $" + minNextBid.toFixed(2);
  document.getElementById("custom-bid-input").min = minNextBid.toFixed(2);
  document.getElementById("custom-bid-input").placeholder =
    "$" + minNextBid.toFixed(2) + " or more";
}

function setupBidButtons(item) {
  var minNextBid = item.currentPrice + 0.01;

  // "Bid +minimum" button
  document
    .getElementById("bid-increment-btn")
    .addEventListener("click", async function () {
      await fetch("http://localhost:8080/bid", {
        method: "POST",
        body: new URLSearchParams({ name: item.name, user: "user1", amount: minNextBid }),
      });
    });

  // "Place Bid" button
  document
    .getElementById("custom-bid-btn")
    .addEventListener("click", async function () {
      var input = document.getElementById("custom-bid-input");
      var bidAmount = parseFloat(input.value);

      if (isNaN(bidAmount) || bidAmount < minNextBid) {
        alert("Bid must be at least $" + minNextBid.toFixed(2));
        return;
      }

      await fetch("http://localhost:8080/bid", {
        method: "POST",
        body: new URLSearchParams({ name: item.name, user: "user1", amount: bidAmount }),
      });
    });

  // "Buy It Now" button
  document
    .getElementById("buyout-btn")
    .addEventListener("click", async function () {
      await fetch("http://localhost:8080/buyout", {
        method: "POST",
        body: new URLSearchParams({ name: item.name, user: "user1" }),
      });
    });
}
