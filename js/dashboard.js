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

  var activeUser = getActiveUser();

  // Active listings
  for (var i = 0; i < listings.length; i++) {
    var item = listings[i];

    var card = document.createElement("a");
    card.className = "item-card";
    card.href = "pages/item.html?name=" + encodeURIComponent(item.name);

    var bidCount = item.bids ? item.bids.length : 0;
    var bidWord = bidCount !== 1 ? "bids" : "bid";
    var timeLeft = Math.max(0, Math.floor(item.timeLeft));
    var timeStr = timeLeft > 3600
      ? Math.floor(timeLeft / 3600) + "h " + Math.floor((timeLeft % 3600) / 60) + "m"
      : timeLeft > 60
        ? Math.floor(timeLeft / 60) + "m " + (timeLeft % 60) + "s"
        : timeLeft + "s";
    var isOwn = item.seller === activeUser;

    card.innerHTML =
      '<div class="item-image"></div>' +
      '<div class="item-info">' +
      (isOwn ? '<span class="item-own-badge">Your listing</span>' : "") +
      "  <h2>" + item.name + "</h2>" +
      '  <div class="item-price-row">' +
      '    <span class="item-price">$' + item.currentPrice.toFixed(2) + "</span>" +
      '    <span class="item-bids">' + bidCount + " " + bidWord + "</span>" +
      "  </div>" +
      '  <p class="item-buyout">Buy Now: $' + item.buyOutrightPrice.toFixed(2) + "</p>" +
      '  <div class="item-meta">' +
      '    <span class="item-seller">Seller: ' + item.seller + "</span>" +
      '    <span class="item-time">' + timeStr + " left</span>" +
      "  </div>" +
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

function setView(view) {
  var grid = document.getElementById("listings-grid");
  var cardBtn = document.getElementById("view-card-btn");
  var listBtn = document.getElementById("view-list-btn");
  if (view === "list") {
    grid.classList.add("list-view");
    listBtn.classList.add("active");
    cardBtn.classList.remove("active");
  } else {
    grid.classList.remove("list-view");
    cardBtn.classList.add("active");
    listBtn.classList.remove("active");
  }
  localStorage.setItem("listingsView", view);
}

document.addEventListener("DOMContentLoaded", function () {
  var currentSort = null;

  setView(localStorage.getItem("listingsView") || "card");
  document.getElementById("view-card-btn").addEventListener("click", function () { setView("card"); });
  document.getElementById("view-list-btn").addEventListener("click", function () { setView("list"); });

  loadListings(currentSort);

  document
    .getElementById("sort-select")
    .addEventListener("change", function () {
      currentSort = this.value;
      loadListings(currentSort);
    });

  setInterval(function () {
    loadListings(currentSort);
  }, 5000);

  document.getElementById("add-listing-btn").addEventListener("click", function () {
    document.getElementById("add-listing-form").style.display = "flex";
    this.style.display = "none";
  });

  document.getElementById("cancel-listing-btn").addEventListener("click", function () {
    document.getElementById("add-listing-form").style.display = "none";
    document.getElementById("add-listing-btn").style.display = "";
  });

  document.getElementById("add-listing-form").addEventListener("submit", async function (e) {
    e.preventDefault();
    await fetch("http://localhost:8080/add-listing", {
      method: "POST",
      body: new URLSearchParams({
        name:        document.getElementById("new-name").value,
        description: document.getElementById("new-desc").value,
        startPrice:  document.getElementById("new-start").value,
        buyoutPrice: document.getElementById("new-buyout").value,
        sellTime:    document.getElementById("new-time").value,
        user:        getActiveUser(),
      }),
    });
    this.reset();
    this.style.display = "none";
    document.getElementById("add-listing-btn").style.display = "";
    loadListings(currentSort);
  });
});
