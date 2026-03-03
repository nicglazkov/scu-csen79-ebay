fetch("data/listings.json")
  .then((res) => res.json())
  .then((listings) => {
    const grid = document.getElementById("listings-grid");

    listings.forEach((item) => {
      const card = document.createElement("a");
      card.className = "item-card";
      card.href = `pages/item.html?id=${item.id}`;

      card.innerHTML = `
        <div class="item-image"></div>
        <div class="item-info">
          <h2>${item.title}</h2>
          <p class="item-price">$${item.price.toFixed(2)}</p>
          <p class="item-bids">${item.bids} bid${item.bids !== 1 ? "s" : ""}</p>
        </div>
      `;

      grid.appendChild(card);
    });
  });
