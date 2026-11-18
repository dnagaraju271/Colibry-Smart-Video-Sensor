# ColibryNPU Evaluation Kit User Manual v0.3

> Searchable text extraction from the user-provided PDF reference. Page labels and extracted text are retained where available; figures and page layout may not be represented. Consult the original PDF for diagrams and exact visual formatting.

---

<PARSED TEXT FOR PAGE: 1 / 32>
ColibryNPU Evaluation Kit

User Manual
V0.3
<PARSED TEXT FOR PAGE: 2 / 32>
Contents

1 Introduction....................................................................................................................4
2 Available Evaluation Platforms.................................................................................... 5
   2.1 ColibryNPU EvalboardXP...................................................................................................................... 5
   2.2 ColibryNPU EvalboardTiny................................................................................................................... 5
3 Evaluation Kit Setup.......................................................................................................6
   3.1 Software Resources............................................................................................................................... 6
   3.2 System Requirements........................................................................................................................... 6
   3.3 Recommended Installation.................................................................................................................6
4 Hardware Description....................................................................................................9
   4.1 EvalboardXP Description...................................................................................................................... 9
   4.2 EvalboardTiny Description................................................................................................................... 9
   4.3 ColibryProbe Description................................................................................................................... 18
5 Programming Setup.....................................................................................................22
   5.1 EvalboardXP.......................................................................................................................................... 22
   5.2 EvalboardTiny....................................................................................................................................... 22
6 Developing with ColibryNPU......................................................................................23
   6.1 ColibryNPU SDK Description............................................................................................................. 23
   6.2 Programming the ColibryNPU.......................................................................................................... 25
   6.3 Programming the nRF52832............................................................................................................26
   6.4 Loading Data into the External Flash..............................................................................................26
   6.5 Neural Network Training and Quantization...................................................................................27
   6.6 Compiling a Neural Network Model................................................................................................ 27
   6.7 Using the NNPA Emulator................................................................................................................. 29
7 TFLite Hello World Example........................................................................................ 30
   7.1 Training the Model..............................................................................................................................30
   7.2 Converting to TFlite............................................................................................................................. 30
   7.3 Compiling the Model.......................................................................................................................... 30
   7.4 Loading the Bitstream into Flash.....................................................................................................31
   7.5 Running Inferences............................................................................................................................. 31
8 Frequently Asked Questions.......................................................................................32




2/32                                         ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 3 / 32>
Document history

 Version   Date                 Description

 0.1       2026/05/06           First release

 0.2       2026/06/22           Update for EvalboardTiny rev1

 0.3       2026/08/06           Installation process update + minor fixes




3/32                    ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 4 / 32>
                                                                                 Introduction




1 Introduction
The ColibryNPU System-On-Chip is available for evaluation on different hardware platforms
presented in this document. These platforms provides ready-to-use examples and enable the
embedding of real application with AI processing at ultra-low power consumption.

The Evaluation Kit consists of the following hardware components:
       ●   Kit1: EvalboardXP
       ●   Kit2: EvalboardTiny + ColibryProbe

Software resources for developing with ColibryNPU are also provided:
       ●   ColibryNPU SDK – Includes ColibryNPU drivers, sample and demo codes, and
           programming tools
       ●   GNU GCC compiler toolchain for RISC-V, optimized for the ColibryNPU target
       ●   Neural network model compiler – NNPA compiler and NNPA emulator
       ●   Online model compiler – Colibry Web Service (CWS)
       ●   Documentation set – Datasheets and user manuals
These resources are available at docs.ai.asygn.com.


All Evaluation Boards feature the following common elements:

       ●   The ColibryNPU microcontroller and required peripheral components
       ●   External Flash memory for non-volatile data or code storage
       ●   USB-C interface for programming the ColibryNPU internal SRAM or external Flash
       ●   JTAG debugger compatible with GDB and internal tools
       ●   LEDs and buttons for user interaction
       ●   UART over USB interface for ASCII communication between the ColibryNPU and a PC
       ●   Efficient and configurable power management
       ●   Various I/O headers or sockets for external communication
       ●   Parallel interface camera socket




4/32                           ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 5 / 32>
                                                              Available Evaluation Platforms




2      Available Evaluation Platforms
Two evaluation platforms are available: the Colibry EvalboardXP and the EvalboardTiny.
Both platforms use the same firmware and tools.



2.1 ColibryNPU EvalboardXP
[This section is under development and will be available in a future update.]



2.2 ColibryNPU EvalboardTiny
The EvalboardTiny is optimized for physical size and cost. As a result, power management
and USB-C bridge functions are not embedded on the board itself, unlike the EvalboardXP.
Instead, the kit includes a second programming board, the ColibryProbe, which provides
means of programming and debugging, and UART communication via a host PC.




         Figure 1: EvalboardTiny (A) connected to the ColibryProbe (B)




5/32                       ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 6 / 32>
                                                                           Evaluation Kit Setup




3          Evaluation Kit Setup

3.1 Software Resources
We strongly recommend downloading the last version of the ColibryNPU Software
Development Kit (SDK) and verifying that the following components are present after
installation:

       ●   ColibryNPU drivers
       ●   Sample and demo code
       ●   Colibry RISC-V runtime and tools

Please make sure that the root path of installation do not contain names with ‘space’
character, otherwise the Makefile will not work correctly.


3.2 System Requirements
To ensure full SDK functionality and avoid compatibility issues, the host PC used for code
development and interfacing with the Evaluation Boards must meet the following
requirements:

       ●   A Linux or windows host computer. The Linux OS is highly recommended for easier
           installation and development
       ●   Python 3.10 to 3.12 installed
       ●   A free USB port on host computer


3.3 Recommended Installation
The host PC used for developing and programming the boards should be properly set up
before use with the kit. This section describes a recommended step-by-step setup procedure.


             3.3.1       Windows Installation
