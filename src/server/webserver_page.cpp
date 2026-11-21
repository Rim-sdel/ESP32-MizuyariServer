#include "WString.h"
#include "data_struct.h"
#include "id_register.h"
#include "pots_handler.h"
#include <Arduino.h>
#include <WebServer.h>

WebServer server(80);
const String page = R"del(
<!doctype html>
<html>
  <head>
    <meta charset="UTF-8" />
    <title>ESP32 Web Server</title>
  </head>
  <body>
    <h1>ESP32 HTTP Mizuyari Server...</h1>
    <h2>鉢リスト</h2>
    <div id="potsList">読込み中…</div>

    <script>
      // --- pots.jsonからリストを取得して表示 --
      async function loadPots() {
        const res = await fetch("/pots");
        const pots = await res.json();
        const container = document.getElementById("potsList");
        container.innerHTML = "";

        pots.forEach((pot) => {
          const div = document.createElement("div");
          div.className = "pot";
          div.innerHTML = `
			<span class="id">${pot.ID}</span> 
			<span class="pot-name">${pot.name || '名前未定'}</span>
			<button onclick="renamePot(${pot.name})">名前変更</button><br>
			<span class="moisture">湿度: ${pot.moisture || '未取得'}</span>
			<span class="last-seen">最終取得: ${pot.lastSeen || '不明'}</span><br>
			<span class="mac">MAC: ${pot.mac}</span>
            <!--鉢を有効にしたり無効にしたり、削除したりするボタンをつくりたい-->
		  `;
		container.appendChild(div);
		});
	  }
	  loadPots();
    </script>
  </body>
</html>
)del";

void handleRoot() { server.send(200, "text/html", page); }

void getPotsData() {
  String res = loadPotsStr();
  server.send(200, "text/plain", res);
}
void serverSetup() {
  server.on("/", handleRoot);
  server.on("/pots", getPotsData);
  server.begin();
  Serial.println("Web server start!!");
}

void serverHandle() { server.handleClient(); }
