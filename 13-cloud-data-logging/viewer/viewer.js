(function () {
  "use strict";

  var PATH = "data_logger/readings";
  var readings = [];
  var chart = null;
  var activeQuery = null;

  var els = {
    message: document.getElementById("message"),
    login: document.getElementById("login"),
    email: document.getElementById("email"),
    password: document.getElementById("password"),
    app: document.getElementById("app"),
    range: document.getElementById("range"),
    csv: document.getElementById("csv"),
    logout: document.getElementById("logout"),
    summary: document.getElementById("summary"),
    rows: document.getElementById("rows"),
    chart: document.getElementById("chart"),
  };

  function showMessage(text) {
    els.message.textContent = text;
    els.message.classList.toggle("hidden", !text);
  }

  function timeOf(reading) {
    return new Date(reading.ts * 1000);
  }

  function renderChart() {
    var labels = readings.map(function (r) { return timeOf(r).toLocaleTimeString(); });
    var datasets = [
      { label: "Temperature (°C)", data: readings.map(function (r) { return r.temperature; }),
        borderColor: "#d9480f", yAxisID: "y", pointRadius: 0, tension: 0.2 },
      { label: "Humidity (%)", data: readings.map(function (r) { return r.humidity; }),
        borderColor: "#1971c2", yAxisID: "y", pointRadius: 0, tension: 0.2 },
      { label: "Light (ADC)", data: readings.map(function (r) { return r.light; }),
        borderColor: "#5c940d", yAxisID: "light", pointRadius: 0, tension: 0.2 },
    ];

    if (chart) {
      chart.data.labels = labels;
      chart.data.datasets.forEach(function (dataset, i) { dataset.data = datasets[i].data; });
      chart.update();
      return;
    }

    chart = new Chart(els.chart, {
      type: "line",
      data: { labels: labels, datasets: datasets },
      options: {
        animation: false,
        scales: {
          y: { position: "left", title: { display: true, text: "°C / %" } },
          light: { position: "right", title: { display: true, text: "Light ADC" }, grid: { drawOnChartArea: false } },
        },
      },
    });
  }

  function renderTable() {
    els.rows.innerHTML = "";
    readings.slice(-10).reverse().forEach(function (r) {
      var row = document.createElement("tr");
      [timeOf(r).toLocaleString(), Number(r.temperature).toFixed(1), Number(r.humidity).toFixed(0), r.light]
        .forEach(function (text) {
          var cell = document.createElement("td");
          cell.textContent = text;
          row.appendChild(cell);
        });
      els.rows.appendChild(row);
    });
  }

  function render() {
    els.summary.textContent = readings.length
      ? readings.length + " readings, " + timeOf(readings[0]).toLocaleString() + " to " +
        timeOf(readings[readings.length - 1]).toLocaleString()
      : "No readings yet.";
    renderChart();
    renderTable();
  }

  function listen(database) {
    if (activeQuery) {
      activeQuery.off();
    }
    // Ordered by the ESP32's own timestamp, so late uploads land in the right place.
    activeQuery = database.ref(PATH).orderByChild("ts").limitToLast(Number(els.range.value));
    activeQuery.on("value", function (snap) {
      readings = [];
      snap.forEach(function (child) {
        readings.push(child.val());
      });
      render();
    }, function (error) {
      showMessage("Cannot read the log: " + error.message);
    });
  }

  function downloadCsv() {
    var lines = ["time_utc,temperature_c,humidity_percent,light_adc"];
    readings.forEach(function (r) {
      lines.push([timeOf(r).toISOString(), r.temperature, r.humidity, r.light].join(","));
    });
    var url = URL.createObjectURL(new Blob([lines.join("\n")], { type: "text/csv" }));
    var link = document.createElement("a");
    link.href = url;
    link.download = "data-log.csv";
    link.click();
    URL.revokeObjectURL(url);
  }

  function start() {
    if (!window.FIREBASE_CONFIG || String(window.FIREBASE_CONFIG.apiKey).indexOf("YOUR_") === 0) {
      showMessage("Copy firebase-config.example.js to firebase-config.js, add your Firebase values, and reload.");
      els.login.classList.add("hidden");
      return;
    }

    firebase.initializeApp(window.FIREBASE_CONFIG);
    var auth = firebase.auth();
    var database = firebase.database();

    els.login.addEventListener("submit", function (event) {
      event.preventDefault();
      showMessage("");
      auth.signInWithEmailAndPassword(els.email.value, els.password.value).catch(function (error) {
        showMessage("Sign-in failed: " + error.message);
      });
    });
    els.logout.addEventListener("click", function () {
      auth.signOut().then(function () { window.location.reload(); });
    });
    els.range.addEventListener("change", function () { listen(database); });
    els.csv.addEventListener("click", downloadCsv);

    auth.onAuthStateChanged(function (user) {
      els.login.classList.toggle("hidden", Boolean(user));
      els.app.classList.toggle("hidden", !user);
      if (user) {
        listen(database);
      }
    });
  }

  start();
})();
