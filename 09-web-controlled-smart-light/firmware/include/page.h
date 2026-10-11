#pragma once

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart Light</title>
<style>
  body { font-family: system-ui, sans-serif; margin: 0; padding: 16px; background: #f4f6f8; color: #1c2733; }
  main { max-width: 420px; margin: 0 auto; background: #fff; border-radius: 12px; padding: 20px; box-shadow: 0 1px 3px rgba(0,0,0,.12); }
  h1 { font-size: 1.3rem; margin-top: 0; }
  #power { width: 100%; padding: 18px; font-size: 1.2rem; border: 0; border-radius: 10px; background: #dee2e6; cursor: pointer; }
  #power.on { background: #ffd43b; }
  label { display: block; margin: 18px 0 6px; font-size: .9rem; color: #5b6b7b; }
  input[type=range] { width: 100%; }
  .timers { display: flex; gap: 8px; }
  .timers button { flex: 1; padding: 10px; border-radius: 8px; border: 1px solid #adb5bd; background: #fff; cursor: pointer; }
  #status { margin-top: 16px; font-size: .85rem; color: #5b6b7b; }
</style>
</head>
<body>
<main>
  <h1>Smart Light</h1>
  <button id="power">--</button>

  <label for="brightness">Brightness: <span id="percent">--</span>%</label>
  <input id="brightness" type="range" min="5" max="255" value="255">

  <label>Switch off automatically</label>
  <div class="timers">
    <button data-seconds="0">Never</button>
    <button data-seconds="60">1 min</button>
    <button data-seconds="600">10 min</button>
    <button data-seconds="1800">30 min</button>
  </div>

  <div id="status">Loading...</div>
</main>
<script>
  var power = document.getElementById('power');
  var slider = document.getElementById('brightness');
  var percent = document.getElementById('percent');
  var statusEl = document.getElementById('status');
  var dragging = false;

  function render(state) {
    power.textContent = state.on ? 'ON' : 'OFF';
    power.className = state.on ? 'on' : '';
    if (!dragging) { slider.value = state.brightness; }
    percent.textContent = Math.round(slider.value * 100 / 255);
    statusEl.textContent = state.timer_s > 0
      ? 'Switching off in ' + state.timer_s + ' s'
      : 'No timer set';
  }

  function send(params) {
    return fetch('/api/light', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: params
    }).then(function (r) { return r.json(); }).then(render)
      .catch(function () { statusEl.textContent = 'Cannot reach the ESP32'; });
  }

  function refresh() {
    fetch('/api/state').then(function (r) { return r.json(); }).then(render)
      .catch(function () { statusEl.textContent = 'Cannot reach the ESP32'; });
  }

  power.addEventListener('click', function () {
    send('on=' + (power.className === 'on' ? '0' : '1'));
  });
  slider.addEventListener('input', function () {
    dragging = true;
    percent.textContent = Math.round(slider.value * 100 / 255);
  });
  slider.addEventListener('change', function () {
    dragging = false;
    send('brightness=' + slider.value);
  });
  document.querySelectorAll('.timers button').forEach(function (button) {
    button.addEventListener('click', function () {
      send('timer=' + button.getAttribute('data-seconds'));
    });
  });

  refresh();
  // Polling also picks up presses of the physical button on the breadboard.
  setInterval(refresh, 2000);
</script>
</body>
</html>
)HTML";
