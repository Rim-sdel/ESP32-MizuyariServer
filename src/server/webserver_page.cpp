#include "WString.h"
#include "data_struct.h"
#include "esp_now_server_cb_handler.h"
#include "id_register.h"
#include "pots_handler.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>

WebServer server(80);
const String page = R"del(
<!doctype html>
<html>
  <head>
	<meta charset="UTF-8">
    <title>ESP32 Web Server</title>
  </head>
  <body>
    <h1>ESP32 HTTP Mizuyari Server...</h1>
    <h2>鉢リスト</h2>
    <div id="potsList">読込み中…</div>

    <script>
// --- pots.jsonからリストを取得して表示 ---
async function loadPots() {
  // --- dev ---
  const IS_DEV =
    location.port !== "80" &&
    location.hostname !== location.hostname.match(/^\d/)?.[0];
  const BASE = IS_DEV ? "." : "";
  // -----------
  // const res = await fetch("/pots");g
  const res = await fetch(BASE + (IS_DEV ? "/mock-pots.json" : "/pots"));
  const pots = await res.json();
  const container = document.getElementById("potsList");

  container.innerHTML = "";

  pots.forEach((pot) => {
    const div = document.createElement("div");
    div.className = "pot";

    const moistBtn =
      pot.moist != null
        ? pot.moist
        : `<button onclick="setMoist(${pot.ID}, event)">未設定</button>`;
    const dryBtn =
      pot.dry != null
        ? pot.dry
        : `<button onclick="setDry(${pot.ID}, event)">未設定</button>`;
    const threshBtn =
      pot.threshold != null
        ? pot.threshold
        : `<button onclick="choiceThresh(${pot.ID}, event)">未設定</button>`;

	const dbgBtn =
	  pot.debugMode == 0
	  ? `<button onclick="startDbgMode(${pot.ID})">start debug</button>`
	  : `<button onclick="endDbgMode(${pot.ID})">end debug</button>`;

    div.innerHTML = `
			<span class="id">${pot.ID}</span> 
			<span class="potName">${pot.name || "名前未定"}</span>
			<button onclick="renamePot(${pot.ID})">名前変更</button><button onclick="removePot(${pot.ID})">削除</button><br>
		    <span class="moisture">湿度： ---</span>
			<span class="raw">センサー値: ${pot.raw || "---"}</span><br>
			<span class="lastSeen">最終取得: ${pot.lastSeen || "不明"}</span><br>
			<span class="calibPot">
			  moist: ${moistBtn}
			  dry: ${dryBtn}
			  threshold: ${threshBtn}
			</span><br>
			<span class="mac">MAC: ${pot.mac}</span><br>
			${dbgBtn}
            <!--鉢を有効にしたり無効にしたり、削除したりするボタンをつくりたい-->
		  `;
    container.appendChild(div);
  });
}

function removeOld() {
  const oldCl = document.getElementById("calib-popup");
  const oldTh = document.getElementById("thresh-popup");
  if (oldCl) oldCl.remove();
  if (oldTh) oldTh.remove();
}

// --- 名前変更ボタンの処理をここに書く ---
async function renamePot(id) {
  const newName = prompt("新しい名前を入力:");
  if (!newName) return;

  await fetch("/rename", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ id: id, name: newName }),
  });
  loadPots();
}

// --- キャリブレーション値登録ボタン ---

async function setMoist(id, event) {
  showCalibPopup(id, "moist", event);
}

async function setDry(id, event) {
  showCalibPopup(id, "dry", event);
}

function showCalibPopup(id, type, event) {
  removeOld();

  const potDiv = event.target.closest(".pot");
  const rawText = potDiv.querySelector(".raw").textContent;
  const rawVal = Number(rawText.replace(/[^0-9.\-]/g, "")) || 0;

  const btn = event.target;
  const popup = document.createElement("div");
  popup.id = "calib-popup";
  popup.className = "inline-popup";
  popup.innerHTML = `
    <button onclick="setCalib(${id}, '${type}', ${rawVal})">
      現在値:${rawVal}を使う
    </button>
    <span>または</span>
    <input type="number" placeholder="raw値を入力">
    <button onclick="
	  const val = this.previousElementSibling.value;
	  if(val) setCalib(${id}, '${type}', Number(val))">
      決定
    </button>
    <button onclick="this.parentElement.remove()">キャンセル</button>
  `;
  btn.after(popup);
}

async function setCalib(id, type, val) {
  const body = { id };
  body[type] = val;

  await fetch("/calibrate", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  document.getElementById("calib-popup")?.remove();
  loadPots();
}

// --- thresholdを設定する
function choiceThresh(id, event) {
  removeOld();

  const btn = event.currentTarget;
  const popup = document.createElement("div");
  popup.id = "thresh-popup";
  popup.className = "inline-popup";

  popup.innerHTML = `
    <input type="range" min="0" max="100" value="50" oninput="this.nextElementSibling.textContent = this.value">
    <span>50</span>
    <button onclick="setThresh(${id}, this)">決定</button>
    <button onclick="this.parentElement.remove()">キャンセル</button>
  `;
  btn.after(popup);
}

async function setThresh(id, btnEl) {
  const threshold =
    btnEl.parentElement.querySelector("input[type=range]").value;

  await fetch("/calibrate", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ id, threshold }),
  });
  btnEl.parentElement.remove();
  loadPots();
}

// ---
async function removePot(id) {
  await fetch("/remove", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ id: id }),
  });
  loadPots();
}

async function startDbgMode(id) {
  const choice = confirm("デバッグモードに入りますか？");
  if (!choice) {
	return;
  } else {
  
	  await fetch("/debug", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ id: id, debugMode: 1 }),
  });
  loadPots();  } 
}
async function endDbgMode(id) {
  const choice = confirm("デバッグモードを終了しますか？");
  if (!choice) {
	return;
  } else {
  
	  await fetch("/debug", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ id: id, debugMode: 0 }),
  });
  loadPots();  } 
}
loadPots();
setInterval(loadPots, 30000);
    </script>
  </body>
</html>
)del";

void handleRoot() { server.send(200, "text/html", page); }

void getPotsData() {
  String res = loadPotsStr();
  server.send(200, "text/plain", res);
}

void handleRename() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int id = doc["id"];
  const char *newName = doc["name"];

  updatePotName(id, String(newName));
  server.send(200, "text/plain", "OK");
}

void handleCalib() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int id = doc["id"];

  if (!doc["moist"].isNull()) {
    updatePotCalib(id, "moist", doc["moist"].as<int>());
  } else if (!doc["dry"].isNull()) {
    updatePotCalib(id, "dry", doc["dry"].as<int>());
  } else if (!doc["threshold"].isNull()) {
    updatePotCalib(id, "threshold", doc["threshold"].as<int>());
  }

  server.send(200, "text/plain", "OK");
}

void handleRemove() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int id = doc["id"];
  sendRemoveID(id);

  server.send(200, "text/plain", "OK");
}

void handleDbg() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int id = doc["id"];
  int dbgMode = doc["debugMode"];
  toggleDbg(id, dbgMode);

  server.send(200, "text/plain", "OK");
}

void serverSetup() {
  server.on("/", handleRoot);
  server.on("/pots", getPotsData);
  server.on("/rename", HTTP_POST, handleRename);
  server.on("/calibrate", HTTP_POST, handleCalib);
  server.on("/remove", HTTP_POST, handleRemove);
  server.on("/debug", HTTP_POST, handleDbg);
  server.begin();
  Serial.println("Web server start!!");
}

void serverHandle() { server.handleClient(); }