The ColibryNPU Evaluation Kit is supported on Windows via the Windows Subsystem for
Linux (WSL).

    1. Install WSL2 (by default this is version 2) with a Linux distribution. This setup has been
           tested with Ubuntu 24.04 distribution. See Microsoft’s documentation.

    2. Configure WSL to use NAT networking.

    3. Set up the USB device sharing by following the instructions at Microsoft’s WSL USB
           guide. Ensure your antivirus/firewall settings do not interfere with WSL and USB



6/32                            ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 7 / 32>
                                                                                          Evaluation Kit Setup


         configurations. For example, ESET may block USB access, so add an exception for it.
         Keep this in mind for the following steps.

   4. Check the ColibryProbe detection: plug in the USB-C cable and run usbipd list in a
         terminal. The device list should show a USB device with VID:PID like 0403:6043.

   5. Set up the USB device sharing In an administrator PowerShell, bind the device with:
         usbipd bind -i 0403:6043 --force. This method uses the hardware ID, which remains
         consistent across reboots (instead of bus ID explained in Microsoft documentation).

         If you prefer not to use the --force option, manually attach the device to WSL in a
         user PowerShell: usbipd attach --wsl -i 0403:6043 -a

   6. Log into WSL and open a Linux shell. You can use VS Code to do that.

   7. Continue by following the Linux installation described in Section 3.3.2. All instructions
         must be executed in the WSL command interpreter, not in the Windows command
         interpreter. If the unplug and replug the USB cable has no effect on VCOM port
         detection, please restart WSL with wsl --shutdown

   8. To verify successful installation, try opening the USB device using VS Code’s Serial
         Monitor tool or another serial monitor.


            3.3.2          Linux Installation
For any Linux system, follow these instructions:
       1. Download the GNU GCC compiler toolchain for RISC-V, optimized for the ColibryNPU
          target from Asygn’s website.
       2. Install the toolchain: sudo apt install ./gcc-riscv32-corev-elf_14.1.0+asygn*amd64.deb
       3. Install required Python packages and USB/debug tools: sudo apt install python3-numpy
          python3-matplotlib python3-pyelftools make libusb-1.0-0 openocd gcc-arm-none-eabi
          It is mandatory to install pyftdi package version >= 0.57.2 to support the FTDI chip
          embedded on the ColibryProbe (pip install "pyftdi>=0.57.2").
       4. Add the necessary udev rules by creating a new file at /etc/udev/rules.d/99-
          asygn.rules with the following content:

           # Asygn Colibry Evaluation Board
           SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="0403", ATTR{idProduct}=="6043", MODE="0666"
           SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6043", MODE="0666"
           ACTION=="add", SUBSYSTEM=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6043", RUN+="/sbin/modprobe
           ftdi_sio", RUN+="/bin/sh -c 'echo 0403 6043 > /sys/bus/usb-serial/drivers/ftdi_sio/new_id'"


       5. Reload the udev rules: sudo udevadm control --reload && sudo udevadm trigger
       6. Check if the VCOM ports are detected. Plug the USB-C cable and run: ls /dev/ttyUSB* .
          This should display: ‘/dev/ttyUSB0 /dev/ttyUSB1 /dev/ttyUSB2 /dev/ttyUSB3’.
          If not, unplug and the cable and run the following commands before plugging it
          back in:
              sudo modprobe ftdi_sio
              echo 0403 6011 | sudo tee -a /sys/bus/usb-serial/drivers/ftdi_sio/new_id

7/32                                ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 8 / 32>
                                                                                  Evaluation Kit Setup


             echo 0403 6043 | sudo tee -a /sys/bus/usb-serial/drivers/ftdi_sio/new_id

           3.3.3       Visual Studio Code Support (Optional)

       1. Install the recommended VS Code extensions Native Debug (webfreak.debug) and
          Serial Monitor (ms-vscode.vscode-serial-monitor). Search @recommended in the
          Extensions panel and install all "Workspace Recommendations".

       2. Open .vscode/settings.json file and update the app.path and app.name settings
          to target your application folder. Press F5 to start debug mode. You can change
          other settings in launch.json, such as enabling/disabling the stopAtEntry
          parameter. VS Code will automatically generate the binary file with the optimization
          options configured in settings.json. OpenOCD and GDB will launch, and GDB will
          close automatically after OpenOCD.
       3. Open the Serial Monitor (near the Debug Console panel). Set the port to
          /dev/ttyUSB2 - FTDI with a baud rate of 115200 and line ending set to CRLF.




8/32                           ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 9 / 32>
                                                                            Hardware Description




    4      Hardware Description
    The full boards schematics are provided in separate documents.



     4.1 EvalboardXP Description
    [This section is under development and will be available in a future update.]



     4.2 EvalboardTiny Description
    The EvalboardTiny functionalities are described below. All interfacing signals on the different
    connectors and pin headers operate within the VDDIO voltage domain, typically 1.8V.




Figure 2: EvalboardTiny top side




    9/32                       ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 10 / 32>
                                                                 Hardware Description




Figure 3: EvalboardTiny bottom side


The main components and features of the EvalboardTiny are summarized in the following
table.




