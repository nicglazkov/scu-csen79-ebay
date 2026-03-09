async function loadLogs() {
  var res = await fetch("http://localhost:8080/logs");
  var data = await res.json();

  var list = document.getElementById("log-list");
  list.innerHTML = "";

  if (data.length === 0) {
    list.innerHTML = "<p id='no-logs'>No transactions yet.</p>";
    return;
  }

  // Show newest first
  for (var i = data.length - 1; i >= 0; i--) {
    var row = document.createElement("div");
    row.className = "log-row";
    row.textContent = data[i];
    list.appendChild(row);
  }
}

document.addEventListener("DOMContentLoaded", function () {
  loadLogs();
  document.getElementById("refresh-btn").addEventListener("click", loadLogs);
});
