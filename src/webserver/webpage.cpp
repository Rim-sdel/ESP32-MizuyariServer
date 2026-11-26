#include "WString.h"

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
  const res = await fetch("/pots");
  const pots = await res.json();
  const container = document.getElementById("potsList");

  container.innerHTML = "";

  pots.forEach((pot) => {
    const div = document.createElement("div");
    div.className = "pot";

    const moistBtn =
      pot.moistVal != null
        ? pot.moistVal
        : `<button onclick="setMoist(${pot.ID}, event)">未設定</button>`;
    const dryBtn =
      pot.dryVal != null
        ? pot.dryVal
        : `<button onclick="setDry(${pot.ID}, event)">未設定</button>`;
    const threshBtn =
      pot.threshold != null
        ? pot.threshold
        : `<button onclick="choiceThresh(${pot.ID}, event)">未設定</button>`;

	const dbgBtn =
	  pot.debugMode == 0
	  ? `<button onclick="startDbgMode(${pot.ID})">start debug</button>`
	  : `<button onclick="endDbgMode(${pot.ID})">end debug</button>`;

	const editMoistBtn = 
	  pot.moistVal != null
	  ? `<button onclick="setMoist(${pot.ID}, event)">変更</button>`
	  : "";
	const editDryBtn = 
	  pot.dryVal != null 
	  ? `<button onclick="setDry(${pot.ID}, event)">変更</button>`
	  : "";
	const editThreshBtn =
	  pot.threshold != null 
	  ? `<button onclick="choiceThresh(${pot.ID}, event)">変更</button>`
	  : "";

    div.innerHTML = `
			<span class="id">${pot.ID}</span> 
			<span class="potName">${pot.name || "名前未定"}</span>
			<button onclick="renamePot(${pot.ID})">名前変更</button><button onclick="removePot(${pot.ID})">削除</button><br>
			<span class="moisture">湿度： ${pot.moisture || "---"}％</span>
			<span class="raw">センサー値: ${pot.sensorRaw || "---"}</span><br>
			<span class="lastSeen">最終取得: ${pot.lastSeen || "不明"}</span><br>
			<span class="calibPot">
			  moist: ${moistBtn} ${editMoistBtn}
			  dry: ${dryBtn} ${editDryBtn}
			  threshold: ${threshBtn} ${editThreshBtn}
			</span><br>
			<span class="batteryLevel">バッテリー残量: ${pot.battLev || "---"}</span><br>
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
  showCalibPopup(id, "moistVal", event);
}

async function setDry(id, event) {
  showCalibPopup(id, "dryVal", event);
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