10/32                    ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 11 / 32>
                                                                                   Hardware Description



 Component                  Description                                                         Reference

 User button                User push-button connected to ColibryNPU DIO_00                     S2, C20

                            Slider switch that connects the EXT_VBAT source to the power-tree
 User switch                                                                                    S1
                            input. When in OFF position, the full board is powered off

 Camera FPC connector       21-pin connector for PAG79120J1 camera module                       J1

 Pin header top             6-pin header at 2.54mm pitch for external connection                J2

 Pin header bottom          6-pin header at 2.54mm pitch for external connection                J3

 nRF52832 BLE chip          Bluetooth LE SoC for RF 2.4 GHz communication                       U8

 2.4 GHz antenna            Chip antenna for BLE communication                                  AE1

 nRF RGB LED                RGB LED connected to the nRF52832 I/Os                              D2

 Probe connector            Connector at 1.27mm pitch for ColibryProbe link                     J5

 ColibryNPU core supply     Programmable core voltage to the ColibryNPU                         U5

 Flash memory               S25FS064SAGNFN030 64 Mbit flash memory                              U1

 8 MHz clock oscillator     ASEDVN-8.000MHz-XC-T to provide ColibryNPU reference HF clock       Y1

 MEMS microphone            DMM-4026-B-I2S-R microphone connected to ColibryNPU SPIM1           U2

 CR927 battery holder       Holder for a CR927 coin-cell battery                                BT1

 JST ZH battery connector   Slot for a JST ZH 1.5mm battery connector                           J6

 ColibryNPU user LED        Red LED connected to ColibryNPU DIO_43                              D1

 LIS2DW12 accelerometer     Accelerometer connected to ColibryNPU SPIM0                         U3

 EXT_VBAT monitoring        EXT_VBAT voltage measurement through a voltage divider              U9, R32, R33

 Camera board connector     Camera connector used to connect an Arducam daughter board          J4

 ColibryNPU                 ColibryNPU microcontroller                                          U7


           4.2.1          User Button and ColibryNPU Boot Mode




When pressed, DIO_00 is tied to VDDIO, and a high level is seen at this input. Otherwise, a
100nF capacitor is discharged through the pull-down resistor connected by default within
ColibryNPU DIO_00. This structure acts as a debouncing filter.

By default, DIO_00 is mapped to the BOOT_MODE_0 functionality, while BOOT_MODE_1 is
tied high. Therefore, the boot mode of the ColibryNPU can be:



11/32                          ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 12 / 32>
                                                                            Hardware Description


       ●   Button released: Boot mode = b’10 corresponding to “Flash Copy”. The external flash
           is expected to contain a RISC-V compiled code starting at flash address 0x00000000.
           This code is automatically copied into the ColibryNPU internal SRAM, and the CPU
           boots.

       ●   Button pressed: Boot mode = b’11 corresponding to “Wait start”. The CPU remains in
           a waiting state. Code can be manually loaded into the ColibryNPU SRAM, and a
           register write command triggers the CPU boot.


             4.2.2       User Switch and Power Supply
   ●       ON position: The EXT_VBAT power source is connected to the board’s power tree,
           powering all components, including the ColibryNPU. EXT_VBAT is connected to the “+”
           terminal of the CR927 battery holder and JST ZH battery connector. EXT_VBAT is also
           available on the pin header and probe connector.

   ●       OFF position: The entire board is powered off. Additionally, the power tree is tied to a
           1k ohms resistor to discharge the system.

The minimum EXT_VBAT voltage for proper operation is 2.0V.


             4.2.3       Camera FPC Connector
This connector allows direct connection of a PixArt Imaging PAG7920J1 camera module for
seamless camera integration. This module embeds the PAG7920LT sensor.

The board provides the required 1.8V and 3.3V power supplies. R40 can be soldered in case
the I2C address of the PAG7920LT must be changed.




12/32                          ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 13 / 32>
                                                                      Hardware Description



          4.2.4      Pin Header Top and Bottom
These connectors are not mounted by default, but pin headers can be soldered to access the
signals. Note that the connectors are shown as looking from the top side of the board.

    Top Connector (J2)




The following signals are provided, from left to right:

    ●   GND (power ground)
    ●   EXT_VBAT (main power supply, externally provided)
    ●   VDD_1V8 (1.8V supply generated on the board)
    ●   NRF_P0.20 (connected to GPIO P0.20 of the nRF52832 chip)
    ●   I2C_SCL (I2C clock signal; ColibryNPU is the master)
    ●   I2C_SDA (I2C data signal; ColibryNPU is the master )

    Bottom Connector (J3)




The following signals are provided, from left to right:

    ●   COLIBRY_SPIS_MISO (SPI slave data from ColibryNPU)
    ●   COLIBRY_SPIS_MOSI (SPI slave data to ColibryNPU)
    ●   COLIBRY_SPIS_SCK (SPI slave clock to ColibryNPU)
    ●   COLIBRY_SPIS_CSN (SPI slave chip select to ColibryNPU)
    ●   JTAG_TDO (connected to ColibryNPU DIO_13; can be used as a GPIO)
    ●   JTAG_TDI (connected to ColibryNPU DIO_12; can be used as a GPIO)

          4.2.5      nRF52832 BLE Chip
This microcontroller primarily manages Bluetooth LE communications. It features an
integrated flash memory that must be programmed with code for the ARM Cortex-M4F CPU
core. The chip is programmed via the SWD interface (SWDCLK and SWDIO) using the
ColibryProbe board.



13/32                       ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 14 / 32>
                                                                         Hardware Description


It communicates with the ColibryNPU through the SPIM0 interface, an interrupt signal,
and/or the I2C bus. Additionally, it can provide an accurate low-frequency clock (typically 16
kHz) to the ColibryNPU. The nRF52832 also controls the RGB LED and can measure the
EXT_VBAT level through its internal ADC module.




         4.2.6       nRF RGB LED
The RGB LED contains three LEDs in a single package, powered by the 2.8V supply. The
cathodes of the red, green, and blue LEDs are connected to P0.19, P0.18, and P0.17 GPIOs of
the nRF52832, respectively. By default, the cathodes are left floating, so no current flows
through the LEDs. If the cathodes are connected to ground (output low configuration), the
corresponding LED lights up. Applying PWM to each cathode can modulate the luminosity of
each LED, thus controlling the overall color of the RGB LED.




         4.2.7       Probe Connector
