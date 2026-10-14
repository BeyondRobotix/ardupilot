# Beyond Robotix CAN Node Plus

AP_Periph firmware for the [Beyond Robotix CAN Node Plus](https://beyond-robotix.gitbook.io/docs/can-ecosystem/can-node-plus),
an STM32H723VGH6 node with two CAN FD interfaces.

## Bootloader

The board keeps the Beyond Robotix bootloader
([BR_bootloader](https://github.com/BeyondRobotix/BR_bootloader)) in the
first 128k flash sector. There is no ArduPilot bootloader for this board.

- The app is linked at 0x08020000, where BR_bootloader jumps to.
- AP_Periph and BR_bootloader share `struct app_bootloader_comms` at
  0x20000000 (start of DTCM), so a DroneCAN BeginFirmwareUpdate sent to the
  running app hands over to the bootloader, which then takes the update.
- `AP_BOOTLOADER_FLASHING_ENABLED` is off, so AP_Periph never writes sector 0.

If the bootloader is lost, flash `variants/MicroNodePlus/bootloader.bin`
from [br_platformio_hwdef](https://github.com/BeyondRobotix/br_platformio_hwdef)
at 0x08000000 with an ST-LINK, or recover over USB DFU by holding BOOT0
during reset.

## Building and loading

```
./waf configure --board BR_CANNodePlus
./waf AP_Periph
```

Load `build/BR_CANNodePlus/bin/AP_Periph.bin` over DroneCAN, or with an
ST-LINK at 0x08020000 (leave sector 0 alone):

```
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
  -c "program build/BR_CANNodePlus/bin/AP_Periph.bin 0x08020000 verify reset exit"
```

## Ports

| Connector | Port | Pins | Notes |
| --------- | ---- | ---- | ----- |
| USB-C | SERIAL0 | PA11/PA12 | console |
| S1 | SERIAL1 (USART2) | TX PD5, RX PD6, CTS PD3, RTS PD4 | |
| S2+I2C | SERIAL2 (USART3), I2C2 | TX PC10, RX PC11, SCL PB10, SDA PB11 | GPS and compass by default |
| Debug header | SERIAL3 (UART4) | PB8/PB9 | TX/RX swapped on the board, `UART3_OPTIONS` 8 by default |
| C1 | CAN1 | PD0/PD1 | |
| C2 | CAN2 | PB5/PB6 | |
| PWM | PWM 1-8 | PA3, PA2, PA1, PE13, PA9, PA8, PC7, PC6 | |
| SPI | SPI4 | SCK PE2, MISO PE5, MOSI PE6, CS PD15/PD14 | |

PA5 enables the current limited 5V peripheral rail (GPIO 80, on by default).
The green LED is on PB0.

## Known limitations

- The 5V bus monitor on PB1 is not used: the common H723 clock tree runs the
  ADC kernel clock at 96MHz, over the 50MHz limit, so the ADC is disabled.
- `APJ_BOARD_ID` 7140 is a provisional local ID, not allocated upstream.
