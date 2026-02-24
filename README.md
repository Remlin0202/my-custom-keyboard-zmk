# Hub84 客制化 ZMK 键盘

Hub84 是一把 84 配列的客制化机械键盘，主打集成的 USB Hub 功能以及 ZMK 蓝牙无线连接。

## 设备特征
- **结构设计**：一体化结构 (Integrated Plate)，上壳即定位板。分上、下两层外壳。
- **外观尺寸**：前高 13mm，输入倾角 5°。
- **拓展接口**：集成 USB Hub 功能。
- **固件支持**：使用 ZMK 固件，支持蓝牙连接。
- **配置工具**：兼容 ZMK Studio 网页端改键。

## 📂 目录结构与源文件
本仓库包含了该键盘从硬件到固件的完整开源资料：
- `boards/` & `config/`：ZMK 键盘固件的核心源码与配置文件。
- `hardware/`：硬件工程文件，包含 EDA 源码 (`eda/`)。
- `case/`：外壳及结构件设计文件，包含fusion360项目文件 (`fusion360/`)。
- `bootloader/`：主板使用的引导程序文件。

⬇️ **文件下载**: [固件(.uf2) | 生产图纸(Gerber) | 外壳模型(STL) | Bootloader](https://github.com/Remlin0202/hub84/releases)

### 📌 Releases说明 
Gerber文件被分为了两部分上传在 Release 中：
- **`hub84-board`**：这是键盘的**主按键板 (PCB)**。
- **`hub84-core`**：这包含了Type-C 小板、主控板以及 USB Hub 板的 PCB 图纸。
  `print` 字样的文件，这是方便在嘉立创一次下订单而手动拼版的文件（不一定过审）。

## ⚡ Bootloader
本项目的主控基于 nRF52840 开发，使用了 [**Adafruit nRF52 Bootloader (v0.5.0)**](https://github.com/adafruit/Adafruit_nRF52_Bootloader/releases/tag/0.5.0)。
在第一次使用前，必须通过 J-Link 或 DAPLink 将 `bootloader/` 目录下的 `.hex` 文件刷入芯片，之后才能通过 USB 拖入 ZMK `.uf2` 固件。

##  🙏 致谢与参考资料
本项目的硬件设计参考了 [luantty2/nRF52840-instruction](https://github.com/luantty2/nRF52840-instruction)。
固件基于 [ZMK Firmware](https://zmk.dev/) 构建。
Bootloader 使用 [Adafruit nRF52 Bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader)。