The probe connector layout is shown in the figure below, with the same orientation as in
Figure 2.




14/32                       ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 15 / 32>
                                                                     Hardware Description


                            Pin1 GND
                            Pin2 INT_VBAT, VBAT power after the switch
                            Pin3 UART TX from ColibryNPU
                            Pin4 UART RX to ColibryNPU
                            Pin5 JTAG_TDO through a 1kohm resistor
                            Pin6 JTAG_TDI
                            Pin7 JTAG_TMS or nRF interrupt
                            Pin8 JTAG TCK
                            Pin9 ColibryNPU SPI slave clock
                            Pin10 ColibryNPU SPI slave chip select
                            Pin11 ColibryNPU SPI slave MOSI
                            Pin12 ColibryNPU SPI slave MISO
                            Pin13 Flash SPI slave MOSI
                            Pin14 Flash SPI slave MISO
                            Pin15 Flash SPI slave clock
                            Pin16 Flash SPI slave chip select
                            Pin17 SWD clock for the nRF52832
                            Pin18 SWD data for the nRF52832
                            Pin19 P0.22 GPIO of the nRF52832
         4.2.8      ColibryNPU
                            Pin20Core
                                  SystemSupply
                                          reset (active low)

The ColibryNPU core power supply, typically 0.9V, can be dynamically adjusted based on the
application's computing requirements. For ultra-low power processing with an internal clock
below 50 MHz, the core VDD can be as low as 0.78V, optimizing power consumption.
Conversely, this voltage can be set to 1.2V to achieve the highest frequency of 300 MHz. The
programmable voltage is managed by a TPS628606YCHR step-down buck converter, which
features an I2C slave interface and can be programmed by either the ColibryNPU or the
nRF52832.


         4.2.9      Flash Memory
The external flash memory, based on the S25FS064SAGNFN030 component, provides 64
Mbits (8 Mbytes) of non-volatile storage. It interfaces with the ColibryNPU through the
dedicated Flash SPI (SPIMF) peripheral. The flash power supply is directly controlled by the




ColibryNPU via DIO_21.


15/32                      ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 16 / 32>
                                                                         Hardware Description



          4.2.10       8 MHz Clock Oscillator
The ColibryNPU can operate using its internal uncalibrated RC oscillator, but the clock
frequency is highly variable and dependent on temperature and voltage. Alternatively, the
internal RC can be calibrated if a temporary external reference clock is available.

The ASEDVN-8.000MHz component provides an accurate 8 MHz clock to the ColibryNPU,
either for RC calibration or when the PLL is used in closed-loop mode. This allows the
generation of an accurate internal clock whose frequency is an integer multiple of 8 MHz.
Since the power consumption of this clock source may be significant, it can be enabled or
disabled via DIO_20.


          4.2.11       MEMS Microphone




The EvalboardTiny includes a DMM-4026-B-I2S-R microphone with an I2S interface, enabling
the development of sound-based applications. The SPIMx peripherals of the ColibryNPU
support the I2S protocol, allowing audio data to be streamed through the SPIM1 peripheral.
The microphone's power supply is controlled via DIO_20.


          4.2.12       Battery Connectors




For embedded applications, the EXT_VBAT supply can be provided either by:
   ●    A CR927 coin-cell battery inserted in the BT1 holder, or
   ●    A rechargeable battery (LiPo or else) with a JST ZH 1.5mm connector.
When using the JST connector, ensure correct polarity, as no clear standard exists for such
connectors.




16/32                       ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 17 / 32>
                                                                    Hardware Description



          4.2.13      ColibryNPU User LED




A standard red LED is available on the board for the application use. It lights up when
DIO_43 is configured in an output at 0V.


          4.2.14      Accelerometer




The EvalboardTiny features a LIS2DW12 accelerometer with an SPI slave interface, enabling
the development of accelerometer-based applications. Accelerometer data is retrieved via
the ColibryNPU SPIM0 peripheral. The accelerometer's power supply is controlled via
DIO_20.


          4.2.15      EXT_VBAT Monitoring
The voltage level of the EXT_VBAT source can be indirectly measured by the nRF52832
through its P0.28 GPIO. To enable monitoring:
    ●   Configure P0.27 as an output low.
    ●   Enable the internal ADC with P0.28 as the analog input.
The voltage at P0.28 must not exceed VDDIO (1.8V). R32 and R33 resistors are configured as
a voltage divider, allowing to measure Vmeas = (VBAT / 3.2) at P0.28 input. To enable the
monitoring, P0.27 must be driven high. Otherwise, VBAT will be disconnected from the
voltage divider.




17/32                       ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 18 / 32>
                                                                   Hardware Description




          4.2.16    Camera Board Connector
The camera board connector layout is shown in the figure below, with the same orientation
as in Figure 3. A daughter camera board, such as the Arducam B0461 with an HM0360
image sensor, can be connected here.


                             Pin1 0.9V voltage supply
                             Pin2 Camera MCLK_SEL
                             Pin3 GND
                             Pin4 1.8V voltage supply
                             Pin5 INT_VBAT supply
                             Pin6 GND
                             Pin7 I2C SCL
                             Pin8 I2C SDA
                             Pin9 Camera VSYNC
                             Pin10 Camera HSYNC
                             Pin11 Camera PCLK
                             Pin12 Camera MCLK
                             Pin13 Camera DATA7
                             Pin14 Camera DATA6
                             Pin15 Camera DATA5
                             Pin16 Camera DATA4
                             Pin17 Camera DATA3
                             Pin18 Camera DATA2
                             Pin19 Camera DATA1
                             Pin20 Camera DATA0
                             Pin21 Camera PDWN_IRQ
                             Pin22 Unconnected
                             Pin23 Unconnected
                             Pin24 Unconnected
