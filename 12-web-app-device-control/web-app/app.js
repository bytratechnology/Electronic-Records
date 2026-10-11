(function () {
  "use strict";

  var ROOT = "device_control";
  var CHANNELS = ["ch1", "ch2", "ch3", "ch4"];
  var ONLINE_WINDOW_MS = 30000;  // three missed 10 s heartbeats = offline

  var state = {
    desired: {},
    reported: {},
    lastSeen: null,
    serverOffset: 0,
  };

  var els = {
    message: document.getElementById("message"),
    login: document.getElementById("login"),
    email: document.getElementById("email"),
    password: document.getElementById("password"),
    app: document.getElementById("app"),
    channels: document.getElementById("channels"),
    logout: document.getElementById("logout"),
    pill: document.getElementById("device-pill"),
  };

  function showMessage(text) {
    els.message.textContent = text;
    els.message.classList.toggle("hidden", !text);
  }

  function label(channel) {
    return (window.CHANNEL_LABELS && window.CHANNEL_LABELS[channel]) || channel;
  }

  function buildCards(database) {
    els.channels.innerHTML = "";
    CHANNELS.forEach(function (channel) {
      var card = document.createElement("div");
      card.className = "card";
      card.id = "card-" + channel;

      var name = document.createElement("div");
      name.className = "name";
      name.textContent = label(channel);

      var status = document.createElement("div");
      status.className = "state";

      var button = document.createElement("button");
      button.addEventListener("click", function () {
        database.ref(ROOT + "/desired/" + channel).set(!state.desired[channel]);
      });

      card.appendChild(name);
      card.appendChild(status);
      card.appendChild(button);
      els.channels.appendChild(card);
    });
  }

  function render() {
    CHANNELS.forEach(function (channel) {
      var card = document.getElementById("card-" + channel);
      if (!card) {
        return;
      }
      var want = Boolean(state.desired[channel]);
      var actual = Boolean(state.reported[channel]);
      var pending = want !== actual;

      card.classList.toggle("pending", pending);
      card.querySelector(".state").textContent = pending
        ? "pending — relay is " + (actual ? "ON" : "OFF")
        : "relay is " + (actual ? "ON" : "OFF");

      var button = card.querySelector("button");
      button.textContent = want ? "Turn off" : "Turn on";
      button.setAttribute("aria-pressed", want ? "true" : "false");
    });
  }

  function renderOnline() {
    var serverNow = Date.now() + state.serverOffset;
    var online = typeof state.lastSeen === "number" && serverNow - state.lastSeen < ONLINE_WINDOW_MS;
    els.pill.textContent = online ? "ESP32 online" : "ESP32 offline";
    els.pill.className = "pill " + (online ? "ok" : "bad");
  }

  function listen(database) {
    database.ref(ROOT + "/desired").on("value", function (snap) {
      state.desired = snap.val() || {};
      render();
    });
    database.ref(ROOT + "/reported").on("value", function (snap) {
      state.reported = snap.val() || {};
      render();
    });
    database.ref(ROOT + "/status/last_seen").on("value", function (snap) {
      state.lastSeen = snap.val();
      renderOnline();
    });
    // Difference between this computer's clock and Firebase's clock.
    database.ref(".info/serverTimeOffset").on("value", function (snap) {
      state.serverOffset = snap.val() || 0;
      renderOnline();
    });
    setInterval(renderOnline, 5000);
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
    var listening = false;

    els.login.addEventListener("submit", function (event) {
      event.preventDefault();
      showMessage("");
      auth.signInWithEmailAndPassword(els.email.value, els.password.value).catch(function (error) {
        showMessage("Sign-in failed: " + error.message);
      });
    });

    els.logout.addEventListener("click", function () {
      auth.signOut().then(function () {
        window.location.reload();
      });
    });

    auth.onAuthStateChanged(function (user) {
      els.login.classList.toggle("hidden", Boolean(user));
      els.app.classList.toggle("hidden", !user);
      if (user && !listening) {
        listening = true;
        buildCards(database);
        listen(database);
      }
    });
  }

  start();
})();
