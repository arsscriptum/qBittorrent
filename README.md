# qBittorrent - A BitTorrent client in Qt

## Description:

**Branch Of** [qbittorrent/qBittorrent](https://github.com/qbittorrent/qBittorrent). My branch is aimed at having a stable codebase for my **headless qBittorrent version** used for my docker container](https://github.com/arsscriptum/docker.qBittorrentNoxVpn) which runs this latest headless qBittorrent client while connecting to OpenVPN with integrated VPN.

It can be used in standalone mode, but it's not supported.

## Qt Dependency 

### Install Qt6 using the official Qt installer (if you don't have Qt6)

1. Download the Qt installer:
   ```sh
   wget https://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run
   ```
2. Make it executable:
   ```sh
   chmod +x qt-unified-linux-x64-online.run
   ```
3. Run the installer:
   ```sh
   ./qt-unified-linux-x64-online.run
   ```
4. Follow the GUI installation steps to install Qt6.

5. Set the correct Qt6 environment variables:
   ```sh
   export Qt6_DIR=~/Qt/6.x.x/gcc_64/lib/cmake/Qt6
   export PATH=~/Qt/6.x.x/gcc_64/bin:$PATH
   ```

## Build 

Make sure you have [Qt6 Installed](https://doc.qt.io/qt-6/get-and-install-qt.html) and export those paths:

```bash
export Qt6_DIR=/path/to/Qt/6.8.2/gcc_64/lib/cmake/Qt6
export PATH=/path/to/Qt/6.8.2/gcc_64/bin:$PATH
```


### Build using script

```
./scripts/build.sh
```

![ss](doc/img/ss.png)

#### Version generation

Avoid version files generation to mess your commits:

```bash
git update-index --assume-unchanged version.nfo
git update-index --assume-unchanged build.nfo
```

#### Build / Installation:

Refer to the [INSTALL](INSTALL) file.

## Features

| **FEATURES**                            |  **STATUS** | **VERSION** | **DOCKERHUB** |
|-----------------------------------------|:-----------:|:-----------:|:-------------:|
| ➭ View IP Data in About Menu            |    🚧 wip    |      ♒︎      |       ❌       |
| ➭ Read DHT bootstrap nodes from cfg     |    ✔️ done   |     1.0     |      1.0      |
| ➭ Integrate speedtest-cli in build      |    ✔️ done   |     1.0     |      1.0      |
| ➭ Add a menu to show speed test results | ⏳ whishlist |      ♒︎      |       ❌       |
| ➭                                       |             |             |               |


## Versions

| **VERSION** |   **STATUS**   | **DOCKERHUB** |  **DATE**  |
|-------------|:--------------:|:-------------:|:----------:|
|  **1.0.0**  |    ✔️ Stable    |     [1.0.0](https://hub.docker.com/repository/docker/arsscriptum/qbittorrentvpn/tags/1.0/sha256-eb4e39680cf805d838f765a137169f51463704ccab2dc66cfd700b1eb60178cd)     | 20/02/2025 |
|  **1.2.0**  | ■■□□□□□□□□ 20% |       ❌       |      ❌     |
|  **1.3.0**  |        ⏳       |       ♒︎       |      ♒︎     |
|  **1.4.0**  |        ⏳       |       ♒︎       |      ♒︎     |
|  **1.5.0**  |        ⏳       |       ♒︎       |      ♒︎     |