4.3 ColibryProbe Description
The ColibryProbe establishes a link between the EvalboardTiny and an external host. It
supports the following features:

    ●   JTAG programming and/or debugger for ColibryNPU


18/32                      ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 19 / 32>
                                                                                Hardware Description


      ●   SWD programming and/or debugger for ARM-based CPU targets
      ●   Flash SPI direct programming
      ●   ColibryNPU SPI interface (master SPI on the probe side)
      ●   UART RX/TX with the ColibryNPU
      ●   UART TX with the nRF chip (TX from the nRF)
      ●   3 general purpose I/Os
      ●   Reset and power of the daughter board. The VBAT signal is the 3.3V power source to
          the daughter board


 The bottom side of the ColibryProbe includes:

      ●   An FT4232HPQ USB bridge for communication between the EvalboardTiny and a host
          PC.
      ●   Level-shifters to adapt the signals between the 1.8V domain (ColibryNPU VDDIO) and
          the 3.3V domain on the FT4232HPQ side.
      ●   Multiplexers to select which signals should be provided to the probe connectors.


 The top side is shown in Figure 4.




Figure 4: ColibryProbe top side



     Main Components and Features of the ColibryProbe

  Component                Description                                                    Reference

  Probe connector          Connector at 1.27mm pitch for EvalboardTiny link               J2

  Function selector        Switch to select between JTAG or SWD interface                 S3

  Power button             Disconnects power from VBAT, removing power to the             S2



 19/32                        ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 20 / 32>
                                                                                    Hardware Description



 Component                Description                                                         Reference

                          EvalboardTiny when pressed

                          Applies an active-low reset to the EvalboardTiny (SYS_RESETN)
 Reset button                                                                                 S1
                          when pressed

                          Indicates if the 3.3V power supply is available for the board
 Power LED                                                                                    D3
                          functions

 VBAT jumper              Provides the 3.3V to VBAT when closed with a jumper                 J1

 USB-C connector          USB-C cable connector to a host PC                                  J5

 Pin header               Provides access to interface signals                                J4


          4.3.1        Probe Connector
This is the programming connector to which the EvalboardTiny should be plugged when it
needs to be programmed or debugged. An SPI interface also allows direct communication
with the ColibryNPU.


          4.3.2        Function Selector Switch
This switch can be positioned to JTAG or SWD, depending on which MCU needs to be
programmed or debugged. The ColibryNPU requires the JTAG signals whereas the nRF52832
programming is based on the SWD protocol. The switch position can be changed dynamically
without needing powering down the probe. v


          4.3.3        Power and Reset Buttons
The power button allows to disconnect the 3.3V (VBAT) power to the daughter board when
pressed. This generates a power-on-reset of the board connected through the probe
connector.
The reset button, when pressed, generates a reset on the system reset (pin 20) of the probe
connector. This reset is active low.


          4.3.4        Power LED
This LED indicates that a valid 3.3V power source is available on the probe, typically when the
USB-C cable is connected and providing sufficient power.


          4.3.5        VBAT Jumper
Place a jumper on the 2-pin header J1 (2.54mm pitch) to provide a 3.3V power source to the
EvalboardTiny. If the board is already powered by a battery, remove the jumper from J1 to
avoid power sources conflict.

For power measurement purposes, an ammeter can be connected to J1 instead of placing
the jumper.


20/32                        ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 21 / 32>
                                                                       Hardware Description



         4.3.6      Pin Header
This 2x5 header (not mounted by default) provides easy access to interface signals.

                                            Pin1 GND
                                            Pin2 3.3V power source
                                            Pin3 Colibry SPIS SCK or Flash SPI SCK
                                            Pin4 Colibry SPIS MOSI or Flash SPI MOSI
                                            Pin5 Colibry SPIS MISO or Flash SPI MISO
                                            Pin6 Colibry SPIS CSN or Flash SPI CSN
                                            Pin7 1.8V power source
                                            Pin8 JTAG TDO or SWD IO
                                            Pin9 JTAG TDI or SWD IO
                                            Pin10 JTAG TCK or SWD CLK




21/32                      ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 22 / 32>
                                                                       Programming Setup




5       Programming Setup
The flash memories available on the Evaluation Boards and embedded in the nRF52832
component of the EvalboardTiny enable the creation of embedded, autonomous, and
battery-powered systems. However, during application development, a connection to a host
PC is required to upload code into the memories. This section describes the hardware setup
based on the available Evaluation Board.



5.1 EvalboardXP
Connect a USB-C cable between the host PC and the EvalboardXP. Ensure that J17 has a
jumper placed on the "USB" selector to power the board via the USB 5V power supply.



5.2 EvalboardTiny
    1. Connect the EvalboardTiny to the ColibryProbe as shown in Figure 1.

    2. Connect a USB-C cable between the host PC and the ColibryProbe.



Ensure the Function selector switch and VBAT jumper are correctly set, as described in
Sections 4.3.2 and 4.3.5, depending on whether the programming target is the ColibryNPU
or the nRF52832.




22/32                     ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 23 / 32>
                                                                    Developing with ColibryNPU




6       Developing with ColibryNPU
The ColibryNPU SDK includes tools to easily load binary data into the ColibryNPU’s internal
RAM or the external flash memory, as well as C/C++ drivers and application code examples.



