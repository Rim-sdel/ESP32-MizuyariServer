#include "WString.h"

extern const String page = R"del(
<!doctype html>
<html>
<head>
	<meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Mizuyari Observer</title>
    <style>
      :root {
        color-scheme: dark;
        --bg: #080b0d;
        --surface: #0d1215;
        --surface-2: #11181c;
        --line: #223038;
        --line-soft: #172126;
        --text: #dbe7e7;
        --muted: #71858a;
        --dim: #4b5d62;
        --aqua: #7fc9c5;
        --aqua-bright: #a8e4df;
        --danger: #c98787;
        --debug: #d2b878;
        --radius: 12px;
      }

      * { box-sizing: border-box; }

      body {
        margin: 0;
        min-height: 100vh;
        background:
          radial-gradient(circle at 15% -10%, rgba(127, 201, 197, .07), transparent 32rem),
          linear-gradient(180deg, #090d0f 0%, var(--bg) 38rem);
        color: var(--text);
        font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas,
          "Liberation Mono", "Noto Sans Mono CJK JP", monospace;
        letter-spacing: .02em;
      }

      .shell {
        width: min(1080px, calc(100% - 32px));
        margin: 0 auto;
        padding: 54px 0 72px;
      }

      header {
        display: flex;
        align-items: flex-end;
        justify-content: space-between;
        gap: 24px;
        padding-bottom: 20px;
        border-bottom: 1px solid var(--line);
        margin-bottom: 26px;
      }

      .eyebrow {
        margin: 0 0 8px;
        color: var(--aqua);
        font-size: 11px;
        letter-spacing: .22em;
        text-transform: uppercase;
      }

      h1 {
        margin: 0;
        font-size: clamp(24px, 4vw, 38px);
        font-weight: 500;
        letter-spacing: -.03em;
      }

      .system-state {
        display: flex;
        align-items: center;
        gap: 8px;
        color: var(--muted);
        font-size: 11px;
        white-space: nowrap;
      }

      .system-state::before {
        content: "";
        width: 7px;
        height: 7px;
        border-radius: 50%;
        background: var(--aqua);
        box-shadow: 0 0 12px rgba(127, 201, 197, .75);
      }

      .section-label {
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin: 0 2px 12px;
        color: var(--muted);
        font-size: 11px;
        letter-spacing: .16em;
        text-transform: uppercase;
      }

      #potsList {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(min(100%, 310px), 1fr));
        gap: 14px;
      }

      .pot {
        position: relative;
        overflow: visible;
        padding: 20px;
        border: 1px solid var(--line);
        border-radius: var(--radius);
        background: linear-gradient(145deg, rgba(17, 24, 28, .96), rgba(11, 16, 19, .98));
        box-shadow: 0 16px 40px rgba(0, 0, 0, .16);
      }

      .pot::before {
        content: "";
        position: absolute;
        inset: 0 auto 0 0;
        width: 2px;
        border-radius: var(--radius) 0 0 var(--radius);
        background: var(--aqua);
        opacity: .35;
      }

      .pot[data-debug="1"] { border-color: rgba(210, 184, 120, .45); }
      .pot[data-debug="1"]::before { background: var(--debug); opacity: .9; }

      .pot-head {
        display: flex;
        align-items: flex-start;
        justify-content: space-between;
        gap: 12px;
        margin-bottom: 22px;
      }

      .identity { min-width: 0; }

      .id {
        display: block;
        margin-bottom: 5px;
        color: var(--muted);
        font-size: 10px;
        letter-spacing: .16em;
      }

      .id::before { content: "NODE / "; }

      .potName {
        display: block;
        overflow: hidden;
        color: #eef7f6;
        font-family: system-ui, -apple-system, "Noto Sans JP", sans-serif;
        font-size: 20px;
        font-weight: 520;
        letter-spacing: .01em;
        text-overflow: ellipsis;
        white-space: nowrap;
      }

      .mode {
        padding: 5px 7px;
        border: 1px solid var(--line);
        border-radius: 999px;
        color: var(--muted);
        font-size: 9px;
        letter-spacing: .12em;
      }

      .pot[data-debug="1"] .mode {
        border-color: rgba(210, 184, 120, .35);
        color: var(--debug);
      }

      .moist-block { margin-bottom: 18px; }

      .moist-row {
        display: flex;
        align-items: baseline;
        justify-content: space-between;
        gap: 12px;
        margin-bottom: 9px;
      }

      .moist-label { color: var(--muted); font-size: 10px; letter-spacing: .12em; }

      .moisture {
        color: var(--aqua-bright);
        font-size: 29px;
        font-variant-numeric: tabular-nums;
        letter-spacing: -.04em;
      }

      .moisture small { color: var(--muted); font-size: 11px; letter-spacing: .04em; }

      .gauge {
        position: relative;
        height: 5px;
        overflow: hidden;
        border-radius: 999px;
        background: #182226;
      }

      .gauge-fill {
        height: 100%;
        border-radius: inherit;
        background: linear-gradient(90deg, #527f7d, var(--aqua-bright));
        box-shadow: 0 0 14px rgba(127, 201, 197, .28);
        transition: width .35s ease;
      }

      .gauge-threshold {
        position: absolute;
        top: -2px;
        width: 1px;
        height: 9px;
        background: rgba(219, 231, 231, .65);
      }

      .telemetry {
        display: grid;
        grid-template-columns: 1fr 1fr;
        gap: 1px;
        overflow: hidden;
        margin: 0 -4px 18px;
        border: 1px solid var(--line-soft);
        border-radius: 8px;
        background: var(--line-soft);
      }

      .datum {
        min-width: 0;
        padding: 10px 11px;
        background: rgba(10, 15, 17, .88);
      }

      .datum.wide { grid-column: 1 / -1; }
      .datum-label { display: block; margin-bottom: 4px; color: var(--dim); font-size: 9px; letter-spacing: .12em; }
      .datum-value { color: #a9b9bb; font-size: 11px; font-variant-numeric: tabular-nums; }
      .raw .datum-value { color: var(--text); }

      .calibPot {
        display: grid;
        grid-template-columns: repeat(3, 1fr);
        gap: 7px;
        margin-bottom: 16px;
      }

      .calib-item {
        position: relative;
        min-width: 0;
        padding: 9px;
        border: 1px solid var(--line-soft);
        border-radius: 8px;
        background: rgba(8, 12, 14, .45);
      }

      .calib-label { display: block; margin-bottom: 6px; color: var(--dim); font-size: 8px; letter-spacing: .12em; }
      .calib-value { color: #aebdbf; font-size: 11px; }
      .calib-value button { margin-top: 6px; }

      .mac {
        display: block;
        overflow: hidden;
        margin-bottom: 16px;
        color: var(--dim);
        font-size: 9px;
        letter-spacing: .08em;
        text-overflow: ellipsis;
        white-space: nowrap;
      }

      .actions { display: flex; flex-wrap: wrap; gap: 7px; }

      button {
        appearance: none;
        border: 1px solid #2b3b42;
        border-radius: 6px;
        background: #11191c;
        color: #9fb0b3;
        padding: 6px 9px;
        font: inherit;
        font-size: 9px;
        letter-spacing: .04em;
        cursor: pointer;
        transition: border-color .15s ease, color .15s ease, background .15s ease;
      }

      button:hover { border-color: #527078; color: #e1ecec; background: #162126; }
      button.danger:hover { border-color: rgba(201, 135, 135, .55); color: #e2abab; }
      button.debug-action { margin-left: auto; }
      .pot[data-debug="1"] button.debug-action { color: var(--debug); border-color: rgba(210, 184, 120, .3); }

      .inline-popup {
        position: absolute;
        z-index: 20;
        display: flex;
        flex-wrap: wrap;
        align-items: center;
        gap: 7px;
        width: min(330px, calc(100vw - 48px));
        margin-top: 8px;
        padding: 10px;
        border: 1px solid #40545b;
        border-radius: 8px;
        background: #10181b;
        box-shadow: 0 18px 45px rgba(0, 0, 0, .45);
        color: var(--muted);
        font-size: 9px;
      }

      input[type="number"] {
        width: 100px;
        border: 1px solid #2b3b42;
        border-radius: 5px;
        outline: none;
        background: #090e10;
        color: var(--text);
        padding: 6px 7px;
        font: inherit;
      }

      input[type="range"] { accent-color: var(--aqua); width: 130px; }

      .empty {
        grid-column: 1 / -1;
        padding: 48px 20px;
        border: 1px dashed var(--line);
        border-radius: var(--radius);
        color: var(--muted);
        text-align: center;
        font-size: 11px;
      }

      @media (max-width: 620px) {
        .shell { width: min(100% - 20px, 1080px); padding-top: 30px; }
        header { align-items: flex-start; flex-direction: column; }
        .calibPot { grid-template-columns: 1fr; }
        .telemetry { grid-template-columns: 1fr; }
        .datum.wide { grid-column: auto; }
      }
    </style>
  </head>
  <body>
    <main class="shell">
      <header>
        <div>
          <p class="eyebrow">ESP32 / SOIL OBSERVATION NETWORK</p>
          <h1>Mizuyari Observer</h1>
        </div>
        <div class="system-state">SERVER ONLINE</div>
      </header>
      <div class="section-label"><span>registered nodes</span><span id="nodeCount">-- nodes</span></div>
      <div id="potsList"><div class="empty">同期中...</div></div>

	  <br><br>
    <button onclick="resetAll()">全データリセット</button>
    </main>


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
	div.dataset.debug = pot.debugMode == 1? "1":"0";

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

	const moisture = pot.moisture ?? null;
	const moistureText = moisture == null ? "---" : moisture;
	const moistureWidth = moisture == null ? 0 : Math.max(0,Math.min(100, Number(moisture)));
	const threshold = pot.threshold ?? null;
	const thresholdPos = threshold ==null? -1 : Math.max(0,Math.min(100, Number(threshold)));

    div.innerHTML = `
      <div class="pot-head">
        <div class="identity">
          <span class="id">${String(pot.ID).padStart(2, "0")}</span>
			<span class="potName">${pot.name || "名前未定"}</span>
        </div>
        <span class="mode">${pot.debugMode == 1 ? "DEBUG / 30s" : "ACTIVE"}</span>
      </div>

      <div class="moist-block">
        <div class="moist-row">
          <span class="moist-label">SOIL MOISTURE</span>
          <span class="moisture">${moistureText}<small> %</small></span>
        </div>

		<div class="gauge">
          <div class="gauge-fill" style="width:${moistureWidth}%"></div>
          ${thresholdPos >= 0 ? `<span class="gauge-threshold" style="left:${thresholdPos}%" title="threshold ${threshold}%"></span>` : ""}
        </div>
      </div>

      <div class="telemetry">
        <div class="datum raw"><span class="datum-label">RAW ADC</span><span class="datum-value">${pot.sensorRaw ?? "---"}</span></div>
        <div class="datum"><span class="datum-label">THRESHOLD</span><span class="datum-value">${threshold ?? "---"}%</span></div>
        <div class="datum wide lastSeen"><span class="datum-label">LAST SEEN</span><span class="datum-value">${pot.lastSeen ? new Date(pot.lastSeen * 1000).toLocaleString(): "不明"}</span></div>
	<div class="datum wide lastSeen"><span class ="datum-label">BATTERY POWER</span><span class="datum-value"> ${pot.battLev || "---"} V</span></div><br>
      </div>


      <div class="calibPot">
        <div class="calib-item"><span class="calib-label">MOIST CALIB</span><span class="calib-value">${moistBtn} ${editMoistBtn}</span></div>
        <div class="calib-item"><span class="calib-label">DRY CALIB</span><span class="calib-value">${dryBtn} ${editDryBtn}</span></div>
        <div class="calib-item"><span class="calib-label">THRESHOLD</span><span class="calib-value">${threshBtn} ${editThreshBtn}</span></div>
      </div>


	  <span class="mac">MAC / ${pot.mac || "UNKNOWN"}</span>
      <div class="actions">
        <button onclick="renamePot(${pot.ID})">名前変更</button>
        <button class="danger" onclick="removePot(${pot.ID})">削除</button>
        ${pot.debugMode == 0
          ? `<button class="debug-action" onclick="startDbgMode(${pot.ID})">start debug</button>`
          : `<button class="debug-action" onclick="endDbgMode(${pot.ID})">end debug</button>`}
      </div>
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
async function renamePot(ID) {
  const newName = prompt("新しい名前を入力:");
  if (!newName) return;

  await fetch("/rename", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ ID: ID, name: newName }),
  });
  loadPots();
}

// --- キャリブレーション値登録ボタン ---

async function setMoist(ID, event) {
  showCalibPopup(ID, "moistVal", event);
}

async function setDry(ID, event) {
  showCalibPopup(ID, "dryVal", event);
}

function showCalibPopup(ID, type, event) {
  removeOld();

  const potDiv = event.target.closest(".pot");
  const rawText = potDiv.querySelector(".raw").textContent;
  const rawVal = Number(rawText.replace(/[^0-9.\-]/g, "")) || 0;

  const btn = event.target;
  const popup = document.createElement("div");
  popup.id = "calib-popup";
  popup.className = "inline-popup";
  popup.innerHTML = `
    <button onclick="setCalib(${ID}, '${type}', ${rawVal})">
      現在値:${rawVal}を使う
    </button>
    <span>または</span>
    <input type="number" placeholder="raw値を入力">
    <button onclick="
	  const val = this.previousElementSibling.value;
	  if(val) setCalib(${ID}, '${type}', Number(val))">
      決定
    </button>
    <button onclick="this.parentElement.remove()">キャンセル</button>
  `;
  btn.after(popup);
}

async function setCalib(ID, type, val) {
  const body = { ID };
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
function choiceThresh(ID, event) {
  removeOld();

  const btn = event.currentTarget;
  const popup = document.createElement("div");
  popup.id = "thresh-popup";
  popup.className = "inline-popup";

  popup.innerHTML = `
    <input type="range" min="0" max="100" value="50" oninput="this.nextElementSibling.textContent = this.value">
    <span>50</span>
    <button onclick="setThresh(${ID}, this)">決定</button>
    <button onclick="this.parentElement.remove()">キャンセル</button>
  `;
  btn.after(popup);
}

async function setThresh(ID, btnEl) {
  const threshold =
    btnEl.parentElement.querySelector("input[type=range]").value;

  await fetch("/calibrate", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ ID, threshold }),
  });
  btnEl.parentElement.remove();
  loadPots();
}

// ---
async function removePot(ID) {
  await fetch("/remove", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ ID: ID }),
  });
  loadPots();
}

async function startDbgMode(ID) {
  const choice = confirm("デバッグモードに入りますか？");
  if (!choice) {
	return;
  } else {

	  await fetch("/debug", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ ID: ID, debugMode: 1 }),
  });
  loadPots();  } 
}
async function endDbgMode(ID) {
  const choice = confirm("デバッグモードを終了しますか？");
  if (!choice) {
	return;
  } else {
  
	  await fetch("/debug", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ ID: ID, debugMode: 0 }),
  });
  loadPots();  } 
}
async function resetAll() {
  if (!confirm("全ての鉢データをリセットします。よろしいですか？")) return;
  if (!confirm("本当に？この操作は取り消せません。")) return;
  await fetch("/reset", { method: "POST" });
  loadPots();
}

loadPots();
setInterval(loadPots, 30000);
    </script>
  </body>
</html>
)del";
