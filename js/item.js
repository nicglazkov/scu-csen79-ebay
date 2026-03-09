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
    var soldRes = await fetch("http://localhost:8080/sold-listings");
    var soldList = await soldRes.json();
    var soldItem = null;
    for (var s = 0; s < soldList.length; s++) {
      if (soldList[s].name === itemName) { soldItem = soldList[s]; break; }
    }
    if (!soldItem) {
      document.getElementById("item-detail").innerHTML = "<p>Item not found.</p>";
      return;
    }
    showSoldDetail(soldItem);
    return;
  }

  fillItemDetails(item);
  if (getActiveUser() === item.seller) {
    document.getElementById("your-listing-banner").style.display = "block";
    document.getElementById("remove-listing-btn").addEventListener("click", async function () {
      if (!confirm("Remove this listing?")) return;
      await fetch("http://localhost:8080/remove-listing", {
        method: "POST",
        body: new URLSearchParams({ name: item.name, user: getActiveUser() }),
      });
      window.location.href = "../index.html";
    });
  }
  setupBidButtons(item);
  startCountdown(item.timeLeft);
});

function showSoldDetail(sold) {
  document.title = "eBay Lite - " + sold.name;
  var activeUser = getActiveUser();
  var isWinner = sold.winner && sold.winner === activeUser;
  var isSeller = sold.seller === activeUser;
  var bidWord = sold.bidCount !== 1 ? "bids" : "bid";

  var outcome = "";
  if (isWinner)       outcome = '<p class="sold-outcome sold-outcome--won">You won this auction!</p>';
  else if (isSeller)  outcome = '<p class="sold-outcome sold-outcome--sold">You sold this item.</p>';
  else if (sold.winner === "") outcome = '<p class="sold-outcome">No bids — item expired unsold.</p>';
  else                outcome = '<p class="sold-outcome sold-outcome--lost">You did not win this auction.</p>';

  document.getElementById("item-detail").innerHTML =
    '<div id="item-image-col"><div id="item-image"></div></div>' +
    '<div id="item-info-col">' +
    '  <p class="sold-badge-large">SOLD</p>' +
    '  <h1>' + sold.name + '</h1>' +
    outcome +
    '  <div class="sold-stats">' +
    '    <div class="sold-stat"><span class="sold-stat-label">Final price</span><span class="sold-stat-value">$' + parseFloat(sold.finalPrice).toFixed(2) + '</span></div>' +
    '    <div class="sold-stat"><span class="sold-stat-label">Total bids</span><span class="sold-stat-value">' + sold.bidCount + ' ' + bidWord + '</span></div>' +
    '    <div class="sold-stat"><span class="sold-stat-label">Winner</span><span class="sold-stat-value">' + (sold.winner || '—') + '</span></div>' +
    '    <div class="sold-stat"><span class="sold-stat-label">Seller</span><span class="sold-stat-value">' + sold.seller + '</span></div>' +
    '  </div>' +
    (sold.description ? '<p class="sold-description">' + sold.description + '</p>' : '') +
    '</div>';
}

// Helper Functions

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

function startCountdown(seconds) {
  var timeLeft = Math.floor(seconds);
  var el = document.getElementById("time-left");

  var interval = setInterval(function () {
    timeLeft--;
    if (timeLeft <= 0) {
      clearInterval(interval);
      el.textContent = "Auction ended";
    } else {
      el.textContent = "Time left: " + timeLeft + "s";
    }
  }, 1000);
}

function setupBidButtons(item) {
  // state.minNextBid is updated in-place after each bid so spam-clicking
  // always sends the correct next amount without a page reload
  var state = { minNextBid: item.currentPrice + 0.01 };

  async function refreshBidUI() {
    var res = await fetch("http://localhost:8080/listings");
    var listings = await res.json();
    var updated = findItemByName(listings, item.name);
    if (!updated) return;

    var bidCount = updated.bids ? updated.bids.length : 0;
    var bidWord = bidCount !== 1 ? "bids" : "bid";
    state.minNextBid = updated.currentPrice + 0.01;

    document.getElementById("current-bid").textContent =
      "$" + updated.currentPrice.toFixed(2);
    document.getElementById("bid-count").textContent = bidCount + " " + bidWord;
    document.getElementById("bid-increment-btn").textContent =
      "Bid $" + state.minNextBid.toFixed(2);
    document.getElementById("custom-bid-input").min =
      state.minNextBid.toFixed(2);
    document.getElementById("custom-bid-input").placeholder =
      "$" + state.minNextBid.toFixed(2) + " or more";
  }

  // "Bid +minimum" button
  document
    .getElementById("bid-increment-btn")
    .addEventListener("click", async function () {
      if (getActiveUser() === item.seller) {
        alert("You can't bid on your own listing.");
        return;
      }
      await fetch("http://localhost:8080/bid", {
        method: "POST",
        body: new URLSearchParams({
          name: item.name,
          user: getActiveUser(),
          amount: state.minNextBid,
        }),
      });
      var rect = document.getElementById("bid-increment-btn").getBoundingClientRect();
      confetti({ particleCount: 120, spread: 70, origin: { x: (rect.left + rect.width / 2) / window.innerWidth, y: (rect.top + rect.height / 2) / window.innerHeight }, colors: ["#3665f3", "#ffd700", "#ffffff"] });
      await refreshBidUI();
    });

  // "Place Bid" button
  document
    .getElementById("custom-bid-btn")
    .addEventListener("click", async function () {
      if (getActiveUser() === item.seller) {
        alert("You can't bid on your own listing.");
        return;
      }
      var input = document.getElementById("custom-bid-input");
      var bidAmount = parseFloat(input.value);

      if (isNaN(bidAmount) || bidAmount < state.minNextBid) {
        alert("Bid must be at least $" + state.minNextBid.toFixed(2));
        return;
      }

      await fetch("http://localhost:8080/bid", {
        method: "POST",
        body: new URLSearchParams({
          name: item.name,
          user: getActiveUser(),
          amount: bidAmount,
        }),
      });
      var rect = document.getElementById("custom-bid-btn").getBoundingClientRect();
      confetti({ particleCount: 120, spread: 70, origin: { x: (rect.left + rect.width / 2) / window.innerWidth, y: (rect.top + rect.height / 2) / window.innerHeight }, colors: ["#3665f3", "#ffd700", "#ffffff"] });
      await refreshBidUI();
    });

  // "Buy It Now" — redirect to dashboard after purchase
  document
    .getElementById("buyout-btn")
    .addEventListener("click", async function () {
      if (getActiveUser() === item.seller) {
        alert("You can't buy your own listing.");
        return;
      }
      await fetch("http://localhost:8080/buyout", {
        method: "POST",
        body: new URLSearchParams({ name: item.name, user: getActiveUser() }),
      });
      window.location.href = "../index.html";
    });
}