6.1 ColibryNPU SDK Description
The uncompressed ColibryNPU SDK archive should contain the following components:

    ●   colibry_drivers

        ◦   Each peripheral of the ColibryNPU can be configured and controlled through
            configuration, control, and status registers, as described in the ColibryNPU
            Product Specification. For easier programming, the SDK provides a C/C++ driver
            for each peripheral. These drivers offer higher-level functions while also allowing
            direct access to peripheral registers. Each peripheral has a pre-declared instance
            of its class, named colibry<PERIPHERAL>. For example, colibrySPIM1 is an
            instance of SpimDriver, corresponding to the SPIM1 peripheral.

        ◦   <peripheral>_driver.cpp – Implements methods for the <Peripheral>Driver
            class, which inherits from the <Peripheral>Hal class. The latter provides low-level
            register field access functions.

        ◦   <peripheral>_driver.hpp – Declares the <Peripheral>Driver class and related
            variables or constants.

        ◦   <peripheral>_hal.hpp – Provides getter and setter functions for each register
            field, avoiding direct register access.

        ◦   <peripheral>_mmap.h – Defines all registers and fields of the peripheral using C
            struct formatting. Fields can be accessed using syntax like:
               <colibryPERIPHERAL>->regmap-><register_name>.fields.<field_name>.

    ●   components_drivers

        ◦   This folder contains drivers and C/C++ code for various components available on
            the Evaluation boards:

        ◦   BOARDS – Headers with definitions for different Evaluation Boards. The
            colibry_board.* files define the ColibryBoard class, which simplifies resource
            and component usage. An initialization example is provided in its methods. The
            Evaluation Board model is selected at compile time via a Makefile option.

        ◦   I2CM_SW – A C++ driver for a software-implemented I2C master, allowing SDA and
            SCL control on any GPIO. The I2CM module of the ColibryNPU can also be used as
            an I2C master, but it does not support repeated start.

23/32                         ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 24 / 32>
                                                                  Developing with ColibryNPU


        ◦   EIS316 – A C++ driver for the E-peas EIS316 image sensor.

        ◦   HM0360 – A C++ driver for the Himax HM0360 image sensor.

        ◦   S25FLxx – A C++ driver for the Infineon external flash memory.

        ◦   TPS6286X – A C++ driver for the programmable DC-DC voltage regulator. It uses
            the I2CM_SW component.

        ◦   SPIS_PROTOCOL – An implementation of the SPI slave (SPIS) for the ColibryNPU,
            supporting a lightweight command/response protocol for accessing internal data.

        ◦   NRF52832 – Contains a partial version of the Nordic nRF5 SDK (v13.1.0), including
            component and documentation subfolders. It can be used to develop applications
            for the nRF52832 on the EvalboardTiny.

   ●    riscv_runtime

        ◦   chip – C header files related to the ColibryNPU chip, including peripheral address
            mapping, interrupt vectors, default handlers, and GPIO alternate functions. Two
            linker scripts are provided as examples: link_flash_boot.ld (for direct execution
            from external flash) and link.ld (for general use).

        ◦   kernel – C header files for the RISC-V core and low-level macro definitions for
            code optimization. The minimal boot code in assembly is also included.

        ◦   lib – A minimal and partial libc implementation, including printf() and I/O
            functions.

        ◦   tools/bin_loader – The colibry-flashtool.py script writes binary files (raw data)
            into the external flash memory on the Evaluation Boards. The starting address can
            be specified as a parameter. The colibry-sramtool.py script performs the same
            function but copies data into the ColibryNPU internal SRAM. It can also send a
            boot command or write a message to a reserved register.

        ◦   tools/openocd – OpenOCD specific build to support the FTDI FT4232HP chip and
            needed configuration files for both the ColibryNPU and nRF52832, used for
            programming and debugging.

        ◦   tools/colibry_interfaces – A Python API for accessing the ColibryNPU via JTAG,
            SPID, or SPIS interfaces. SPID and SPIS peripherals are detailed in the Product
            Specification. Note that SPIS mode requires a running SPIS_PROTOCOL module on
            the ColibryNPU.

        ◦   tools/add_bin_header.py – A script using during code compilation to generate the
            <app>_flash_copy.bin files.

        ◦   runtime.mk – A common Makefile for the ColibryNPU and its drivers, to be
            included when compiling application code.



24/32                         ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 25 / 32>
                                                                  Developing with ColibryNPU


   ●    examples

           Each subfolder includes a C/C++ application example and a Makefile. A
           README.txt file for each application provides a brief description of its purpose.



6.2 Programming the ColibryNPU
The provided examples are a good starting point for developing custom applications.
Initializing peripherals is required, and using the ColibryBoard C++ class is recommended
for beginners.

Each application should include a minimal Makefile that calls runtime.mk. To view available
parameters and targets, run make help.


          6.2.1       Compiling a Code
By default, an Evaluation Board model is selected. If your board differs, specify it using the
board parameter. For example: make all board=<your_board_identifier>. The board identifier
provided in the kit should be EVBXP or EVBTINY.

If compilation succeeds, the build/ folder will contain three .bin files, each corresponding
to a different Boot Mode. Use xxx_flash_copy.bin when the code must be stored in flash
but executed in SRAM.


          6.2.2       Loading and Running an Application
The colibry-flashtool.py and colibry-sramtool.py scripts load compiled code into
external flash or internal SRAM, respectively. The choice depends on the Boot Mode:

    ●   Flash Memory: Useful for autonomous systems that start from a power-off state. The
        Boot Mode should be b'10 ("Flash Copy"). The toflash target in the Makefile
        automates this process. For example: make all board=EVBTINY toflash
    ●   SRAM: Preferred for testing and debugging, as it is faster and does not require a
        power cycle. The Boot Mode should be b'00 or b'11 ("Wait Start"). After loading the
        code, the CPU must be started or restarted by writing to the SCU_CORE_CTRL register.
        The tosram target in the Makefile automates this process. For example: make all tosram

          6.2.3       Debugging your Application
    ●   UART Debugging: The printf() function outputs text over UART, which can be
        viewed on a virtual COM port (typically /dev/ttyUSB2). Use a serial monitor with a
        baud rate of 115200 (default setting). For example, with Picocom:
              picocom -b 115200 --imap lfcrlf /dev/ttyUSB2

    ●   JTAG Debugging: For advanced debugging, use GDB with OpenOCD. An IDE like VS
        Code is recommended for visual debugging. The .vscode/tasks.json file is pre-

