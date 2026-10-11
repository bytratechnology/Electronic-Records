#pragma once

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Plant Watering</title>
<style>
  body { font-family: system-ui, sans-serif; margin: 0; padding: 16px; background: #f4f6f8; color: #1c2733; }
  main { max-width: 480px; margin: 0 auto; }
  h1 { font-size: 1.3rem; }
  .panel { background: #fff; border-radius: 10px; padding: 18px; margin-bottom: 12px; box-shadow: 0 1px 3px rgba(0,0,0,.12); }
  .value { font-size: 2.4rem; font-weight: 600; }
  .bar { height: 12px; background: #dee2e6; border-radius: 6px; overflow: hidden; margin: 8px 0; }
  .bar div { height: 100%; background: #2f9e44; width: 0; }
  .muted { color: #5b6b7b; font-size: .85rem; }
  button { padding: 10px 14px; border-radius: 8px; border: 1px solid #adb5bd; background: #fff; cursor: pointer; }
  label { display: flex; justify-content: space-between; align-items: center; margin: 8px 0; font-size: .9rem; }
  input[type=number] { width: 90px; padding: 6px; }
</style>
</head>
<body>
<main>
  <h1>Plant Watering</h1>

  <div class="panel">
    <div class="muted">Soil moisture</div>
    <div class="value"><span id="moisture">--</span>%</div>
    <div class="bar"><div id="bar"></div></div>
    <div class="muted">Raw ADC <span id="raw">--</span> | Pump <strong id="pump">--</strong> | <span id="phase">--</span></div>
    <div class="muted">Automatic runs in the last 24 h: <span id="runs">--</span></div>
    <p><button id="water">Water now</button></p>
  </div>

  <form class="panel" id="settings">
    <label>Automatic watering <input type="checkbox" id="auto"></label>
    <label>Water when below (%) <input type="number" id="start" min="5" max="90"></label>
    <label>Pump time (s) <input type="number" id="pumpSeconds" min="1" max="30"></label>
    <label>Soak wait (min) <input type="number" id="soak" min="1" max="240"></label>
    <label>Dry calibration (raw) <input type="number" id="dry" min="0" max="4095"></label>
    <label>Wet calibration (raw) <input type="number" id="wet" min="0" max="4095"></label>
    <button type="submit">Save settings</button>
    <span class="muted" id="saved"></span>
  </form>
</main>
<script>
  var formFilled = false;

  function byId(id) { return document.getElementById(id); }

  function render(s) {
    byId('moisture').textContent = s.moisture;
    byId('bar').style.width = s.moisture + '%';
    byId('raw').textContent = s.raw;
    byId('pump').textContent = s.pump ? 'ON' : 'OFF';
    byId('phase').textContent = s.phase;
    byId('runs').textContent = s.runs_today + ' / ' + s.max_runs;
    if (!formFilled) {
      byId('auto').checked = s.auto;
      byId('start').value = s.start_percent;
      byId('pumpSeconds').value = s.pump_seconds;
      byId('soak').value = s.soak_minutes;
      byId('dry').value = s.dry_raw;
      byId('wet').value = s.wet_raw;
      formFilled = true;
    }
  }

  function refresh() {
    fetch('/api/state').then(function (r) { return r.json(); }).then(render).catch(function () {});
  }

  function post(url, body) {
    return fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: body
    }).then(function (r) { return r.json(); });
  }

  byId('water').addEventListener('click', function () {
    post('/api/water', '').then(render);
  });

  byId('settings').addEventListener('submit', function (event) {
    event.preventDefault();
    var body = 'auto=' + (byId('auto').checked ? 1 : 0) +
      '&start_percent=' + byId('start').value +
      '&pump_seconds=' + byId('pumpSeconds').value +
      '&soak_minutes=' + byId('soak').value +
      '&dry_raw=' + byId('dry').value +
      '&wet_raw=' + byId('wet').value;
    post('/api/settings', body).then(function (s) {
      formFilled = false;
      render(s);
      byId('saved').textContent = 'Saved';
      setTimeout(function () { byId('saved').textContent = ''; }, 2000);
    });
  });

  refresh();
  setInterval(refresh, 2000);
</script>
</body>
</html>
)HTML";
