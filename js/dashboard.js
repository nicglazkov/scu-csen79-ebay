// dashboard.js — Loads item listings and displays them as cards on the homepage

// Wait for the page to fully load before running our code
document.addEventListener("DOMContentLoaded", async function () {
  // Step 1: Fetch the listings data from our JSON file
  var response = await fetch("data/listings.json");
  var listings = await response.json();

  // Step 2: Get the grid container where we'll put the cards
  var grid = document.getElementById("listings-grid");

  // Step 3: Loop through each item and create a card for it
  for (var i = 0; i < listings.length; i++) {
    var item = listings[i];

    // Create a clickable card (an <a> tag so the whole card is a link)
    var card = document.createElement("a");
    card.className = "item-card";
    card.href = "pages/item.html?id=" + item.id;

    // Fill in the card's HTML with the item's info
    var bidWord = item.bids !== 1 ? "bids" : "bid";
    card.innerHTML =
      '<div class="item-image"></div>' +
      '<div class="item-info">' +
      "  <h2>" + item.title + "</h2>" +
      '  <p class="item-price">$' + item.currentBid.toFixed(2) + "</p>" +
      '  <p class="item-bids">' + item.bids + " " + bidWord + "</p>" +
      "</div>";

    // Add the card to the grid
    grid.appendChild(card);
  }
});
