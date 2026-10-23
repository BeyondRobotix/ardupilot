# BR-Ardupilot

Beyond Robotix fork of [ArduPilot](https://github.com/ArduPilot/ardupilot).

This default branch contains only this README. The firmware source lives on the
branches listed below. Pick the branch for your board and clone it directly:

```sh
git clone --recurse-submodules -b <branch> https://github.com/BeyondRobotix/BR-Ardupilot.git
```

Build instructions are the same as upstream ArduPilot, see
[BUILD.md](https://github.com/ArduPilot/ardupilot/blob/master/BUILD.md). For example:

```sh
./waf configure --board BR_Core
./waf plane
```

## Branches

### Upstream

| Branch | Description |
|---|---|
| `master` | Mirror of ArduPilot `master`. Contains no Beyond Robotix changes; base for upstream contributions. |

### Flight controllers

| Branch | Board (hwdef) | Description |
|---|---|---|
| `BR_CORE_HW_DEF_2-4.7.0` | `BR_Core` | BR Core hardware **V1.5** (boards in the field) on ArduPilot 4.7.0 (Plane-4.7.0 tag). Board ID `AP_HW_MATEKH743` (1013), matching the MatekH743 bootloader V1.5 boards ship with. PWM 11 on PB1 (TIM3). |
| `BR_CORE_HW_DEF_V1.6-4.7.0` | `BR_Core` | BR Core hardware **V1.6** on ArduPilot 4.7.0. Board ID `AP_HW_BR_CORE` (1700): the board needs the V1.6 bootloader (`Tools/bootloaders/BR_Core_bl.bin` on this branch) flashed first. PWM 11 on PA3 (TIM15). |

Use the branch that matches the hardware revision. The uploader checks the board
ID, so V1.6 firmware won't load through the MatekH743 (1013) bootloader on V1.5
boards, and V1.5 firmware drives PWM 11 on the wrong pin on V1.6 boards.

### CAN / DroneCAN peripherals (AP_Periph)

| Branch | Board (hwdef) | Description |
|---|---|---|
| `CAN-Node-Plus-Periph` | `BR_CANNodePlus` | CAN Node Plus (STM32H723) AP_Periph firmware. |
| `Micro-Node-hwdef-2` | `BeyondRobotixM`, `BeyondRobotixM-Periph` | Micro Node AP_Periph firmware. |
| `GNSS-MAG-Combo` | `BR_GNSS_MAG`, `BR_GNSS_DAN_F10N`, `BR_Mag_RM3100` | Combined GNSS + magnetometer node. |
| `BR_GNSS_DAN_F10N` | `BR_GNSS_DAN_F10N`, `BR_Mag_RM3100` | u-blox DAN-F10N GNSS node. |
| `BR_MAG_RM3100` | `BR_Mag_RM3100` | RM3100 magnetometer node. |
| `ArduinoDroneCAN-Bootloader` | `BeyondRobotixM` | Bootloader for the Arduino DroneCAN (Micro Node) board. |

### Features

| Branch | Description |
|---|---|
| `dronecan-indicated-airspeed` | DroneCAN `IndicatedAirspeed` / `TrueAirspeed` support in AP_Airspeed, with autotest. Intended for upstream. |
| `AUAV-Add-All` | Additional AUAV airspeed sensor support (`MatekL431-AUAV`). |

### Personal / experimental

| Branch | Description |
|---|---|
| `dev/harvey` | Development branch based on 4.6.3. |
| `anon` | Experimental Micro Node work. |
| `copilot/*` | Automated assistant working branches. |

## Contributing upstream

Changes that are useful to the wider ArduPilot community should be submitted to
[ArduPilot/ardupilot](https://github.com/ArduPilot/ardupilot) from a branch based
on `master`, following the
[ArduPilot contributing guidelines](https://github.com/ArduPilot/ardupilot/blob/master/.github/CONTRIBUTING.md).

## License

ArduPilot is licensed under the
[GNU General Public License v3](https://github.com/ArduPilot/ardupilot/blob/master/COPYING.txt).
All branches in this repository are distributed under the same license.