25/32                        ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 26 / 32>
                                                                  Developing with ColibryNPU


          configured with default compiler options and paths, including the OpenOCD
          configuration.



6.3 Programming the nRF52832
The EvalboardTiny includes an nRF52832 microcontroller, which can be programmed by
loading compiled code into its internal flash. When no code is present in the nRF52832’s
internal flash, this component will consume a few milliamps of power. Consequently, even
if the application does not use the nRF52832, a minimal code that puts this microcontroller
into low-power mode should be loaded.

The video_numbers_ble example includes an NRF52832_APP subfolder, which contains an
example code that turns the system in low-power at boot. To compile and flash the code,
run: make all flash        in the NRF52832_APP subfolder. Alternately, to include the Nordic
SoftDevice (required for Bluetooth LE), run: make all flash_full. The SoftDevice is a pre-
compiled binary enabling Bluetooth LE API support. It only needs to be flashed once, so
make all flash_full should be used only for the first time. Subsequent flashes can use make all
flash for faster application code updates.

Debug and printf() functions becomes available on the nRF52832 when using the dbg=1
option, i.e.     using make all flash dbg=1. To view the output, open a serial monitor on
/dev/ttyUSB3 with a baudrate set to 115200.


Note1: To program the nRF52832 with the ColibryProbe, set the switch to SWD (see Section
4.3.2).

Note2: The EvalboardTiny boards are shipped with the nRF52832 preflashed with the factory
sample code, which can be found in video_numbers_ble/NRF52832_APP folder.



6.4 Loading Data into the External Flash
The external flash memory can store compiled RISC-V code, NNPA bitstreams, or application
data that must persist after power-off. This memory can be accessed externally (via a host
computer) or directly from the ColibryNPU.


            6.4.1          External Data Loading
The colibry-flashtool.py script writes binary files to the flash memory, with the starting
address specified as a parameter. This script is typically called by the Makefile in examples to
write compiled code or NNPA bitstreams.




26/32                           ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 27 / 32>
                                                                     Developing with ColibryNPU



          6.4.2       Access from ColibryNPU
The ColibryNPU can directly read and write to the external flash using the S25FLxx
component driver. For read-only access, the EXTF peripheral also provides direct memory
mapping of the flash content.


          6.4.3       Loading an NNPA bitstream
The colibry-flashtool.py script can be directly used to load a .nnpa file into the Flash
memory, at a given start address. In the examples provided, the Makefile facilitates this step.
Running make nnpa bitstream=<bs.nnpa> in an example folder, with bs.nnpa a compiled
bitstream, allows to load it at a predefined address of the Flash.



6.5 Neural Network Training and Quantization
A neural network model can be trained in any framework until it achieves the desired
accuracy. Initially, the model uses floating-point numbers to represent weights and
activations. To optimize for deployment, these values must be converted to 8-bit integers
through a process called quantization.

The NNPA compiler (see Section 6.6) can perform this quantization, or it can be done
independently before using the compiler.

    Quantization Methods

   1) Post-Training Quantization (PTQ)

    ●   Applied after training as a post-processing step.

    ●   Requires a calibration dataset (a small subset of the training data) to analyze input
        data dynamics across the network.

    ●   Drawback: May reduce model accuracy due to data loss during conversion.

   2) Quantization-Aware Training (QAT)

    ●   Recommended for preserving accuracy close to the floating-point model.

    ●   Simulates quantization effects during training.

    ●   Requires annotation of the original model to specify which layers to quantize and
        retraining the model with these annotations.



6.6 Compiling a Neural Network Model
Converting a neural network model into an NNPA bitstream (interpretable by the
ColibryNPU) requires the NNPA compiler.


27/32                        ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 28 / 32>
                                                                   Developing with ColibryNPU



            6.6.1        NNPA Compiler Package
The NNPA compiler is available as a Python package and can be installed via: pip install
colibry-npu

or with UV: uv add colibry-npu

It can be used as a Python package using an import, or the compiler can be run in
standalone mode using one of the following command:
          nnpa-compiler , or with UV

          uvx --python 3.12 --from colibry-npu nnpa-compiler



    Compatible Versions

 Python version               Compatible TensorFlow Versions

 3.10.x                       >=2.8 and <2.13

 3.11.x                       >=2.12 and <2.15

 3.12.x                       >=2.13 and <2.19



For the latest supported versions, refer to the colibry-npu package documentation.


            6.6.2        Supported model formats
The NNPA compiler supports multiple Neural Network representations:

    1) Native TensorFlow or PyTorch export format

     ●    Floating-point model
     ●    QAT methodology and annotated model

    2) ONNX format

Floating-point or pre-quantized model, exported from any framework.

    3) TensorflowLite (.tflite) uint8 model

In this case, the model is already converted to 8-bit integers.

When the floating-point model without quantization information is provided to the NNPA
compiler, an additional PTQ quantization step needs to be done. This is achieved by providing
a calibration dataset, as explained in the following section.


            6.6.3        Using PTQ methodology
TODO: describe how to provide the calibration dataset.


28/32                            ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 29 / 32>
                                                                 Developing with ColibryNPU



          6.6.4      Input data formatting
Depending on your application, the input data fed to the NNPA peripheral requires rescaling.
This is generally the case when these data do no fall into the range [0:255], as the accelerator
only supports 8-bit unsigned integer as input data. The input data loader allows data scaling
and offset if needed, as described in the document ColibryNPU Product Specification,
section 11.2.5.

