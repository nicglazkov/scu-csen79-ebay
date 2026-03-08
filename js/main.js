// main.js — loaded on every page. Manages the active user via localStorage.

function getActiveUser() {
  return localStorage.getItem("activeUser") || "user0";
}

document.addEventListener("DOMContentLoaded", function () {
  var select = document.getElementById("user-select");
  if (!select) return;

  select.value = getActiveUser();

  select.addEventListener("change", function () {
    localStorage.setItem("activeUser", this.value);
    location.reload();
  });
});
