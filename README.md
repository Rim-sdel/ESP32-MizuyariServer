# mizuyari

ESP32 と ESP-NOW を使った植木鉢モニタリングシステム。
土壌水分とバッテリー電圧を定期計測し、水切れ・充電切れを Discord Webhook で通知する。

## 構成

```
[Pot (ESP32-C6/C3)]  ── ESP-NOW ──>  [Server (ESP32)]  ── WiFi ──>  Discord
   センサー読み取り                     データ集約                    Webhook 通知
   Deep Sleep で省電力                  Web UI (port 80)
                                        LittleFS でデータ永続化
```

- **Pot** : 土壌水分センサーとバッテリーを搭載したセンサーノード。計測後 ESP-NOW でサーバへ送信し、Deep Sleep に入る。最大 10 台。
- **Server** : ESP32 上で動作する親機。Pot からのデータを受信・保存し、閾値を下回ると Discord へ通知する。Web UI でキャリブレーションや名前変更も可能。

## ハードウェア

### Pot

| ピン | 用途 |
|------|------|
| D0   | バッテリー電圧 (分圧回路経由) |
| D2   | 土壌水分センサー |
| D3   | センサー電源制御 |

対応ボード:
- Seeed XIAO ESP32-C6 (`env:pot`)
- ESP32-C3-DevKitM-1 (`env:potC3`)

### Server

- ESP32-DevKitC 等の標準 ESP32 ボード (`env:server`)

## セットアップ

### 開発環境

[PlatformIO](https://platformio.org/) が必要。Nix 環境では `nix develop` で FHS 互換シェルに入れる。

### secrets.h

`include/secrets.h` を作成する（git 管理外）:

```cpp
#pragma once
#include <cstdint>

#ifdef POT_MACHINE
inline const uint8_t serverAddr[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // サーバの MAC アドレス
#endif

#ifdef SERVER_MACHINE
inline const char *ssid = "YOUR_SSID";
inline const char *pass = "YOUR_PASSWORD";
inline const char *webhookUrl = "https://discord.com/api/webhooks/...";
#endif
```

### ビルド・書き込み

```bash
# Pot (C6)
pio run -e pot -t upload

# Pot (C3)
pio run -e potC3 -t upload

# Server
pio run -e server -t upload

# Server (LittleFS ファイルシステム)
pio run -e server -t uploadfs
```

## 動作の流れ

1. Pot が起床し、土壌水分とバッテリー電圧を計測
2. ESP-NOW でサーバへ `MSG_POT_DATA` を送信
3. サーバは受信データを処理し、Pot に `MSG_SERVER_RES` でスリープ時間を返送
4. Pot は指定時間 Deep Sleep に入る
5. サーバ側で水分がキャリブレーション閾値を下回るか、バッテリーが 3.45V 以下になると Discord に通知

### スリープスケジュール

- **デバッグモード** (新規登録時のデフォルト): 30 秒間隔
- **通常モード**: 毎日 12:00 と 0:00 に起床（Pot ID 分の秒数オフセット）

## Web UI

サーバの IP アドレスにブラウザでアクセスすると管理画面が開く。

| エンドポイント | メソッド | 機能 |
|----------------|----------|------|
| `/`            | GET      | 管理画面 |
| `/pots`        | GET      | 全 Pot データ取得 (JSON) |
| `/rename`      | POST     | Pot 名変更 |
| `/calibrate`   | POST     | キャリブレーション値設定 (moistVal / dryVal / threshold) |
| `/remove`      | POST     | Pot 削除 |
| `/debug`       | POST     | デバッグモード切替 |
| `/reset`       | POST     | 全 Pot データリセット |

## ESP-NOW メッセージ型

| 型 | 方向 | 内容 |
|----|------|------|
| `MSG_POT_DATA`      | Pot → Server | センサー値 + バッテリー電圧 |
| `MSG_SERVER_RES`     | Server → Pot | スリープ時間 |
| `MSG_ID_REGISTER`    | Server → Pot | 新規 ID 付与 |
| `MSG_ID_REMOVE`      | Server → Pot | ID 削除要求 |
| `MSG_ID_REMOVE_DONE` | Pot → Server | ID 削除完了 |
