// profile.js — Loads the current user's profile data and displays it

document.addEventListener("DOMContentLoaded", async function () {
  var response = await fetch("http://localhost:8080/user?name=" + getActiveUser());

  if (!response.ok) {
    document.getElementById("profile-name").textContent = "not found";
    return;
  }

  var user = await response.json();

  document.getElementById("profile-name").textContent = user.name;

  fillList("list-selling",    user.selling,    "Nothing listed yet.");
  fillList("list-interested", user.interested, "No active bids.");
  fillList("list-purchased",  user.purchased,  "Nothing purchased yet.");
  fillList("list-lost",       user.lost,       "No lost auctions.");
});

// Fills a <ul> with item names, or shows a fallback message if the list is empty
function fillList(elementId, items, emptyMessage) {
  var ul = document.getElementById(elementId);
  ul.innerHTML = "";

  if (!items || items.length === 0) {
    ul.innerHTML = "<li class='empty'>" + emptyMessage + "</li>";
    return;
  }

  for (var i = 0; i < items.length; i++) {
    var li = document.createElement("li");
    var link = document.createElement("a");
    link.textContent = items[i];
    link.href = "item.html?name=" + encodeURIComponent(items[i]);
    li.appendChild(link);
    ul.appendChild(li);
  }
}
