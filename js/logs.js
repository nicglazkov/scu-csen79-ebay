var allLogs = [];
var currentPage = 1;
var PAGE_SIZE = 100;

function totalPages() {
  return Math.max(1, Math.ceil(allLogs.length / PAGE_SIZE));
}

function renderPage() {
  var list = document.getElementById("log-list");
  list.innerHTML = "";

  if (allLogs.length === 0) {
    list.innerHTML = "<p id='no-logs'>No transactions yet.</p>";
    updatePagination();
    return;
  }

  var start = (currentPage - 1) * PAGE_SIZE;
  var end = Math.min(start + PAGE_SIZE, allLogs.length);

  for (var i = start; i < end; i++) {
    var row = document.createElement("div");
    row.className = "log-row";
    row.textContent = allLogs[i];
    list.appendChild(row);
  }

  updatePagination();
}

function updatePagination() {
  var tp = totalPages();
  document.getElementById("page-info").textContent =
    "Page " + currentPage + " of " + tp + " (" + allLogs.length + " logs)";
  document.getElementById("prev-btn").disabled = currentPage <= 1;
  document.getElementById("next-btn").disabled = currentPage >= tp;
  document.getElementById("pagination").style.display =
    allLogs.length === 0 ? "none" : "flex";
}

async function loadLogs() {
  var list = document.getElementById("log-list");
  list.innerHTML = "<p id='loading-logs'>Loading...</p>";
  document.getElementById("pagination").style.display = "none";

  var res = await fetch("http://localhost:8080/logs");
  var data = await res.json();

  // Store newest-first
  allLogs = data.slice().reverse();
  currentPage = 1;
  renderPage();
}

async function clearLogs() {
  await fetch("http://localhost:8080/logs", { method: "DELETE" });
  loadLogs();
}

document.addEventListener("DOMContentLoaded", function () {
  loadLogs();
  document.getElementById("refresh-btn").addEventListener("click", loadLogs);
  document.getElementById("clear-btn").addEventListener("click", clearLogs);

  document.getElementById("prev-btn").addEventListener("click", function () {
    if (currentPage > 1) { currentPage--; renderPage(); window.scrollTo(0, 0); }
  });
  document.getElementById("next-btn").addEventListener("click", function () {
    if (currentPage < totalPages()) { currentPage++; renderPage(); window.scrollTo(0, 0); }
  });
});
