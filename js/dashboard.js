// dashboard.js — Loads item listings and displays them as cards on the homepage

async function loadListings(sort) {
  var url = "http://localhost:8080/listings";
  if (sort) url += "?sort=" + sort;

  var response = await fetch(url);
  var listings = await response.json();

  var soldResponse = await fetch("http://localhost:8080/sold-listings");
  var soldListings = await soldResponse.json();

  var grid = document.getElementById("listings-grid");
  grid.innerHTML = "";

  // Active listings
  for (var i = 0; i < listings.length; i++) {
    var item = listings[i];

    var card = document.createElement("a");
    card.className = "item-card";
    card.href = "pages/item.html?name=" + encodeURIComponent(item.name);

    var bidCount = item.bids ? item.bids.length : 0;
    var bidWord = bidCount !== 1 ? "bids" : "bid";
    card.innerHTML =
      '<div class="item-image"></div>' +
      '<div class="item-info">' +
      "  <h2>" + item.name + "</h2>" +
      '  <p class="item-price">$' + item.currentPrice.toFixed(2) + "</p>" +
      '  <p class="item-bids">' + bidCount + " " + bidWord + "</p>" +
      "</div>";

    grid.appendChild(card);
  }

  // Sold listings — greyed out at the bottom
  for (var j = 0; j < soldListings.length; j++) {
    var sold = soldListings[j];

    var soldCard = document.createElement("div");
    soldCard.className = "item-card item-card--sold";

    soldCard.innerHTML =
      '<div class="item-image"></div>' +
      '<div class="item-info">' +
      "  <h2>" + sold.name + "</h2>" +
      '  <p class="item-price">$' + parseFloat(sold.currentPrice).toFixed(2) + "</p>" +
      '  <p class="item-sold-badge">SOLD</p>' +
      "</div>";

    grid.appendChild(soldCard);
  }
}

document.addEventListener("DOMContentLoaded", function () {
  var currentSort = null;

  loadListings(currentSort);

  document
    .getElementById("sort-select")
    .addEventListener("change", function () {
      currentSort = this.value;
      loadListings(currentSort);
    });

  // Refresh the listings every 5 seconds so new bids and sold items show up automatically
  setInterval(function () {
    loadListings(currentSort);
  }, 5000);
});
