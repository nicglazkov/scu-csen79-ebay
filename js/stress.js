document.addEventListener("DOMContentLoaded", function () {
  var btn = document.getElementById("run-btn");

  btn.addEventListener("click", async function () {
    btn.disabled = true;
    btn.textContent = "Running...";
    document.getElementById("stats-section").style.display = "none";

    var res = await fetch("http://localhost:8080/stress-test", { method: "POST" });
    var data = await res.json();

    document.getElementById("stat-ops").textContent = data.totalOps.toLocaleString();
    document.getElementById("stat-time").textContent = data.elapsedMs + " ms";
    document.getElementById("stat-throughput").textContent = data.throughput.toLocaleString() + "/s";
    document.getElementById("stat-avg").textContent = data.avgMsPerOp + " ms";
    document.getElementById("stats-section").style.display = "flex";

    btn.textContent = "Run Again";
    btn.disabled = false;
  });
});
