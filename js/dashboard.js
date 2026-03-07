// dashboard.js — Loads item listings and displays them as cards on the homepage

async function loadListings(sort) {
  var url = "http://localhost:8080/listings";
  if (sort) url += "?sort=" + sort;

  var response = await fetch(url);
  var listings = await response.json();

  var grid = document.getElementById("listings-grid");
  grid.innerHTML = "";

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
      "  <h2>" +
      item.name +
      "</h2>" +
      '  <p class="item-price">$' +
      item.currentPrice.toFixed(2) +
      "</p>" +
      '  <p class="item-bids">' +
      bidCount +
      " " +
      bidWord +
      "</p>" +
      "</div>";

    grid.appendChild(card);
  }
}

document.addEventListener("DOMContentLoaded", function () {
  loadListings();

  document
    .getElementById("sort-select")
    .addEventListener("change", function () {
      loadListings(this.value);
    });
});
