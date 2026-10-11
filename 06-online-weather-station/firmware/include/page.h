#pragma once

#include <Arduino.h>

// The whole web page lives in flash so the ESP32 needs no file system.
const char INDEX_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Weather Station</title>
<style>
  body { font-family: system-ui, sans-serif; margin: 0; padding: 16px; background: #f4f6f8; color: #1c2733; }
  main { max-width: 640px; margin: 0 auto; }
  h1 { font-size: 1.3rem; }
  .cards { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }
  .card { background: #fff; border-radius: 10px; padding: 16px; box-shadow: 0 1px 3px rgba(0,0,0,.12); }
  .label { font-size: .8rem; color: #5b6b7b; text-transform: uppercase; letter-spacing: .05em; }
  .value { font-size: 2.2rem; font-weight: 600; }
  canvas { width: 100%; height: 180px; background: #fff; border-radius: 10px; margin-top: 12px; }
  #status { font-size: .85rem; color: #5b6b7b; margin-top: 12px; }
  .bad { color: #b3261e; }
</style>
</head>
<body>
<main>
  <h1>ESP32 Weather Station</h1>
  <div class="cards">
    <div class="card"><div class="label">Temperature</div><div class="value"><span id="t">--</span> &deg;C</div></div>
    <div class="card"><div class="label">Humidity</div><div class="value"><span id="h">--</span> %</div></div>
  </div>
  <canvas id="chart" width="640" height="180"></canvas>
  <div id="status">Loading...</div>
</main>
<script>
  var statusEl = document.getElementById('status');

  function drawSeries(ctx, values, color, w, h) {
    var points = values.filter(function (v) { return v !== null; });
    if (points.length < 2) { return; }
    var min = Math.min.apply(null, points), max = Math.max.apply(null, points);
    if (max - min < 1) { max = min + 1; }
    ctx.strokeStyle = color;
    ctx.lineWidth = 2;
    ctx.beginPath();
    var started = false;
    values.forEach(function (v, i) {
      if (v === null) { return; }
      var x = 10 + (w - 20) * i / (values.length - 1);
      var y = h - 15 - (h - 30) * (v - min) / (max - min);
      if (started) { ctx.lineTo(x, y); } else { ctx.moveTo(x, y); started = true; }
    });
    ctx.stroke();
  }

  function refreshHistory() {
    fetch('/api/history').then(function (r) { return r.json(); }).then(function (d) {
      var canvas = document.getElementById('chart');
      var ctx = canvas.getContext('2d');
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      drawSeries(ctx, d.temperature, '#d9480f', canvas.width, canvas.height);
      drawSeries(ctx, d.humidity, '#1971c2', canvas.width, canvas.height);
      ctx.fillStyle = '#5b6b7b';
      ctx.font = '12px system-ui';
      ctx.fillText('Last ' + d.temperature.length + ' min  (orange: temperature, blue: humidity, each auto-scaled)', 10, 12);
    }).catch(function () {});
  }

  function refresh() {
    fetch('/api/readings').then(function (r) { return r.json(); }).then(function (d) {
      document.getElementById('t').textContent = d.temperature === null ? '--' : d.temperature.toFixed(1);
      document.getElementById('h').textContent = d.humidity === null ? '--' : d.humidity.toFixed(0);
      statusEl.className = d.ok ? '' : 'bad';
      statusEl.textContent = (d.ok ? 'Sensor OK' : 'Sensor read failed') +
        ' | uptime ' + d.uptime_s + ' s | Wi-Fi ' + d.rssi + ' dBm';
    }).catch(function () {
      statusEl.className = 'bad';
      statusEl.textContent = 'Cannot reach the ESP32';
    });
  }

  refresh();
  refreshHistory();
  setInterval(refresh, 5000);
  setInterval(refreshHistory, 60000);
</script>
</body>
</html>
)HTML";