The required scaling factor and offset may also be pre-calculated by the NNPA compiler and
added to the bitstream, provided that the -sfin and -offin parameters are correctly set.
The scaling factor and offset can further be read from the application code in the
RD_INPUT_SCALING and RD_INPUT_OFFSET registers and copied to INPUT_SCALING_FXP and
INPUT_OFFSET_INT register fields. This step is optional but may help the application to
format the input data correctly.


          6.6.5      Compilation Example
The      Visual   Wake      Word      (VWW)      Tensorflow      model     is   provided      in
example/mlperf_vww/TF_MODEL. This pre-trained and quantized model can be compiled
using:
 nnpa-compiler TF_MODEL/trained_model_qat/
If successful, a TF_MODEL/trained_model_qat.nnpa bitstream is generated. Additional
metrics can be obtained with the --verbose option.

To load an image or feature map into NNPA memory, with the help of the input loader or
going though the ISP path, a valid bitstream must be pre-loaded. The NNPA compiler can
generate a dummy bitstream for a specific input size using the --gen_dummy option. The
generated bitstream is valid but does not contain any weights.



6.7 Using the NNPA Emulator
The NNPA emulator is not currently included in the ColibryNPU Evaluation Kit.




29/32                       ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 30 / 32>
                                                                       TFLite Hello World Example




7       TFLite Hello World Example
This section provides a step-by-step guide to train, convert, and deploy a neural network on
the ColibryNPU target. For more details about the Hello World example, refer to the official
TensorFlow Lite Micro example.



7.1 Training the Model
The git repository or example folder should be locally downloaded from the URL given
above. To train the hello_world model from scratch:
    1. Navigate to the hello_world TensorFlow Lite example folder, referred to as
        <PATH_TO_TF_EXAMPLE>.

        This folder should be ./tflite-micro/tensorflow/lite/micro/examples/hello_world

    2. Run the following command:
        python train.py --save_tf_model --save_dir=./float_model --epochs=1000
    3. After training, the floating-point version of the neural network model is saved to
        ./float_model.

    4. Ensure the loss is below 1e-3; otherwise, retrain the model.



7.2 Converting to TFlite
TensorFlow Lite uses Post-Training Quantization (PTQ) to convert a floating-point model to
an int8 model. To quantize the model, run:
python quantization/ptq.py --source_model_dir=./float_model/ --target_dir=./quant_model

This generates a hello_world_int8.tflite file in the quant_model folder, which is the 8-bit
quantized version of the model.


7.3 Compiling the Model
The TFLite model must be compiled using the NNPA compiler to generate the bitstream for
the ColibryNPU target. Navigate to the examples/20_hello_world_tflite/ folder in the
SDK and run one of the following commands:

    ●   nnpa-compiler <PATH_TO_TF_EXAMPLE>/quant_model/ hello_world_int8.tflite

    ●   uvx --python 3.12 --from colibry-npu nnpa-compiler <PATH_TO_TF_EXAMPLE>/quant_model/
        hello_world_int8.tflite

The compiler generates a hello_world_int8.tflite.nnpa bitstream file alongside the
hello_world_int8.tflite file.



30/32                         ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 31 / 32>
                                                                    TFLite Hello World Example



7.4 Loading the Bitstream into Flash
Run the following command to load the bitstream:

make nnpa bitstream=<PATH_TO_TF_EXAMPLE>/quant_model/hello_world_int8.tflite.nnpa

After execution, unplug and replug the USB-C cable.



7.5 Running Inferences
   1. Use printf over UART to verify if the boot process completed successfully. Launch a
        terminal as described in Section 6.2.3 (e.g. using picocom):

        picocom -b 115200 --imap lfcrlf /dev/ttyUSB2

   2. Compile and flash the ColibryNPU application code:
        make clean all tosram board=<xxx>
        Replace <xxx> with the board identifier corresponding to your Evaluation Board. The
        terminal should display: “System ready. Waiting for SPI input.”

   3. The application waits for an external trigger to start inferences. Use the following
        command to run inferences on all possible input values (0 to 255), retrieve the results,
        and plot the predicted sine values against the reference:

        python hello_world_spis.py --plot

        Alternatively, use this command to predict a single sin(<f>) value:
        python hello_world_spis.py -v <f>
   4. This kind of figure should be obtained as a result:




31/32                          ColibryNPU Evaluation Kit User Manual
<PARSED TEXT FOR PAGE: 32 / 32>
                                                                  Frequently Asked Questions




8       Frequently Asked Questions

    4) When running a Makefile, or using colibry-flashtool.py or colibry-sramtool.py
       scripts, I get the “No FTDI SPI device found. Exiting.” message.

    Ensure that the ColibryProbe is correctly plugged to your computer with a USB-C cable.
    Also check that all installation steps have been followed.

    5) The JTAG does not seems to work (JTAG IDCODE: 0xFFFFFFFF or 0x00000000) or
       the Flash has incorrect manufacturer ID.

    Check that the EvalboardTiny is correctly plugged to the probe and that the Function
    selector switch is positioned to JTAG.

    Also, because the default boot mode is “Flash Copy”, the ColibryNPU may be in an
    unexpected state or the running code may have stopped the internal clocks. In this
    situation, it is recommended to reset the microcontroller in “Wait Start”:
         •   Press and maintain the PWR_OFF button of the ColibryProbe
         •   Press and maintain the User Button of the EvalboardTiny
         •   Release the PWR_OFF button
         •   Release the User Button
    Then try again using the JTAG or Flash access.

    6) When plugged, the EvalboardTiny red LED is flashing.

This is because the boards are shipped with a preflashed code, corresponding to the
example “10_uart_printf_blink”.




32/32                       ColibryNPU Evaluation Kit User Manual
