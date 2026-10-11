(function () {
  "use strict";

  var ROOT = "smart_home";
  var state = {
    mode: "auto",
    devices: { light: false, fan: false, relay: false, buzzer: false },
    settings: {
      temperature_threshold: 30,
      light_threshold: 1500,
      gas_threshold: 1800,
      motion_hold_ms: 10000,
    },
    history: [],
    signedIn: false,
  };

  var els = {
    banner: document.getElementById("setup-banner"),
    alert: document.getElementById("alert-banner"),
    linkPill: document.getElementById("link-pill"),
    lastUpdate: document.getElementById("last-update"),
    temperature: document.getElementById("val-temperature"),
    humidity: document.getElementById("val-humidity"),
    light: document.getElementById("val-light"),
    motion: document.getElementById("val-motion"),
    gasStatus: document.getElementById("val-gas-status"),
    gas: document.getElementById("val-gas"),
    gasCard: document.getElementById("card-gas"),
    motionCard: document.getElementById("card-motion"),
    deviceHint: document.getElementById("device-hint"),
    modeCopy: document.getElementById("mode-copy"),
    btnAuto: document.getElementById("btn-auto"),
    btnManual: document.getElementById("btn-manual"),
    form: document.getElementById("settings-form"),
    thTemperature: document.getElementById("th-temperature"),
    thLight: document.getElementById("th-light"),
    thGas: document.getElementById("th-gas"),
    thMotion: document.getElementById("th-motion"),
    historyBody: document.getElementById("history-body"),
    csv: document.getElementById("btn-csv"),
  };

  function showBanner(message, isError) {
    els.banner.textContent = message;
    els.banner.classList.remove("hidden");
    if (isError) {
      els.banner.classList.add("banner-alert");
    }
  }

  function setPill(text, ok) {
    els.linkPill.textContent = text;
    els.linkPill.classList.remove("ok", "bad");
    els.linkPill.classList.add(ok ? "ok" : "bad");
  }

  function path(child) {
    return ROOT + "/" + child;
  }

  function isManual() {
    return state.mode === "manual";
  }

  function formatTime(value) {
    if (value === null || value === undefined || value === "") {
      return "—";
    }
    var date = new Date(typeof value === "number" ? value : Number(value));
    if (isNaN(date.getTime())) {
      return String(value);
    }
    return date.toLocaleString();
  }

  function renderDevices() {
    var buttons = document.querySelectorAll(".toggle");
    buttons.forEach(function (button) {
      var name = button.getAttribute("data-device");
      var on = Boolean(state.devices[name]);
      button.textContent = on ? "ON" : "OFF";
      button.setAttribute("aria-pressed", on ? "true" : "false");
      button.disabled = !isManual() || !state.signedIn;
    });

    els.deviceHint.textContent = isManual()
      ? "Manual mode: the dashboard commands the ESP32. Automatic rules do not override you."
      : "Auto mode: buttons are read-only. The ESP32 applies temperature, light/motion, and gas rules.";
  }

  function renderMode() {
    els.btnAuto.classList.toggle("active", !isManual());
    els.btnManual.classList.toggle("active", isManual());
    els.modeCopy.textContent = isManual()
      ? "Manual: Light, Fan, Relay, and Buzzer follow the buttons below."
      : "Auto: Fan follows temperature. Light follows darkness and recent motion. Buzzer follows gas. Relay stays unchanged.";
    renderDevices();
  }

  function renderSettings() {
    els.thTemperature.value = state.settings.temperature_threshold;
    els.thLight.value = state.settings.light_threshold;
    els.thGas.value = state.settings.gas_threshold;
    els.thMotion.value = state.settings.motion_hold_ms;
  }

  function renderHistory() {
    if (!state.history.length) {
      els.historyBody.innerHTML = '<tr><td colspan="6">No snapshots yet.</td></tr>';
      return;
    }

    var rows = state.history
      .slice()
      .reverse()
      .map(function (item) {
        return (
          "<tr>" +
          "<td>" +
          formatTime(item.timestamp) +
          "</td>" +
          "<td>" +
          (item.temperature != null ? Number(item.temperature).toFixed(1) : "—") +
          "</td>" +
          "<td>" +
          (item.humidity != null ? Number(item.humidity).toFixed(0) : "—") +
          "</td>" +
          "<td>" +
          (item.light != null ? item.light : "—") +
          "</td>" +
          "<td>" +
          (item.motion ? "Detected" : "Clear") +
          "</td>" +
          "<td>" +
          (item.gas != null ? item.gas : "—") +
          "</td>" +
          "</tr>"
        );
      })
      .join("");
    els.historyBody.innerHTML = rows;
  }

  function applySensors(data) {
    if (!data) {
      return;
    }

    els.temperature.textContent =
      data.temperature == null ? "—" : Number(data.temperature).toFixed(1);
    els.humidity.textContent =
      data.humidity == null ? "—" : Number(data.humidity).toFixed(0);
    els.light.textContent = data.light == null ? "—" : data.light;

    var motion = Boolean(data.motion);
    els.motion.textContent = motion ? "Detected" : "Clear";
    els.motionCard.classList.toggle("motion", motion);

    var gas = Number(data.gas);
    var alert = !isNaN(gas) && gas > Number(state.settings.gas_threshold);
    els.gas.textContent = isNaN(gas) ? "ADC —" : "ADC " + gas;
    els.gasStatus.textContent = alert ? "Alert" : "Normal";
    els.gasCard.classList.toggle("alert", alert);
    els.alert.classList.toggle("hidden", !alert);
  }

  function applySystem(data) {
    if (!data) {
      setPill("ESP32 status unknown", false);
      return;
    }

    var updated = data.last_update;
    els.lastUpdate.textContent = "Last update: " + formatTime(updated);

    var fresh = false;
    if (typeof updated === "number") {
      fresh = Date.now() - updated < 20000;
    }
    var online = Boolean(data.online) && fresh;
    setPill(online ? "ESP32 online" : "ESP32 offline or stale", online);
  }

  function bindToggles(database) {
    document.querySelectorAll(".toggle").forEach(function (button) {
      button.addEventListener("click", function () {
        if (!isManual()) {
          return;
        }
        var name = button.getAttribute("data-device");
        var next = !state.devices[name];
        database.ref(path("devices/" + name)).set(next);
      });
    });
  }

  function bindMode(database) {
    els.btnAuto.addEventListener("click", function () {
      database.ref(path("settings/mode")).set("auto");
    });
    els.btnManual.addEventListener("click", function () {
      database.ref(path("settings/mode")).set("manual");
    });
  }

  function bindSettings(database) {
    els.form.addEventListener("submit", function (event) {
      event.preventDefault();
      database.ref(path("settings")).update({
        temperature_threshold: Number(els.thTemperature.value),
        light_threshold: Number(els.thLight.value),
        gas_threshold: Number(els.thGas.value),
        motion_hold_ms: Number(els.thMotion.value),
      });
    });
  }

  function bindCsv() {
    els.csv.addEventListener("click", function () {
      var lines = ["time,temperature,humidity,light,motion,gas"];
      state.history.forEach(function (item) {
        lines.push(
          [
            formatTime(item.timestamp),
            item.temperature,
            item.humidity,
            item.light,
            item.motion,
            item.gas,
          ].join(",")
        );
      });
      var blob = new Blob([lines.join("\n")], { type: "text/csv" });
      var url = URL.createObjectURL(blob);
      var link = document.createElement("a");
      link.href = url;
      link.download = "smart-home-history.csv";
      link.click();
      URL.revokeObjectURL(url);
    });
  }

  function listen(database) {
    database.ref(path("sensors")).on("value", function (snap) {
      applySensors(snap.val());
    });

    database.ref(path("devices")).on("value", function (snap) {
      state.devices = Object.assign(state.devices, snap.val() || {});
      renderDevices();
    });

    database.ref(path("settings")).on("value", function (snap) {
      var value = snap.val() || {};
      state.mode = value.mode || "auto";
      state.settings = Object.assign(state.settings, value);
      renderMode();
      renderSettings();
    });

    database.ref(path("system")).on("value", function (snap) {
      applySystem(snap.val());
    });

    database
      .ref(path("history"))
      .limitToLast(30)
      .on("value", function (snap) {
        var value = snap.val() || {};
        state.history = Object.keys(value).map(function (key) {
          return value[key];
        });
        renderHistory();
      });
  }

  function start() {
    if (!window.FIREBASE_CONFIG || !window.FIREBASE_AUTH) {
      showBanner(
        "Missing dashboard/firebase-config.js. Copy firebase-config.example.js, add your Firebase values, and reload.",
        true
      );
      setPill("Dashboard not configured", false);
      return;
    }

    if (window.FIREBASE_ROOT) {
      ROOT = window.FIREBASE_ROOT;
    }

    if (
      String(window.FIREBASE_CONFIG.apiKey).indexOf("YOUR_") === 0 ||
      String(window.FIREBASE_AUTH.email).indexOf("YOUR_") === 0
    ) {
      showBanner(
        "firebase-config.js still contains placeholder values. Replace YOUR_… fields, then reload.",
        true
      );
      setPill("Placeholder config", false);
      return;
    }

    firebase.initializeApp(window.FIREBASE_CONFIG);
    var auth = firebase.auth();
    var database = firebase.database();

    bindToggles(database);
    bindMode(database);
    bindSettings(database);
    bindCsv();
    renderMode();
    renderSettings();

    auth
      .signInWithEmailAndPassword(window.FIREBASE_AUTH.email, window.FIREBASE_AUTH.password)
      .then(function () {
        state.signedIn = true;
        setPill("Signed in, waiting for ESP32…", true);
        listen(database);
        renderDevices();
      })
      .catch(function (error) {
        showBanner("Firebase sign-in failed: " + error.message, true);
        setPill("Auth failed", false);
      });
  }

  start();
})();
