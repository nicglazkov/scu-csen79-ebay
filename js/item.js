const itemId = new URLSearchParams(window.location.search).get("id");

fetch("../data/listings.json")
  .then((res) => res.json())
  .then((listings) => {
    const item = listings.find((l) => l.id === Number(itemId));

    if (!item) {
      document.getElementById("item-detail").innerHTML =
        "<p>Item not found.</p>";
      return;
    }

    document.title = `eBay Lite - ${item.title}`;
    document.getElementById("item-title").textContent = item.title;
    document.getElementById("item-condition").textContent = item.condition;
    document.getElementById("current-bid").textContent =
      `$${item.currentBid.toFixed(2)}`;
    document.getElementById("bid-count").textContent =
      `${item.bids} bid${item.bids !== 1 ? "s" : ""}`;
    document.getElementById("time-left").textContent = `Time left: ${item.timeLeft}`;
    document.getElementById("buyout-price").textContent =
      `$${item.buyout.toFixed(2)}`;
    document.getElementById("seller-name").textContent = item.seller;
    document.getElementById("item-description").textContent = item.description;

    const minNext = item.currentBid + item.bidIncrement;
    document.getElementById("bid-increment-btn").textContent =
      `Bid $${minNext.toFixed(2)}`;
    document.getElementById("custom-bid-input").min = minNext.toFixed(2);
    document.getElementById("custom-bid-input").placeholder =
      `$${minNext.toFixed(2)} or more`;

    // Button handlers (just alerts for now — backend will replace these)
    document.getElementById("bid-increment-btn").addEventListener("click", () => {
      alert(`Bid placed: $${minNext.toFixed(2)}`);
    });

    document.getElementById("custom-bid-btn").addEventListener("click", () => {
      const val = parseFloat(document.getElementById("custom-bid-input").value);
      if (isNaN(val) || val < minNext) {
        alert(`Bid must be at least $${minNext.toFixed(2)}`);
        return;
      }
      alert(`Bid placed: $${val.toFixed(2)}`);
    });

    document.getElementById("buyout-btn").addEventListener("click", () => {
      alert(`Purchased "${item.title}" for $${item.buyout.toFixed(2)}`);
    });
  });
