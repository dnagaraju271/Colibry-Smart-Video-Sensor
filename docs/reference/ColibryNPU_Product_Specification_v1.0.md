# ColibryNPU Product Specification v1.0

> Searchable text extraction from the user-provided PDF reference. Page labels and extracted text are retained where available; figures and page layout may not be represented. Consult the original PDF for diagrams and exact visual formatting.

---

<PARSED TEXT FOR PAGE: 1 / 159>
ColibryNPU Product Specification v1.0
    Features and characteristics

    Ultra-low power Neural Network Accelerator             FFT accelerator
       Fully programmable CNN configuration                   Fast Fourier Transform on 16-bit data
       Optimized for depthwise convolutions                   Fully automated DMA and processing
       640 kB Embedded SRAM for model and data
       9.6 GOPs at 300 MHz                                 Timers
       2.1 mA continuous inferences (0.8V, 40 MHz)            2x fast clock timers and interrupts
                                                               1x RTC low-power timer
    RISC-V 32-bit processor with FPU                          Watchdog function with system reset
       20 uA/MHz running from SRAM
       Up to 150 MHz in worst conditions                   Communication interfaces
       JTAG debugging enabled                                44 Digital I/Os, multiplexed function
                                                              2x SPI masters
    Internal system memory                                   1x SPI master Flash
       128 kB of SRAM for code and data                      1x SPI slave + SPI debug
       32 individual buffers of 4096 byes with power         1x I2C master
        management                                            1x UART
                                                              JTAG
    Flexible booting modes
       Load code into SRAM from SPI or JTAG                Package
       Autonomous code copy from external flash               QFN56 package,
       Direct fetch and execute from external flash            7 x 7 mm

    Clocks sources
       External HF clock input up to 70 MHz or             Applications and benefits
       4 MHz crystal oscillator
       External LF 32 kHz clock input or crystal           TinyML benefits
       Crystal-less internal HF oscillator                    Low-latency
       Internal PLL clock source up to 300 MHz                Frugal AI inferences
                                                                Low-power, energy autonomy
     Power management
                                                             

                                                                Increased privacy of sensors data
       RTC running on LF clock with retention
                                                             
     
                                                                Reduced information bandwith
       Sleep with retention, GPIO wake-up
                                                             
     
      Per-peripheral clocks enable                         Sensors information extraction
      Individual memory buffers shutdown                      Objects detection in images
      0.8V to 1.2V core power with DVFS                       Video analysis automation
      1.8V, up to 3.3V tolerant IO power                      Sounds and keywords recognition
                                                               Accelerometer patterns classification
    Image pre-processing accelerator
       8-12bits flexible parallel camera interface         Artificial Intelligence of Things (AIoT)
       Image cropping/resizing/ROI selection                  Industrial
       Bayer-RGB demosaicing                                  Home automation
       RGB to gray converter                                  Agriculture
                                                               Medical
                                                               Smart cities




                                                                                                        1
<PARSED TEXT FOR PAGE: 2 / 159>
Contents
1 Introduction.............................................................................................................................................5
    1.1 Naming convention...................................................................................................................................... 5
    1.2 Peripherals registers description............................................................................................................... 5
2 Block diagram..........................................................................................................................................6
3 QFN package and pinout......................................................................................................................7
    3.1 QFN56 pinout............................................................................................................................................... 7
    3.2 Pins description............................................................................................................................................ 8
4 Global specifications...........................................................................................................................10
    4.1 Absolute maximum ratings....................................................................................................................... 10
    4.2 Recommended operating conditions..................................................................................................... 10
    4.3 DVFS power optimization........................................................................................................................ 10
5 System overview..................................................................................................................................12
    5.1 RISC-V core................................................................................................................................................. 12
    5.2 Internal communication bus.................................................................................................................... 14
    5.3 Memory map............................................................................................................................................... 14
    5.4 SRAM architecture..................................................................................................................................... 24
    5.5 Boot modes................................................................................................................................................. 25
    5.6 System clocks............................................................................................................................................. 27
    5.7 Resets management.................................................................................................................................. 33
6 System control unit (SCU)..................................................................................................................35
    6.1 Functional description.............................................................................................................................. 35
    6.2 Registers...................................................................................................................................................... 37
7 I/O ports controller (PORT)...............................................................................................................46
    7.1 Functional description.............................................................................................................................. 46
    7.2 Registers...................................................................................................................................................... 48
    7.3 Electrical and timing specification.......................................................................................................... 52
8 Direct memory access controller (DMA)........................................................................................54
    8.1 Functional description.............................................................................................................................. 54
    8.2 Registers...................................................................................................................................................... 56
9 Camera interface (CAMIF).................................................................................................................60
    9.1 Functional description.............................................................................................................................. 60
    9.2 Registers...................................................................................................................................................... 66
10 Image signal processor (ISP)............................................................................................................71
    10.1 Functional description............................................................................................................................ 71
    10.2 Registers.................................................................................................................................................... 75
11 Neural Network Accelerator (NNPA)............................................................................................81
    11.1 Neural network inference flow............................................................................................................. 81
    11.2 Functional description............................................................................................................................ 81
    11.3 NNPA specifications summary.............................................................................................................. 94
    11.4 Registers.................................................................................................................................................... 95
12 Fast Fourier Transform (FFT)........................................................................................................107
    12.1 Functional description.......................................................................................................................... 107
    12.2 Registers................................................................................................................................................. 111
13 Timers (TIMERx)..............................................................................................................................114
    13.1 Functional description.......................................................................................................................... 114
    13.2 Registers................................................................................................................................................. 115
14 Real-time counter and watchdog (RTC).....................................................................................118
    14.1 Functional description.......................................................................................................................... 118


                                                                                                                                                                 2
<PARSED TEXT FOR PAGE: 3 / 159>
    14.2 Registers................................................................................................................................................. 120
15 Universal asynchronous RX/TX (UART)....................................................................................124
    15.1 Functional description.......................................................................................................................... 124
    15.2 Registers................................................................................................................................................. 127
16 I2C master (I2CM)...........................................................................................................................130
    16.1 Functional description.......................................................................................................................... 130
    16.2 Registers................................................................................................................................................. 133
17 SPI slave (SPIS).................................................................................................................................136
    17.1 Functional description.......................................................................................................................... 136
    17.2 Registers................................................................................................................................................. 139
18 SPI master / I2S (SPIMx)................................................................................................................143
    18.1 Functional description.......................................................................................................................... 143
    18.2 Registers................................................................................................................................................. 148
19 SPI master flash (SPIMF)...............................................................................................................152
    19.1 Functional description.......................................................................................................................... 152
    19.2 Registers................................................................................................................................................. 153
20 SPI direct access (SPID).................................................................................................................154
    20.1 Functional description.......................................................................................................................... 154
    20.2 Registers................................................................................................................................................. 157
21 Debug module (DBGM).................................................................................................................158
22 Package information.......................................................................................................................159




                                                                                                                                                              3
<PARSED TEXT FOR PAGE: 4 / 159>
Document history
Version   Date         Description

1.0       25/06/2026   First release with EvalboardTiny kit




                                                              4
<PARSED TEXT FOR PAGE: 5 / 159>
                                                                                               Introduction




1            Introduction
This Product Specification document describes the functionalities and programming model of the
ColibryNPU System-On-Chip (SoC), which incorporates an ultra-low power Neural Network
Processing Accelerator (NNPA), a RISC-V 32-bit microcontroller core, and a number of
communication and processing peripherals.

The different sections of the document present the general system description, the memory
mapping, and peripherals. For each peripheral, the corresponding section includes the following
information:

        ●    The functionality and usage of the peripheral
        ●    The registers associated with the peripheral
        ●    Where relevant, timing and electrical specifications


1.1 Naming convention
Each peripheral is identified by a unique name in capitalized letters. If multiple instances of the
same peripheral are available in the chip, a number suffix is added to the name, e.g. TIMER0 and
TIMER1.

Registers and register fields are named in capitalized letter with a dedicated font, e.g.
REGISTER_FIELD.


1.2 Peripherals registers description
The register bank description included in each peripheral section indicates the relative address of
each register with respect to the peripheral base address. The absolute register address is obtained
by adding the peripheral base address specified in Table 6 to the relative address.

Register fields that are not described within a register are assumed to be 'Reserved'. They are read
as '0' and writing them has no effect.

Each register has an access type defined as follows:

    ●       RO (Read Only) The field can only be read, writing them has no effect.

    ●       RW (Read/Write) The field can be read and written.

    ●       RWC (Read/Write Clear) The field can be read and written but write effect is to clear bits
            where the written value bits are at '1'. The '0' positions in the written value do not modify the
            corresponding bits in the field.

    ●       RWS (Read/Write Set) The field can be read and written but write effect is to set bits where
            the written value bits are at '1'. The '0' positions in the written value do not modify the
            corresponding bits in the field.




                                                                                                       5
<PARSED TEXT FOR PAGE: 6 / 159>
                                                                                   Block diagram




 2      Block diagram
 The following figure illustrates the overall system and the peripherals. The SoC is built around a
 RISC-V CPU core, a fast system RAM (SRAM) and a flexible AXI bus connecting the different
 modules. A simpler APB bus connects the periperals to the rest of the system. Each block is
 described in details in the following sections.

 The “M” letter in the figure indicates the ports which are master with respect to the AXI bus,
 meaning that they can initiate data transfers towards memory or other peripherals.




Figure 1: ColibryNPU block diagram



                                                                                              6
<PARSED TEXT FOR PAGE: 7 / 159>
                                                                      QFN package and pinout




3      QFN package and pinout


3.1 QFN56 pinout
This microcontroller is available in a QFN56 package with 44 Digital I/O, in addition to power and
fixed-function pins. The exposed pad in the center of the chip connects all ground (GND) power
references together.




    Figure 2: QFN56 pinout




                                                                                             7
<PARSED TEXT FOR PAGE: 8 / 159>
                                                                                  QFN package and pinout



3.2 Pins description
The table below details the functions available for each pin of the chip. All DIO.xx pins are Digital
Input/Output (I/O) pins having a General Purpose I/O (GPIO) capability. Each pin features an
activatable input buffer, output buffer and internal pull-up or pull-down resistors, as explained in
section 7. The “Dir” column indicates the default function of each I/O after the chip reset or power-
up. It can be a power (PWR) pin, input (IN), output (OUT) or high-impedance (HZ).

All DIO pins can be configured to an alternate function, instead of using them as a GPIO. An
alternate function connects the I/O to the corresponding peripheral interface. The cells with blue
shading in the table indicate the pin default function after the chip reset or power-up. If a DIO pin
has no shading, it is configured as a high-impedance GPIO by default. This configuration can be
modified through the PORT peripheral by modifying its registers.

It should be noted that the same alternate function can be mapped to several DIOs. For example,
the clock of SPI master 1 (SPIM1_SCK) can be used with DIO.09 alternate 2, DIO.14 alternate 2 or
DIO.19 alternate 3. This leaves the flexibility for selecting optional features and map them to some
DIOs.

Pin   Name      Description                            Alternate1    Alternate2       Alternate3    Dir
1     VDD       Core PLL voltage supply                      -             -                -       PWR
2     DIO.00    Digital I/O, boot mode 0         (d)   BOOT_MODE_0   CAM_DATA_8       CAM_DATA_10   IN
3     DIO.01    Digital I/O, boot mode 1         (d)   BOOT_MODE_1   CAM_DATA_9       CAM_DATA_11   IN
4     DIO.02    Digital I/O, I2C master SDA            I2CM_SDA      CAM_DATA_8       CLK_OUT_0     HZ
5     DIO.03    Digital I/O, I2C master SCL            I2CM_SCL      CAM_DATA_9       CLK_OUT_1     HZ
6     VDDIO     I/O voltage supply                           -             -                -       PWR
7     DIO.04    Digital I/O, SPIMF clock               SPIMF_SCK     CLK_OUT_0              -       OUT
8     DIO.05    Digital I/O, SPIMF chip select         SPIMF_CSN_0   SPIM1_CSN_1            -       OUT
9     DIO.06    Digital I/O, SPIMF MOSI                SPIMF_MOSI    SPIM0_CSN_2      CLK_OUT_0     OUT
10    DIO.07    Digital I/O, SPIMF MISO                SPIMF_MISO    SPIM0_CSN_3      CLK_OUT_1     IN
11    DIO.08    Digital I/O, SPIMF reset out           SPIMF_RSTN    SPIM1_CSN_2            -       OUT
12    DIO.09    Digital I/O, JTAG clock                JTAG_TCK      SPIM1_SCK        CLK_OUT_0     IN
13    DIO.10    Digital I/O, JTAG test mode            JTAG_TMS      SPIM1_CSN_0      CLK_OUT_1     IN
14    DIO.11    Digital I/O, JTAG test reset           JTAG_TRSTN    SPIM1_MOSI             -       IN
15    DIO.12    Digital I/O, JTAG test data in         JTAG_TDI      SPIM1_MISO             -       IN
16    DIO.13    Digital I/O, JTAG test data out        JTAG_TDO      CLK_OUT_1              -       OUT
17    DIO.14    Digital I/O, SPIS clock                SPIS_SCK      SPIM1_SCK              -       HZ
18    DIO.15    Digital I/O, SPIS chip select          SPIS_CSN      SPIM1_CSN_0            -       HZ
19    DIO.16    Digital I/O, SPIS MOSI                 SPIS_MOSI     SPIM1_MOSI             -       HZ
20    DIO.17    Digital I/O, SPIS MISO                 SPIS_MISO     SPIM1_MISO             -       HZ
21    VDDIO     I/O voltage supply                           -             -                -       PWR
22    DIO.18    Digital I/O, SPID clock                SPID_SCK      SPIM0_CSN_2      SPIM1_MISO    IN
23    DIO.19    Digital I/O, SPID chip select          SPID_CSN      CAM_DATA_8       SPIM1_SCK     IN



                                                                                                     8
<PARSED TEXT FOR PAGE: 9 / 159>
                                                                                QFN package and pinout


Pin   Name      Description                          Alternate1    Alternate2       Alternate3    Dir
24    DIO.20    Digital I/O, SPID MOSI               SPID_MOSI     CAM_DATA_9       SPIM1_CSN_0   IN
25    DIO.21    Digital I/O, SPID MISO               SPID_MISO     SPIM0_CSN_3      SPIM1_MOSI    OUT
26    DIO.22    Digital I/O, SPIM0 clock             SPIM0_SCK     CLK_OUT_0        PLL_CP_UP     HZ
27    DIO.23    Digital I/O, SPIM0 chip select 0     SPIM0_CSN_0   CLK_OUT_1        PLL_CP_DOWN   HZ
28    DIO.24    Digital I/O, SPIM0 chip select 1     SPIM0_CSN_1   SPIMF_CSN_1            -       HZ
29    DIO.25    Digital I/O, SPIM0 MOSI              SPIM0_MOSI    SPIMF_CSN_2      CAM_DATA_10   HZ
30    DIO.26    Digital I/O, SPIM0 MISO              SPIM0_MISO    SPIMF_CSN_3      CAM_DATA_11   HZ
31    DIO.27    Digital I/O, UART RX                 UART_RX       CAM_DATA_8       CAM_DATA_10   HZ
32    DIO.28    Digital I/O, UART TX                 UART_TX       CAM_DATA_9       CAM_DATA_11   HZ
33    DIO.29    Digital I/O, Camera clock            CAM_PCLK      SPIM1_SCK              -       HZ
34    DIO.30    Digital I/O, Camera HSYNC            CAM_HSYNC     SPIM1_CSN_0            -       HZ
35    VDDIO     I/O voltage supply                         -             -                -       PWR
36    VDD       Core voltage supply                        -             -                -       PWR
37    DIO.31    Digital I/O, Camera VSYNC     (d)    CAM_VSYNC     SPIM1_CSN_1      EXTCLK_SEL    IN
38    DIO.32    Digital I/O, Camera data 0           CAM_DATA_0    SPIM1_CSN_2            -       HZ
39    DIO.33    Digital I/O, Camera data 1           CAM_DATA_1    SPIM1_CSN_3            -       HZ
40    DIO.34    Digital I/O, Camera data 2           CAM_DATA_2    SPIM1_MOSI             -       HZ
41    DIO.35    Digital I/O, Camera data 3           CAM_DATA_3    SPIM1_MISO             -       HZ
42    DIO.36    Digital I/O, Camera data 4           CAM_DATA_4    CLK_OUT_0        SPIM1_MISO    HZ
43    DIO.37    Digital I/O, Camera data 5           CAM_DATA_5    SPIMF_CSN_1      SPIM1_SCK     HZ
44    DIO.38    Digital I/O, Camera data 6           CAM_DATA_6    SPIMF_CSN_2      SPIM1_CSN_0   HZ
45    DIO.39    Digital I/O, Camera data 7           CAM_DATA_7    SPIMF_CSN_3      SPIM1_MOSI    HZ
46    DIO.40    Digital I/O, SPIM1 clock             SPIM1_SCK     CAM_DATA_8       CLK_OUT_0     HZ
47    DIO.41    Digital I/O, SPIM1 chip select 0     SPIM1_CSN_0   CAM_DATA_9       CLK_OUT_1     HZ
48    DIO.42    Digital I/O, SPIM1 MOSI              SPIM1_MOSI    CAM_DATA_10            -       HZ
49    DIO.43    Digital I/O, SPIM1 MISO              SPIM1_MISO    CAM_DATA_11            -       HZ
50    NRESET    Chip reset, active low         (u)         -             -                -       IN
51    TM        Test pin, left unconnected     (d)         -             -                -       IN
52    LFXO1     Low-frequency oscillator                   -             -                -       IN
53    LFXO0     Low-frequency oscillator                   -             -                -       IN
54    HFXO1     High-frequency oscillator                  -             -                -       IN
55    HFXO0     High-frequency oscillator                  -             -                -       IN
56    VDDIO     I/O voltage supply                         -             -                -       PWR

Table 1: QFN56 pins assignment and DIO functions

(d) Internal pull-down resistor enabled by default for these pins
(u) Internal pull-up resistor enabled by default for these pins

Note: While under a chip reset (NRESET is low), all DIO are in high-impedance mode, except for
DIO.00, DIO.01 and DIO.31 which have an internal pull-down.


                                                                                                   9
<PARSED TEXT FOR PAGE: 10 / 159>
                                                                                     Global specifications




4         Global specifications

4.1 Absolute maximum ratings
The table below indicates the maximum limits beyond which the chip can be permanently damaged
in case of prolonged exposure.

 Symbol       Parameter                                                       Min.        Max.      Unit

 VDD          Core supply voltage                                             -0.3        1.3       V

 GND          Ground reference                                                0           0         V

 VDDIO        I/O supply voltage                                              -0.3        4.0       V

 VIO_IN       I/O input pin voltage                                           -0.3        4.0       V

 I_IO_O       Output current sunk by any I/O                                  0           15        mA

 T_STORE      Storage temperature                                             -40         +125      °C

 ESD_HBM      Electrostatic discharge voltage (human body model)                          2000      V

 ESD_CDM      Electrostatic discharge voltage (charged device model)                      500       V

Table 2: Absolute maximum ratings


4.2 Recommended operating conditions
The table below indicates the parameters for which the chip can be operated in normal conditions.

 Symbol       Parameter                                                Min.              Max.       Unit

 VDD          Core supply voltage                                      0.8               1.2        V

 VDDIO        I/O supply voltage                                       1.6               3.6        V

 I_IO_O       Output current sunk by any I/O                           0                 15         mA

 F_CPU        CPU core maximum frequency (best conditions, 1.2V)       1                 330        MHz

 F_NNPA       NNPA maximum frequency (best conditions, 1.2V)           1                 330        MHz

 F_APB        Peripherals maximum frequency (best conditions, 1.2V)    1                 165        MHz

 T_EXT        External temperature                                     -40               +85        °C

Table 3: Recommended operating conditions



4.3 DVFS power optimization
The ColibryNPU is designed to consume minimal energy for a given task and external conditions by
enabling dynamic voltage and frequency scaling (DVFS). It involves determining, for a given AI
application or task, the minimal clock frequency requested for the CPU core and/or the AI


                                                                                                    10
<PARSED TEXT FOR PAGE: 11 / 159>
                                                                                Global specifications


accelerator (NNPA). From these clocks settings, there is a minimal VDD voltage supply of the core
for which the chip remains functional while the power consumption is decreased to its safe
minimum. A software self-test can help find this minimal voltage while ensuring the functionality of
the system.

The table below provides some characterized operating points under various external conditions
and their corresponding power consumption from the VDD supply. The Fmax corresponds to the
maximum frequency measured for the NNPA on a typical neural network inference, while remaining
functional. The optimal operating point for a given application is not necessarily a point in this table
but can instead be found with a self-test algorithm or interpolating the needed operating point.

  Temperature (°C)     VDD supply (V)      NNPA Fmax (MHz)      VDD power (mW)

         25                  0.8

         25                  1.0

         25                  1.2

        -40                  0.8

        -40                  1.0

        -40                  1.2

         85                  0.8

         85                  1.0

         85                  1.2

Table 4: Characterized operating points




                                                                                                 11
<PARSED TEXT FOR PAGE: 12 / 159>
                                                                                  System overview




5       System overview

5.1 RISC-V core
The processor is a 4-stage, 32-bit, in-order RISC-V core compliant with the RISC-V instruction set
(rv32imc) with a hardware floating-point unit for efficient FP32 handling. The core implemented is
referred to as CV32E40P, which is an open-source implementation supported by the OpenHW
group. The detailed documentation and characteristics of the core can be found on this dedicated
website: https://cv32e40p.readthedocs.io/en/latest/intro.html.

As represented in Figure 1, the RISC-V core has separate instruction and data ports that can access
the memory and peripherals individually. In addition, the instruction port has a direct read access to
the SRAM, allowing both data and instruction fetch in the same clock cycle, as long as they are
located in separate SRAM buffers.

The core has internal Control and Status Registers (CSR) that are not described in this document.
Refer to the CV32E40P documentation for more information.


          5.1.1        Hardware configuration
The following optional extensions are available within the core and can be enabled with specific
C/C++ compiler flags:
    ●   Zfinx: Single-Precision Floating-Point using X registers, version 1.0
    ●   Xcorev: CORE-V ISA Extensions (excluding cv.elw), version 1.0

The MHP performance counters and PULP Cluster options are not supported.


          5.1.2        Exception and interrupts
The core implements trap handling for interrupts and exceptions according to the RISC-V Privileged
Specification, version 1.11. There are 16 interrupts and exceptions reserved to the core (indices 0 to
15) and 16 interrupts reserved to the chip peripherals (indices 16 to 31). The hardware does not
support interrupt nesting but this feature can be implemented in software by re-enabling interrupts
within an interrupt service routine (ISR) through the mstatus.MIE RISC-V bit.

When entering an interrupt/exception handler, the core sets the Machine Exception PC (mepc) CSR
to the current program counter and saves the mstatus.MIE content to mstatus.MPIE. All exceptions
cause the core to jump to the base address of the vector table in the Machine Trap-Vector (mtvec)
CSR. Interrupts are handled in either direct mode or vectored mode, depending on the value of
mtvec.MODE. In direct mode the core jumps to the base address of the vector table in the mtvec
CSR. In vectored mode the core jumps to the base address plus four times the interrupt ID. Upon
executing an MRET instruction, the core jumps to the program counter previously saved in the
mepc CSR and restores the mstatus.MIE value from mstatus.MPIE.




                                                                                               12
<PARSED TEXT FOR PAGE: 13 / 159>
                                                                              System overview


The interrupt controller automatically handles interrupts clearing when entering in the interrupt
handler. Consequently, no action is needed from the software to allow new occurrences of the
same interrupt and ISR call.

Note: The base address of the vector table must be aligned to 256 bytes.

 Index      Relative address   Description

 0          0x00000000         System reset (if defined in vector table)

 1          0x00000004         Instruction fault

 2          0x00000008         Illegal instruction (code not recognized)

 3          0x0000000C         Breakpoint

 4          Reserved

 5          0x00000014         Load fault

 6          Reserved

 7          0x0000001C         Store fault

 8          0x00000020         Ecall instruction in User mode

 9-10       Reserved

 11         0x0000002C         Ecall instruction in Machine mode

 12-15      Reserved

 16         0x00000040         SCU peripheral interrupt

 17         0x00000044         PORT peripheral interrupt

 18         0x00000048         SPIS peripheral interrupt

 19         0x0000004C         SPIM0 peripheral interrupt

 20         0x00000050         SPIM1 peripheral interrupt

 21         0x00000054         SPIMF peripheral interrupt

 22         0x00000058         UART peripheral interrupt

 23         0x0000005C         I2CM peripheral interrupt

 24         0x00000060         TIMER0 peripheral interrupt

 25         0x00000064         TIMER1 peripheral interrupt

 26         0x00000068         RTC peripheral interrupt

 27         0x0000006C         CAMIF peripheral interrupt

 28         0x00000070         ISP peripheral interrupt

 29         0x00000074         NNPA peripheral interrupt

 30         0x00000078         FFT peripheral interrupt

 31         0x0000007C         DMA controller interrupt

Table 5: Exception and interrupt mapping




                                                                                          13
<PARSED TEXT FOR PAGE: 14 / 159>
                                                                                  System overview



 5.2 Internal communication bus
The main internal interconnect is based on a 32-bit Advanced eXtensible Interface (AXI) bus. Most
peripherals can be accessed through an Advanced Peripheral Bus (APB) that has an independent
clock frequency setting, which allows for a reduction in power consumption when lower data
throughput is needed. All peripherals connected to the APB are necessarily slaves, meaning that
they cannot initiate data transfers. Conversely, some peripherals are directly connected to the AXI
bus and can act as masters (or initiators) of data transfers.

The following figure shows the connectivity matrix that specifies the different bus masters and
slaves as well as how they are interconnected. A master can initiate a transfer with a slave only if a
node marker is present for them in the matrix. As an example, the FFT peripheral cannot access the
APB_BRIDGE or the NNPA memory spaces.




Figure 3: Bus masters/slaves matrix


 5.3 Memory map
The ColibryNPU features a 32-bit address space, providing access to all resources of the SOC –
including memory spaces, peripheral registers, and the external flash controller. Data is stored using
Little Endian format, meaning that in a word of four bytes, the byte with the lowest address is
placed on the LSBs of the word.


          5.3.1        Memory map overview
The full memory map of the SOC is provided in Figure 4.

The list of peripherals and their base addresses are indicated in Table 6.


                                                                                               14
<PARSED TEXT FOR PAGE: 15 / 159>
                                                           System overview




  Figure 4: Full memory map


Peripheral   Base address   Description

SCU          0x20010000     System control unit

PORT         0x20011000     I/O ports controller

SPIS         0x20012000     SPI slave

SPIM0        0x20013000     SPI master 0

SPIM1        0x20014000     SPI master 1

SPIMF        0x20015000     Flash SPI master

UART         0x20016000     Universal Asynchronous RX/TX

I2CM         0x20017000     I2C master




                                                                    15
<PARSED TEXT FOR PAGE: 16 / 159>
                                                                                        System overview


 Peripheral   Base address   Description

 TIMER0       0x20018000     General-purpose timer 0

 TIMER1       0x20019000     General-purpose timer 1

 RTC          0x2001A000     Real-time counter

 CAMIF        0x2001E000     Camera interface

 ISP          0x2001F000     Image signal processor

 FFT          0x20020000     Fast-Fourier transform accelerator

 DMA          0x20021000     Direct memory access

Table 6: Peripherals mapping


          5.3.2        List of registers and memory spaces

The full registers map and memory spaces starting addresses are provided in the table below. Every
fields contained in a register is detailed in its corresponding peripheral section.

 Register Name                   Address Offset        Description
 SRAM_BANK                       0x10000000            SRAM memory space
 DBGM_BANK                       0x20001000            DBGM memory space
 SCU_HF_CLK_CFG                  0x20010000            High-frequency clock configuration
 SCU_AXI_CLK_CFG                 0x20010004            AXI clock configuration
 SCU_APB_CLK_CFG                 0x20010008            APB clock configuration
 SCU_ISP_CLK_CFG                 0x2001000C            ISP clock configuration
 SCU_NNPA_CLK_CFG                0x20010010            NNPA clock configuration
 SCU_PERIPH_CLK_EN               0x20010014            Blocks clocks enable
 SCU_PERIPH_RESET                0x20010018            Blocks clocks enable
 SCU_CLK_OUT0_CFG                0x2001001C            CLK_OUT_0 configuration
 SCU_CLK_OUT1_CFG                0x20010020            CLK_OUT_1 configuration
 SCU_SYSTEM_CTRL                 0x20010024            Global system control
 SCU_SYSTEM_CONFIG               0x20010028            Global system configuration
 SCU_CORE_CTRL                   0x20010030            CPU core control
 SCU_BOOT_ADDR                   0x20010034            Current code boot address
 SCU_BOOT_ADDR_OVW               0x20010038            Overwrite code boot address
 SCU_BOOT_ADDR_SET               0x2001003C            New boot address setting
 SCU_CORE_STATUS                 0x20010040            CPU core user status
 SCU_CORE_MSG_0                  0x20010050            CPU core message
 SCU_CORE_MSG_1                  0x20010054            CPU core message
 SCU_CORE_MSG_2                  0x20010058            CPU core message
 SCU_CORE_MSG_3                  0x2001005C            CPU core message
 SCU_EXT_MSG_0                   0x20010070            External message
 SCU_EXT_MSG_1                   0x20010074            External message
 SCU_EXT_MSG_2                   0x20010078            External message
 SCU_EXT_MSG_3                   0x2001007C            External message



                                                                                                 16
<PARSED TEXT FOR PAGE: 17 / 159>
                                                                          System overview


SCU_GENERATE_IRQ        0x20010090   Interrupt generation
SCU_WR_PROT_START       0x20010094   Write protection start address
SCU_WR_PROT_END         0x20010098   Write protection end address
SRAM_CUTS_CTRL          0x2001009C   SRAM cuts control pins
SRAM_CUTS_PD_CTRL       0x200100A0   SRAM cuts power down
SRAM_FORCE_CLK_EN       0x200100A4   SRAM cuts clock enable force
SRAM_MBIST_CTRL         0x200100B0   SRAM SRAM BIST control
SRAM_MBIST_STATUS       0x200100B4   SRAM SRAM BIST status
SRAM_MBIST_FAIL         0x200100B8   SRAM BIST fail
PLL_CONTROL             0x200100C0   PLL control
PLL_CAL_CONFIG          0x200100C4   PLL calibration configuration
PLL_CAL_BYPASS          0x200100C8   PLL calibration bypass
PLL_ANA_CONTROL         0x200100CC   PLL analog control
PLL_STATUS              0x200100D0   PLL status
CHIP_VERSION            0x200100F0   Chip hardware version
PORT_DIOA_IN            0x20011000   DIO 00 to 31 port read data
PORT_DIOB_IN            0x20011004   DIO 32 to 43 port read data
PORT_DIOA_OUT           0x20011010   DIO 00 to 31 port write data
PORT_DIOB_OUT           0x20011014   DIO 32 to 43 port write data
PORT_DIOA_INT_STATUS    0x20011020   DIO 00 to 31 port interrupt status
PORT_DIOB_INT_STATUS    0x20011024   DIO 32 to 43 port interrupt status
PORT_DIOA_INT_RISE_EN   0x20011030   DIO 00 to 31 port rise event enable
PORT_DIOB_INT_RISE_EN   0x20011034   DIO 32 to 43 port rise event enable
PORT_DIOA_INT_FALL_EN   0x20011040   DIO 00 to 31 port fall event enable
PORT_DIOB_INT_FALL_EN   0x20011044   DIO 32 to 43 port fall event enable
PORT_DIOA_INT_SET       0x20011050   DIO 00 to 31 port interrupt set
PORT_DIOB_INT_SET       0x20011054   DIO 32 to 43 port interrupt set
PORT_DIOA_INT_CLEAR     0x20011060   DIO 00 to 31 port interrupt clear
PORT_DIOB_INT_CLEAR     0x20011064   DIO 32 to 43 port interrupt clear
PORT_LFXO_CFG           0x20011080   LFXO ports configuration
PORT_HFXO_CFG           0x20011084   HFXO ports configuration
PORT_DIO_CFG_0          0x20011090   DIO ports configuration
PORT_DIO_CFG_1          0x20011094   DIO ports configuration
PORT_DIO_CFG_2          0x20011098   DIO ports configuration
PORT_DIO_CFG_3          0x2001109C   DIO ports configuration
PORT_DIO_CFG_4          0x200110A0   DIO ports configuration
PORT_DIO_CFG_5          0x200110A4   DIO ports configuration
PORT_DIO_CFG_6          0x200110A8   DIO ports configuration
PORT_DIO_CFG_7          0x200110AC   DIO ports configuration
PORT_DIO_CFG_8          0x200110B0   DIO ports configuration
PORT_DIO_CFG_9          0x200110B4   DIO ports configuration
PORT_DIO_CFG_10         0x200110B8   DIO ports configuration
PORT_DIO_CFG_11         0x200110BC   DIO ports configuration




                                                                                   17
<PARSED TEXT FOR PAGE: 18 / 159>
                                                               System overview


PORT_DIO_CFG_12         0x200110C0   DIO ports configuration
PORT_DIO_CFG_13         0x200110C4   DIO ports configuration
PORT_DIO_CFG_14         0x200110C8   DIO ports configuration
PORT_DIO_CFG_15         0x200110CC   DIO ports configuration
PORT_DIO_CFG_16         0x200110D0   DIO ports configuration
PORT_DIO_CFG_17         0x200110D4   DIO ports configuration
PORT_DIO_CFG_18         0x200110D8   DIO ports configuration
PORT_DIO_CFG_19         0x200110DC   DIO ports configuration
PORT_DIO_CFG_20         0x200110E0   DIO ports configuration
PORT_DIO_CFG_21         0x200110E4   DIO ports configuration
PORT_DIO_CFG_22         0x200110E8   DIO ports configuration
PORT_DIO_CFG_23         0x200110EC   DIO ports configuration
PORT_DIO_CFG_24         0x200110F0   DIO ports configuration
PORT_DIO_CFG_25         0x200110F4   DIO ports configuration
PORT_DIO_CFG_26         0x200110F8   DIO ports configuration
PORT_DIO_CFG_27         0x200110FC   DIO ports configuration
PORT_DIO_CFG_28         0x20011100   DIO ports configuration
PORT_DIO_CFG_29         0x20011104   DIO ports configuration
PORT_DIO_CFG_30         0x20011108   DIO ports configuration
PORT_DIO_CFG_31         0x2001110C   DIO ports configuration
PORT_DIO_CFG_32         0x20011110   DIO ports configuration
PORT_DIO_CFG_33         0x20011114   DIO ports configuration
PORT_DIO_CFG_34         0x20011118   DIO ports configuration
PORT_DIO_CFG_35         0x2001111C   DIO ports configuration
PORT_DIO_CFG_36         0x20011120   DIO ports configuration
PORT_DIO_CFG_37         0x20011124   DIO ports configuration
PORT_DIO_CFG_38         0x20011128   DIO ports configuration
PORT_DIO_CFG_39         0x2001112C   DIO ports configuration
PORT_DIO_CFG_40         0x20011130   DIO ports configuration
PORT_DIO_CFG_41         0x20011134   DIO ports configuration
PORT_DIO_CFG_42         0x20011138   DIO ports configuration
PORT_DIO_CFG_43         0x2001113C   DIO ports configuration
SPIS_VERSION            0x20012000   SPIS hardware version
SPIS_CTRL               0x20012004   SPI slave control
SPIS_LEN                0x20012008   Transaction length
SPIS_TX_DEFAULT_VALUE   0x2001200C   TX default data
SPIS_STATUS             0x20012010   SPI slave status
SPIS_STATUS_CLEAR       0x20012014   SPI slave status clear
SPIS_TX_PUSH            0x20012018   Transmit data
SPIS_RX_POP             0x2001201C   Receive data
SPIM0_VERSION           0x20013000   SPIM hardware version
SPIM0_CTRL              0x20013004   SPIM protocol control
SPIM0_LEN               0x20013008   Transaction length




                                                                        18
<PARSED TEXT FOR PAGE: 19 / 159>
                                                           System overview


SPIM0_TRIG_CTRL      0x2001300C   Trigger control
SPIM0_STATUS         0x20013010   SPIM status
SPIM0_STATUS_CLEAR   0x20013014   SPIM status clear
SPIM0_TX_PUSH        0x20013018   TX data pushed
SPIM0_RX_POP         0x2001301C   TX data popped
SPIM0_I2S_CTRL       0x20013020   I2S mode control
SPIM1_VERSION        0x20014000   SPIM hardware version
SPIM1_CTRL           0x20014004   SPIM protocol control
SPIM1_LEN            0x20014008   Transaction length
SPIM1_TRIG_CTRL      0x2001400C   Trigger control
SPIM1_STATUS         0x20014010   SPIM status
SPIM1_STATUS_CLEAR   0x20014014   SPIM status clear
SPIM1_TX_PUSH        0x20014018   TX data pushed
SPIM1_RX_POP         0x2001401C   TX data popped
SPIM1_I2S_CTRL       0x20014020   I2S mode control
SPIMF_VERSION        0x20015000   SPIM hardware version
SPIMF_CTRL           0x20015004   SPIM protocol control
SPIMF_LEN            0x20015008   Transaction length
SPIMF_TRIG_CTRL      0x2001500C   Trigger control
SPIMF_STATUS         0x20015010   SPIM status
SPIMF_STATUS_CLEAR   0x20015014   SPIM status clear
SPIMF_TX_PUSH        0x20015018   TX data pushed
SPIMF_RX_POP         0x2001501C   TX data popped
SPIMF_I2S_CTRL       0x20015020   I2S mode control
UART_VERSION         0x20016000   UART hardware version
UART_CTRL            0x20016004   UART control
UART_IRQ_CTRL        0x20016008   UART IRQ enable
UART_STATUS          0x2001600C   UART status
UART_CLEAR           0x20016010   UART status clear
UART_RX_POP          0x20016014   RX read data
UART_TX_PUSH         0x20016018   TX write data
I2CM_VERSION         0x20017000   I2CM hardware version
I2CM_CTRL            0x20017004   I2C master control
I2CM_STATUS          0x20017008   I2C master status
I2CM_CLEAR           0x2001700C   Events clear
I2CM_TX_PUSH         0x20017010   I2C master TX data
I2CM_RX_POP          0x20017014   I2C master RX data
TIMER0_VERSION       0x20018000   TIMER hardware version
TIMER0_CFG           0x20018004   Timer configuration
TIMER0_CTRL          0x20018008   Timer control
TIMER0_STATUS        0x2001800C   Timer status
TIMER0_L_VAL         0x20018010   Timer L current
TIMER0_H_VAL         0x20018014   Timer H current




                                                                    19
<PARSED TEXT FOR PAGE: 20 / 159>
                                                                         System overview


TIMER0_L_RELOAD          0x20018018   Timer L reload
TIMER0_H_RELOAD          0x2001801C   Timer H reload
TIMER1_VERSION           0x20019000   TIMER hardware version
TIMER1_CFG               0x20019004   Timer configuration
TIMER1_CTRL              0x20019008   Timer control
TIMER1_STATUS            0x2001900C   Timer status
TIMER1_L_VAL             0x20019010   Timer L current
TIMER1_H_VAL             0x20019014   Timer H current
TIMER1_L_RELOAD          0x20019018   Timer L reload
TIMER1_H_RELOAD          0x2001901C   Timer H reload
RTC_VERSION              0x2001A000   IP hardware version
RTC_CONTROL              0x2001A004   RTC control
RTC_CNT_VAL              0x2001A008   RTC counter current value
RTC_CNT_UPDATE_VAL       0x2001A00C   RTC counter reload value
RTC_WDT_VAL              0x2001A010   Watchdog current value
RTC_COMP_A               0x2001A020   Comparison value A
RTC_COMP_B               0x2001A024   Comparison value B
RTC_WDT_WARN             0x2001A030   Watchdog warning value
RTC_WDT_TIMEOUT          0x2001A034   Watchdog timeout value
RTC_INT_STATUS           0x2001A040   RTC interrupt status
RTC_INT_EN               0x2001A044   RTC interrupt enable
RTC_INT_SET              0x2001A048   RTC interrupt set
RTC_INT_CLEAR            0x2001A04C   RTC interrupt clear
CAMIF_PROTOCOL_CFG       0x2001E000   Input protocol configuration
CAMIF_IMG_SIZE_CFG       0x2001E004   Image size configuration
CAMIF_IMG_CURR_PTR       0x2001E008   Image pixel pointer
CAMIF_IMG_COUNT          0x2001E00C   Received images counter
CAMIF_IMG_COUNT_RST      0x2001E010   Image counter reset
CAMIF_PATGEN_CONTROL     0x2001E014   Pattern generator frame trigger
CAMIF_PATGEN_CFG         0x2001E018   Pattern generator configuration
CAMIF_PATGEN_RAMP_INIT   0x2001E01C   Pattern generator initial values
CAMIF_PATGEN_STATUS      0x2001E020   Pattern generator status
CAMIF_CDC_FIFO_BYPASS    0x2001E024   CDC FIFO bypass mode
CAMIF_CAM_DATA           0x2001E028   Data FIFO read
CAMIF_DATA_FIFO_STATUS   0x2001E02C   Data FIFO status
CAMIF_DATA_FIFO_THR      0x2001E030   Data FIFO threshold
CAMIF_INT_STATUS         0x2001E040   CAMIF interrupt status
CAMIF_INT_EN             0x2001E044   CAMIF interrupt enable
CAMIF_INT_SET            0x2001E048   CAMIF interrupt set
CAMIF_INT_CLEAR          0x2001E04C   CAMIF interrupt clear
ISP_CTRL                 0x2001F000   ISP global control
ISP_STATUS               0x2001F004   ISP global status
ISP_CROPPER_PSB_INFO     0x2001F010   CROPPER input frame information




                                                                                  20
<PARSED TEXT FOR PAGE: 21 / 159>
                                                                           System overview


ISP_CROPPER_ROI_TL       0x2001F014   ROI top-left corner coordinate
ISP_CROPPER_ROI_BR       0x2001F018   ROI bottom-right corner coordinate
ISP_B2RGB_PSB_INFO       0x2001F020   BAYER2RGB input frame information
ISP_B2RGB_RGGB_MODE      0x2001F024   BAYER2RGB pattern mode
ISP_RGB2GRAY_PSB_INFO    0x2001F028   RGB2GRAY input frame information
ISP_SCALER_PSB_INFO      0x2001F030   SCALER input frame information
ISP_SCALER_SCALING_CFG   0x2001F034   Image scaling factor configuration
ISP_SCALER_SIZE_OUT      0x2001F038   SCALER output image size
ISP_INT_STATUS           0x2001F040   ISP interrupt status
ISP_INT_EN               0x2001F044   ISP interrupt enable
ISP_INT_SET              0x2001F048   ISP interrupt set
ISP_INT_CLEAR            0x2001F04C   ISP interrupt clear
ISP_MBIST_CTRL           0x2001F060   ISP SRAM BIST control
ISP_MBIST_STATUS         0x2001F064   ISP SRAM BIST status
ISP_MBIST_FAIL           0x2001F068   SRAM BIST fail
FFT_CONTROL              0x20020000   FFT block control
FFT_STATUS               0x20020004   FFT block status
FFT_CONFIG               0x20020008   FFT configuration
FFT_DIN_START_ADDR       0x2002000C   Input data buffer start address
FFT_COEFF_START_ADDR     0x20020010   Coefficients buffer start address
FFT_DOUT_START_ADDR      0x20020014   Output data buffer start address
FFT_BUFFER_START_ADDR    0x20020018   Optional buffer start address
FFT_INT_STATUS           0x20020030   FFT interrupt status
FFT_INT_EN               0x20020034   FFT interrupt enable
FFT_INT_SET              0x20020038   FFT interrupt set
FFT_INT_CLEAR            0x2002003C   FFT interrupt clear
DMA_VERSION              0x20021000   DMA hardware version
DMA_INT_STATUS           0x20021004   DMA interrupt status
DMA_INT_EN               0x20021008   DMA interrupt enable
DMA_INT_SET              0x2002100C   DMA interrupt set
DMA_INT_CLEAR            0x20021010   DMA interrupt clear
DMA_CHAN_CTRL_0          0x20021020   Channel control
DMA_CHAN_CTRL_1          0x20021024   Channel control
DMA_CHAN_CTRL_2          0x20021028   Channel control
DMA_CHAN_CTRL_3          0x2002102C   Channel control
DMA_CHAN_CTRL_4          0x20021030   Channel control
DMA_CHAN_CTRL_5          0x20021034   Channel control
DMA_CHAN_CTRL_6          0x20021038   Channel control
DMA_CHAN_CTRL_7          0x2002103C   Channel control
DMA_LEN_0                0x20021040   Transfer length configuration
DMA_LEN_1                0x20021044   Transfer length configuration
DMA_LEN_2                0x20021048   Transfer length configuration
DMA_LEN_3                0x2002104C   Transfer length configuration




                                                                                    21
<PARSED TEXT FOR PAGE: 22 / 159>
                                                                           System overview


DMA_LEN_4                 0x20021050   Transfer length configuration
DMA_LEN_5                 0x20021054   Transfer length configuration
DMA_LEN_6                 0x20021058   Transfer length configuration
DMA_LEN_7                 0x2002105C   Transfer length configuration
DMA_SRC_0                 0x20021060   Source address configuration
DMA_SRC_1                 0x20021064   Source address configuration
DMA_SRC_2                 0x20021068   Source address configuration
DMA_SRC_3                 0x2002106C   Source address configuration
DMA_SRC_4                 0x20021070   Source address configuration
DMA_SRC_5                 0x20021074   Source address configuration
DMA_SRC_6                 0x20021078   Source address configuration
DMA_SRC_7                 0x2002107C   Source address configuration
DMA_DST_0                 0x20021080   Destination address configuration
DMA_DST_1                 0x20021084   Destination address configuration
DMA_DST_2                 0x20021088   Destination address configuration
DMA_DST_3                 0x2002108C   Destination address configuration
DMA_DST_4                 0x20021090   Destination address configuration
DMA_DST_5                 0x20021094   Destination address configuration
DMA_DST_6                 0x20021098   Destination address configuration
DMA_DST_7                 0x2002109C   Destination address configuration
DMA_SDL_0                 0x200210A0   Stride data length configuration
DMA_SDL_1                 0x200210A4   Stride data length configuration
DMA_SDL_2                 0x200210A8   Stride data length configuration
DMA_SDL_3                 0x200210AC   Stride data length configuration
DMA_SDL_4                 0x200210B0   Stride data length configuration
DMA_SDL_5                 0x200210B4   Stride data length configuration
DMA_SDL_6                 0x200210B8   Stride data length configuration
DMA_SDL_7                 0x200210BC   Stride data length configuration
DMA_SKL_0                 0x200210C0   Stride skip length configuration
DMA_SKL_1                 0x200210C4   Stride skip length configuration
DMA_SKL_2                 0x200210C8   Stride skip length configuration
DMA_SKL_3                 0x200210CC   Stride skip length configuration
DMA_SKL_4                 0x200210D0   Stride skip length configuration
DMA_SKL_5                 0x200210D4   Stride skip length configuration
DMA_SKL_6                 0x200210D8   Stride skip length configuration
DMA_SKL_7                 0x200210DC   Stride skip length configuration
EXTF_BANK                 0x20040000   EXTF memory space
NNPA_VERSION              0x30000000   NNPA hardware version
NNPA_TRIG_CONTROL         0x30000004   NNPA triggers control
NNPA_CONTROL              0x30000008   NNPA main control
NNPA_STATUS               0x3000000C   NNPA main status
NNPA_NB_DATA_BUFF         0x30000010   Buffers allocation
NNPA_CONFIG_START_MADDR   0x30000014   Configuration start address




                                                                                    22
<PARSED TEXT FOR PAGE: 23 / 159>
                                                                                System overview


 NNPA_INPUT_START_MADDR     0x30000018     Input data area start address
 NNPA_DATA_START_MADDR      0x3000001C     Activation data area start address
 NNPA_MEM_BUFF_PD0          0x30000020     Memory buffers power-down control
 NNPA_MEM_BUFF_PD1          0x30000024     Memory buffers power-down control
 NNPA_MEM_BUFF_SLEEP        0x30000028     Memory buffers sleep control
 NNPA_MEM_BUFF_CUTS_CTRL    0x3000002C     Memory buffers cuts control
 NNPA_INPUT_LOADER_DATA     0x30000030     Input loader data store
 NNPA_INPUT_LOADER_CFG      0x30000034     Input loader configuration
 NNPA_INPUT_LOADER_LEVEL    0x30000038     Input loader current level
 NNPA_INPUT_DATA_RESCALE    0x3000003C     Input data re-scaling
 NNPA_RD_MODEL_ID           0x30000040     Model identifier
 NNPA_RD_INPUT_SCALING      0x30000044     Input data scaling
 NNPA_RD_INPUT_OFFSET       0x30000048     Input data offset
 NNPA_RD_OUTPUT_SCALING     0x3000004C     Output data scaling
 NNPA_RD_OUTPUT_OFFSET      0x30000050     Output data offset
 NNPA_RD_MODEL_SIZE         0x30000054     Model configuration size
 NNPA_RD_INPUT_SIZE         0x30000058     Input feature map size
 NNPA_RD_DATA_SIZE          0x3000005C     Model inference needs
 NNPA_RD_IFMAP_INFO         0x30000060     Input feature map information
 NNPA_RD_IFMAP_SIZE         0x30000064     Input feature map size
 NNPA_RD_OFMAP_INFO         0x30000068     Output feature map information
 NNPA_RD_OFMAP_SIZE         0x3000006C     Output feature map size
 NNPA_DATA_END_MADDR        0x30000070     Activation data area end address
 NNPA_OFMAP_START_MADDR     0x30000074     Output feature map start address
 NNPA_INFERENCE_COUNTER     0x30000078     Inference counter
 NNPA_MEM_CONFLICT_INFO     0x3000007C     Memory conflict information
 NNPA_XTEN_IFMAP_MADDR      0x30000080     XTEN layer input fmap address
 NNPA_XTEN_OFMAP_MADDR      0x30000084     XTEN layer output fmap address
 NNPA_XTEN_WEIGHT_MADDR     0x30000088     XTEN layer weights address
 NNPA_CONFIG_CHECK_CRC      0x3000008C     Configuration check CRC
 NNPA_MBIST_CTRL            0x30000090     NNPA SRAM BIST control
 NNPA_MBIST_STATUS          0x30000094     NNPA SRAM BIST status
 NNPA_MBIST_FAIL0           0x30000098     SRAM BIST fail
 NNPA_MBIST_FAIL1           0x3000009C     SRAM BIST fail
 NNPA_MBIST_FAIL2           0x300000A0     SRAM BIST fail
 NNPA_INT_STATUS            0x300000B0     NNPA interrupt status
 NNPA_INT_EN                0x300000B4     NNPA interrupt enable
 NNPA_INT_SET               0x300000B8     NNPA interrupt set
 NNPA_INT_CLEAR             0x300000BC     NNPA interrupt clear
 NNPAMEM_BANK               0x30001000     NNPA memory space

Table 7: ColibryNPU registers and memory spaces mapping




                                                                                         23
<PARSED TEXT FOR PAGE: 24 / 159>
                                                                                 System overview



5.4 SRAM architecture
The SRAM block is an embedded static random-access memory (SRAM), organized into 32 buffers,
each 4 Kbyte in size, for a total of 128 Kbytes. It can be used to store both RISC-V program code
and data, as well as the call stack. The code may also be fetched from the external flash memory,
which leaves the full SRAM capacity available for the data but at the cost of performance and
power consumption degradation.




Figure 5: SRAM architecture diagram

The SRAM architecture is represented in Figure 5. Each buffer can be read or written through the
AXI bus. The “CPU instr” interface can exclusively read words from the buffers and serves the RISC-
V core by directly fetching its instructions in the code section. The example provided in the figure
shows that roughly 2.3 buffers are needed to store the program code and 1.7 buffers are needed for
the program data. Notably, the data section starts in a new buffer rather than not right after the
code. This design allows the “CPU instr” to read from the first three buffers while the AXI
concurrently accesses the rest of the buffer.


         5.4.1        Memories power control
Toave power consumption, unused SRAM buffers can be set into shutdown mode. While in
shutdown, an SRAM buffer cannot be used for read or write operations, and its content is lost.

Each bit of the SRAM_CUTS_PD_CTRL register allows to control the power mode of the
corresponding buffer. For example, if SRAM_CUTS_PD_CTRL[2] = 1 the buffer #2 will enter in power-
down. Consequently, SRAM addresses 0x10002000 to 0x10002FFF become unusable.


         5.4.2        Low-voltage management
The SRAM_CUTS_CTRL register allows to control read and write assists and timing margins, which
can be used to reach the lowest voltage supply (< 0.8 V) while remaining functional. The
configuration provided in this register is outside the scope of this document.



                                                                                             24
<PARSED TEXT FOR PAGE: 25 / 159>
                                                                                 System overview



         5.4.3        Memories testing
SRAM_MBIST_* registers are dedicated to memory Built-In Self-Test (BIST), which allows to detect
defective memory cells with predefined write-read sequences. They are normally not used in
application.



5.5 Boot modes
The first instruction executed by the RISC-V core depends on the configured boot mode, which is
defined by the DIO alternate functions BOOT_MODE_0 and BOOT_MODE_1, mapped to chip pins
DIO.00 and DIO.01. These pins are sampled once the chip reset is released – that is, when pin
NRESET transitions from low to high. Once the boot sequence is started, the DIO.00 and DIO.01
functions can be changed without impacting the boot mode until the next chip reset.

 BOOT_MODE_1       BOOT_MODE_0        Boot mode

        0                  0           Wait Start

        0                  1           Flash Run

        1                  0           Flash Copy

        1                  1           Wait Start

Table 8: Boot mode selection


         5.5.1        Wait Start mode
When BOOT_MODE_0 = 0 and BOOT_MODE_1 = 0, or alternately when BOOT_MODE_0 = 1 and
BOOT_MODE_1 = 1 at chip reset, the boot mode is defined to “Wait Start”. In this configuration,
the CPU will not try to fetch any instruction until the SCU_CORE_RUN_EN SCU register bit is written
to ‘1’. The RISC-V code can be loaded in the SRAM, typically beginning at address 0x10000000,
using the SPID or DBGM (JTAG) peripherals. Please refer to corresponding sections for more
information on their usage. Once the code is loaded, the instruction fetch can be enabled.

The CPU will fetch the first instruction located at the defined boot address. This address is set to
0x10000000 after reset and can be changed using the SCU_BOOT_ADDR_OVW and
SCU_BOOT_ADDR_SET fields before enabling the instruction fetch.
Note1: In this mode, an external flash memory is not required, but the code loaded in the SRAM is
lost at power-down (when the VDD voltage level falls below the functional minimum).

Note2: Once the instruction fetch has been enabled ( SCU_CORE_RUN_EN = ‘1’), it cannot be stopped
unless a Wait For Interrupt instruction is executed, even if SCU_CORE_RUN_EN is set to ‘0’.


         5.5.2        Flash Copy mode
When BOOT_MODE_0 = 0 and BOOT_MODE_1 = 1 at chip reset, the boot mode is defined to
“Flash Copy”. This mode is recommended for most applications as the CPU runs efficiently from
SRAM while it enables power cycling of the chip without requiring manual code loading.



                                                                                             25
<PARSED TEXT FOR PAGE: 26 / 159>
                                                                                 System overview


After approximately one millisecond of delay during which SPIMF_RSTN is driven low, this function
pin is raised and the SPIMF starts reading data from the external flash using in SPI mode. The
program code is then automatically copied from the “flash start address”, i.e. the internal address
0x00000000 of the flash memory, to the location given by the SRAM_START_ADDR field of the
image header detailed below. At the end of the copy process, the RISC-V core executes the first
instruction fetched from SRAM_START_ADDR.

Note: The SPIMF_RSTN signal may not be required by the flash memory, leaving the corresponding
DIO available for another use.




Figure 6: Flash Copy boot sequence

    Flash image with header
This boot mode requires a specific binary flash image file to which a header has been added.
Compared to the original .bin or .hex file, 16 bytes (the HEADER) are inserted at the beginning of
these files before the code.




Figure 7: Flash image with header


The byte mapping of the header is detailed in the following table.

 HEADER byte index    Description                     HEADER byte index   Description

         0            CODE_LEN[7:0]                            5          SRAM_START_ADDR[15:8]

         1            CODE_LEN[15:8]                           6          SRAM_START_ADDR[23:16]

         2            CODE_LEN[23:16]                          7          SRAM_START_ADDR[31:24]

         3            CODE_LEN[31:24]                       8 to 15       Byte padding with 0x00



                                                                                               26
<PARSED TEXT FOR PAGE: 27 / 159>
                                                                                  System overview


 HEADER byte index    Description                     HEADER byte index    Description

         4            SRAM_START_ADDR[7:0]

Table 9: HEADER bytes description
The CODE_LEN field is a 32-bit integer defining the number of bytes of RISC-V code to be copied in
the SRAM, excluding the header.

The SRAM_START_ADDR field is a 32-bit integer defining the SRAM address to which the first byte
of the code should be copied. It can be set to the special value 0x00000000 meaning that the
destination of the first byte copied is assumed to be the first SRAM address, i.e. 0x10000000.

    Requirements
This mode requires that an external flash memory device is powered and connected to channel 0 of
the SPIMF peripheral (SPIMF_CSN_0 pin function). The flash memory should also contain an image
of the RISC-V program code prepended with the HEADER, starting at its internal address
0x00000000. More information about the flash controller is provided in section SPI master flash
(SPIMF).

During the copy process, the channel 0 of the DMA peripheral is used and configured for this
purpose. Once finished, all DMA channels are released and available for the application.


         5.5.3        Flash Run mode
When BOOT_MODE_0 = 1 and BOOT_MODE_1 = 0 at chip reset, the boot mode is defined to
“Flash Run”. The RISC-V program code is executed from the external flash memory directly, the
instructions being read when needed through the SPIMF.

This mode also requires that an external flash memory device is powered and connected to the
channel 0 of the SPIMF peripheral (SPIMF_CSN_0 pin function). The flash should contain an image
of the RISC-V program code without a HEADER, starting at its internal address 0x00000000.

The SPIMF_RSTN signal is driven as in the previous mode. The CPU starts fetching the first
instruction from the external flash through the SPIMF in flash bridge mode (refer to section 19.1.1).
This is achieved by reading from the global address 0x20040000 of the ColibryNPU, which
corresponds to the flash internal address 0x00000000. This mode does not require storing the code
in the SRAM, but the execution is slower because each instruction is fetched serially through the
SPI link.



5.6 System clocks
The clock tree of the system is represented in Figure 8. It features a high-frequency crystal-based
oscillator (HFXO), a low-frequency crystal-based oscillator (LFXO), a PLL and several integer clock
dividers. The different clock gaters, allowing to stop the clock propagation to specific paths or
blocks, are not shown in the figure.

The register fields controlling the clocks are provided in the SCU module, except for the crystal-
based oscillators modes which can be configured in the PORT module.


                                                                                              27
<PARSED TEXT FOR PAGE: 28 / 159>
                                                                             System overview




Figure 8: ColibryNPU clock-tree


         5.6.1       High-frequency clock domain

    HF_CLK derived clocks
The high-frequency clock HF_CLK is used as a primary clock source, which is further divided and
distributed to the peripherals of the system. The following clocks are derived from HF_CLK:




                                                                                         28
<PARSED TEXT FOR PAGE: 29 / 159>
                                                                                     System overview


  ●       AXI_CLK, the AXI bus clock driving the RISC-V core, the SRAM and the bus masters allowed
          to initiate 32-bit data transfers. The SCU_AXI_CLK_DIV field allows to configure an integer
          division ratio of the HF_CLK frequency to generate the AXI_CLK.

  ●       APB_CLK, the APB bus clock used by the peripherals connected to this bus for their register
          configuration or their internal operations. The SCU_APB_CLK_DIV field allows to configure an
          integer division ratio of the AXI_CLK frequency to generate the APB_CLK.

  ●       NNPA_CLK clock is used by the NNPA peripheral when running an inference. Other clocks
          are used when loading data into its internal memory (refer to section 11 for more
          information). The SCU_NNPA_CLK_DIV field allows to configure an integer division ratio of the
          HF_CLK frequency to generate the NNPA_CLK.

  ●       ISP_CLK clock drives the ISP peripheral data path until the NNPA input. As this block receives
          its input data from the CAMIF, the CAM_PCLK clock can be used as the ISP_CLK to make the
          interface synchronous. This is achieved by setting SCU_ISP_CLK_SRC to 0. On the opposite,
          the HF_CLK can also be employed to clock the ISP. In both cases, the ISP clock frequency can
          be further divided by configuring the SCU_ISP_CLK_DIV field.

      HF_CLK source selection
The HF_CLK can be either the external high-frequency clock (EXT_HF_CLK) or the programmable
PLL clock (PLL_DIV_CLK). These source clocks are described in the following paragraphs.

EXT_HF_CLK is selected as the HF_CLK if one of these conditions are met:
    ● SCU_HF_CLK_SRC = 1, or

      ●    DIO.31 pin input is at high level and is configured in alternate_3 mode

Otherwise, the PLL_DIV_CLK is used as the HF_CLK. After a chip reset and if DIO.31 is not driven
by a high level voltage, the PLL_DIV_CLK clock is selected.

      EXT_HF_CLK description
The EXT_HF_CLK is generated by the HFXO internal oscillator. It has two operating modes:

  ●       External clock mode – When PORT_HFXO_MODE = 2, the HFXO acts as a buffer, transmitting
          the clock provided at HFXO0 input pin to EXT_HF_CLK. In this mode, the input clock
          frequency is limited to 70 MHz.

  ●       Crystal-based oscillator mode – When PORT_HFXO_MODE = 1, the HFXO generates a clock
          from a crystal quartz connected between HFXO0 and HFXO1 pins. The clock frequency
          corresponds to the resonant frequency of the quartz. It is recommended to use a 4 MHz
          crystal.

In case the frequency chosen is different from 4 MHz, the PORT_HFXO_FSEL field must be
configured according to the specification given in Table 10.




                                                                                                 29
<PARSED TEXT FOR PAGE: 30 / 159>
                                                                                  System overview


      Quartz frequency       PORT_HFXO_FSEL

         1 MHz – 4 MHz               0

        4.1 MHz – 12 MHz             1

       12.1 MHz – 24 MHz             3

       24.1 MHz – 48 MHz             4

      Table 10: HFXO frequency configuration

      PLL_DIV_CLK description

The PLL_DIV_CLK clock is provided by an internal phase-locked loop (PLL) device, which can be
further divided by configuring the PLL_POST_DIV register field that controls the corresponding
PROG_DIV post divider. The PLL offers a flexible and programmable clock source, as detailed in
section 5.6.2.


          5.6.2            Internal PLL (PLL)
The PLL control registers are available in the SCU peripheral. The PLL_ANA_CONTROL register should
not be modified while in functional mode and is not required bby the application. The PLL_STATUS
register provides information on the status of the PLL.

Three operating modes are available, depending on the availability of an EXT_HF_CLK clock:

  ●    Free-running uncalibrated – This is the only mode available when no clock is active at the
       output of the HXFO. The clock provided by the PLL has a +/- 30% accuracy.

  ●    Free-running calibrated – A calibration loop allows to match the clock at the output of the
       HXFO at +/- 5% accuracy.

  ●    Phase-locked mode – After a calibration phase and a locking delay, the PLL provides a clock
       which is as accurate as the clock at the HFXO output. If a +/- 50 ppm crystal is used, the PLL
       clock will have the same frequency accuracy. This mode is recommended for most
       applications.

The PLL can be seen as a clock frequency multiplier where: output_frequency = input_frequency *
multiplier. The multiplier is configured in the PLL_FREQ_MULT field and the input frequency
corresponds to the HXFO clock frequency. The output frequency after multiplication must be in the
range specified in Table 12 PLL output frequency limits, to be functional under all conditions.

If the input_frequency is provided by the recommended 4 MHz crystal oscillator, in which case the
PLL can generate any clock frequency within the limits in steps of 4 MHz.

      Free-running uncalibrated
This mode corresponds to the PLL in open-loop mode (PLL_EN_MODE = 1) with direct control of the
oscillator generating the clock, hence the PLL_FREQ_MULT field is not used. Instead,
PLL_CAL_BYPASS_EN should be set to ‘1’ and PLL_CAL_BYPASS_VAL can be used to modify the PLL
output clock frequency. The frequency increases non-linearly with the value configured in
PLL_CAL_BYPASS_VAL. The PLL_GAIN_BYPASS_VAL applies an additional scaling factor to the
uncalibrated PLL frequency:

                                                                                              30
<PARSED TEXT FOR PAGE: 31 / 159>
                                                                                     System overview


      PLL_GAIN_BYPASS_VAL         Frequency multiplier

                    0                        1.0

                    1                        0.66

                    2                        1.2

                    3                        1.0

Table 11: PLL gain effect on output frequency


     Free-running calibrated
This mode corresponds to the PLL in open-loop mode ( PLL_EN_MODE = 1) and PLL_CAL_BYPASS_EN
set to ‘0’. The target frequency is programmed using PLL_FREQ_MULT multiplier. The calibration
takes place automatically after a reset, as long as an active EXT_HF_CLK clock is available. It can
also be triggered manually by writing ‘1' in the PLL_RESTART bit of the SCU.

The calibration process tries to match the PLL output frequency divided by PLL_FREQ_MULT with
the EXT_HF_CLK clock frequency. The frequency is modified step by step by incrementing or
decrementing a tuning code. When no calibration can be initiated after reset, the tuning code is set
to 0.

     Phase-locked mode
This is the regular working mode of the PLL in closed-loop mode (PLL_EN_MODE = 2). The target
frequency is programmed using the PLL_FREQ_MULT multiplier.

The PLL_AUTO_CAL bit can be set to 1 to ensure that automatic re-calibrations occur if the PLL is
abut. However, this may induce a fast change in the system clock frequency during the calibration
while running the application.

The PLL characteristics and specifications are listed in Table 12.

 Parameter                         Conditions                                 Min.     Max.   Unit

                                   Tuning code 0
 PLL output frequency at reset                                                30       60     MHz
                                   Free-running uncalibrated

                                   Tuning code 128
 PLL output frequency mid code
                                   Free-running uncalibrated

                                   Tuning code 255
 PLL output frequency max code
                                   Free-running uncalibrated

 PLL output frequency limits
                                   Any                                        60       350    MHz
 input_frequency * multiplier

 Calibration time                  Default settings, 4 MHz EXT_HF_CLK clock   60       1600   us

 PLL settling time, including
                                   Default settings, 4 MHz EXT_HF_CLK clock
 calibration

Table 12: PLL characteristics and specification




                                                                                              31
<PARSED TEXT FOR PAGE: 32 / 159>
                                                                                           System overview



            5.6.3     System clocks after reset
The registers configuration related to the system clocks, after a chip reset, are summarized in Table
13. This configuration can be changed once the CPU is running by the program code itself or with
the debug interfaces.

The system clocks at boot behave differently whether an external active clock is provided at
HFXO0 pin or if a passive quartz is connected between HFXO0 and HFXO1 pins:

   1. Active external clock: the PLL output clock frequency will be set to 15 * external clock
      frequency with +/- 5 % accuracy after a calibration phase. It is recommended to use an
      external clock at 4 MHz to get a PLL output clock around 60 MHz. Consequently, the
      HF_CLK will be a 30 MHz +/- 5% clock in this mode at boot and if the post_divided PLL
      clock is selected.

   2. Using a quartz: the PLL output clock frequency will be set to “PLL output frequency at
      reset” specified in Table 12. No calibration takes place because the EXT_HF_CLK clock will
      not be active until the mode is changed to PORT_HFXO_MODE = 1. Consequently, the
      HF_CLK, AXI_CLK, APB_CLK, and NNPA_CLK clocks have the same frequency, equals to
      the PLL output clock frequency but divided by 2.

 Register                   Description                                    Reset value

                            PLL frequency multiplier. Frequency
 PLL_FREQ_MULT              multiplied by (PLL_FREQ_MULT + 1)
                                                                           14

 PLL_EN_MODE                PLL enable mode                                1 (open-loop)

                            Post divider factor. Frequency divided by
 PLL_POST_DIV               (PLL_POST_DIV + 1)
                                                                           1

                            High-frequency clock source selection. 0:
 SCU_HF_CLK_SRC             PLL, 1: EXT_HF_CLK
                                                                           0

                            AXI clock division ratio. Frequency divided
 SCU_AXI_CLK_DIV            by (SCU_AXI_CLK_DIV + 1)
                                                                           0

                            APB clock division ratio. Frequency divided
 SCU_APB_CLK_DIV            by (SCU_APB_CLK_DIV + 1)
                                                                           0

                            NNPA clock division ratio. Frequency divided
 SCU_NNPA_CLK_DIV           by (SCU_NNPA_CLK_DIV + 1)
                                                                           0

 PORT_HFXO_MODE             HFXO mode selection                            2 (external)

Table 13: Clock-related registers configuration after reset


            5.6.4     Peripherals clock management
Apart from the NNPA and ISP, each peripheral receives the APB_CLK and/or the AXI_CLK clocks.
The clock of each peripheral can be individually stopped if unused, allowing a reduction in their
dynamic power consumption. This is achieved by writing a ‘0’ to each individual bits of the
SCU_PERIPH_CLK_EN register. The clock of the SCU peripheral itself is always active.




                                                                                                    32
<PARSED TEXT FOR PAGE: 33 / 159>
                                                                                     System overview



           5.6.5        Low-frequency clock domain
The low-frequency clock EXT_LF_CLK is used as a primary clock source for the RTC peripheral,
enabling a low-power time reference by remaining active while the system is in a Sleep mode. This
clock is generated by the LFXO low-frequency oscillator, which has two operating modes:

   ●   External clock mode – When PORT_LFXO_MODE = 2, the LFXO acts as a buffer, transmitting
       the clock provided at the LFXO0 input pin to EXT_LF_CLK.

   ●   Crystal-based oscillator mode – When PORT_LFXO_MODE = 1, the LFXO generates a clock
       from a crystal quartz connected between HFXO0 and HFXO1 pins. The frequency of the
       quartz must be 32.768 kHz.

The mode configured after a chip reset is “crystal-based oscillator mode”.

Note: having an active EXT_LF_CLK available is optional, but without this clock the RTC
functionalities cannot be used.


           5.6.6        Chip clock outputs
Some DIOs have the CLK_OUT0 or CLK_OUT1 alternate functions, which can be used to generate
clock waveforms outside the chip, as illustrated in Figure 8. CLK_OUT0 and CLK_OUT1 have a
separate configuration register which can be used to select an internal clock and optionally apply a
division ratio:

   ●   SCU_CLKOUTX_SRC (X=0 or 1): Selection of the clock source according to the following table.

         SCU_CLKOUTX_SRC        CLK_OUTX

                0x00            No clock

                0x01            EXT_HF_CLK

                0x02            PLL_DIV_CLK

                0x03            EXT_LF_CLK


   ●   SCU_CLKOUTX_DIV (X=0 or 1): Integer division ratio applied to the selected clock. If this field is
       set to 0, no division of the clock is applied.



5.7 Resets management
This section describes the different reset scopes and reset sources available in the ColibryNPU.


           5.7.1        Global reset
The global reset operates over the full chip state. When asserted, all blocks and peripherals are
reset to their default state. In particular, all register fields take their specified reset value.

A global reset is applied when at least one of these conditions is met:
   ●   The NRESET input pin is in low state (voltage applied below VIO_IN_L). The reset is released
       once the pin returns in high state

                                                                                                  33
<PARSED TEXT FOR PAGE: 34 / 159>
                                                                                   System overview


   ●   The SCU_GLOBAL_RESET bit field is written to ‘1’. The reset is automatically de-asserted

   ●   A watchdog event occurs and SCU_WDT_RESET_MODE = 0x3


          5.7.2        RISC-V core reset
This reset takes effect only to the RISC-V core and does not affect the rest of the system. It is
asserted when at least one of these conditions is met:

   ●   The SCU_CORE_RESET bit field is written to ‘1’. The reset is automatically de-asserted

   ●   A watchdog event occurs and SCU_WDT_RESET_MODE = 0x1


          5.7.3        Peripherals reset
Each peripheral can be individually reset without impacting the rest of the system. This is achieved
by writing a ‘1’ in the corresponding bit of the SCU_PERIPH_RESET register. The peripheral resets
are released by writing back a ‘0’ to the same bit.

For safe operation, it is recommended to disable the clocks of the peripherals requiring a reset,
apply the reset, release the reset and finally re-enable the clocks.

Note: The APB reset is specifically used to reset the APB bus (but not the peripherals) and the PLL
calibration controller.




                                                                                                  34
<PARSED TEXT FOR PAGE: 35 / 159>
                                                                        System control unit (SCU)




6         System control unit (SCU)

6.1 Functional description
The SCU is responsible for the global control of the system in terms of clocks, reset, power
management and SRAM memories. The functions related to clocks and resets are described in
sections 5.6 and 5.7. The SRAM memories power modes and testing modes are described in section
5.4. The other functionalities of the SCU are described in the following sections.


            6.1.1        Power optimization and Sleep mode
The ColibryNPU static and dynamic power consumption can be decreased in several ways:

    ●    Disabling the clock for any unused peripheral (SCU_PERIPH_CLK_EN register)

    ●    Adapting the frequency of the AXI, APB, ISP or NNPA clocks ( SCU_*_CLK_CFG registers) to
         the needed computation load

    ●    Optimizing the power voltage operating point (see 4.3)

    ●    Powering down unused memories of the SRAM (SRAM_CUTS_PD_CTRL register)

    ●    Powering down unused memories of the NNPA (MEM_BUFF_PD* registers)

    ●    Setting the RISC-V core in standby mode

    ●    Entering in Sleep mode

The RISC-V enters in standby when the WFI instruction is executed, stopping the AXI_CLK clock at
the core input. This mode is exited on any interrupt request (IRQ) event.

The Sleep mode is a configurable low-power mode in which the HF_CLK clock is stopped for the
entire system.

        Sleep mode configuration
The Sleep mode is allowed only if the SCU_SLEEP_ENABLE bit is set to ‘1’. When in Sleep mode, the
HF_CLK and all other clocks generated from it are gated (stopped) but the clock source is not
necessarily stopped.

In case the PLL_DIV_CLK clock is selected and the PLL is enabled through the registers, the Sleep
mode can be configured to also shut down the PLL, further decreasing the power consumption. This
is achieved by setting SCU_PLL_AUTO_SLEEP to ‘1’. Upon wake-up event, the PLL returns to its state
before the Sleep mode was requested after the PLL start-up and locking delay.

        Entering in Sleep mode
The sleep mode is entered if the SCU_SLEEP_ENABLE bit is set to ‘1’ and when either the WFI
instruction is executed by the core, or the SCU_SLEEP_REQ bit is written to ‘1’. If the WFI


                                                                                           35
<PARSED TEXT FOR PAGE: 36 / 159>
                                                                       System control unit (SCU)


instruction is run while SCU_SLEEP_ENABLE = ‘0’, only the clock to the RISC-V core is stopped
(standby mode).

       Exiting Sleep mode
The HF_CLK clock source is restored on one of the following events:

   ●    A global reset

   ●    An RTC wake-up compA, compB or watchdog event. An active LF clock is needed for this
        wake-up method

   ●    A PORT wake-up event, not requiring any clock to wake-up the system

As wake-up events have an associated peripheral IRQ, the corresponding IRQ handler are called on
Sleep mode exit.


           6.1.2         RISC-V core boot address change
By default, in “Wait Start” boot mode, the first instruction is fetched by the core at address
0x10000000, which is the start of the SRAM memory space. This first instruction address – or boot
address, can be read from SCU_BOOT_ADDR.

To modify it, the new boot address should be written in SCU_BOOT_ADDR_OVW, then a ‘1’ should be
written to SCU_BOOT_ADDR_SET. This update has no impact on the program execution flow if the
core has already started and is running.


           6.1.3         CPU/system message box
The SCU features a message box composed of eight general-purpose 32-bit registers for data
exchange between the RISC-V core and any external interface.

By convention, SCU_CORE_MSG_X (X=0 to 3) are written by the core and SCU_EXT_MSG_X (X=0 to 3)
are written by an external interface. In addition, the SCU_CORE_STATUS register can be used by the
CPU to indicate any application-specific information or state.

The SCU has a dedicated IRQ signal, triggered when a ‘1’ is written to the SCU_GENERATE_IRQ bit. It
can be used to inform the core that a message has been written in the SCU_EXT_MSG_X registers for
example.


           6.1.4         SRAM write protection
This feature allows one to define a range of SRAM addresses for which write access is disabled and
has no effect. Typically, it can be used to prevent any accidental writing in the code section of the
memory.

The write-protected area is register-defined between SCU_WR_PROT_START and SCU_WR_PROT_END
addresses (inclusive). The protection addresses must be 4-byte aligned, i.e. the two LSB of these
registers should be ‘0’.




                                                                                              36
<PARSED TEXT FOR PAGE: 37 / 159>
                                                                           System control unit (SCU)



6.2 Registers

         6.2.1        SCU_HF_CLK_CFG
High-frequency clock configuration register
Address offset: 0x00
Reset value: 0x00000002

Field                     Bits     Type Reset   Description
SCU_HF_CLK_SRC            [0]     RW   0        High-frequency clock source:
                                                0: PLL
                                                1: External
SCU_HF_CLK_ENFORCE        [1]     RW   1        Bypass clock switcher and force selected clock at output



         6.2.2        SCU_AXI_CLK_CFG
AXI clock configuration register
Address offset: 0x04
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_AXI_CLK_DIV           [5:0]   RW   0        AXI clock frequency division ratio minus 1



         6.2.3        SCU_APB_CLK_CFG
APB clock configuration register
Address offset: 0x08
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_APB_CLK_DIV           [5:0]   RW   0        APB clock frequency division ratio minus 1



         6.2.4        SCU_ISP_CLK_CFG
ISP clock configuration register
Address offset: 0x0C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_ISP_CLK_DIV           [5:0]   RW   0        ISP clock frequency division ratio minus 1
SCU_ISP_CLK_SRC           [8]     RW   0        ISP clock source:
                                                0: CAM_PCLK
                                                1: HF_CLK
SCU_ISP_CLK_ENFORCE       [9]     RW   0        Bypass clock switcher and force selected clock at output




                                                                                                     37
<PARSED TEXT FOR PAGE: 38 / 159>
                                                                          System control unit (SCU)



         6.2.5      SCU_NNPA_CLK_CFG
NNPA clock configuration register
Address offset: 0x10
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
SCU_NNPA_CLK_DIV          [5:0]   RW   0       NNPA clock frequency division ratio minus 1



         6.2.6      SCU_PERIPH_CLK_EN
Blocks clocks enable register
Address offset: 0x14
Reset value: 0x0003FFFF

Field                     Bits    Type Reset   Description
SCU_APB_CLK_EN            [0]     RW   1       APB soft reset
SCU_ISP_CLK_EN            [1]     RW   1       ISP clock enable
SCU_NNPA_CLK_EN           [2]     RW   1       NNPA clock enable
SCU_SPID_CLK_EN           [3]     RW   1       SPID clock enable
SCU_FFT_CLK_EN            [4]     RW   1       FFT clock enable
SCU_DBGM_CLK_EN           [5]     RW   1       DBGM clock enable
SCU_DMA_CLK_EN            [6]     RW   1       DMA clock enable
SCU_CAMIF_CLK_EN          [7]     RW   1       CAMIF clock enable
SCU_PORT_CLK_EN           [8]     RW   1       PORT clock enable
SCU_SPIS_CLK_EN           [9]     RW   1       SPIS clock enable
SCU_SPIMF_CLK_EN          [10]    RW   1       SPIMF clock enable
SCU_SPIM0_CLK_EN          [11]    RW   1       SPIM0 clock enable
SCU_SPIM1_CLK_EN          [12]    RW   1       SPIM1 clock enable
SCU_UART_CLK_EN           [13]    RW   1       UART clock enable
SCU_I2CM_CLK_EN           [14]    RW   1       I2CM clock enable
SCU_TIMER0_CLK_EN         [15]    RW   1       TIMER0 clock enable
SCU_TIMER1_CLK_EN         [16]    RW   1       TIMER1 clock enable
SCU_RTC_CLK_EN            [17]    RW   1       RTC clock enable



         6.2.7      SCU_PERIPH_RESET
Blocks clocks enable register
Address offset: 0x18
Reset value: 0x00000000




                                                                                             38
<PARSED TEXT FOR PAGE: 39 / 159>
                                                                            System control unit (SCU)


Field                     Bits     Type Reset   Description
SCU_APB_RESET             [0]      RW   0       APB soft reset
SCU_ISP_RESET             [1]      RW   0       ISP soft reset
SCU_NNPA_RESET            [2]      RW   0       NNPA soft reset
SCU_SPID_RESET            [3]      RW   0       SPID soft reset
SCU_FFT_RESET             [4]      RW   0       FFT soft reset
SCU_DBGM_RESET            [5]      RW   0       DBGM soft reset
SCU_DMA_RESET             [6]      RW   0       DMA soft reset
SCU_CAMIF_RESET           [7]      RW   0       CAMIF soft reset
SCU_PORT_RESET            [8]      RW   0       PORT soft reset
SCU_SPIS_RESET            [9]      RW   0       SPIS soft reset
SCU_SPIMF_RESET           [10]     RW   0       SPIMF soft reset
SCU_SPIM0_RESET           [11]     RW   0       SPIM0 soft reset
SCU_SPIM1_RESET           [12]     RW   0       SPIM1 soft reset
SCU_UART_RESET            [13]     RW   0       UART soft reset
SCU_I2CM_RESET            [14]     RW   0       I2CM soft reset
SCU_TIMER0_RESET          [15]     RW   0       TIMER0 soft reset
SCU_TIMER1_RESET          [16]     RW   0       TIMER1 soft reset
SCU_RTC_RESET             [17]     RW   0       RTC soft reset



         6.2.8      SCU_CLK_OUT0_CFG
CLK_OUT_0 configuration register
Address offset: 0x1C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_CLK_OUT0_SRC          [1:0]    RW   0       CLK_OUT_0 clock source selection
SCU_CLK_OUT0_DIV          [15:8]   RW   0       CLK_OUT_0 clock division ratio (Freq / (1 + DIV))



         6.2.9      SCU_CLK_OUT1_CFG
CLK_OUT_1 configuration register
Address offset: 0x20
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_CLK_OUT1_SRC          [1:0]    RW   0       CLK_OUT_1 clock source selection
SCU_CLK_OUT1_DIV          [15:8]   RW   0       CLK_OUT_1 clock division ratio (Freq / (1 + DIV))



         6.2.10     SCU_SYSTEM_CTRL
Global system control register
Address offset: 0x24
Reset value: 0x00000000




                                                                                                    39
<PARSED TEXT FOR PAGE: 40 / 159>
                                                                             System control unit (SCU)


Field                     Bits     Type Reset   Description
SCU_GLOBAL_RESET          [0]      RW   0       When written 1, a global reset is issued. This field is always
                                                read as 0. Automatically returns to 0 after a write request
SCU_SLEEP_REQ             [1]      RW   0       When written 1, a sleep request is issue, as for a WFI
                                                instruction. This field is always read as 0. Automatically
                                                returns to 0 after a write request



         6.2.11        SCU_SYSTEM_CONFIG
Global system configuration register
Address offset: 0x28
Reset value: 0x00000002

Field                     Bits     Type Reset   Description
SCU_SLEEP_ENABLE          [0]      RW   0       When ‘1’, sleep requests or WFI will put the chip in sleep
                                                mode
SCU_PLL_AUTO_SLEEP        [1]      RW   1       When ‘1’, the PLL is automatically disabled/re-enabled with
                                                sleep requests
SCU_WDT_RESET_MODE        [5:4]    RW   0       Reset mode applied on a watchdog event:
                                                0x0 or 0x2: Disabled (no reset)
                                                0x1: MCU core only
                                                0x3: Full chip



         6.2.12        SCU_CORE_CTRL
CPU core control register
Address offset: 0x30
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_CORE_RUN_EN           [0]      RW   0       Enable instruction fetching from CPU core
SCU_CORE_RESET            [1]      RW   0       When written ‘1’, a CPU core reset is issued. This field is
                                                always read as 0. Automatically returns to 0 after a write
                                                request



         6.2.13        SCU_BOOT_ADDR
Current code boot address register
Address offset: 0x34
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_BOOT_ADDR             [31:0]   RO   0       Current code boot address



         6.2.14        SCU_BOOT_ADDR_OVW
Overwrite code boot address register
Address offset: 0x38


                                                                                                         40
<PARSED TEXT FOR PAGE: 41 / 159>
                                                                            System control unit (SCU)


Reset value: 0x00000000

Field                      Bits     Type Reset   Description
SCU_BOOT_ADDR_OVW          [31:0]   RW   0       Overwrite code boot address



         6.2.15       SCU_BOOT_ADDR_SET
New boot address setting register
Address offset: 0x3C
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
SCU_BOOT_ADDR_SET          [0]      RW   0       When written 1, SCU_BOOT_ADDR is set to
                                                 SCU_BOOT_ADDR_OVW . This field is always read as 0.
                                                 Automatically returns to 0 after a write request



         6.2.16       SCU_CORE_STATUS
CPU core user status register
Address offset: 0x40
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
SCU_CORE_STATUS            [31:0]   RW   0       CPU core user status (shall be written by CPU only)



         6.2.17       SCU_CORE_MSG_x (x = 0 to 3)
CPU core message register
Address offset: 0x50 + x * 4
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
SCU_CORE_MSG               [31:0]   RW   0       CPU core message (shall be written by CPU only)



         6.2.18       SCU_EXT_MSG_x (x = 0 to 3)
External message register
Address offset: 0x70 + x * 4
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
SCU_EXT_MSG                [31:0]   RW   0       External message to CPU (shall be written by external device
                                                 only)



         6.2.19       SCU_GENERATE_IRQ
Interrupt generation register

                                                                                                       41
<PARSED TEXT FOR PAGE: 42 / 159>
                                                                            System control unit (SCU)


Address offset: 0x90
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_GENERATE_IRQ          [0]      RW   0       Generate an SCU interrupt when written ‘1’. This field is
                                                always read as 0. Automatically returns to 0 after a write
                                                request



         6.2.20     SCU_WR_PROT_START
Write protection start address register
Address offset: 0x94
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_WR_PROT_START         [31:0]   RW   0       Write protection start address (included)



         6.2.21     SCU_WR_PROT_END
Write protection end address register
Address offset: 0x98
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SCU_WR_PROT_END           [31:0]   RW   0       Write protection end address (included)



         6.2.22     SRAM_CUTS_CTRL
SRAM cuts control pins register
Address offset: 0x9C
Reset value: 0x0000050A

Field                     Bits     Type Reset   Description
SRAM_CUTS_TEST1           [0]      RW   0       Test mode to bypass self-timed circuit.The external clock
                                                controls the read and write control signals
SRAM_CUTS_RME             [1]     RW    1       Read-Write margin enable
SRAM_CUTS_RM              [5:2]   RW    2       Read-Write margin
SRAM_CUTS_RA              [7:6]   RW    0       Read Assist setting
SRAM_CUTS_WA              [10:8] RW     5       Write Assist setting
SRAM_CUTS_WPULSE          [13:11] RW    0       Write Assist pulse width setting



         6.2.23     SRAM_CUTS_PD_CTRL
SRAM cuts power down register
Address offset: 0xA0
Reset value: 0x00000000



                                                                                                       42
<PARSED TEXT FOR PAGE: 43 / 159>
                                                                           System control unit (SCU)


Field                     Bits     Type Reset   Description
SRAM_CUTS_PD_CTRL         [31:0]   RW   0       Each bit controls the power-down mode of SRAM memory
                                                cuts



         6.2.24       SRAM_FORCE_CLK_EN
SRAM cuts clock enable force register
Address offset: 0xA4
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SRAM_FORCE_CLK_EN         [0]      RW   0       When ‘1’, all SRAM cuts receive a clock, otherwise only when
                                                needed



         6.2.25       SRAM_MBIST_CTRL
SRAM SRAM BIST control register
Address offset: 0xB0
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SRAM_MBIST_RUN            [3:0]    RWS 0        Launch memory BIST test



         6.2.26       SRAM_MBIST_STATUS
SRAM SRAM BIST status register
Address offset: 0xB4
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SRAM_MBIST_DONE           [3:0]    RO   0       Set by HW on memory BIST completion. Cleared by HW when
                                                BIST start
SRAM_MBIST_ANY_FAIL       [31]     RO   0       Set by HW on any memory BIST failure. Cleared by HW when
                                                BIST start



         6.2.27       SRAM_MBIST_FAIL
SRAM BIST fail register
Address offset: 0xB8
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SRAM_MBIST_FAIL           [31:0]   RO   0       BIST status of SRAM cuts 0 to 31




                                                                                                      43
<PARSED TEXT FOR PAGE: 44 / 159>
                                                                            System control unit (SCU)



         6.2.28        PLL_CONTROL
PLL control register
Address offset: 0xC0
Reset value: 0x000010E1

Field                     Bits     Type Reset   Description
PLL_EN_MODE               [1:0]    RW   1       PLL enable mode:
                                                00: Disabled
                                                01: Enabled, PLL in open-loop
                                                10: Enabled, PLL with frequency locking
                                                11: Reserved
PLL_AUTO_CAL              [3]      RW   0       When ‘1’, recalibration is automatically triggered if PLL is
                                                abut
PLL_FREQ_MULT             [10:4]   RW   14      PLL frequency multiplier. PLL clock frequency PLL_CLK = 4
                                                MHz * (this + 1). Valid range [14:127]
PLL_POST_DIV              [15:12] RW    1       PLL output post divider. Clock frequencies: PLL_DIV_CLK =
                                                PLL_CLK / (this + 1)
PLL_RESTART               [16]     RW   0       When written 1, the PLL is recalibrated and relocked. This
                                                field is always read as 0. Automatically returns to 0 after a
                                                write request



         6.2.29        PLL_CAL_CONFIG
PLL calibration configuration register
Address offset: 0xC4
Reset value: 0x010320C8

Field                     Bits     Type Reset   Description
PLL_CAL_PERIODS           [9:0]    RW   200     Number of periods of calibration clock when measuring the
                                                PLL frequency
PLL_CAL_TARGET            [19:10] RW    200     Calibration target value
PLL_CAL_START_DLY         [29:20] RW    16      Number of periods of calibration clock before measurement



         6.2.30        PLL_CAL_BYPASS
PLL calibration bypass register
Address offset: 0xC8
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PLL_CAL_BYPASS_EN         [0]      RW   0       When ‘1’, the PLL calibration inputs are taken from
                                                PLL_CAL_BYPASS_VAL and PLL_GAIN_BYPASS_VAL
PLL_CAL_BYPASS_VAL        [11:4] RW     0       VCO tune value applied when PLL_CAL_BYPASS_EN=1
PLL_GAIN_BYPASS_VAL       [13:12] RW    0       VCO gain value applied when PLL_CAL_BYPASS_EN=1



         6.2.31        PLL_ANA_CONTROL
PLL analog control register


                                                                                                        44
<PARSED TEXT FOR PAGE: 45 / 159>
                                                                           System control unit (SCU)


Address offset: 0xCC
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
PLL_ANA_CONFIG            [7:0]   RW   0       Reserved
PLL_CTRL_IN_EN            [8]     RW   0       When ‘1’, enable the debug IO into PLL
PLL_FORCE_EN              [9]     RW   0       When ‘1’, the PLL enabled is force to ‘1’
PLL_FORCE_CLOSEDLOOP      [10]    RW   0       When ‘1’, the PLL loop is force to be closed



         6.2.32       PLL_STATUS
PLL status register
Address offset: 0xD0
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
PLL_CAL_STATE             [2:0]   RO   0       PLL calibration state. States are {IDLE, STABILIZE_DELAY,
                                               MEASURE, END_DELAY}
PLL_CAL_RUNNING           [4]     RO   0       Calibration running indicator
PLL_LOCKED                [5]     RO   0       PLL locked flag
PLL_VVCO_FLAG_L           [6]     RO   0       VCO voltage low flag
PLL_VVCO_FLAG_H           [7]     RO   0       VCO voltage high flag
PLL_COARSE_TUNE           [15:8] RO    0       PLL coarse tuning value
PLL_CAL_MEAS              [25:16] RO   0       PLL VCO clock measurement



         6.2.33       CHIP_VERSION
Chip hardware version register
Address offset: 0xF0
Reset value: 0x00010000

Field                     Bits    Type Reset   Description
CHIP_REV_MINOR            [15:0] RO    0       Hardware version minor
CHIP_REV_MAJOR            [31:16] RO   1       Hardware version major




                                                                                                     45
<PARSED TEXT FOR PAGE: 46 / 159>
                                                                       I/O ports controller (PORT)




7        I/O ports controller (PORT)

7.1 Functional description
The PORT peripheral manages all Inputs/Outputs (I/Os) of the chip. Most of them are Digital I/Os
(DIOs) that can have different functions as specified in Table 1.

Each DIO has the following features and configuration settings:

    ●   Configurable as a General-Purpose I/O (GPIO) or to an alternate function

    ●   Controllable enables of input and output buffers

    ●   Configurable drive strength and slew rate

    ●   Internal pull-up and pull-down resistors

    ●   Rising and falling edge events detection for IRQ and system wake-up

The PORT_LFXO_CFG and PORT_HFXO_CFG registers provide configuration for the LFXO and HFXO
components respectively, mainly to set the oscillators in crystal mode or external clock mode.


           7.1.1        DIO configuration




          Figure 9: Digital I/O cell


The ColibryNPU has 44 configurable digital I/O cells implementing the DIO functions. Each I/O cell
has an input buffer, an output buffer, resistors and pin access.




                                                                                            46
<PARSED TEXT FOR PAGE: 47 / 159>
                                                                          I/O ports controller (PORT)


The PORT_DIOA_OUT and PORT_DIOB_OUT registers define the output voltage level (high or low)
applied to each DIO pin when in GPIO mode and with the output buffer enabled. PORT_DIOA_OUT
corresponds to output values for DIO.00 to DIO.31 and PORT_DIOB_OUT corresponds to output
values for DIO.32 to DIO.43.

The PORT_DIOA_IN and PORT_DIOB_IN registers can be used to get the state (high or low) of each
DIO pin separately as long as their input buffer is enabled. PORT_DIOA_IN corresponds to input
values for DIO.00 to DIO.31 and PORT_DIOB_IN corresponds to input values for DIO.32 to DIO.43.

The PORT_DIO_CFG_X (x = 0 to 43) registers are used to configure each DIO as follows:

  ●     When PORT_DIO_OUT_EN = 1, the output buffer is enabled and the pin is driven with the
        corresponding bit of PORT_DIOA_OUT or PORT_DIOB_OUT. When the output buffer is disabled,
        the pin is in high-impedance.

  ●     PORT_DIO_DRIVE selects the drive strength when the I/O is in GPIO output mode. Table 15
        specifies the drive strength according to the configuration.

  ●     PORT_DIO_SR selects the slew rate when the I/O is in GPIO output mode. Table 15 specifies
        the slew rate according to the configuration.

  ●     When PORT_DIO_IN_EN = 1, the input buffer is enabled and the pin state can be read from
        the corresponding bit of PORT_DIOA_IN or PORT_DIOB_IN.

  ●     PORT_DIO_IN_CFG configures the input mode: When ‘1’, a Schmitt trigger is enabled, allowing
        it to sample the input with an hysteresis. Otherwise, the direct input pin is sampled.

  ●     The pull-up or pull-down resistors are enabled when PORT_DIO_PULL_EN = 1. When
        PORT_DIO_PULL_CFG = 1, this is a pull-up, else this is a pull-down.


            7.1.2        DIO functions
Each DIO can be configured in GPIO mode or any available alternate function, as specified in Table
1. The PORT_DIO_FUNCTION field of each DIO configuration register defines the DIO mode as
follows:

      PORT_DIO_FUNCTION[1:0]        DIO function

              0x00                  GPIO

              0x01                  Alternate1

              0x10                  Alternate2

              0x11                  Alternate3

Table 14: DIO function settings

When an alternate function is selected for a DIO, additional configuration such as enabling the
output buffer for an output function is not needed. The input and output buffer enables are
automatically controlled by the PORT logic, depending on the alternate function selected. Other
I/O configurations, such as pull resistors, remain controllable through registers.




                                                                                                 47
<PARSED TEXT FOR PAGE: 48 / 159>
                                                                          I/O ports controller (PORT)



         7.1.3        PORT events and wake-up
Each DIO can contribute to the PORT peripheral interrupt signal (IRQ) and system wake-up event
generation. The wake-up event is directly linked to the IRQ, which means the event is triggered as
soon as the interrupt is raised. This occurs when at least one out of the 44 DIOs detects a state
change on its input pin, rising or falling edge, and the corresponding enable bit in the
PORT_DIO*_INT_RISE_EN or PORT_DIO*_INT_FALL_EN is set to ‘1’. Both edges can be detected if
the rising and falling edges are enabled.

On a DIO event detection, the corresponding flag bit in a PORT_DIO*_INT_STATUS register is set to
‘1’. The flag bits can be individually cleared by writing a ‘1’ in the corresponding
PORT_DIO*_INT_CLEAR bit positions. Also, events can be emulated by software by writing a ‘1’ in
the PORT_DIO*_INT_SET bit positions.



7.2 Registers

         7.2.1        PORT_DIOA_IN
DIO 00 to 31 port read data register
Address offset: 0x00
Reset value: 0x00000000

Field                      Bits    Type Reset   Description
PORT_DIOA_IN              [31:0]   RO   0       DIO 00 to 31 port read data



         7.2.2        PORT_DIOB_IN
DIO 32 to 43 port read data register
Address offset: 0x04
Reset value: 0x00000000

Field                      Bits    Type Reset   Description
PORT_DIOB_IN              [11:0]   RO   0       DIO 32 to 43 port read data



         7.2.3        PORT_DIOA_OUT
DIO 00 to 31 port write data register
Address offset: 0x10
Reset value: 0x00000000

Field                      Bits    Type Reset   Description
PORT_DIOA_OUT             [31:0]   RW   0       DIO 00 to 31 port write data




                                                                                               48
<PARSED TEXT FOR PAGE: 49 / 159>
                                                                          I/O ports controller (PORT)



         7.2.4          PORT_DIOB_OUT
DIO 32 to 43 port write data register
Address offset: 0x14
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOB_OUT             [11:0]   RW   0       DIO 32 to 43 port write data



         7.2.5          PORT_DIOA_INT_STATUS
DIO 00 to 31 port interrupt status register
Address offset: 0x20
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOA_INT_STATUS      [31:0]   RO   0       DIO 00 to 31 port interrupt status



         7.2.6          PORT_DIOB_INT_STATUS
DIO 32 to 43 port interrupt status register
Address offset: 0x24
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOB_INT_STATUS      [11:0]   RO   0       DIO 32 to 43 port interrupt status



         7.2.7          PORT_DIOA_INT_RISE_EN
DIO 00 to 31 port rise event enable register
Address offset: 0x30
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOA_INT_RISE_EN     [31:0]   RW   0       DIO 00 to 31 port rise event enable



         7.2.8          PORT_DIOB_INT_RISE_EN
DIO 32 to 43 port rise event enable register
Address offset: 0x34
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOB_INT_RISE_EN     [11:0]   RW   0       DIO 32 to 43 port rise event enable




                                                                                               49
<PARSED TEXT FOR PAGE: 50 / 159>
                                                                            I/O ports controller (PORT)



         7.2.9          PORT_DIOA_INT_FALL_EN
DIO 00 to 31 port fall event enable register
Address offset: 0x40
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOA_INT_FALL_EN     [31:0]   RW   0       DIO 00 to 31 port fall event enable



         7.2.10         PORT_DIOB_INT_FALL_EN
DIO 32 to 43 port fall event enable register
Address offset: 0x44
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOB_INT_FALL_EN     [11:0]   RW   0       DIO 32 to 43 port fall event enable



         7.2.11         PORT_DIOA_INT_SET
DIO 00 to 31 port interrupt set register
Address offset: 0x50
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOA_INT_SET         [31:0]   RW   0       DIO 00 to 31 port interrupt set. This field is always read as 0.
                                                Automatically returns to 0 after a write request



         7.2.12         PORT_DIOB_INT_SET
DIO 32 to 43 port interrupt set register
Address offset: 0x54
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PORT_DIOB_INT_SET         [11:0]   RW   0       DIO 32 to 43 port interrupt set. This field is always read as 0.
                                                Automatically returns to 0 after a write request



         7.2.13         PORT_DIOA_INT_CLEAR
DIO 00 to 31 port interrupt clear register
Address offset: 0x60
Reset value: 0x00000000




                                                                                                         50
<PARSED TEXT FOR PAGE: 51 / 159>
                                                                             I/O ports controller (PORT)


Field                      Bits     Type Reset   Description
PORT_DIOA_INT_CLEAR        [31:0]   RW   0       DIO 00 to 31 port interrupt clear. This field is always read as
                                                 0. Automatically returns to 0 after a write request



         7.2.14       PORT_DIOB_INT_CLEAR
DIO 32 to 43 port interrupt clear register
Address offset: 0x64
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
PORT_DIOB_INT_CLEAR        [11:0]   RW   0       DIO 32 to 43 port interrupt clear. This field is always read as
                                                 0. Automatically returns to 0 after a write request



         7.2.15       PORT_LFXO_CFG
LFXO ports configuration register
Address offset: 0x80
Reset value: 0x00000001

Field                      Bits     Type Reset   Description
PORT_LFXO_MODE             [1:0]    RW   1       LFXO mode:
                                                 When 0x1: Crystal mode
                                                 When 0x2: External clock (no crystal)



         7.2.16       PORT_HFXO_CFG
HFXO ports configuration register
Address offset: 0x84
Reset value: 0x00000002

Field                      Bits     Type Reset   Description
PORT_HFXO_MODE             [1:0]    RW   2       HFXO mode:
                                                 0x1: Crystal mode
                                                 0x2: External clock (no crystal)
PORT_HFXO_FSEL             [3:2]    RW   0       HFXO frequency range selection



         7.2.17       PORT_DIO_CFG_x (x = 0 to 43)
DIO ports configuration register
Address offset: 0x90 + x * 4
Reset value: 0x00000000




                                                                                                          51
<PARSED TEXT FOR PAGE: 52 / 159>
                                                                              I/O ports controller (PORT)


Field                         Bits     Type Reset   Description
PORT_DIO_FUNCTION             [1:0]    RW       0   Port functional mode
PORT_DIO_IN_EN                [2]      RW       0   Port input buffer enable
PORT_DIO_OUT_EN               [3]      RW       0   Port output buffer enable
PORT_DIO_DRIVE                [5:4]    RW       0   Port drive setting
PORT_DIO_SR                   [6]      RW       0   Port slew rate setting
PORT_DIO_IN_CFG               [7]      RW       0   Port input Schmitt trigger enable
PORT_DIO_PULL_CFG             [8]      RW       0   Port weak pull setting:
                                                    0: Pull-down
                                                    1: Pull-up
PORT_DIO_PULL_EN              [9]      RW       0   Port weak pull enable




7.3 Electrical and timing specification
Each DIO is linked to a physical I/O cell, thus having some limitations in terms of operating
frequency or voltage level. These parameters are specified in the following table.

                                                               Output max     Output max     Output max
 PORT_DIO_     PORT_      Driving
                                        Slew rate   C load     frequency      frequency      frequency
 DRIVE[1:0]    DIO_SR     capability
                                                               VDDIO=1.8V     VDDIO=2.5V     VDDIO=3.3V

        00         0      2 mA          Fast        5 pF       30 MHz         50 MHz         75 MHz

        01         0      4 mA          Fast        10 pF      40 MHz         60 MHz         90 MHz

        10         0      8 mA          Fast        20pF       50 MHz         75 MHz         100 MHz

        11         0      12 mA         Fast        30 pF      60 MHz         90 MHz         120 MHz

        00         1      2 mA          Slow        5 pF       20 MHz         40 MHz         50 MHz

        01         1      4 mA          Slow        10 pF      20 MHz         40 MHz         50 MHz

        10         1      8 mA          Slow        20 pF      20 MHz         40 MHz         50 MHz

        11         1      12 mA         Slow        30 pF      20 MHz         40 MHz         50 MHz

Table 15: I/O output drive and slew rate


 Symbol          Parameter                                         Min.             Max.          Unit

 IO_PUR          I/O internal pull-up resistor                     9.3              27,5          kΩ

 IO_PDR          I/O internal pull-down resistor                   6,8              28,8          kΩ

 VIO_IN_H        I/O input voltage, high level                     0.7 * VDDIO      VDDIO         V

 VIO_IN_L        I/O input voltage, low level                      GND              0.3 * VDDIO   V

 VIO_OUT_H       I/O output voltage, high level                    0.8 * VDDIO      VDDIO         V

 VIO_OUT_L       I/O output voltage, low level                     GND              0.2 * VDDIO   V

 VYO_HYS         Schmitt trigger hysteresis (when enabled)         0.2                  -         V

 IO_IN_FMAX      I/O maximum input frequency, regular DIO          -                70            MHz

                 I/O maximum input frequency, I2C IO (DIO.02
 IO_I2C_FMAX                                                       -                50            MHz
                 and DIO.03)




                                                                                                       52
<PARSED TEXT FOR PAGE: 53 / 159>
                                                            I/O ports controller (PORT)


 Symbol         Parameter                            Min.       Max.           Unit

 IO_PCLK_FMAX   PCLK input clock maximum frequency   -          70             MHz

Table 16: I/O specifications




                                                                                 53
<PARSED TEXT FOR PAGE: 54 / 159>
                                                           Direct memory access controller (DMA)




8        Direct memory access controller (DMA)

8.1 Functional description

          8.1.1        Overview
The DMA enhances block-to-block data transfers efficiently by taking charge of the control required
to perform these operations instead of using a software loop. During a data transfer, the CPU is
completely free to perform other operations or may enter into standby mode.

A data transfer consists of reading a group of data from a source and writing them to a destination.
The source and destination addresses indicate where to start the transfer, with auto-increment of
the read/write pointers. The transfer can be memory-to-peripheral, peripheral-to-memory or
memory-to-memory.

The DMA features 8 channels, allowing up to 8 transfers configured and launched at the same time
with possibly time-interleaved data read or write. The transfers with higher priority will be served
first when multiple channels are ready simultaneously. If a high-priority channel is active but waiting
for data readiness, the other channels will be able to resume.


          8.1.2        Transfers configuration
Each channel can be configured and launched by programming the corresponding set of registers.
For channel X (X = 0 to 7), the following registers are used:
     ● DMA_CHAN_CTRL_X – Control and status of the transfer

     ● DMA_LEN_X – Number of bytes that should be transferred
     ● DMA_SRC_X – Source start address

     ● DMA_DST_X – Destination start address

     ● DMA_SDL_X – Stride length parameter

     ● DMA_SKL_X – Stride skip length parameter



The source and destination addresses should be programmed according to the global memory map
of the chip. For a communication peripheral, the address is generally the register address allowing to
read from an RX FIFO or to write to a TX FIFO. For a memory, the address corresponds to the first
address to which the first byte transferred should be fetched.

DMA_LEN_X must be set to the desired number of bytes minus 1 for the full transfer.
In case the source of the transfer is a peripheral FIFO, i.e. the data is read from a fixed address, the
DMA_PERIPH_SRC_EN bit must be set to ‘1’. Otherwise it should be set to ‘0’ to allow auto-increment
of the read pointer. In addition, the DMA_PERIPH_SRC_ID should be set according to Table 17.

In case the destination of the transfer is a peripheral FIFO, i.e. the data is written to a fixed address,
the DMA_PERIPH_DST_EN bit must be set to ‘1’. Otherwise it should be set to ‘0’ to allow auto-




                                                                                                   54
<PARSED TEXT FOR PAGE: 55 / 159>
                                                          Direct memory access controller (DMA)


increment of the write pointer. In addition, the DMA_PERIPH_DST_ID should be set according to
Table 17 and DMA_DST_APB_EN must be set to ‘1’.

The DMA_SRC_WIDTH and DMA_DST_WIDTH are used to configure the data width that should be
transferred for the source and the destination over each active cycle:
    ● DMA_*_WIDTH = 0x0 → 1 byte
    ● DMA_*_WIDTH = 0x1 → 2 bytes

    ● DMA_*_WIDTH = 0x2 → 4 bytes



The default is 4-byte transfers to optimize the transfer time and AXI bandwidth. However some
peripherals only allow byte transfers and a smaller transfer width needs to be configured.

The configuration related to strided transfers is described in the following paragraphs.

    Global control and status
The priority of the transfer for a given channel is set in DMA_PRIORITY. The higher the value, the
higher the priority. When several transfers are started but queued because another transfer is on
going, the channels with higher priority are served first. It is recommended to use higher priority for
channel involving a peripheral because its FIFO is limited in size.

Once configured, the transfer can be launched by writing ‘1’ into DMA_START. The transfer stops
automatically once the requested number of bytes has been transferred. The same transfer is
relaunched automatically if DMA_RESTART_EN has been set to ‘1’.

The transfer can also be interrupted by writing ‘1’ into DMA_STOP. Once stopped, the transfer cannot
be resumed and should be configured and started again.

The DMA_RUNNING flag indicates when the current channel is active.

Caution: when using the DMA with a peripheral FIFO, additional configuration to the peripheral
may be required and the peripheral should be enabled after starting the transfer.

    Peripherals ID
Each communication peripheral is associated with an ID, allowing the DMA to handle correctly the
transfer with its internal FIFO. The following table indicates the ID to use when targeting such a
peripheral.

     Peripheral   Associated ID

        SPIS            0

        SPIM0           1

        SPIM1           2

        SPIMF           3

        UART            4

        I2CM            5

        CAMIF           6

  Table 17: Peripherals ID


                                                                                                55
<PARSED TEXT FOR PAGE: 56 / 159>
                                                          Direct memory access controller (DMA)



       Strided transfers
This mode is enabled when the DMA_STRIDE_EN bit is set to ‘1’. In that case, the DMA_SDL_X and
DMA_SKL_X registers are used to define the Stride Data Length (SDL) and SKip Length (SKL)
parameters. This mode allows memory-to-memory copy with a configurable pattern. A first group of
SDL consecutive bytes are transferred, then the read pointer is incremented by SKL. The transfer
continues with another group of SDL bytes and so on, until the transfer length is reached.

The following pseudo-code illustrates the transfer behavior when copying bytes from a source[]
array to a destination[] array.

   source_ptr = DMA_SRC_x
   destination_ptr = DMA_DST_x
   while transfer active:
         for i = 1 to SDL:
                destination[destination_ptr] = source[source_ptr]
                destination_ptr =       destination_ptr + 1
                source_ptr =      source_ptr + 1
         source_ptr =     source_ptr + SKL


           8.1.3        DMA events and interrupt
Each channel can generate three interrupt events, recorded in the DMA_INT_STATUS register:

   ●    DMA_EVT_DONE: Indicates a regular end of transfer that was previously initiated. The event is
        not generated if DMA_RESTART_EN = 1.

   ●    DMA_EVT_READ_ERROR: event raised when an internal bus read error is detected. This may
        occur when reading from an unmapped address.

   ●    DMA_EVT_WRITE_ERROR: event raised when an internal bus write error is detected. This may
        occur when writing to an unmapped address.

On an event detection for a given channel, the corresponding flag bit in the DMA_INT_STATUS
register is set to ‘1’. Each event field is 8 bits wide, corresponding to the 8 available channels. The
flag bits can be individually cleared by writing a ‘1’ in the DMA_INT_CLEAR bit positions. Also, events
can be emulated in software by writing a ‘1’ in the DMA_INT_SET bit positions.

The DMA peripheral interrupt signal is asserted each time any IRQ-enabled DMA event occurs. An
event is IRQ-enabled if its corresponding bit in the DMA_INT_EN register is set to ‘1’.



8.2 Registers

           8.2.1        DMA_VERSION
DMA hardware version register
Address offset: 0x0000



                                                                                                56
<PARSED TEXT FOR PAGE: 57 / 159>
                                                          Direct memory access controller (DMA)


Reset value: 0x00010001

Field                     Bits     Type Reset   Description
DMA_VERSION_MINOR         [15:0] RO     1       DMA version minor
DMA_VERSION_MAJOR         [31:16] RO    1       DMA version major



        8.2.2         DMA_INT_STATUS
DMA interrupt status register
Address offset: 0x0004
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DMA_EVT_DONE              [7:0]   RO    0       Event latched on transfer done, per channel
DMA_EVT_READ_ERROR        [15:8] RO     0       Event latched on read transfer error, per channel
DMA_EVT_WRITE_ERROR       [23:16] RO    0       Event latched on write transfer error, per channel



        8.2.3         DMA_INT_EN
DMA interrupt enable register
Address offset: 0x0008
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DMA_INT_EN                [23:0]   RW   0       When an event has its corresponding enable bit set, it will
                                                contribute to IRQ generation



        8.2.4         DMA_INT_SET
DMA interrupt set register
Address offset: 0x000C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DMA_INT_SET               [23:0]   RW   0       Allows to emulate events by software by writing ‘1’ to
                                                corresponding positions. This field is always read as 0.
                                                Automatically returns to 0 after a write request



        8.2.5         DMA_INT_CLEAR
DMA interrupt clear register
Address offset: 0x0010
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DMA_INT_CLEAR             [23:0]   RW   0       Allows to erase event flags of DMA_INT_STATUS by writing ‘1’
                                                to corresponding positions. This field is always read as 0.
                                                Automatically returns to 0 after a write request



                                                                                                           57
<PARSED TEXT FOR PAGE: 58 / 159>
                                                           Direct memory access controller (DMA)




           8.2.6      DMA_CHAN_CTRL_x (x = 0 to 7)
Channel control register
Address offset: 0x0020 + x * 4
Reset value: 0x00808000

Field                      Bits     Type Reset   Description
DMA_START                  [0]      RW   0       When written ‘1’, starts DMA transfer for this channel. This
                                                 field is always read as 0. Automatically returns to 0 after a
                                                 write request
DMA_STOP                   [1]      RW   0       When written ‘1’, stops current DMA transfer for this
                                                 channel . This field is always read as 0. Automatically returns
                                                 to 0 after a write request
DMA_RUNNING                [2]      RO   0       Flag indicating when this channel is active
DMA_RESTART_EN             [3]      RW   0       When ‘1’, the DMA transfers restarts automatically on copy
                                                 completion. Otherwise, the DMA transfers stops when over
DMA_PRIORITY               [5:4]    RW   0       DMA priority for this channel. The higher value, the higher
                                                 priority over other channels
DMA_STRIDE_EN              [6]      RW   0       When ‘1’, strided transfers are enabled
DMA_PERIPH_SRC_EN          [8]      RW   0       When ‘1’, the source data are fetched from a peripheral. The
                                                 read pointer is not incremented
DMA_PERIPH_SRC_ID          [11:9] RW     0       Peripheral RX trigger ID
DMA_SRC_WIDTH              [15:14] RW    2       Source peripheral data transfer width in byte
DMA_PERIPH_DST_EN          [16]    RW    0       When ‘1’, the destination data are written to a peripheral. The
                                                 write pointer is not incremented
DMA_PERIPH_DST_ID          [19:17] RW    0       Peripheral TX trigger ID
DMA_DST_WIDTH              [23:22] RW    2       Destination peripheral data transfer width in byte
DMA_EXT_TRIG_EN            [24]    RW    0       When ‘1’, an external trigger can start the DMA channel
DMA_EXT_TRIG_ID            [26:25] RW    0       External trigger selection
DMA_DST_APB_EN             [31]    RW    0       When ‘1’, indicates that the destination is reached through an
                                                 APB interface and only 32-bit word can be transferred



           8.2.7      DMA_LEN_x (x = 0 to 7)
Transfer length configuration register
Address offset: 0x0040 + x * 4
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
DMA_LEN                    [31:0]   RW   0       Number of bytes to transfer = DMA_LEN + 1



           8.2.8      DMA_SRC_x (x = 0 to 7)
Source address configuration register
Address offset: 0x0060 + x * 4
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
DMA_SRC                    [31:0]   RW   0       Source start address of the transfer




                                                                                                        58
<PARSED TEXT FOR PAGE: 59 / 159>
                                                           Direct memory access controller (DMA)



          8.2.9       DMA_DST_x (x = 0 to 7)
Destination address configuration register
Address offset: 0x0080 + x * 4
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
DMA_DST                    [31:0]   RW   0       Destination start address of the transfer



          8.2.10      DMA_SDL_x (x = 0 to 7)
Stride data length configuration register
Address offset: 0x00A0 + x * 4
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
DMA_SDL                    [31:0]   RW   0       Stride data byte length = DMA_SDL + 1



          8.2.11      DMA_SKL_x (x = 0 to 7)
Stride skip length configuration register
Address offset: 0x00C0 + x * 4
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
DMA_SKL                    [31:0]   RW   0       Stride skip byte length = DMA_SKL + 1




                                                                                             59
<PARSED TEXT FOR PAGE: 60 / 159>
                                                                        Camera interface (CAMIF)




9      Camera interface (CAMIF)
The CAMIF peripheral facilitates the interfacing of any image sensor having a parallel interface,
commonly referred to as Digital Video Port (DVP) or Digital Camera Interface (DCMI). The stream of
pixels is sent to an internal FIFO or directly to the ISP peripheral through a pixel streaming bus
(PSB), an internal data bus dedicated to pixels stream. The PSB has three channels of 8-bit data,
thus it can transfer a full RGB or monochrome pixel per clock cycle.

The interface manager is flexible and can be adapted to different protocol and camera control
signals relationship. It supports multiple color/RAW input modes and data bit-width.

In this context, all images are supposed to be streamed row by row, starting with the top left pixel.




            Figure 10: CAMIF interfaces


9.1 Functional description

          9.1.1        Overview
The block diagram of the CAMIF is provided in Figure 11. This module serves three primary
functions:

    1. Protocol conversion: the signals of the parallel camera interface are converted to a PSB
       stream.

    2. Test pixels generation: pre-defined pixels can be generated for testing and check purposes.

    3. Configuration and status through the register bank (REGBANK) and events management.

The CDC_FIFO blocks in the figure allows the re-synchronization of data streams which are on
different clock domains. In case the ISP clock is selected as the CAM_PCLK, i.e. when
SCU_ISP_CLK_SRC = 0, the CDC_FIFO placed on the PSB data path can be bypassed by setting
CDC_FIFO_BYPASS to ‘1’. This slightly reduces the stream latency and power consumption.




                                                                                                60
<PARSED TEXT FOR PAGE: 61 / 159>
                                                                         Camera interface (CAMIF)


The PSB_CONVERTER sub-block ensures the protocol compatibility in terms of control signals are
data format. It is imperative that the registers are appropriately configured in accordance with the
camera specifications and signal behavior prior to receiving the first frame from the image sensor.




Figure 11: CAMIF block diagram


           9.1.2        Protocol conversion

       Parallel camera interface signals
A typical parallel camera interface comprises the following set of signals:

   ●    CAM_DATA[N-1:0] Parallel pixel data, coded on maximum N=12 bits.

   ●    CAM_PCLK The pixel clock provided by the camera. It can run at different frequencies and is
        used to sample the CAM_DATA bus.

   ●    CAM_HSYNC Sometimes referred to as HREF or HVLD. This signal is high for all cycles
        where a valid data is available on CAM_DATA.

   ●    CAM_VSYNC Sometimes referred to as FVLD. This vertical synchronization signal indicates
        the start of frames.

When using the CAMIF module as a camera interface, the pins of the chip must be configured in
the correct alternate mode.

       Supported video modes
The DATA_IN_MODE field should be used to select a video mode and pixel formatting according to
the configuration of the image sensor. Six pixel formats are supported by the CAMIF: RGB555,
RGB565, RGB444x, RGBx444, RAW and RGBSPLIT.



                                                                                             61
<PARSED TEXT FOR PAGE: 62 / 159>
                                                                               Camera interface (CAMIF)


The first four color modes are “standard” RGB modes over 8-wire transport. The three color
component of each pixel are transmitted by group of two consecutive 8-bit data – D1 and D2 –
with a pre-defined mapping. The bits mapping according to the selected mode is specified in Table
18 and must be identical to the mapping used by the image sensor.

The peripheral reconstructs a full RGB pixel with three 8-bit channels (red-green-blue) sent over the
PSB by replacing the missing bits with zeroes.

                                         Bits mapping for each RGB component
        Mode
                                 Red                    Green                   Blue

        RGB555                 D1[6:2]             {D1[1:0], D2[7:5]}          D2[4:0]

        RGB565                 D1[7:3]             {D1[2:0], D2[7:5]}          D2[4:0]

        RGB444x                D1[7:4]                  D1[3:0]                D2[7:4]

        RGBx444                D1[3:0]                  D2[7:4]                D2[3:0]

       Table 18: Supported 8-bit RGB modes

The RGBSPLIT and RAW modes support any number of valid bits from the camera, up to 12. This
number must be specified in the PIXEL_IN_WIDTH field. In both modes, one pixel data or pixel
channel is expected from the sensor per valid cycle. Pixel data are processed differently when the
number of bits differs than 8:

   ●    If more than 8 bits are received and configured in PIXEL_IN_WIDTH, the PSB will keep only
        the 8 most significant bits (MSB) of the sensor data. However, the data FIFO (described
        hereafter) will keep all transmitted bits.

   ●    If less than 8 bits are received and configured in PIXEL_IN_WIDTH, the missing bits are
        replaced with zeroes. Also the CONV_DATA_SHIFT field indicates how many zeroes are
        inserted to the LSBs of the pixel data, which effectively multiply each pixel value by 2 to the
        power of CONV_DATA_SHIFT.

The RAW mode should be used for single-channel images, for example images in grayscale or
colored with a Bayer pattern. Only one channel of the output PSB, over the three available, is used.

The RGBSPLIT mode should be used when RGB pixels are sent by groups of 3 active cycles, each
cycle corresponding to red, green or blue component. The RGBSPLIT_R_FIRST bit indicates
whether the received order is red-green-blue or blue-green-red. The three channels of the output
PSB are used in this mode.

In RGBSPLIT or RAW modes with an even number of pixel data bits, the NIBBLE_SPLIT_EN register
bit can be optionally set to ‘1’ to halve the width of the camera data bus. Each pixel data is split over
two active cycles and only half of each data bits are received during every CAM_PCLK cycle. For
example, if PIXEL_IN_WIDTH = 8 and NIBBLE_SPLIT_EN = 1, the CAM_DATA signal can be 4-bit
wide, requiring only 7 wires (instead of 11) between the camera and the chip. With this option
enabled, the first nibble may correspond to either the MSBs or the LSBs of the pixel data, which is
controlled by the NIBBLE_ENDIANNESS register bit.




                                                                                                  62
<PARSED TEXT FOR PAGE: 63 / 159>
                                                                        Camera interface (CAMIF)



    Supported camera protocols
Various camera protocols are supported and most sensor are supported, as long as the following
requirements are met:

   1. CAM_VSYNC should rise to indicate a start of frame. It can be high for one CAM_PCLK
      cycle or remain high throughout the entire frame transmission.

   2. The first valid pixel data of a frame, indicated with a high level on CAM_HSYNC for one
      clock cycle, may arrive either at the same time as the CAM_VSYNC first high cycle, or some
      cycles later.

   3. CAM_HSYNC must be high only for cycles during which a pixel data is actually transmitted.
      There is no restriction on the waveform of this signal, in particular it can be maintained high
      for any number of consecutive clocks cycles.

CAM_DATA, CAM_VSYNC and CAM_HSYNC signals are sampled by the CAMIF using the
CAM_PCLK clock. The PCLK_SAMPLING_CFG bit allows to choose whether the sampling point is at
the rising edge or the falling edge of CAM_PCLK. The sampling point selected should be the one
where signals are stable within the clock period.

The maximum input image size supported is 2048 in both directions (2048 x 2048 pixels).

    Image size configuration
In addition to the video mode configuration, the user needs to indicate the expected image width
and height in the CAMIF_IMG_SIZE_CFG register. The number of channels is inferred from the
configured video mode: one channel for RAW mode and three channels for all other modes.

A check is performed on the actual number of pixels received, resulting in an error event if it differs
from the configuration.

    Pixels counting and image status
Two internal row and column counters are employed to determine current position within the
received image. These counters are reset at each new frame start, triggered by a rising edge on the
CAM_VSYNC signal. The prevailing values of these counters are accessible through the
IMG_ROW_CNT and IMG_COL_CNT fields.
Additionally, a frame counter, which increments with each frame start, is integrated and mapped to
the IMG_COUNT field. The frame counter can be reset by writing ‘1’ into IMG_COUNT_RST.


          9.1.3        Direct data FIFO
The pixel data received from the image sensor are sent to the internal PSB, but they are also stored
in a 16-word FIFO, accessible through registers and DMA. The full 12-bit data precision or lower is
kept, meaning that each word is 12 bits possibly with zero padding.

The data stored in the FIFO can be fetched by reading the CAMIF_CAM_DATA register. Each time
this register is read, a word is popped from the FIFO. The current word level of the FIFO is provided
in the DATA_FIFO_WORD_NB register field. Also an interrupt event is generated each time a new



                                                                                                63
<PARSED TEXT FOR PAGE: 64 / 159>
                                                                             Camera interface (CAMIF)


word is stored in the FIFO while the number of words stored gets higher than or equal to
DATA_FIFO_THR.
Another possibility is to use the DMA with the source address set to CAMIF_CAM_DATA.

If the direct data FIFO functionality is not required, it will just fill up after receiving the first 16 pixel
data, without impacting the other CAMIF functionalities.


          9.1.4         Pattern generator
This block is employed to generate a PSB stream containing either pseudo-random or fixed-pattern
data coded on 8 bits. A common application is to check the full video data path and verify the
functionality of the processing (built-in self-test). Configuration and triggering are entirely
controlled through registers.

    General configuration
The pattern generator sends frame-based data to the output PSB. Each time the
PATGEN_FRAME_START register is written to ‘1’ and the block is idle – no frame transmission already
ongoing – a new frame is generated. The PATGEN_BUSY status indicates whether a frame is
currently being transmitted or if the block is idle. Also, in order to enable the pattern generator data
path towards the output PSB, the PATGEN_MUX_SEL register bit should be set to 1. In this case, the
functional path from the camera cannot be used.

The width, height and number of channels of the generated frame are respectively defined in the
IMG_WIDTH, IMG_HEIGHT and PATGEN_IMG_NCHAN register fields. IMG_WIDTH and IMG_HEIGHT
correspond to the same registers as for the functional mode.

By default, valid data is provided for all clock cycles and no empty cycles are inserted. However, if
the PATGEN_SLOW_RATE field is set higher than 0, each active cycle is followed by as many empty
cycles as the value defined in PATGEN_SLOW_RATE. For example, if this field is configured to 2, the
overall duration of a frame transmission will be tripled.

    Random data mode
If PATGEN_DATA_MODE = 0, all pixel channels are generated as a pseudo-random binary sequence
(PRBS). The random bits generation is based on a 32-bit linear feedback shift register (LFSR). The
state of the LFSR is reset by writing ‘1’ in the PATGEN_LFSR_RST bit field, which results in re-
generating the same sequence from the beginning.

If LFSR_AUTO_RESTART = 1, the LFSR state is reset at each frame start, meaning that all generated
frames are identical. On the other hand, if LFSR_AUTO_RESTART = 0, the LFSR state is kept between
frames, unless a block reset or LFSR reset happens.

When multi-channel pixels are generated, the first data channel is stored in the lowest PSB channel
index.

    Ramp data mode
If PATGEN_DATA_MODE = 1, pixel channels are generated incrementally as a ramp. Each row of an
image corresponds to the previous row incremented by 1 on all components. The components of


                                                                                                       64
<PARSED TEXT FOR PAGE: 65 / 159>
                                                                          Camera interface (CAMIF)


the first top-left pixel are defined in the PATGEN_RAMP_INIT0, PATGEN_RAMP_INIT1, and
PATGEN_RAMP_INIT2.
If the initial pixel components are (A,B,C) and Pxy(z) is the pixel channel z at row y, column x then:

   P00(0) = A, P00(1) = B, P00(2) = C
   P10(0) = A+1, P10(1) = B+1, P10(2) = C+1
   P20(0) = A+2, P20(1) = B+2, P20(2) = C+2
   …
   P01(0) = A+1, P01(1) = B+1, P01(2) = C+1
   P11(0) = A+2, P11(1) = B+2, P11(2) = C+2
   …
   P02(0) = A+2, P02(1) = B+2, P02(2) = C+2
   P12(0) = A+3, P12(1) = B+3, P12(2) = C+3
   …

In case PATGEN_IMG_NCHAN = 1, only the PATGEN_RAMP_INIT0 register is used to define the initial
A value. The next pixels are still incremented by 1 in row and columns.


          9.1.5        Reference camera support
This list provides some image sensors references supported by the ColibryNPU (not exhaustive):

   ●   NT99141 from Novatek, HD CMOS Image Sensor

   ●   OV7670 from Omnivision, CMOS VGA Sensor

   ●   HM0360 from Himax, VGA 60 FPS CMOS Image Sensor

   ●   HM01B0 from Himax, QVGA 60 FPS CMOS Image Sensor


          9.1.6        CAMIF events and interrupt
The CAMIF_INT_STATUS register records the following events:

   ●   EVT_IMG_OVERFLOW: Input image overflow, rises when more pixel data are received than
       expected, according to the image size configured in the registers.

   ●   EVT_IMG_UNDERFLOW: Input image underflow, rises on CAM_VSYNC rising edge when less
       pixel data are received than expected, according to the image size configured in the registers.

   ●   EVT_IMG_START: Input image start event. Event triggered at every image start from the
       sensor.

   ●   EVT_IMG_END: Input image end event. Event triggered when all expected pixels from an image
       are received.

   ●   EVT_DATA_FIFO_RDY: Data FIFO ready event. Rises each time a word is stored while the
       threshold is reached.

   ●   EVT_DATA_FIFO_OVF: Data FIFO overflow event. Rises when an overflow occurs in the Data
       FIFO.



                                                                                                   65
<PARSED TEXT FOR PAGE: 66 / 159>
                                                                            Camera interface (CAMIF)


On a an event detection, the corresponding flag bit in the CAMIF_INT_STATUS register is set to ‘1’.
The flag bits can be individually cleared by writing a ‘1’ in the CAMIF_INT_CLEAR bit positions. Also,
events can be emulated in software by writing a ‘1’ in the CAMIF_INT_SET bit positions.

The CAMIF peripheral interrupt signal is asserted each time any IRQ-enabled CAMIF event occurs.
An event is IRQ-enabled if its corresponding bit in the CAMIF_INT_EN register is set to ‘1’.



9.2 Registers

          9.2.1       CAMIF_PROTOCOL_CFG
Input protocol configuration register
Address offset: 0x00
Reset value: 0x00000180

Field                       Bits    Type Reset   Description
DATA_IN_MODE                [3:0]   RW   0       Camera input data mode.
                                                 0 : RAW
                                                 1 : RGBSPLIT
                                                 2 : RGB555
                                                 3 : RGB565
                                                 4 : RGB444x
                                                 5 : RGBx444
PIXEL_IN_WIDTH              [7:4]   RW   8       Number of bits on which pixels channels are coded in RAW
                                                 and RGBSPLIT modes. Max value is 14
RGBSPLIT_R_FIRST            [8]     RW   1       First component received in RGBSPLIT mode definition.
                                                 0 : Blue-Green-Red order
                                                 1 : Red-Green-Blue order
NIBBLE_SPLIT_EN             [9]     RW   0       Enable nibble mode transmission of pixel channels
NIBBLE_ENDIANNESS           [10]    RW   0       Nibble mode endianness definition.
                                                 0 : First nibble corresponds to the MSBs of the pixel channel
                                                 value
                                                 1 : First nibble corresponds to the LSBs of the pixel channel
                                                 value
PCLK_SAMPLING_CFG           [11]    RW   0       Sampling edge selection for CAM_PCLK clock.
                                                 0 : Signals sampled on rising edge
                                                 1 : Signals sampled on falling edge
CONV_DATA_SHIFT             [14:12] RW   0       Left shift value applied to the input pixel channel



          9.2.2       CAMIF_IMG_SIZE_CFG
Image size configuration register
Address offset: 0x04
Reset value: 0x007800A0

Field                       Bits    Type Reset   Description
IMG_WIDTH                   [12:0] RW    160     Expected input image width (in pixels)
IMG_HEIGHT                  [28:16] RW   120     Expected input image height (in rows of pixels)




                                                                                                        66
<PARSED TEXT FOR PAGE: 67 / 159>
                                                                           Camera interface (CAMIF)



         9.2.3       CAMIF_IMG_CURR_PTR
Image pixel pointer register
Address offset: 0x08
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
IMG_COL_CNT               [12:0] RO    0       Current image column counter value
IMG_ROW_CNT               [28:16] RO   0       Current image row counter value



         9.2.4       CAMIF_IMG_COUNT
Received images counter register
Address offset: 0x0C
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
IMG_COUNT                 [9:0]   RO   0       Current number of images received from the camera



         9.2.5       CAMIF_IMG_COUNT_RST
Image counter reset register
Address offset: 0x10
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
IMG_COUNT_RST             [0]     RW   0       Writing a 1 to this bit will clear the image counter provided in
                                               IMG_COUNT. This field is always read as 0. Automatically
                                               returns to 0 after a write request



         9.2.6       CAMIF_PATGEN_CONTROL
Pattern generator frame trigger register
Address offset: 0x14
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
PATGEN_FRAME_START        [0]     RW   0       Writing a 1 to this bit will start a frame transmission from the
                                               pattern generator. This field is always read as 0.
                                               Automatically returns to 0 after a write request
PATGEN_LFSR_RST           [1]     RW   0       Writing a 1 to this bit will reset the pattern generator LFSR.
                                               This field is always read as 0. Automatically returns to 0 after
                                               a write request



         9.2.7       CAMIF_PATGEN_CFG
Pattern generator configuration register


                                                                                                       67
<PARSED TEXT FOR PAGE: 68 / 159>
                                                                           Camera interface (CAMIF)


Address offset: 0x18
Reset value: 0x0000000C

Field                     Bits     Type Reset   Description
PATGEN_DATA_MODE          [0]      RW   0       Patten generator data mode.
                                                0 : Generate pseudo-random data
                                                1 : Generate incrementing data
PATGEN_MUX_SEL            [1]      RW   0       When ‘1’, the pattern generator data path is activated while
                                                the functional path is disabled
PATGEN_IMG_NCHAN          [3:2]    RW   3       Number of channels used by the pattern generator
PATGEN_SLOW_RATE          [11:4]   RW   0       Number of empty cycles inserted after a valid cycle. Max
                                                value is 255
LFSR_AUTO_RESTART         [12]     RW   0       When ‘1’, LFSR state is reset at each frame start



         9.2.8       CAMIF_PATGEN_RAMP_INIT
Pattern generator initial values register
Address offset: 0x1C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PATGEN_RAMP_INIT0         [9:0]   RW    0       Initial value for first component of generated pixels
PATGEN_RAMP_INIT1         [19:10] RW    0       Initial value for second component of generated pixels
PATGEN_RAMP_INIT2         [29:20] RW    0       Initial value for third component of generated pixels



         9.2.9       CAMIF_PATGEN_STATUS
Pattern generator status register
Address offset: 0x20
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
PATGEN_BUSY               [0]      RO   0       Indicates when the pattern generator is sending a frame



         9.2.10      CAMIF_CDC_FIFO_BYPASS
CDC FIFO bypass mode register
Address offset: 0x24
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
CDC_FIFO_BYPASS           [0]      RW   0       CDC FIFO bypass control.
                                                0 : FIFO utilized
                                                1 : FIFO bypassed



         9.2.11      CAMIF_CAM_DATA
Data FIFO read register

                                                                                                      68
<PARSED TEXT FOR PAGE: 69 / 159>
                                                                             Camera interface (CAMIF)


Address offset: 0x28
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
CAM_DATA                  [11:0]   RO   0       Direct data from Data FIFO



           9.2.12   CAMIF_DATA_FIFO_STATUS
Data FIFO status register
Address offset: 0x2C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DATA_FIFO_WORD_NB         [4:0]    RO   0       Current number of words stored in the Data FIFO



           9.2.13   CAMIF_DATA_FIFO_THR
Data FIFO threshold register
Address offset: 0x30
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DATA_FIFO_THR             [4:0]    RW   0       Threshold above which EVT_DATA_FIFO_RDY event is
                                                generated



           9.2.14   CAMIF_INT_STATUS
CAMIF interrupt status register
Address offset: 0x40
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
EVT_IMG_OVERFLOW          [0]      RO   0       Input image overflow event. Will rise when the event occurs
EVT_IMG_UNDERFLOW         [1]      RO   0       Input image underflow event. Will rise when the event occurs
EVT_IMG_START             [2]      RO   0       Input image start event. Will rise when the event occurs
EVT_IMG_END               [3]      RO   0       Input image end event. Will rise when the event occurs
EVT_DATA_FIFO_RDY         [4]      RO   0       Data FIFO ready event. Will rise each time a word is stored
                                                while the threshold is reached
EVT_DATA_FIFO_OVF         [5]      RO   0       Data FIFO overflow event. Will rise when an overflow occurs



           9.2.15   CAMIF_INT_EN
CAMIF interrupt enable register
Address offset: 0x44
Reset value: 0x00000000




                                                                                                     69
<PARSED TEXT FOR PAGE: 70 / 159>
                                                                           Camera interface (CAMIF)


Field                     Bits    Type Reset   Description
CAMIF_INT_EN              [5:0]   RW   0       Interrupt enable. Each event latched in the
                                               CAMIF_INT_STATUS register will generate an interrupt if the
                                               corresponding bit in this field is set



         9.2.16     CAMIF_INT_SET
CAMIF interrupt set register
Address offset: 0x48
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
CAMIF_INT_SET             [5:0]   RW   0       Interrupt set. For each bit written to 1, emulates an event by
                                               latching a 1 in the corresponding bits of the
                                               CAMIF_INT_STATUS register. This field is always read as 0.
                                               Automatically returns to 0 after a write request



         9.2.17     CAMIF_INT_CLEAR
CAMIF interrupt clear register
Address offset: 0x4C
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
CAMIF_INT_CLEAR           [5:0]   RW   0       Interrupt clear. For each bit written to 1, clears the
                                               corresponding bits of the CAMIF_INT_STATUS register. This
                                               field is always read as 0. Automatically returns to 0 after a
                                               write request




                                                                                                       70
<PARSED TEXT FOR PAGE: 71 / 159>
                                                                        Image signal processor (ISP)




10 Image signal processor (ISP)
The ISP used in ColibryNPU employs several image processing sub-blocks chained together with a
proprietary Pixel Streaming Bus (PSB). The goal of this ISP is to remain lightweight in terms of
hardware resources and processing power while offering a reasonable compromise on processing
quality. The ISP additionally integrates a register bank for configuration and status of the different
sub-blocks.

The ISP includes the following characteristics:
       ●    Integrated register bank with APB interface
       ●    Pixel Streaming Bus (PSB) between IPs
       ●    Support for RGB, grayscale or raw Bayer color input image
       ●    Image cropping, down-sampling, conversion from RGB/Bayer to grayscale or RGB
       ●    Interrupts management


10.1               Functional description




 Figure 12: ISP block diagram


The goal of the ISP is to transform or modify some characteristics of an image provided as a stream
of pixels through the internal PSB. The block diagram of the ISP is provided in Figure 12. Each sub-
block ensures a specific transformation which can be parameterized:

   ●       CROPPER – Allows selecting a specific Region-Of-Interest from the input image.

   ●       BAYER2RGB – Raw Bayer color to RGB conversion for each pixel.

   ●       RGB2GRAY – Reduces an RGB image to a single channel, resulting in a grayscale conversion.

   ●       SCALER – Down-scale an input image to a new image size with a non-integer ratio.



                                                                                               71
<PARSED TEXT FOR PAGE: 72 / 159>
                                                                       Image signal processor (ISP)


These sub-blocks functionalities and configuration are detailed in the following sections.

Each sub-block can be bypassed, effectively disabling the associated functionality. This is achieved
by setting to ‘1’ the corresponding ISP_*_BYPASS bits of the ISP_CTRL register. When all sub-
blocks are bypassed, the images at the output of the ISP are identical to the input images provided
by the CAMIF.

Each sub-block of the ISP processes the pixels as they arrive at their respective inputs and
generates the output pixels with a small delay. Therefore, a sub-block gets busy between an input
frame start and end, and remains idle outside. The current busy status of each sub-block can be
read from the ISP_STATUS register.


          10.1.1       CROPPER description and configuration
The image cropper generates on the output PSB an extracted region of interest (ROI) from the input
image received from the input PSB.

A example is shown in the following figure to illustrate the cropping function.




 Figure 13: Cropping function example

After the cropper, only the pixels situated in the rectangle (the ROI) defined by its top-left and
bottom-right corners are transmitted. The pixels outside the defined rectangle are simply discarded.
The only configuration needed for the CROPPER is done through the ISP_CROPPER_ROI_TL and
ISP_CROPPER_ROI_BR registers, which define respectively the top-left and bottom-right
coordinates of the ROI. The pixels defining the outline of the rectangle are also transmitted.

The ISP_CROPPER_PSB_INFO status register is updated at every input frame start of the CROPPER.
It contains the input image size and number of channels retrieved from the PSB protocol.


          10.1.2       BAYER2RGB description and configuration
When capturing raw colored pixels from an image sensor, a commonly utilized method is the Bayer
pattern, also known as the color filter array. In this arrangement, each pixel within the sensor matrix
is either a red, blue or green color pixel. The color pattern is distributed with half pixels being green,
one quarter are red and one quarter are blue.


                                                                                                   72
<PARSED TEXT FOR PAGE: 73 / 159>
                                                                      Image signal processor (ISP)


Given that a complete RGB image consists of 3 channels per pixel, a process called demosaicing
becomes essential to reconstruct the missing values from the original raw image data. To this
purpose, the BAYER2RGB sub-block employs a bilinear interpolation technique, averaging adjacent
pixels of the same channel within a 3x3 receptive field. When the block is enabled and not
bypassed, the output image retains the same width and height as the original Bayer image, but gets
three color channels per pixel.

In terms of configuration, the BAYER2RGB only needs to know if the patterning is RGGB or BGGR,
which describes the color corresponding to a group of four adjacent pixels. If the first pixel of the
image (in the top-left corner) is red, the pattern is RGGB. Otherwise, if it is blue, the pattern is
BGGR. This information must be provided in the ISP_B2RGB_RGGB_MODE field.

The ISP_B2RGB_PSB_INFO status register is updated at every input frame start of the BAYER2RGB.
It contains the input image size and number of channels retrieved from the PSB protocol.


          10.1.3       RGB2GRAY description and configuration
The RGB2GRAY can be used to transform a color image in RGB format (three channels) into a
grayscale image (single channel). The luminance of each pixel of the image is obtained by combining
the three RGB channels.

There is no specific configuration required for this sub-block. If the conversion is not used, it should
be bypassed with the ISP_RGB2GRAY_BYPASS bit.

The ISP_RGB2GRAY_PSB_INFO status register is updated at every input frame start of the
RGB2GRAY. It contains the input image size and number of channels retrieved from the PSB
protocol.


          10.1.4       SCALER description and configuration
This sub-block allows scaling an input image, which has either one or three channels, to a smaller
image with a division factor higher than 1.0, but holding the same number of channels. An example
is shown in the Figure 14 below to illustrate the scaling function.

There are two scaling factors, one applied to the width of the image, the X direction, and another to
the height of the image, the Y direction. Both are coded in a 4.8 bits fixed-point format, meaning
that the integer part is coded on 4 bits (valid values ranging from 1 to 15, 0 is forbidden) and the
decimal part is coded on 8 bits (valid values ranging from 0 to 255). The decimal part, coded with an
integer, must be divided by 256 to obtain the actual meaning. For example, a ratio 3.625 is coded as
3 for the integer part and 0.625 x 256= 160 for the decimal part. The final scaling factor, coded as a
fixed-point value, is an integer equals to (3 << 8 ) + 160 = 928 or 0x3A0.

The ISP_SCALER_SCALING_X and ISP_SCALER_SCALING_Y fields must be used to configure the
scaling factors in the X and Y directions. Their reset value is 0x100, which corresponds to a scaling
factor of 1.0, i.e. no scaling.

In addition to these fields, the ISP_SCALER_SIZE_OUT_X and ISP_SCALER_SIZE_OUT_Y must be
configured to the exact output image size. This is required because the scaling factor multiplied by
the input image size in one direction may not be accurate enough to determine the exact output
size. The rule is to configure these fields so that their values are equal to or smaller than the size


                                                                                                 73
<PARSED TEXT FOR PAGE: 74 / 159>
                                                                         Image signal processor (ISP)


obtained by computing it from the scaling factor, otherwise an error event will be raised. The
following formulas can help in determining the scaling factors, in fixed-point format, from the
desired output image size:

  ISP_SCALER_SCALING_X = Floor(SIZE_IN_X * 256 / ISP_SCALER_SIZE_OUT_X)
  ISP_SCALER_SCALING_Y = Floor(SIZE_IN_Y * 256 / ISP_SCALER_SIZE_OUT_Y))

SIZE_IN_X and SIZE_IN_Y correspond to the input image width and height respectively. They should
be know in advance. The Floor() function rounds towards zero.




       Figure 14: Scaling function example with division factor = 3.4

ISP_SCALER_PSB_INFO status register is updated at every input frame start of the SCALER. It
contains the input image size and number of channels retrieved from the PSB protocol.


          10.1.5        ISP events and interrupt
A soft reset event of the ISP and all sub-blocks can be applied by writing ‘1’ in the ISP_SOFT_RESET
field. The internal state is reset but the registers configuration remains unchanged.

Some errors linked to the PSB protocol in each sub-block may appear and generate an error event.
This may occur when the actual number of pixels or image rows are not such as notified by the PSB
protocol itself. A mis-configuration or sizes outside limits can lead to this situation. In particular, it is
recommended to configure all sub-blocks of the ISP before receiving the first frame from the
CAMIF.

The ISP_INT_STATUS register records the following events:

   ●   ISP_CROPPER_PSB_ERR: A PSB error has been detected in the CROPPER sub-block.
   ●   ISP_B2RGB_PSB_ERR: A PSB error has been detected in the BAYER2RGB sub-block.
   ●   ISP_RGB2GRAY_PSB_ERR: A PSB error has been detected in the RGB2GRAY sub-block.
   ●   ISP_SCALER_PSB_ERR: A PSB error has been detected in the SCALER sub-block.


                                                                                                      74
<PARSED TEXT FOR PAGE: 75 / 159>
                                                                           Image signal processor (ISP)


   ●    ISP_SCALER_PARAM_ERR: Scaling parameters not correctly set. The configured output image
        size in X or Y directions is higher than the number of pixels generated by the SCALER.

   ●    ISP_FRAME_START: Input frame start, rises each time a new frame start is detected at ISP
        input.

On a an event detection, the corresponding flag bit in the ISP_INT_STATUS register is set to ‘1’. The
flag bits can be individually cleared by writing a ‘1’ in the ISP_INT_CLEAR bit positions. Also, events
can be emulated in software by writing a ‘1’ in the ISP_INT_SET bit positions.

The ISP peripheral interrupt signal is asserted each time any IRQ-enabled ISP event occurs. An
event is IRQ-enabled if its corresponding bit in the ISP_INT_EN register is set to ‘1’.



10.2             Registers
The ISP_MBIST_* registers are used for testing SRAM memories embedded in the ISP. They are
normally not used in application.


            10.2.1      ISP_CTRL
ISP global control register
Address offset: 0x00
Reset value: 0x0000000F

Field                        Bits   Type Reset    Description
ISP_CROPPER_BYPASS           [0]    RW    1       CROPPER bypass control.
                                                  0 : functional mode
                                                  1 : block bypassed and clock gated
ISP_BAYER2RGB_BYPASS         [1]    RW    1       BAYER2RGB bypass control.
                                                  0 : functional mode
                                                  1 : block bypassed and clock gated
ISP_RGB2GRAY_BYPASS          [2]    RW    1       RGB2GRAY bypass control.
                                                  0 : functional mode
                                                  1 : block bypassed and clock gated
ISP_SCALER_BYPASS            [3]    RW    1       SCALER bypass control.
                                                  0 : functional mode
                                                  1 : block bypassed and clock gated
ISP_SOFT_RESET               [8]    RW    0       Writing a 1 to this bit will generate a reset of the ISP and sub-
                                                  blocks, except for the REGBANK configuration. This field is
                                                  always read as 0. Automatically returns to 0 after a write
                                                  request



            10.2.2      ISP_STATUS
ISP global status register
Address offset: 0x04
Reset value: 0x00000000




                                                                                                           75
<PARSED TEXT FOR PAGE: 76 / 159>
                                                                        Image signal processor (ISP)


Field                     Bits     Type Reset   Description
ISP_CROPPER_BUSY          [0]      RO   0       This bit indicates the busy status of the CROPPER
ISP_BAYER2RGB_BUSY        [1]      RO   0       This bit indicates the busy status of the BAYER2RGB
ISP_RGB2GRAY_BUSY         [2]      RO   0       This bit indicates the busy status of the RGB2GRAY
ISP_SCALER_BUSY           [3]      RO   0       This bit indicates the busy status of the SCALER



         10.2.3        ISP_CROPPER_PSB_INFO
CROPPER input frame information register
Address offset: 0x10
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
ISP_CROPPER_PSB_INFO      [23:0]   RO   0       PSB configuration word corresponding to last frame received
                                                by the CROPPER.
                                                Bits [10:0] : Input image width - 1
                                                Bits [21:11] : Input image height - 1
                                                Bits [23:22] : Input image number of channels - 1



         10.2.4        ISP_CROPPER_ROI_TL
ROI top-left corner coordinate register
Address offset: 0x14
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
ISP_CROPPER_ROI_TL_X      [10:0]   RW   0       Configuration of the top-left corner of the output ROI. This
                                                10-bit value defines its X coordinate
ISP_CROPPER_ROI_TL_Y      [26:16] RW    0       Configuration of the top-left corner of the output ROI. This
                                                10-bit value defines its Y coordinate



         10.2.5        ISP_CROPPER_ROI_BR
ROI bottom-right corner coordinate register
Address offset: 0x18
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
ISP_CROPPER_ROI_BR_X      [10:0]   RW   0       Configuration of the bottom-right corner of the output ROI.
                                                This 10-bit value defines its X coordinate
ISP_CROPPER_ROI_BR_Y      [26:16] RW    0       Configuration of the bottom-right corner of the output ROI.
                                                This 10-bit value defines its Y coordinate



         10.2.6        ISP_B2RGB_PSB_INFO
BAYER2RGB input frame information register
Address offset: 0x20
Reset value: 0x00000000

                                                                                                       76
<PARSED TEXT FOR PAGE: 77 / 159>
                                                                       Image signal processor (ISP)



Field                     Bits     Type Reset   Description
ISP_B2RGB_PSB_INFO        [23:0]   RO   0       PSB configuration word corresponding to last frame received
                                                by the BAYER2RGB.
                                                Bits [10:0] : Input image width - 1
                                                Bits [21:11] : Input image height - 1
                                                Bits [23:22] : Input image number of channels - 1



         10.2.7         ISP_B2RGB_RGGB_MODE
BAYER2RGB pattern mode register
Address offset: 0x24
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
ISP_B2RGB_RGGB_MODE       [0]      RW   0       Bayer color pattern mode received by the block. Shall be set
                                                accordingly to the camera output format.
                                                0 : BGGR pattern
                                                1 : RGGB pattern



         10.2.8         ISP_RGB2GRAY_PSB_INFO
RGB2GRAY input frame information register
Address offset: 0x28
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
ISP_RGB2GRAY_PSB_INFO     [23:0]   RO   0       PSB configuration word corresponding to last frame received
                                                by the RGB2GRAY.
                                                Bits [10:0] : Input image width - 1
                                                Bits [21:11] : Input image height - 1
                                                Bits [23:22] : Input image number of channels - 1



         10.2.9         ISP_SCALER_PSB_INFO
SCALER input frame information register
Address offset: 0x30
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
ISP_SCALER_PSB_INFO       [23:0]   RO   0       PSB configuration word corresponding to last frame received
                                                by the SCALER.
                                                Bits [10:0] : Input image width - 1
                                                Bits [21:11] : Input image height - 1
                                                Bits [23:22] : Input image number of channels - 1




                                                                                                      77
<PARSED TEXT FOR PAGE: 78 / 159>
                                                                        Image signal processor (ISP)



         10.2.10 ISP_SCALER_SCALING_CFG
Image scaling factor configuration register
Address offset: 0x34
Reset value: 0x01000100

Field                     Bits     Type Reset   Description
ISP_SCALER_SCALING_X      [11:0]   RW   0x100   Scaling factor in X direction in fixed-point format 4.8 bits.
                                                Image width is divided by this value, so should be ≥ 1.0 (coded
                                                as 0x100)
ISP_SCALER_SCALING_Y      [27:16] RW    0x100   Scaling factor in Y direction in fixed-point format 4.8 bits.
                                                Image height is divided by this value, so should be ≥ 1.0
                                                (coded as 0x100)



         10.2.11 ISP_SCALER_SIZE_OUT
SCALER output image size register
Address offset: 0x38
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
ISP_SCALER_SIZE_OUT_X     [11:0] RW     0       SCALER exact output image width
ISP_SCALER_SIZE_OUT_Y     [27:16] RW    0       SCALER exact output image height



         10.2.12 ISP_INT_STATUS
ISP interrupt status register
Address offset: 0x40
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
ISP_CROPPER_PSB_ERR       [0]      RO   0       PSB error detection in CROPPER event. Will rise when the
                                                event occurs
ISP_B2RGB_PSB_ERR         [1]      RO   0       PSB error detection in BAYER2RGB event. Will rise when the
                                                event occurs
ISP_RGB2GRAY_PSB_ERR      [2]      RO   0       PSB error detection in RGB2GARY event. Will rise when the
                                                event occurs
ISP_SCALER_PSB_ERR        [3]      RO   0       PSB error detection in SCALER event. Will rise when the
                                                event occurs
ISP_SCALER_PARAM_ERR      [4]      RO   0       Scaling parameters errors in SCALER event. Will rise when the
                                                event occurs
ISP_FRAME_START           [5]      RO   0       ISP frame start detection event. Will rise when the event
                                                occurs



         10.2.13 ISP_INT_EN
ISP interrupt enable register
Address offset: 0x44
Reset value: 0x00000000


                                                                                                       78
<PARSED TEXT FOR PAGE: 79 / 159>
                                                                          Image signal processor (ISP)


Field                        Bits    Type Reset   Description
ISP_INT_EN                   [5:0]   RW   0       Interrupt enable. Each event latched in the ISP_INT_STATUS
                                                  register will generate an interrupt if the corresponding bit in
                                                  this field is set



             10.2.14 ISP_INT_SET
ISP interrupt set register
Address offset: 0x48
Reset value: 0x00000000

Field                        Bits    Type Reset   Description
ISP_INT_SET                  [5:0]   RW   0       Interrupt set. For each bit written to 1, emulates an event by
                                                  latching a 1 in the corresponding bits of the ISP_INT_STATUS
                                                  register. This field is always read as 0. Automatically returns
                                                  to 0 after a write request



             10.2.15 ISP_INT_CLEAR
ISP interrupt clear register
Address offset: 0x4C
Reset value: 0x00000000

Field                        Bits    Type Reset   Description
ISP_INT_CLEAR                [5:0]   RW   0       Interrupt clear. For each bit written to 1, clears the
                                                  corresponding bits of the ISP_INT_STATUS register. This field
                                                  is always read as 0. Automatically returns to 0 after a write
                                                  request



             10.2.16 ISP_MBIST_CTRL
ISP SRAM BIST control register
Address offset: 0x60
Reset value: 0x00000000

Field                        Bits    Type Reset   Description
ISP_MBIST_RUN                [0]     RWS 0        Launch memory BIST test



             10.2.17 ISP_MBIST_STATUS
ISP SRAM BIST status register
Address offset: 0x64
Reset value: 0x00000000




                                                                                                          79
<PARSED TEXT FOR PAGE: 80 / 159>
                                                                      Image signal processor (ISP)


Field                     Bits    Type Reset   Description
ISP_MBIST_DONE            [0]     RO   0       Set by HW on memory BIST completion. Cleared by HW when
                                               BIST starts
ISP_MBIST_ANY_FAIL        [1]     RO   0       Set by HW on any memory BIST failure. Cleared by HW when
                                               BIST starts



          10.2.18 ISP_MBIST_FAIL
SRAM BIST fail register
Address offset: 0x68
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
ISP_MBIST_FAIL            [1:0]   RO   0       BIST status of SRAM cuts 0 to 1




                                                                                                80
<PARSED TEXT FOR PAGE: 81 / 159>
                                                             Neural Network Accelerator (NNPA)




11 Neural Network Accelerator (NNPA)
The Neural Network Processing Accelerator (NNPA) enables the execution of neural networks (NN)
inferences in a very efficient manner, without requiring CPU intervention during the processing. It
contains all needed SRAM-based memory buffers to store locally both the configuration and
weights of the neural network, as well as the data required for its execution. This architecture uses
Near-Memory Computing, meaning that the memory and the computational blocks are physically
placed close together to enhance efficiency.



11.1           Neural network inference flow
Several steps are necessary to run inferences of a pre-trained NN inside the NNPA:

    1) Train a NN for a given task.
    2) Convert the model into an NNPA bitstream. The bitstream contains an encoded
       representation of the neural network structure and its weights.
    3) Load the NNPA bitstream into the NNPA MEMORY.
    4) Configure the NNPA through its registers
    5) Load model inputs through the ISP or directly in the NNPA MEMORY.
    6) Run the inference.
    7) Fetch the model outputs from the NNPA MEMORY.

Steps 1) and 2) are not described in this document as they are not directly related to the chip.
However, the others steps and the way the NNPA can be configured are widely described in the
following sections.

Steps 1) to 4) are generally done once for a given application, whereas multiple inferences of the
same NN over different input data blocks can be performed by looping the steps 5) to 7). In most
applications, the NNPA bitstream is pre-loaded into an external flash memory, then the SPIMF
peripheral can be used to read it from the flash and write it in the NNPA MEMORY.



11.2           Functional description
The NNPA communicates with the rest of the system through the AXI bus, may receive image data
through the PSB, and can generate an interrupt towards the CPU. It features several sub-blocks,
each having specific function:

  ●     The INPUT_LOADER is in charge of placing all input data of the neural network into the
NNPA MEMORY. These data can be the pixels of an image, sound samples or any other type of data
provided by a sensor. Depending on the configuration and parameters of the pre-loaded NN model,
the input data shall be mapped according to a specific pattern. The INPUT_LOADER facilitates this
operation by taking the data in-order and then writes them in the memory to the expected
positions. Data are loaded from the PSB or are written through the REGBANK in a FIFO-like style.



                                                                                              81
<PARSED TEXT FOR PAGE: 82 / 159>
                                                            Neural Network Accelerator (NNPA)


  ●    The PROCESSING ELEMENTS contains the hardware operators allowing to compute 16
       multiply-accumulate (MAC) operations per clock cycle and supporting integer rescaling and
       non-linear ReLU operations.

  ●    The register bank (REGBANK) retains configuration, control and status registers of the NNPA.
       The AXI interface allows to address all registers of the NNPA.

  ●    The IRQ_GENERATOR manages the different events of the NNPA by latching them in a
       status register and can be used to generate an interrupt towards the CPU sub-system.

  ●    The NNPA MEMORY is detailed in section 11.2.2.




      Figure 15: NNPA block diagram


The NNPA MEMORY can be accessed through the PSB, when loading data from the ISP, or from the
system AXI bus. When this memory needs to be read or written through the AXI, either from the
CPU, the DMA, or another AXI master, the CPU_MEM_ACCESS register bit must be set to ‘1’
beforehand. Otherwise, it should be set to ‘0’ by default when the NNPA is running inferences or
when data are loaded through the PSB.


          11.2.1       Clocks and reset
The NNPA has an internal partial reset which can be asserted by writing ‘1’ in the BLOCK_RESET bit.
The reset is released by writing back a ‘0’ in the same bit. This reset does not impact the REGBANK
and the registers configuration.

When the NNPA is not performing any processing, its internal clock is stopped by default. This
behavior can be modified by setting the NNPA_CLK_FORCE_EN bit to ‘1’. In that case, the clock is
always active.



                                                                                            82
<PARSED TEXT FOR PAGE: 83 / 159>
                                                             Neural Network Accelerator (NNPA)


The NNPA MEMORY manages separately the clock for each of its internal buffer. A buffer without
an access request at a time has its clock stopped by default. This behavior can be modified by
setting the MEM_CLK_FORCE_EN bit to ‘1’. In that case, the clock is always active for all buffers.


          11.2.2      NNPA MEMORY description
The NNPA MEMORY sub-block consists of a pool of 40 static random-access memory (SRAM)
buffers, each with a size of 16 kBytes, for a total of 640 kBytes. As shown in Figure 4, the full NNPA
memory space can be accessed directly in read and write modes through the AXI bus, starting from
address 0x30001000. This interface allows reading of 4 bytes at a time.

The buffer number k has a start address defined as: 0x30001000 + k * 0x4000.

The NNPA memory can also be addressed in terms of NNPA memory words. In this context, a word
consists of 16 consecutive bytes and each buffer contains 1024 words. When referring to this
addressing mode, the NNPA memory addresses range from 0 (0x0000) to 40959 (0x9FFF).

To convert an address in memory words to its corresponding global address of the chip, the
following formula can be used: global_address = 0x30001000 + (nnpa_word_address * 16).

    Memory organization
Three types of data are stored in the NNPA MEMORY:

    1) The NN configuration (model structure and weights) as defined by the NNPA bitstream.
    2) The NN input data, for example an RGB image.
    3) The NN activations which corresponds to the intermediate data of any layer.
For a given model, the number of buffers required to store these data can be determined in
advance. This information is reported by the model conversion tool and stored within the NN
configuration itself.

The NN inputs can be written directly by the CPU or the DMA, starting in a new buffer. To optimize
the memory space, it is recommended to start in the next free buffer after the NN configuration.

Finally the NN activations space will be filled by the NNPA while running inferences.

    Loading an NNPA bitstream
The NNPA bitstream is a file containing N bytes of encoded information. It should be copied “as is”
into the NNPA MEMORY at consecutive addresses, from byte 0 to byte N-1. The AXI bus is 4 bytes
wide and uses little endian format. This means the first byte of the NNPA bitstream is placed on
axi_data<7:0>, the second byte on axi_data<15:8>, and so on.

Before writing the bitstream into the memory, the CONFIG_START_MADDR register should be set to
the start address of the NN configuration, using the word addressing mode. For example when
starting at address 0x0 of the Buffer #0, the register should be set to 0.

The CONFIG_START_MADDR can also be defined afterwards to change the pointer to the NN
configuration section. In that case, and when a bitstream has already been stored in memory, the
CONFIG_RELOAD bit should be written to ‘1’. This action will reload the configuration and populate
the RD_* status registers with the information contained in the corresponding bitstream, as long as


                                                                                               83
<PARSED TEXT FOR PAGE: 84 / 159>
                                                            Neural Network Accelerator (NNPA)


the NNPA clock has been enabled. In the rest of this document, when referring to a “loaded”
bitstream or configuration, it means that the RD_* registers contain the information related to this
model.

Multiple NN configurations are supported. The different bitstreams can be stored consecutively in
the memory, the only constraint is that a bitstream starts at a 16-byte word boundary. Before
launching the inference, the CONFIG_START_MADDR should be set to the relevant start address for
which the NN configuration must be run, followed by a configuration reloading.

    Extracting the NNPA bitstream size
It can be helpful to know the bitstream size before it is loaded in memory, typically when the DMA
is used to transfer the bitstream from a memory location to another. This information is contained
in the bitstream header, which is a concatenation of several fields, not described in this document.
The bitstream size, in NNPA memory words, is mapped to bits 16 to 31 of the bitstream. It can be
computed as: B2 + (B3 << 8) , Bi being the ith byte of the bitstream.

    Buffers allocation
After the bitstream has been loaded, the configuration of the buffers allocation needs to be done.
This is part of the step 4) described in section 11.1. The NNPA has to know the start and end
addresses of the different memory spaces, which are derived from some configuration registers.

Figure 16 illustrates a possible buffers allocation: Three buffers are used to store the bitstream –
Note that Buffer #2 may be only partially filled with data. Buffers #3 and #4 are used to store the
input data and Buffers #5 to #8 are needed for the activations. In this example, Buffers #9 to #39
are not used by the NNPA. They can, however, be used for other purposes by the system or be
configured in shutdown mode, as explained in the following sections.




Figure 16: Example of NNPA MEMORY buffers allocation

The NN inputs word start address is configured in the INPUT_START_MADDR register and should be a
multiple of 1024 to start at a buffer boundary. To optimize the memory space, this address can be
calculated as: Floor((RD_MODEL_SIZE + 1023) / 1024). The RD_MODEL_SIZE register contains the
loaded model size in NNPA memory words.



                                                                                             84
<PARSED TEXT FOR PAGE: 85 / 159>
                                                              Neural Network Accelerator (NNPA)


The NN activations word start address is configured in DATA_START_MADDR and can be defined in
either of two ways:

  ●    It is equal to the one defined for the NN inputs, i.e. DATA_START_MADDR =
       INPUT_START_MADDR. That means activations and inputs share the same memory area, and
       the inputs will probably be overwritten during the inference. In this mode, the total number of
       buffers required is lower but a new set of input data can only be loaded while the NNPA is
       idle (no inference running).

  ●    This area is exclusive of the NN input area. This happens by defining its start address to the
       beginning of a free buffer after the last word required to store the NN inputs. This mode
       needs more memory buffers but has the advantage of keeping untouched the input data. Also
       new input data (e.g. the next image to analyze) can be loaded by the PSB in the buffer while
       an inference is running, saving NN inputs loading time.

The number of buffers used by the NN inputs can be calculated as: Floor((RD_INPUT_SIZE + 1023) /
1024). The RD_INPUT_SIZE register contains the loaded model size in NNPA memory words.

In the example given above, the addresses would be set as follows: CONFIG_START_MADDR = 0,
INPUT_START_MADDR = 3072 and DATA_START_MADDR = 5120.

      Declaring unused buffers
Unused buffers are defined as those not required by the NNPA for its processing. Setting correctly
the number of unused buffers is better for power consumption or memory space availability. This
information is provided automatically or manually:

  ●    If the NB_DATA_BUFF register is set to 0, the number of unused buffers is automatically
       determined from the model data requirements stored in the NN configuration, also available
       in the RD_DATA_SIZE register and the DATA_START_MADDR configured field.

  ●    If the NB_DATA_BUFF register is configured to a value different from 0, the number of unused
       buffers is computed as: 40 - Floor((DATA_START_MADDR + 1023) / 1024) - NB_DATA_BUFF.
       NB_DATA_BUFF should correspond to the minimum number of buffers used by the NNPA to
       store its activations during inference.

Finally, the address of the last used memory word computed by the NNPA can be read from the
DATA_END_MADDR register. It corresponds to the last address of the last buffer needed by the NNPA
for running inferences given the loaded NN configuration.

      Buffers power control
In order to save power consumption, unused buffers of the NNPA MEMORY can be set individually
in shutdown mode. When in shutdown, a memory buffer cannot be used in read or write mode and
its content is lost.

If the MANUAL_BUFF_PD bit is set to ‘0’, unused buffers as defined previously, are automatically put
in shutdown mode. On the other hand, if MANUAL_BUFF_PD is set to ‘1’, the user can select which
buffers are shut down individually. This is achieved by configuring the MEM_BUFF_PD0 and
MEM_BUFF_PD1 registers.
In addition to the shutdown mode, the full memory can be turned to sleep mode, which is a lighter
low-power mode. When enabled, the read or write cannot be used but the content of all buffers is

                                                                                               85
<PARSED TEXT FOR PAGE: 86 / 159>
                                                                 Neural Network Accelerator (NNPA)


kept. When MEM_BUFF_SLEEP_AUTO is set to ‘1’, the memory sleep mode is controlled by the NNPA
and enabled only when the block is in idle, i.e. not running an inference and not accessed through
the AXI bus. When MEM_BUFF_SLEEP_AUTO is set to ‘0’, the sleep mode is directly controlled by the
MEM_BUFF_SLEEP_EN register bit.

       Low-voltage management
The MEM_BUFF_CUTS_CTRL register allows to control read and write assists and timing margins,
which can be used to reach the lowest voltage supply (< 0.8 V) while remaining functional. The
configuration provided in this register is outside the scope of this document.

       Memory conflicts
The NNPA MEMORY can be read or written at the same time by different sources as long as the
multiple accesses are done in different buffers. The system is built so that simultaneous access to
the same buffer does not occur. However, if the situation happens, a running inference will probably
provide erroneous results and the source of the error should be identified.

When it happens, a memory conflict will generate a EVT_MEM_CONFLICT event and the
MEM_CONFLICT_INFO register is updated with some information about the conflict:
   ●    MEM_CONFLICT_SOURCE is a 4-bit field indicating the sources involved in the conflict. Each bit
        at ‘1’ corresponds to a conflicting source with the following mapping.

                    Bit index   Corresponding source

                        0       External access (AXI or PSB)

                        1       Read from configuration/weights area

                        2       Read from activations area

                        3       Write to activations area


   ●    MEM_CONFLICT_BUFFER indicates the #index of the buffer in which the conflict occurred
A conflict could be due to a wrong buffers allocation, trying to access the NNPA MEMORY while an
inference is running or an issue in the NN model/configuration.

       Buffers testing
NNPA_MBIST_* registers are dedicated to memory Built-In Self-Test (BIST), which allows to detect
defective memory cells with predefined write-read sequences. They are normally not used in
application.


           11.2.3       Bitstream identifier and integrity check
Each converted model is associated with a 16-bit identifier computed at the bitstream generation.
This identifier can be used to distinguish different models as there are few chances that two models
have the same identifier. Once a bitstream has been loaded or reloaded, its identifier can be read
from the RD_MODEL_ID register.



                                                                                               86
<PARSED TEXT FOR PAGE: 87 / 159>
                                                              Neural Network Accelerator (NNPA)


The identifier can also be used to ensure the integrity of the loaded bitstream. The NNPA module
features an automatic integrity checker based on a CRC calculation. The checker is launched by
writing a ‘1’ in the CONFIG_CHECK bit of the NNPA_TRIG_CONTROL register. As this procedure takes
some time, an EVT_CONFIG_CHECK_END event is generated once it terminates. The
NNPA_INT_STATUS register can be polled to detect this event or it can be mapped to the NNPA
peripheral interrupt. Also the NNPA_BUSY status bit remains high during the procedure.

The result of the check is given by the CONFIG_CHECK_OK bit. If it is ‘1’ after the check processing, it
means the bitstream is correctly loaded in memory. Otherwise, the NN configuration is probably
broken and running the inference may return incorrect results. The last computed CRC is also
available in the CONFIG_CHECK_CRC register.


          11.2.4       Input data loading
To run an inference, the input data of the model should be available in the NNPA MEMORY
beforehand. This can be done in three ways:

    1) Using the PSB interface. Pixels are provided by the ISP, transported through the PSB and
       written in the NNPA memory with the help of the INPUT LOADER.
    2) Writing directly the input feature map in memory through the AXI. These data have to be
       stored in a specific order as described below.
    3) Using the registers interface of the INPUT LOADER. The input data are provided “in order”
       in a FIFO-like style and the reordering taken in charge by the INPUT LOADER.
When working with data from an external camera (CAMIF→ISP→ NNPA), the method 1) should be
used. Otherwise, for other type of data, it is recommended to employ the method 3) which
facilitates the data loading. In all cases, it is assumed that a model configuration has already been
loaded and that the *_START_MADDR registers have been correctly configured, as detailed in section
11.2.2.

Note that each input data is necessarily coded on one byte once stored in the NNPA MEMORY,
which can then store up to 16 input data per memory word.

    INPUT LOADER description
The INPUT LOADER gets the input feature map size information from the loaded configuration, i.e.
is uses the RD_* pre-loaded fields. It also knows in advance how many valid data should be received
from the PSB or through the registers interface. At any time, the current number of data received by
the block and transferred in the NNPA MEMORY is available in the INPUT_LOADER_LEVEL register.

The INPUT_LOADER_FULL status bit rises when the expected number of data or pixels has been
received, meaning that an inference is ready to be launched. The bit returns to ‘0’ upon inference
start or when the INPUT LOADER is restarted. The EVT_DATA_LOADED event is also raised each
time the number of expected inputs is reached.

The block can be restarted by writing ‘1’ in the INPUT_LOADER_RST bit. The effect of this is to
restart the internal counters and to abort any started frame transfer. After this, the next data
handled by the INPUT LOADER should be the first data of a new frame. Normally the INPUT
LOADER does not need to be restarted, a new frame can be loaded after the inference without this
action.


                                                                                                 87
<PARSED TEXT FOR PAGE: 88 / 159>
                                                              Neural Network Accelerator (NNPA)


Two additional events are generated by the INPUT LOADER:
   ●    EVT_INPUT_LOADER_OVF: Input loader overflow event. Rises when the input loader receives
        more data than expected

   ●    EVT_ISP_SIZE_ERR: ISP size error event. Rises when the input image received from the ISP
        does not match the size read from the loaded NN configuration

       Loading with the PSB interface
Before a frame transfer is started at the PSB input, the CPU_MEM_ACCESS bit must be set to ‘0’. The
INPUT_LOADING_RDY status bit indicates when the data can be loaded through the PSB. In case the
NN activations and NN inputs are in exclusive memory areas, a new set of inputs can be loaded
right after the first layer of an inference has finished. The INPUT_LOADING_RDY bit should be
checked before enabling the transfer of an image through the video pipe or the scheduling of the
video stream and the inferences must be carefully synchronized to avoid unwanted overlapping.

       Direct writing in memory
In this mode, the CPU_MEM_ACCESS bit must be set to ‘1’. The NNPA expects the input data in a
specific order which depends on the feature map size and whether it is flattened or not. A flattened
feature map has a single dimension and consists of an ordered list of data. The information related
to the input feature map of a pre-loaded NN is available in the RD_IFMAP_INFO and
RD_IFMAP_SIZE registers.
In case of flattened feature map, the input data are written “in-order” in memory. The NNPA
MEMORY can be represented as a 2D array with a mem_row and mem_col indexes. This array has 16
columns, corresponding to the number of input data that can be stored in a row – or memory word.
In that case, the input data with index k is stored at mem_row = Floor(k / 16) + INPUT_START_MADDR
and mem_col = k % 16. Here the “%” operator represents the modulo function.

In case of unflattened feature map, the order in which the input data should be stored in the NNPA
MEMORY is more complex. The following Python sample code gives the mem_row and mem_col
where input data {x,y,z} should be stored. {x,y,z} represents the 8-bit data of the input feature map
of size (Width, Height, Depth) at column x, row y and depth z.
nb_depth_groups = (Depth + 7) // 8
row_incr = ((Width + 1) // 2) * nb_depth_groups
packing_level = 3 if Depth == 1 else 2 if Depth == 2 else 1 if Depth <= 4 else 0

mem_row = (y * row_incr) + (x // 2) * nb_depth_groups + (z // 8)
mem_col = ((x + y) % 2) * 8 + (z % 8)
if packing_level == 0:
         mem_row = mem_row + INPUT_START_MADDR
else:
         addr_mask = 2**packing_level – 1
         mem_col = mem_col + (mem_row & addr_mask) * (8 >> packing_level)
         mem_row = (mem_row >> packing_level) + INPUT_START_MADDR


Note: in the above code, the “//” operator is an integer division, rounding the result towards zero.

                                                                                                88
<PARSED TEXT FOR PAGE: 89 / 159>
                                                                         Neural Network Accelerator (NNPA)


To illustrate how the input feature map needs to be stored in memory, an example is provided in
Figure 17.




   Figure 17: Inputs mapping example for width=3, height=2, depth=10)

     Loading through INPUT LOADER registers
The INPUT LOADER can be used to facilitate the mapping of the input feature map of a given NN
in the NNPA MEMORY. When combined with the DMA peripheral, any type of data not flowing
through the PSB path can be efficiently copied in memory, before running the inference. The
INPUT_LOADER_CFG register allows configuring the block before its use. Also the CPU_MEM_ACCESS
bit must be set to ‘1’.

This method suppose that an input feature map can be provided in a sequence of data, depth-first
and column-first. With the example used in the Figure 17, the sequence would be: {0,0,0}, {0,0,1}, ...,
{0,0,8}, {0,0,9}, {1,0,0}, {1,0,1}, ..., {1,0,8}, {1,0,9}, {2,0,0}, {2,0,1}, ..., {2,0,8}, {2,0,9}, {0,1,0}, {0,1,1}, ...,
{0,1,8}, {0,1,9}, {1,1,0}, {1,1,1}, ..., {1,1,8}, {1,1,9}, {2,1,0}, {2,1,1}, ..., {2,1,8}, {2,1,9}. These data are
transferred by writing them, in this order, in the 32-bit INPUT_LOADER_DATA register.

The INPUT LOADER can receive from one to four data at a time. When multiple data are provided
per cycle, the first ones shall be put on the LSBs of the data register. For example, if two input bytes
are provided, the first one should be written into INPUT_LOADER_DATA[7:0] and the second one,
simultaneously, into INPUT_LOADER_DATA[15:8]. The number of bytes provided each time
INPUT_LOADER_DATA is written to must be configured in the LOADER_NBYTE_VALID field.
When the input feature map is flattened, as indicated in IFMAP_FLATTENED, the number of multiple
data cannot be 3, i.e. LOADER_NBYTE_VALID cannot be set to 2 in this case.



                                                                                                                  89
<PARSED TEXT FOR PAGE: 90 / 159>
                                                              Neural Network Accelerator (NNPA)


Input data with higher bit-width can also be loaded through the INPUT LOADER. In this case, only
one data at a time can be written in the INPUT_LOADER_DATA register. The data should be aligned
on the LSB of the register. The actual number of bits must be indicated in the LOADER_NBIT_MODE
field. The data encoding can be signed in 2’s complement format or unsigned, this should be
indicated in the LOADER_SIGNED_DATA bit. Note that at the end, once written into memory, each
data can only be coded on 8 bits after the offset/scaling operation described in section 11.2.5.

Once an entire input feature map has been loaded, it is recommended to check that the loading ran
as expected: the EVT_DATA_LOADED and INPUT_LOADER_FULL bits should be ‘1’ and
EVT_INPUT_LOADER_OVF should be ‘0’. The EVT_DATA_LOADED should be cleared to detect again
this event.


          11.2.5       Input data scaling
The INPUT LOADER applies an offset and a scaling factor to all data loaded through this block,
either when using the PSB or the registers interface. By default, the offset is 0 and the scaling factor
is 1.0, which means the input data are not modified. However, when the input data of the training
dataset do not spread over the full 8-bit range (0 to 255), a scaling/offset may be needed to reach
full NN performances.

The INPUT_SCALING_FXP and INPUT_OFFSET_INT fields are used to configure respectively the
scaling factor in 4.12 fixed-point format and the offset in 2’s complement integer format. The offset
can be defined between -215 and 215-1 and the 16-bit scaling factor can be defined between 1/2 12
and 16–1/212.

The following operation is applied to all input data separately:

       input_new = (input_old + INPUT_OFFSET_INT) * INPUT_SCALING_FXP / 212

input_new is then rounded to the nearest integer and clipped between 0 and 255 before being
stored in the NNPA MEMORY.

The NNPA bitstream generally contains the required offset and scaling factor information, which is
stored in the RD_INPUT_SCALING and RD_INPUT_OFFSET registers. The same coding format as
above is utilized for these register values and can they be directly copied in the
INPUT_SCALING_FXP and INPUT_OFFSET_INT fields.


          11.2.6       Running inferences
Once the full input feature map of a NN is loaded in the NNPA_MEMORY, the inference can be
launched. The CPU_MEM_ACCESS bit must be set to ‘0’ if it is not already to this value, and the
RUN_START bit must be written to ‘1’. The NNPA_BUSY status bit indicates when the processing is
running and returns to ‘0’ when finished. At the end of the inference, the EVT_EXEC_END event is
also triggered.

The NNPA processes the layers one by one in the order given by the NN configuration. The current
layer under processing is indicated in the RUN_LAYER_IDX status field, starting with layer index 0.

A 24-bit inference counter is incremented each time an inference terminates. The current value of
the counter is available in INFERENCE_COUNTER. It can be reset to 0 by writing ‘1’ in the
INF_COUNTER_CLEAR bit.

                                                                                                 90
<PARSED TEXT FOR PAGE: 91 / 159>
                                                             Neural Network Accelerator (NNPA)


When using the PSB to load the input data, the inference can be automatically launched on the last
input data reception. The feature is enabled when the AUTO_START_EN bit is set to ‘1’. This mode
should not be used in other loading modes, because the CPU_MEM_ACCESS bit must be set to ‘0’
before the inference starts.


          11.2.7      Accessing neural network outputs
At the end of an inference, the NN output bytes are stored in the NNPA MEMORY starting at the
memory word indicated in OFMAP_START_MADDR. The first four data are then available at AXI
address 0x30001000 + OFMAP_START_MADDR * 16, the next four data at address 0x30001004 +
OFMAP_START_MADDR * 16, and so on. Before reading from the memory, the CPU_MEM_ACCESS bit
must be set to ‘1’ to give the CPU access to the AXI bus.

The NN output feature map has the same format in memory as the input feature map, as described
in 11.2.4, Direct writing in memory. Generally the output feature map is flattened and the outputs
are accessed in the same order as in the NN model. The size and configuration of the output feature
map can be read from the RD_OFMAP_INFO and RD_OFMAP_SIZE registers.

    Floating-point conversion
At the NNPA output, each data is represented by an integer between 0 and 255 because the
inference is run on a quantized model. However, it may be necessary to convert the outputs back to
decimal values using the RD_OUTPUT_OFFSET and RD_OUTPUT_SCALING registers.

The following operation can be applied to all output data separately to retrieve their floating-point
representation:

       output_float = (output_int + RD_OUTPUT_OFFSET) * RD_OUTPUT_SCALING / 216

RD_OUTPUT_OFFSET field is coded in 16 bits using 2’s complement format, so the integer offset in
the formula is negative if RD_OUTPUT_OFFSET [15] = 1. Note that the output in fixed-point (or
integer) format can be preserved by removing the division by 2 16 in the formula, keeping in mind
that the result in floating point is obtained by dividing by this factor.

    Final activations
The activations of the last layer of any neural network cannot generally be processed by the NNPA
because they consist of functions applied to floating-point or fixed-point values. They could be, for
example, a sigmoid or a softmax function. Such final activations can be handled by the RISC-V core
as part of the required post-processing, once the outputs have been converted to floating-point or
fixed-point values.


          11.2.8      Supported layers and operators
The neural processing unit is mainly based on hardware operators and a controller which schedules
and drives all the control signals needed. The operations on data are performed with 8-bit integer
logic, so the weights and internal data are all represented as 8-bit integers. Because the controller
and the operators are specialized for neural processing, some limitations and constraints must be
taken into consideration.



                                                                                              91
<PARSED TEXT FOR PAGE: 92 / 159>
                                                                      Neural Network Accelerator (NNPA)



    Neural network structure
The NNPA supports feed-forward NN structures, optionally with residual connections. Residual
connections may provide better accuracy results but require more memory to store intermediate
features maps.

The output of the network can be split in different branches with different activations, such as a
group of outputs with linear activation and a group of outputs with softmax activations. In any case,
the final actions are generally not supported by the NNPA and are part of the post-processing.

    Supported layers and parameters
The following table specifies the types of layers natively supported by the NNPA, meaning that they
are accelerated and optimally processed. The valid ranges of the parameters are given as integer
values. For instance, [1:8] means any integer between 1 and 8 and ([1:4], [1:4]) means any
combination of two parameters between 1 and 4.

It is worth noting that this table specifies maximum limits without taking into account the NNPA
memory capacity limits.

The maximum number of layers allowed in the model after conversion is 1023.

 Layer name
                      Description                                       Parameters range
 (as in Tensorflow)


                                                                        width (x) – range [1:1024]
 Input or Output      Input or output layers, (x,y,z) size.             height (y) – range [1:1024]
                                                                        depth(z) – range [1:4096]


                                                                        filters – range [1:4096]
                                                                        kernel_size – range ([1:64], [1:64])
                      Convolution of N filters with a 2D kernel         strides – range ([1:64], [1:64])
 Conv2D
                      over the entire input feature-map depth.          padding – ‘valid’ or ‘same’
                                                                        dilation_rate – range (1,1) only
                                                                        activation – see Activation layer


                                                                        depth_multiplier – only 1 supported
                                                                        kernel_size – range ([1:64], [1:64])
                      Convolution of N filters with a 2D kernel but     strides – range ([1:64], [1:64])
 DepthwiseConv2D      separately for each input channels (or
                      depth).                                           padding – ‘valid’ or ‘same’
                                                                        dilation_rate – range (1,1) only
                                                                        activation – see Activation layer


                      Quite similar to a DepthwiseConv2D                kernel_size – range ([1:64], [1:64])
 MaxPooling2D         convolution but takes the max of the values       strides – range ([1:64], [1:64])
                      instead of a multiply-accumulate operation.       padding – ‘valid’ or ‘same’


                                                                        kernel_size – range ([1:64], [1:64])
                      Quite similar to a DepthwiseConv2D                strides – range ([1:64], [1:64])
 AveragePooling2D     convolution but takes the mean of the
                      values instead of a multiply-accumulate.          padding – ‘valid’
                                                                                   ‘same’ not supported




                                                                                                               92
<PARSED TEXT FOR PAGE: 93 / 159>
                                                             Neural Network Accelerator (NNPA)



 Layer name
                      Description                               Parameters range
 (as in Tensorflow)


                      Fully-connected layer without weight      units – range [1:65536]
 Dense
                      sharing.                                  activation – see Activation layer


                                                                linear, relu. Other type of activation
 Activation           Activation function.                      only supported on the last layer and will
                                                                be computed in post-processing



    Special XTEN layers
XTENsion layer is a specific type of layer which can be inserted at model conversion time. When
such layer is decoded by the NNPA, the processing is paused, which means the CPU can take
control of the memory to perform specific operations. While in pause, the NNPA_BUSY bit remains
high and the XTEN_PAUSE_STATE bit is raised to ‘1’. In addition, the EVT_XTEN_PAUSE event is
triggered.

The processing can be resumed, restarting with the next layer in the NN configuration, by writing a
‘1’ in XTEN_PAUSE_RESUME. The XTEN_PAUSE_STATE bit returns to ‘0’ once the pause finishes.

During the pause, the CPU can retrieve some information about the corresponding XTEN layer from
the RD_* registers. In addition, the XTEN_IFMAP_MADDR, XTEN_OFMAP_MADDR and
XTEN_WEIGHT_MADDR registers provide respectively the start address, in memory words, for the
layer input feature map, output feature map and the weights list.

Each layer in the converted model has a structure containing some parameters, which is stored in
the NN configuration. The global address of the first byte of the parameters structure associated to
the XTEN layer can be determined as follows:

address = 0x30001000 + (CONFIG_START_MADDR * 16) + 32 + (24 * RUN_LAYER_IDX)

The RUN_LAYER_IDX field indicates the XTEN layer index during the pause.

Reading the parameters structure in memory requires setting CPU_MEM_ACCESS to ‘1’ beforehand. It
must be written back to ‘0’ before resuming from the pause. The structure is a concatenation of
fields related to the layer, as shown in Figure 18. When reading the first 32-bit word at the address
given above, the field mapped to bits [3:0] should be set to 0x5, indicating an XTEN layer type.




Figure 18: XTEN layer parameters structure



          11.2.9      NNPA events and interrupt
The NNPA_INT_STATUS register records the following events:



                                                                                                    93
<PARSED TEXT FOR PAGE: 94 / 159>
                                                                   Neural Network Accelerator (NNPA)


   ●   EVT_DATA_LOADED: Data loaded event. Rises each time the INPUT LOADER has received all
       expected input data of a frame.

   ●   EVT_CONFIG_CHECK_END: Configuration checking end event. Rises at the end of NN
       configuration check procedure.

   ●   EVT_EXEC_START: Execution start event. Rises every time an inference is launched.
   ●   EVT_EXEC_END: Execution end event. Rises every time an inference terminates.
   ●   EVT_XTEN_PAUSE: XTEN layer pause event. Rises on start of an XTEN layer, pausing the
       NNPA execution.

   ●   EVT_MEM_CONFLICT: Memory conflict event. Rises when a memory conflict is detected.
   ●   EVT_INPUT_LOADER_OVF: Input loader overflow event. Rises when the input loader receives
       more data than expected.

   ●   EVT_ISP_SIZE_ERR: ISP size error event. Rises when the input image received from the ISP
       does not match the size retrieved from the loaded NN configuration.

On a an event detection, the corresponding flag bit in the NNPA_INT_STATUS register is set to ‘1’.
The flag bits can be individually cleared by writing a ‘1’ in the NNPA_INT_CLEAR bit positions. Also,
events can be emulated in software by writing a ‘1’ in the NNPA_INT_SET bit positions.

The NNPA peripheral interrupt signal is asserted each time any IRQ-enabled NNPA event occurs.
An event is IRQ-enabled if its corresponding bit in the NNPA_INT_EN register is set to ‘1’.



11.3              NNPA specifications summary
 Parameter                                                   Value                   Unit

 Maximum processing frequency (NNPA_CLK)                     330 (best conditions)   MHz

 Multiply-Accumulate (MAC) per cycle                         16                      MAC/cycle

 Max compute power (MAC)                                     5.28                    GMAC/s

 Max compute power (OPerations)                              10.56                   GOP/s

 SRAM capacity (model + data)                                640                     kBytes

 Data format                                                 8-bit integer           -

 Weights format                                              8-bit integer           -

 VDD typical power consumption (30 MHz, VDD=0.8V) (*)        20                      uJ/inference

 VDD average power during inference (30 MHz, VDD=0.8V) (*)   1.3                     mW


 (*) Mesured on the VWW use-case (MobileNetV1) MLPerf Tiny benchmark

Table 19: NNPA specifications summary




                                                                                                    94
<PARSED TEXT FOR PAGE: 95 / 159>
                                                             Neural Network Accelerator (NNPA)



11.4            Registers

         11.4.1     NNPA_VERSION
NNPA hardware version register
Address offset: 0x00
Reset value: 0x00010000

Field                     Bits   Type Reset   Description
NNPA_REV_MINOR            [15:0] RO    0      NNPA hardware revision minor
NNPA_REV_MAJOR            [31:16] RO   1      NNPA hardware revision major



         11.4.2     NNPA_TRIG_CONTROL
NNPA triggers control register
Address offset: 0x04
Reset value: 0x00000000

Field                     Bits   Type Reset   Description
CONFIG_RELOAD             [0]    RW    0      Writing ‘1’ reloads the NN configuration (NNPA bitstream)
                                              stored at CONFIG_START_MADDR address. This field is
                                              always read as 0. Automatically returns to 0 after a write
                                              request
CONFIG_CHECK              [1]    RW    0      Writing ‘1’ launches the NN configuration check procedure.
                                              This field is always read as 0. Automatically returns to 0 after
                                              a write request
INPUT_LOADER_RST          [2]    RW    0      Writing ‘1’ restarts the INPUT LOADER and discards any
                                              previously loaded data. This field is always read as 0.
                                              Automatically returns to 0 after a write request
RUN_START                 [3]    RW    0      Writing ‘1’ starts NNPA inference processing. This field is
                                              always read as 0. Automatically returns to 0 after a write
                                              request
INF_COUNTER_CLEAR         [4]    RW    0      Writing ‘1’ erases the inference counter
                                              INFERENCE_COUNTER. This field is always read as 0.
                                              Automatically returns to 0 after a write request
XTEN_PAUSE_RESUME         [5]    RW    0      Writing ‘1’ resumes the NNPA processing after an XTEN
                                              pause. This field is always read as 0. Automatically returns to
                                              0 after a write request



         11.4.3     NNPA_CONTROL
NNPA main control register
Address offset: 0x08
Reset value: 0x00000000




                                                                                                       95
<PARSED TEXT FOR PAGE: 96 / 159>
                                                              Neural Network Accelerator (NNPA)


Field                     Bits     Type Reset   Description
BLOCK_RESET               [0]      RW   0       When ‘1’, the NNPA core is under reset but the registers
                                                configuration and memory content are preserved
MANUAL_BUFF_PD            [1]      RW   0       Manual control of buffers power mode. When this bit is set,
                                                MEM_BUFF_PWR_CTRLx is used to define each buffer power
                                                mode
AUTO_START_EN             [2]      RW   0       When ‘1’, the NN inference processing is automatically
                                                started once all input data are received through PSB
CPU_MEM_ACCESS            [3]      RW   0       When ‘1’, the NNPA memory can be accessed by the system.
                                                When ‘0’, the NNPA memory is reserved to the core
NNPA_CLK_FORCE_EN         [4]      RW   0       When ‘1’, the NNPA clock is forced to be active
MEM_CLK_FORCE_EN          [5]      RW   0       When ‘1’, the clocks to all memory buffers are forced to be
                                                active



         11.4.4      NNPA_STATUS
NNPA main status register
Address offset: 0x0C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
NNPA_BUSY                 [0]      RO   0       Flag high when the NNPA core is running
CONFIG_CHECK_OK           [1]      RO   0       Flag high when the NN configuration check procedure is
                                                passed
INPUT_LOADING_RDY         [2]      RO   0       Flag high when the NNPA is ready to receive input data
INPUT_LOADER_FULL         [3]      RO   0       Flag high when the INPUT LOADER has received the expected
                                                number of data
XTEN_PAUSE_STATE          [4]      RO   0       Flag high when the NNPA is in an XTEN pause state
RUN_LAYER_IDX             [17:8]   RO   0       Current processed layer index while running an inference



         11.4.5      NB_DATA_BUFF
Buffers allocation register
Address offset: 0x10
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
NB_DATA_BUFF              [7:0]    RW   0       Number of buffers allocated to NN execution. If
                                                NB_DATA_BUFF=0, this number is automatically set from the
                                                bitstream information



         11.4.6      CONFIG_START_MADDR
Configuration start address register
Address offset: 0x14
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
CONFIG_START_MADDR        [19:0]   RW   0       NN configuration start address in NNPA memory words




                                                                                                    96
<PARSED TEXT FOR PAGE: 97 / 159>
                                                              Neural Network Accelerator (NNPA)



         11.4.7        INPUT_START_MADDR
Input data area start address register
Address offset: 0x18
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
INPUT_START_MADDR         [19:0]   RW   0       NN input data start address in NNPA memory words



         11.4.8        DATA_START_MADDR
Activation data area start address register
Address offset: 0x1C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DATA_START_MADDR          [19:0]   RW   0       NN activations start address in NNPA memory words



         11.4.9        MEM_BUFF_PD0
Memory buffers power-down control register
Address offset: 0x20
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
MEM_BUFF_PD0              [31:0]   RW   0       Memory buffers power-down control. Each bit corresponds to
                                                memory buffer 0 to 31:
                                                0: Normal mode
                                                1: Power off, buffer cannot be read or written



         11.4.10 MEM_BUFF_PD1
Memory buffers power-down control register
Address offset: 0x24
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
MEM_BUFF_PD1              [31:0]   RW   0       Memory buffers power-down control. Each bit corresponds to
                                                memory buffer 32 to 39:
                                                0: Normal mode
                                                1: Power off, buffer cannot be read or written



         11.4.11 MEM_BUFF_SLEEP
Memory buffers sleep control register
Address offset: 0x28


                                                                                                    97
<PARSED TEXT FOR PAGE: 98 / 159>
                                                               Neural Network Accelerator (NNPA)


Reset value: 0x00000000

Field                     Bits     Type Reset   Description
MEM_BUFF_SLEEP_AUTO       [0]      RW   0       When ‘1’, the memory buffers sleep mode is automatically
                                                controlled
MEM_BUFF_SLEEP_EN         [1]      RW   0       Memory buffers sleep mode enabled (‘1’) or disabled (‘0’)
                                                when MEM_BUFF_SLEEP_AUTO=0



         11.4.12 MEM_BUFF_CUTS_CTRL
Memory buffers cuts control register
Address offset: 0x2C
Reset value: 0x0000050A

Field                     Bits     Type Reset   Description
CUTS_CTRL_TEST1           [0]      RW   0       Test mode to bypass self-timed circuit.The external clock
                                                controls the read and write control signals
CUTS_CTRL_RME             [1]     RW    1       Read-Write margin enable
CUTS_CTRL_RM              [5:2]   RW    2       Read-Write margin
CUTS_CTRL_RA              [7:6]   RW    0       Read Assist setting
CUTS_CTRL_WA              [10:8] RW     5       Write Assist setting
CUTS_CTRL_WPULSE          [13:11] RW    0       Write Assist pulse width setting



         11.4.13 INPUT_LOADER_DATA
Input loader data store register
Address offset: 0x30
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
INPUT_LOADER_DATA         [31:0]   RW   0       When written, loads the next four input bytes (LSB first)
                                                through the input loader



         11.4.14 INPUT_LOADER_CFG
Input loader configuration register
Address offset: 0x34
Reset value: 0x00000000




                                                                                                       98
<PARSED TEXT FOR PAGE: 99 / 159>
                                                                  Neural Network Accelerator (NNPA)


Field                       Bits     Type Reset    Description
LOADER_NBYTE_VALID          [1:0]    RW   0        Number of valid bytes minus 1 that are provided each time
                                                   INPUT_LOADER_DATA is written to. Only relevant when
                                                   LOADER_NBIT_MODE=0
LOADER_NBIT_MODE            [4:2]    RW   0        Number of bits of loaded data.
                                                   0: 8-bit mode (multi-byte enabled)
                                                   1: 10-bit mode
                                                   2: 12-bit mode
                                                   3: 14-bit mode
                                                   4: 16-bit mode
LOADER_SIGNED_DATA          [6]      RW   0        When ‘1’, the INPUT LOADER handles inputs as 2’s
                                                   complement signed data. Otherwise it considers unsigned
                                                   data



         11.4.15 INPUT_LOADER_LEVEL
Input loader current level register
Address offset: 0x38
Reset value: 0x00000000

Field                       Bits     Type Reset    Description
INPUT_LOADER_LEVEL          [23:0]   RO   0        Current number of bytes loaded through the input loader for
                                                   a given frame



         11.4.16 INPUT_DATA_RESCALE
Input data re-scaling register
Address offset: 0x3C
Reset value: 0x00001000

Field                       Bits     Type Reset    Description
INPUT_SCALING_FXP           [15:0]   RW   0x1000 Scaling factor applied to all input data in fixed-point 4.12
                                                 format (4 bits for integer part, 12 bits for decimal part)
INPUT_OFFSET_INT            [31:16] RW    0      2’s complement signed offset added to all input data



         11.4.17 RD_MODEL_ID
Model identifier register
Address offset: 0x40
Reset value: 0x00000000

Field                       Bits     Type Reset    Description
RD_MODEL_ID                 [15:0]   RO   0        Identifier of the loaded model



         11.4.18 RD_INPUT_SCALING
Input data scaling register
Address offset: 0x44


                                                                                                          99
<PARSED TEXT FOR PAGE: 100 / 159>
                                                                  Neural Network Accelerator (NNPA)


Reset value: 0x00000000

Field                        Bits     Type Reset   Description
RD_INPUT_SCALING             [15:0]   RO   0       Required input data scaling in fixed-point 4.12 format



         11.4.19 RD_INPUT_OFFSET
Input data offset register
Address offset: 0x48
Reset value: 0x00000000

Field                        Bits     Type Reset   Description
RD_INPUT_OFFSET              [15:0]   RO   0       Required input data offset in 2’s complement format



         11.4.20 RD_OUTPUT_SCALING
Output data scaling register
Address offset: 0x4C
Reset value: 0x00000000

Field                        Bits     Type Reset   Description
RD_OUTPUT_SCALING            [15:0]   RO   0       Output data scaling for real number conversion



         11.4.21 RD_OUTPUT_OFFSET
Output data offset register
Address offset: 0x50
Reset value: 0x00000000

Field                        Bits     Type Reset   Description
RD_OUTPUT_OFFSET             [15:0]   RO   0       Output data offset for real number conversion



         11.4.22 RD_MODEL_SIZE
Model configuration size register
Address offset: 0x54
Reset value: 0x00000000

Field                        Bits     Type Reset   Description
RD_MODEL_SIZE                [19:0]   RO   0       Model configuration size in NNPA memory words



         11.4.23 RD_INPUT_SIZE
Input feature map size register
Address offset: 0x58


                                                                                                        100
<PARSED TEXT FOR PAGE: 101 / 159>
                                                              Neural Network Accelerator (NNPA)


Reset value: 0x00000000

Field                     Bits     Type Reset   Description
RD_INPUT_SIZE             [19:0]   RO   0       Input data size in NNPA memory words



         11.4.24 RD_DATA_SIZE
Model inference needs register
Address offset: 0x5C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
RD_DATA_SIZE              [19:0]   RO   0       Data inference needs in NNPA memory words



         11.4.25 RD_IFMAP_INFO
Input feature map information register
Address offset: 0x60
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
IFMAP_FLATTENED           [0]      RO   0       Indicates whether the NN input feature map is flattened or
                                                not
IFMAP_PACK_LEVEL          [6:4]    RO   0       Packing level of NN input feature map



         11.4.26 RD_IFMAP_SIZE
Input feature map size register
Address offset: 0x64
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
IFMAP_WIDTH               [9:0]   RO    0       NN input feature map width minus 1
IFMAP_HEIGHT              [19:10] RO    0       NN input feature map height minus 1
IFMAP_DEPTH               [31:20] RO    0       NN input feature map depth minus 1



         11.4.27 RD_OFMAP_INFO
Output feature map information register
Address offset: 0x68
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
OFMAP_FLATTENED           [0]      RO   0       Indicates whether the NN output feature map is flattened or
                                                not
OFMAP_PACK_LEVEL          [6:4]    RO   0       Packing level of NN output feature map




                                                                                                   101
<PARSED TEXT FOR PAGE: 102 / 159>
                                                              Neural Network Accelerator (NNPA)



        11.4.28 RD_OFMAP_SIZE
Output feature map size register
Address offset: 0x6C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
OFMAP_WIDTH               [9:0]   RO    0       NN output feature map width minus 1
OFMAP_HEIGHT              [19:10] RO    0       NN output feature map height minus 1
OFMAP_DEPTH               [31:20] RO    0       NN output feature map depth minus 1



        11.4.29 DATA_END_MADDR
Activation data area end address register
Address offset: 0x70
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DATA_END_MADDR            [19:0]   RO   0       NN data end computed address in NNPA memory words



        11.4.30 OFMAP_START_MADDR
Output feature map start address register
Address offset: 0x74
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
OFMAP_START_MADDR         [19:0]   RO   0       NN output data start memory address in NNPA memory
                                                words



        11.4.31 INFERENCE_COUNTER
Inference counter register
Address offset: 0x78
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
INFERENCE_COUNTER         [23:0]   RO   0       Inference counter incremented at each end of inference



        11.4.32 MEM_CONFLICT_INFO
Memory conflict information register
Address offset: 0x7C
Reset value: 0x00000000




                                                                                                  102
<PARSED TEXT FOR PAGE: 103 / 159>
                                                               Neural Network Accelerator (NNPA)


Field                     Bits     Type Reset   Description
MEM_CONFLICT_SOURCE       [3:0]    RO   0       Indicates the sources of the last memory conflict event
MEM_CONFLICT_BUFFER       [11:4]   RO   0       Indicates in which buffer the last conflict occurred



        11.4.33 XTEN_IFMAP_MADDR
XTEN layer input fmap address register
Address offset: 0x80
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
XTEN_IFMAP_MADDR          [15:0]   RO   0       Start address of input feature map for XTEN layers in NNPA
                                                memory words



        11.4.34 XTEN_OFMAP_MADDR
XTEN layer output fmap address register
Address offset: 0x84
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
XTEN_OFMAP_MADDR          [15:0]   RO   0       Start address of output feature map for XTEN layers in NNPA
                                                memory words



        11.4.35 XTEN_WEIGHT_MADDR
XTEN layer weights address register
Address offset: 0x88
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
XTEN_WEIGHT_MADDR         [15:0]   RO   0       Start address of weights list for XTEN layers in NNPA memory
                                                words



        11.4.36 CONFIG_CHECK_CRC
Configuration check CRC register
Address offset: 0x8C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
CONFIG_CHECK_CRC          [15:0]   RO   0       Result of the last NN configuration check CRC computation



        11.4.37 NNPA_MBIST_CTRL
NNPA SRAM BIST control register


                                                                                                    103
<PARSED TEXT FOR PAGE: 104 / 159>
                                                               Neural Network Accelerator (NNPA)


Address offset: 0x90
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
NNPA_MBIST_RUN            [9:0]    RWS 0        Writing ‘1’ to bits k launches the memory BIST test for
                                                buffers groups k



         11.4.38 NNPA_MBIST_STATUS
NNPA SRAM BIST status register
Address offset: 0x94
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
NNPA_MBIST_DONE           [9:0]    RO   0       Rises on memory BIST completion. Automatically cleared
                                                when the BIST test starts
NNPA_MBIST_ANY_FAIL       [31]     RO   0       Rises on any memory BIST failure. Automatically cleared
                                                when the BIST test starts



         11.4.39 NNPA_MBIST_FAIL0
SRAM BIST fail register
Address offset: 0x98
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
NNPA_MBIST_FAIL0          [31:0]   RO   0       BIST status of SRAM cuts 0 to 31



         11.4.40 NNPA_MBIST_FAIL1
SRAM BIST fail register
Address offset: 0x9C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
NNPA_MBIST_FAIL1          [31:0]   RO   0       BIST status of SRAM cuts 32 to 63



         11.4.41 NNPA_MBIST_FAIL2
SRAM BIST fail register
Address offset: 0xA0
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
NNPA_MBIST_FAIL2          [15:0]   RO   0       BIST status of SRAM cuts 64 to 79




                                                                                                     104
<PARSED TEXT FOR PAGE: 105 / 159>
                                                              Neural Network Accelerator (NNPA)



         11.4.42 NNPA_INT_STATUS
NNPA interrupt status register
Address offset: 0xB0
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
EVT_DATA_LOADED           [0]     RO   0       Event latched once the input loader has received all expected
                                               input data
EVT_CONFIG_CHECK_END      [1]     RO   0       Event latched at the end of NN configuration check
                                               procedure
EVT_EXEC_START            [2]     RO   0       Event latched at the start of each NNPA execution
EVT_EXEC_END              [3]     RO   0       Event latched at the end of each NNPA execution
EVT_XTEN_PAUSE            [4]     RO   0       Event latched at the start of an XTEN layer, pausing the
                                               processing
EVT_MEM_CONFLICT          [5]     RO   0       Event latched when a memory conflict is detected
EVT_INPUT_LOADER_OVF      [6]     RO   0       Event latched when the INPUT LOADER received more data
                                               than expected
EVT_ISP_SIZE_ERR          [7]     RO   0       Event latched when the image size received from the ISP
                                               does not match the expected input feature map size



         11.4.43 NNPA_INT_EN
NNPA interrupt enable register
Address offset: 0xB4
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
NNPA_INT_EN               [7:0]   RW   0       When an event has its corresponding enable bit set, it will
                                               contribute to IRQ generation



         11.4.44 NNPA_INT_SET
NNPA interrupt set register
Address offset: 0xB8
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
NNPA_INT_SET              [7:0]   RW   0       Allows to emulate events by software by writing ‘1’ to
                                               corresponding positions. This field is always read as 0.
                                               Automatically returns to 0 after a write request



         11.4.45 NNPA_INT_CLEAR
NNPA interrupt clear register
Address offset: 0xBC
Reset value: 0x00000000




                                                                                                     105
<PARSED TEXT FOR PAGE: 106 / 159>
                                                     Neural Network Accelerator (NNPA)


Field            Bits    Type Reset   Description
NNPA_INT_CLEAR   [7:0]   RW   0       Allows to erase event flags of the NNPA_INT_STATUS by
                                      writing ‘1’ to corresponding positions. This field is always
                                      read as 0. Automatically returns to 0 after a write request




                                                                                             106
<PARSED TEXT FOR PAGE: 107 / 159>
                                                                  Fast Fourier Transform (FFT)




12 Fast Fourier Transform (FFT)
The FFT peripheral computes the Discrete Fourier Transform (DFT) of a sequence of input data by
autonomously fetching data and coefficients from the central memory (SRAM) via the AXI bus. The
DFT computation is performed using the Fast Fourier Transform algorithm, with the results being
stored in an output buffer allocated in the SRAM.



12.1          Functional description
The block can handle a sequence of 2M input data points, with M up to 15. If the required number of
points is not a power of 2, it is recommended to pad the sequence with zeroes.


         12.1.1       FFT principle
A Fast Fourier Transform (FFT) is an algorithm that efficiently computes the Discrete Fourier
Transform (DFT) of a sequence of data:




Xk are the output of the DFT corresponding to frequency bin k, while xm are the inputs sampled at
time m. The twiddle factors, or coefficients, correspond to the complex exponential terms.

In the case of FFT, the sum above can be calculated as the sum of the even and odd samples of the
sequence. Each sub-sequence sum corresponds to the DFT of these n/2 data. This dichotomous
process is applied recursively until a sequence of one element is obtained.




                           Figure 19: 8-point FFT example




                                                                                           107
<PARSED TEXT FOR PAGE: 108 / 159>
                                                                     Fast Fourier Transform (FFT)




An example is shown in Figure 19 in which the outputs corresponding to the 8 analyzed
frequencies (k * Fsampling / 8) are obtained by combining the 4-point DFT outputs of the data
sequences in which even and odd indexes are treated separately. It’s worth noting that the inputs
are reordered so that data with even and odd indices are grouped together.

WNk is the twiddle factor equals to exp(− j 2 π k / N) .

As exp(− j 2 π⋅( k + N /2)/N ) = −exp(− j 2 π k / N) , we have WNk+N/2 = -Wnk . Hence, only half
of the coefficients (N/2) are needed in memory.


          12.1.2       Data representation
The FFT handles three types of data:

    ●   The input data to which the FFT is to be applied
    ●   The complex coefficients that have been pre-calculated
    ●   The complex output data that result from the FFT computation

It handles 16-bits signed integers in 2’s complement format to represent the real and imaginary
components of the complex numbers.

    Input data representation
The input data are assumed to be real numbers, so only the real component is available. They
should be coded on less than 16 bits. To avoid any saturation which could affect the quality of the
results, it is recommended to quantify the input data between - 2 16-M and 216-M - 1 with M = log2(N)
and N being the number of input points of the FFT. However, a compromise can be found between
no saturation and few data saturations by decreasing the M value computed above.

When stored in the SRAM, the input data are placed in 16-bit half words. In case these data are
received on less that 16 bits, a sign extension is needed. The sign extension can be handled in
software by casting all inputs to int16 values or it can be handle by the FFT itself. The FFT performs
the sign-extension to 16 bits when the FFT_NBIT_INPUT register field is set to a lower value than
15. This field should contain the actual number of bits used to code the input data, minus 1. For
unsigned integer inputs, FFT_NBIT_INPUT should be left to 15.

For example, if the input data are numbers between (- 27) and (27 – 1), then 6 bits are effectively
used to code their values and FFT_NBIT_INPUT should be set to 5. In this case, when stored in
memory, the MSB of all input data may simply be padded with zeroes.

    Coefficients representation
The coefficients should be pre-computed as complex number with 16 bits for each component. The
components are calculated as real numbers between -1 and 1, but they are converted to integers by
doing the operation: Round(real_value * 215 – 1) when stored in memory.




                                                                                              108
<PARSED TEXT FOR PAGE: 109 / 159>
                                                                      Fast Fourier Transform (FFT)



     Output data representation
 Output data are complex numbers with real and complex components coded as 16-bit signed
 integers.


           12.1.3       Memory buffers allocation
 Before initiating an FFT computation, the input data and coefficients must be prepared in the SRAM
 module by the CPU or any other peripheral. Both data and coefficients are expected to be stored in
 order. Additionally, a separate output data buffer must be reserved for storing the results of the
 computation. In this context, an FFT buffer is a reserved area of any size in the SRAM block and
 should not be confused with the SRAM memory buffers described in section 5.4.

 The Figure 20 below represents the three buffers required for FFT processing in the case of an n-
 point FFT. The specifics of each buffer are detailed in the following paragraphs. These buffers must
 be allocated in valid and non-overlapping SRAM locations.




Figure 20: Memory buffers allocation for n-point FFT

     Input data buffer
 The start address of this FFT buffer in the global addressing space is specified in the
 FFT_DIN_START_ADDR register. Each buffer word contains two 16-bit input samples, requiring a
 total of n/2 32-bit words. Here, Di represents the i-th sample of the time sequence. This buffer
 must be populated with the entire sequence of samples before the FFT processing is initiated.

     Coefficients buffer
 The start address of this FFT buffer in the global addressing space is specified in the
 FFT_COEFF_START_ADDR register. The coefficients are complex numbers, and each buffer word
 contains both the real and imaginary parts of a given factor. Since only the first n/2 coefficients are
 required, n/2 32-bit words are needed in total. Wnk represents a coefficient, which is calculated as
   exp(− j 2 π k /n) . This buffer is populated once by the CPU, with the pre-calculated values
 converted to integer using the Round(real_value * 215 – 1) formula. They do not need to be changed
 as long as the length of the input sequence remains the same.


                                                                                                109
<PARSED TEXT FOR PAGE: 110 / 159>
                                                                      Fast Fourier Transform (FFT)



         Output data buffer
The start address of this FFT buffer in the global addressing space is specified by the
FFT_DOUT_START_ADDR register. This buffer is used to stored intermediate FFT computations and
the final results upon completion of the processing. Since n complex numbers have to be stored, n
32-bit words are required. D’i represents the i-th output (frequency bin index i). No specific content
needs to be pre-loaded into this buffer before initiating the FFT processing.

         Optional computing buffer
An additional FFT buffer used for the FFT computation can be reserved in the SRAM by defining its
start address in the FFT_BUFFER_START_ADDR. This memory area will have the same size as that for
the output data and should be mapped to a different SRAM memory buffer than the output data
buffer. Under these conditions, an n-point FFT computation time is optimized and will require
roughly n * log2(n) * 1.5 cycles of the AXI_CLK clock.

In       order to   save
                     memory space, FFT_BUFFER_START_ADDR can              be set equal to
FFT_DOUT_START_ADDR so that the computation and output buffers share the same memory space.
The drawback is the memory contention that could appear during the processing, which would
increase the FFT computation time.


             12.1.4        Running the FFT on a sequence of data
Before running the FFT computation, the input data and coefficients buffers must be loaded in
memory and the FFT buffers allocation correctly configured. In addition, the log2(number of input
points) of the FFT must be indicated in the NB_POINT_LOG2 field, i.e. for 2M points, the field must be
set to M.

Once the FFT peripheral is configured, the computation can be launched. This is achieved by
writing ‘1’ in the PROC_START field of the FFT_CONTROL register. As long as the processing is
running, the FFT_BUSY flag will be set to ‘1’. Finally, at the end of the processing, the results of the
computation can be read from the output data buffer.

The events described below allow start and end detection of the FFT processing phases.

When the FFT module is no longer busy, a new set of inputs can be loaded in the input data buffer
and a new computation launched again. It can be noted that a new set of inputs can be optionally
prepared and written into another area of the SRAM, even during the FFT computation. A new
processing can be started after having set FFT_DIN_START_ADDR to the start address corresponding
to this new memory area.


             12.1.5        FFT events and interrupt
The FFT_INT_STATUS register records the following events:

     ●    FFT_EVT_EXEC_START: FFT execution start, rises when the FFT computation is launched.
     ●    FFT_EVT_EXEC_END: FFT execution end, rises when the FFT computation finishes.




                                                                                               110
<PARSED TEXT FOR PAGE: 111 / 159>
                                                                         Fast Fourier Transform (FFT)


On a an event detection, the corresponding flag bit in the FFT_INT_STATUS register is set to ‘1’. The
flag bits can be individually cleared by writing a ‘1’ in the FFT_INT_CLEAR bit positions. Also, events
can be emulated in software by writing a ‘1’ in the FFT_INT_SET bit positions.

The FFT peripheral interrupt signal is asserted each time any IRQ-enabled FFT event occurs. An
event is IRQ-enabled if its corresponding bit in the FFT_INT_EN register is set to ‘1’.



12.2             Registers

           12.2.1      FFT_CONTROL
FFT block control register
Address offset: 0x00
Reset value: 0x00000000

Field                       Bits    Type Reset   Description
PROC_START                  [0]     RW   0       Writing ‘1’ launches the FFT processing. This field is always
                                                 read as 0. Automatically returns to 0 after a write request



           12.2.2      FFT_STATUS
FFT block status register
Address offset: 0x04
Reset value: 0x00000000

Field                       Bits    Type Reset   Description
FFT_BUSY                    [0]     RO   0       Indicates when the FFT block is idle (‘0’) or computing (‘1’)



           12.2.3      FFT_CONFIG
FFT configuration register
Address offset: 0x08
Reset value: 0x000000F8

Field                       Bits    Type Reset   Description
NB_POINT_LOG2               [3:0]   RW   8       Number of input points of the FFT. The power of 2 exponent
                                                 is provided
FFT_NBIT_INPUT              [7:4]   RW   15      Effective number of bits (minus 1) used to code input data



           12.2.4      FFT_DIN_START_ADDR
Input data buffer start address register
Address offset: 0x0C
Reset value: 0x00000000




                                                                                                        111
<PARSED TEXT FOR PAGE: 112 / 159>
                                                                        Fast Fourier Transform (FFT)


Field                     Bits     Type Reset   Description
DIN_START_ADDR            [31:0]   RW   0       Input data buffer AXI start address



         12.2.5        FFT_COEFF_START_ADDR
Coefficients buffer start address register
Address offset: 0x10
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
COEFF_START_ADDR          [31:0]   RW   0       Coefficients buffer AXI start address



         12.2.6        FFT_DOUT_START_ADDR
Output data buffer start address register
Address offset: 0x14
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
DOUT_START_ADDR           [31:0]   RW   0       Output data buffer AXI start address



         12.2.7        FFT_BUFFER_START_ADDR
Optional buffer start address register
Address offset: 0x18
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
FFT_BUFFER_START_ADDR     [31:0]   RW   0       Optional buffer AXI start addr. If not required, must be set to
                                                the same address as DOUT_START_ADDR



         12.2.8        FFT_INT_STATUS
FFT interrupt status register
Address offset: 0x30
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
FFT_EVT_EXEC_START        [0]      RO   0       Event latched at the FFT computation start
FFT_EVT_EXEC_END          [1]      RO   0       Event latched once the FFT computation ends



         12.2.9        FFT_INT_EN
FFT interrupt enable register
Address offset: 0x34


                                                                                                      112
<PARSED TEXT FOR PAGE: 113 / 159>
                                                                         Fast Fourier Transform (FFT)


Reset value: 0x00000000

Field                        Bits   Type Reset   Description
FFT_INT_EN                [1:0]     RW   0       When an event has its corresponding enable bit set, it will
                                                 contribute to IRQ generation



         12.2.10 FFT_INT_SET
FFT interrupt set register
Address offset: 0x38
Reset value: 0x00000000

Field                        Bits   Type Reset   Description
FFT_INT_SET               [1:0]     RW   0       Allows to emulate events by software by writing ‘1’ to
                                                 corresponding positions. This field is always read as 0.
                                                 Automatically returns to 0 after a write request



         12.2.11 FFT_INT_CLEAR
FFT interrupt clear register
Address offset: 0x3C
Reset value: 0x00000000

Field                        Bits   Type Reset   Description
FFT_INT_CLEAR             [1:0]     RW   0       Allows to erase event flags of FFT_INT_STATUS by writing ‘1’
                                                 to corresponding positions. This field is always read as 0.
                                                 Automatically returns to 0 after a write request




                                                                                                       113
<PARSED TEXT FOR PAGE: 114 / 159>
                                                                                     Timers (TIMERx)




13 Timers (TIMERx)
The ColibryNPU features two TIMER instances, TIMER0 and TIMER1. They are identical but fully
independent with a separate IRQ line towards the CPU.



13.1           Functional description
The TIMER peripheral provides a means to generate accurate delays or to perform time
measurements. It features two 32-bit counters (TIMER_L and TIMER_H), which are clocked by a
pre-divided AXI_CLK, that can be combined to behave as a single 64-bit counter. Essentially, the
timer L or H is set to a start value, then it decrements while running until it reaches zero.




Figure 21: TIMER peripheral schematic

In the following, the “_X_” in the registers name refers to either to L or H timer registers.


          13.1.1       TIMER configuration
Delays are generated through down-counters clocked by the AXI_CLK, divided by a PRESCALER
sub-block. The TIMER_X_PRESCALER field must be used to adjust the division ratio. At the




                                                                                                114
<PARSED TEXT FOR PAGE: 115 / 159>
                                                                                  Timers (TIMERx)


PRESCALER output, the AXI_CLK clock frequency will be divided by ( TIMER_X_PRESCALER + 1). In
particular, if the field is set to 0, no division is applied.

The TIMER_X_VAL register can be read any time to fetch the current DOWN-COUNTER value,
which is decremented by one for every counting clock cycle while running. Additionaly, the register
can be written to an update value while idle or running. Immediately upon the register write, the
DOWN-COUNTER takes the value written.

The TIMER_X_ONE_SHOT field defines the behavior of the timer when the down-counting ends:

   ●   If set to ‘0’, the DOWN-COUNTER is reloaded automatically with the content of
       TIMER_X_RELOAD and restarts a new down-counting phase. A TIMER done event is hence
       generated cyclically and indefinitely.

   ●   If set to ‘1’, the DOWN-COUNTER is stopped and returns to an idle state.

In case a 64-bit timer is required, the TIMER_64BIT_MODE field must be set to ‘1’. The multi-bits
registers for L and H counters are then concatenated. In this mode, only the control and status bits
corresponding to the L timer are relevant for using the 64-bit single counter.


          13.1.2       Running the TIMER
After the configuration of timer L or H, the down-counting can be started by writing ‘1’ into the
TIMER_X_START bit. The down-counting can be paused by writing ‘1’ into TIMER_X_STOPt and
resumed by writing ‘1’ again into TIMER_X_START. While running, the TIMER_X_RUNNING status flag
is raised until the DOWN-COUNTER reaches zero.

When the DOWN-COUNTER reaches zero, the TIMER_X_DONE flag is raised. This flag can be de-
asserted by writing ‘1’ in the TIMER_X_CLEAR_DONE bit.

While running or idle, the counter value can be reset to zero by writing ‘1’ in the TIMER_X_RESET
bit.


          13.1.3       Generating a delay
An accurate delay is generated by correctly setting the start value of the timer L or H and launching
the count down. The CPU can poll the TIMER_X_DONE flag to detect the end. Intermediate delays
could be measured by polling TIMER_X_VAL and subtract it from the start value, providing the
number of counting clock cycles elapsed since the start.

More efficiently, the TIMERx peripheral interrupt can be used to detect the end of the down-
counting. As long as the TIMER_X_IRQ_EN bit is set to ‘1’, an IRQ event is triggered each time the
DOWN-COUNTER reaches the value zero.



13.2           Registers

          13.2.1       TIMER_VERSION
TIMER hardware version register


                                                                                             115
<PARSED TEXT FOR PAGE: 116 / 159>
                                                                                        Timers (TIMERx)


Address offset: 0x0000
Reset value: 0x00010000

Field                     Bits   Type Reset   Description
TIMER_REV_MINOR           [15:0] RO    0      TIMER IP revision minor
TIMER_REV_MAJOR           [31:16] RO   1      TIMER IP revision major



         13.2.2      TIMER_CFG
Timer configuration register
Address offset: 0x0004
Reset value: 0x00000000

Field                     Bits   Type Reset   Description
TIMER_64BIT_MODE          [0]    RW    0      When set, combines two 32-bits timer to create a single 64-
                                              bits timer
TIMER_L_ONE_SHOT          [4]     RW   0      Timer L one shot configuration
TIMER_L_IRQ_EN            [5]     RW   0      Timer L IRQ enable
TIMER_H_ONE_SHOT          [8]     RW   0      Timer H one shot configuration
TIMER_H_IRQ_EN            [9]     RW   0      Timer H IRQ enable
TIMER_L_PRESCALER         [23:16] RW   0      Timer L prescaler value
TIMER_H_PRESCALER         [31:24] RW   0      Timer H prescaler value



         13.2.3      TIMER_CTRL
Timer control register
Address offset: 0x0008
Reset value: 0x00000000

Field                     Bits   Type Reset   Description
TIMER_L_START             [0]    RWS 0        Timer L start bit. This field is always read as 0
TIMER_L_STOP              [1]    RWC 0        Timer L stop bit. This field is always read as 0
TIMER_L_RESET             [2]    RW 0         Timer L value clear. This field is always read as 0.
                                              Automatically returns to 0 after a write request
TIMER_L_CLEAR_DONE        [3]    RWC   0      Timer L done clear. This field is always read as 0
TIMER_H_START             [8]    RWS   0      Timer H start bit. This field is always read as 0
TIMER_H_STOP              [9]    RWC   0      Timer H stop bit. This field is always read as 0
TIMER_H_RESET             [10]   RW    0      Timer H value clear. This field is always read as 0.
                                              Automatically returns to 0 after a write request
TIMER_H_CLEAR_DONE        [11]   RWC 0        Timer H done clear. This field is always read as 0



         13.2.4      TIMER_STATUS
Timer status register
Address offset: 0x000C
Reset value: 0x00000000




                                                                                                     116
<PARSED TEXT FOR PAGE: 117 / 159>
                                                                         Timers (TIMERx)


Field                      Bits     Type Reset   Description
TIMER_L_RUNNING            [0]      RO   0       Timer L running flag
TIMER_L_DONE               [3]      RO   0       Timer L done flag
TIMER_H_RUNNING            [8]      RO   0       Timer H running flag
TIMER_H_DONE               [11]     RO   0       Timer H done flag



         13.2.5     TIMER_L_VAL
Timer L current register
Address offset: 0x0010
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
TIMER_L_VAL                [31:0]   RW   0       Current timer L value



         13.2.6     TIMER_H_VAL
Timer H current register
Address offset: 0x0014
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
TIMER_H_VAL                [31:0]   RW   0       Current timer H value



         13.2.7     TIMER_L_RELOAD
Timer L reload register
Address offset: 0x0018
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
TIMER_L_RELOAD             [31:0]   RW   0       Timer L reload value



         13.2.8     TIMER_H_RELOAD
Timer H reload register
Address offset: 0x01C
Reset value: 0x00000000

Field                      Bits     Type Reset   Description
TIMER_H_RELOAD             [31:0]   RW   0       Timer H reload value




                                                                                 117
<PARSED TEXT FOR PAGE: 118 / 159>
                                                        Real-time counter and watchdog (RTC)




14 Real-time counter and watchdog (RTC)

14.1          Functional description
The RTC peripheral allows counting time and generating low-power delays with the slow
EXT_LF_CLK clock. The RTC remains functional even when in Sleep mode with the HF_CLK
disabled so that it can be used to wake up the system after a programmable delay. Additionally, the
RTC embeds a watchdog functionality, which is able to reset the system if the CPU is stuck in an
unexpected state.




Figure 22: RTC peripheral schematic



         14.1.1       RTC counter description
As long as RTC_CNT_ENABLE is set to ‘1’, the RTC_COUNTER is incremented by 1 for each
EXT_LF_CLK clock cycle. The 32-bit counter returns to 0 after the maximum value is reached (2 32-
1). The current counter value can be read from the RTC_CNT_VAL register.

At any time, the RTC_COUNTER current count can be set to a predefined value configured in
RTC_CNT_UPDATE_VAL. The counter value is updated as soon as the RTC_CNT_UPDATE bit is written
to ‘1’.




                                                                                           118
<PARSED TEXT FOR PAGE: 119 / 159>
                                                             Real-time counter and watchdog (RTC)


Two comparison values, RTC_COMP_A and RTC_COMP_B, can be configured to generate an event
when the RTC_COUNTER becomes equal to them. The EVT_COMP_A or EVT_COMP_B events can be
polled by the CPU or an interrupt can be generated when enabled in the RTC_INT_EN register.

When the EVT_COMP_A or EVT_COMP_B event occurs and is enabled in the interrupt register, the
system exits automatically from Sleep mode if need be (see section 6.1.1).


              14.1.2      Watchdog function description
The watchdog counter WDT_COUNTER works quite similarly to the RTC_COUNTER, but its
purpose is different and mainly focused on ensuring that the system is alive. The counter is
incremented for each EXT_LF_CLK clock cycle while RTC_WDT_ENABLE is set to ‘1’. The current
counter value can be read from the RTC_WDT_VAL register.

At any time, the WDT_COUNTER current count can be reset to zero by writing ‘1’ in the
RTC_WDT_RESET bit. In this case, it restarts counting from zero.

       Watchdog events and effects
When the WDT_COUNTER current value reaches the value configured in RTC_WDT_WARN, an
EVT_WDT_WARN event is generated. This event has no particular effect, but it can inform the CPU
that a timeout will soon be triggered by the watchdog.

When the WDT_COUNTER current value reaches the value configured in RTC_WDT_TIMEOUT, a
EVT_WDT_TIMEOUT event is generated. The effect of a timeout event is not only to be recorded in
the RTC_INT_STATUS register, but it will also cause:

   ●       A system wake-up if it was in Sleep mode

   ●       A CPU core reset if SCU_WDT_RESET_MODE = 0x1

   ●       A full system reset if SCU_WDT_RESET_MODE = 0x3

Note that even after a full system reset, the EVT_WDT_TIMEOUT event will be recorded in the
RTC_INT_STATUS register. This bit can be used by the CPU to detect if it recovers from a faulty
state that incurred a watchdog timeout.

       Watchdog usage
A typical usage of the watchdog functionality is to provide a fall back solution if the CPU is stuck in
a state that cannot be managed by the code itself. An error exception, an infinite loop without exit
control or an issue with the interrupt manager could prevent the code to run correctly. To recover
from this situation, the WDT_COUNTER can be used as follows, assuming an EXT_LF_CLK clock is
available and running at 32768 Hz:

  At code initialization:
     ● Write ‘1’ into RTC_WDT_RESET

       ●    Set RTC_WDT_WARN to 30000 (~ 916 ms)
       ●    Set RTC_WDT_TIMEOUT to 32768 (= 1 s)
       ●    Write ‘1’ into RTC_WDT_ENABLE

  While in the main loop:

                                                                                               119
<PARSED TEXT FOR PAGE: 120 / 159>
                                                              Real-time counter and watchdog (RTC)


       ●    Ensure that the CPU writes a ‘1’ into RTC_WDT_RESET at least once per second. The RTC
            interrupt handler can be used with the EVT_WDT_WARN event to help resetting the watchdog
            at the right time.

              14.1.3      RTC events and interrupt
The RTC_INT_STATUS register records the following events:

   ●       EVT_COMP_A: Triggered when the RTC timer reaches the comparison value A.
   ●       EVT_COMP_B: Triggered when the RTC timer reaches the comparison value B.
   ●       EVT_WDT_WARN: Triggered when the watchdog timer reaches the warning value.
   ●       EVT_WDT_TIMEOUT: Triggered when the watchdog timer reaches the timeout value.
On a an event detection, the corresponding flag bit in the RTC_INT_STATUS register is set to ‘1’. The
flag bits can be individually cleared by writing a ‘1’ in the RTC_INT_CLEAR bit positions. Also, events
can be emulated in software by writing a ‘1’ in the RTC_INT_SET bit positions.

The RTC peripheral interrupt signal is asserted each time any IRQ-enabled RTC event occurs. An
event is IRQ-enabled if its corresponding bit in the RTC_INT_EN register is set to ‘1’.



14.2               Registers

              14.2.1      RTC_VERSION
IP hardware version register
Address offset: 0x0000
Reset value: 0x00010000

Field                          Bits   Type Reset   Description
RTC_REV_MINOR                  [15:0] RO    0      RTC IP revision minor
RTC_REV_MAJOR                  [31:16] RO   1      RTC IP revision major



              14.2.2      RTC_CONTROL
RTC control register
Address offset: 0x0004
Reset value: 0x00000000

Field                          Bits   Type Reset   Description
RTC_CNT_ENABLE                 [0]    RW    0      Enable RTC counter incrementation
RTC_CNT_UPDATE                 [1]    RW    0      When written ‘1’, the current RTC counter is set to
                                                   RTC_CNT_UPDATE_VAL. This field is always read as 0.
                                                   Automatically returns to 0 after a write request
RTC_WDT_ENABLE                 [2]    RW    0      Enable watchdoc counter incrementation
RTC_WDT_RESET                  [3]    RW    0      When written ‘1’, the watchdog counter is reset to 0. This
                                                   field is always read as 0. Automatically returns to 0 after a
                                                   write request



                                                                                                          120
<PARSED TEXT FOR PAGE: 121 / 159>
                                                         Real-time counter and watchdog (RTC)




         14.2.3      RTC_CNT_VAL
RTC counter current value register
Address offset: 0x0008
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
RTC_CNT_VAL               [31:0]   RO   0       Current RTC counter value



         14.2.4      RTC_CNT_UPDATE_VAL
RTC counter reload value register
Address offset: 0x000C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
RTC_CNT_UPDATE_VAL        [31:0]   RW   0       RTC counter reload value when RTC_CNT_UPDATE is written



         14.2.5      RTC_WDT_VAL
Watchdog current value register
Address offset: 0x0010
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
RTC_WDT_VAL               [31:0]   RO   0       Current watchdog counter value



         14.2.6      RTC_COMP_A
Comparison value A register
Address offset: 0x0020
Reset value: 0x00000400

Field                     Bits     Type Reset   Description
RTC_COMP_A                [31:0]   RW   1024    RTC counter comparison value A



         14.2.7      RTC_COMP_B
Comparison value B register
Address offset: 0x0024
Reset value: 0x00000400

Field                     Bits     Type Reset   Description
RTC_COMP_B                [31:0]   RW   1024    RTC counter comparison value B




                                                                                                121
<PARSED TEXT FOR PAGE: 122 / 159>
                                                          Real-time counter and watchdog (RTC)



         14.2.8      RTC_WDT_WARN
Watchdog warning value register
Address offset: 0x0030
Reset value: 0xFFFFFFFF

Field                     Bits     Type Reset   Description
RTC_WDT_WARN              [31:0]   RW   0xFFFFF Watchdog warning value
                                        FFF



         14.2.9      RTC_WDT_TIMEOUT
Watchdog timeout value register
Address offset: 0x0034
Reset value: 0xFFFFFFFF

Field                     Bits     Type Reset   Description
RTC_WDT_TIMEOUT           [31:0]   RW   0xFFFFF Watchdog timeout value
                                        FFF



         14.2.10 RTC_INT_STATUS
RTC interrupt status register
Address offset: 0x0040
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
EVT_COMP_A                [0]      RO   0       Event latched when the timer reaches the comparison value A
EVT_COMP_B                [1]      RO   0       Event latched when the timer reaches the comparison value B
EVT_WDT_WARN              [2]      RO   0       Event latched when watchdog reached its warn value
EVT_WDT_TIMEOUT           [3]      RO   0       Event latched when watchdog reached its timeout value (chip
                                                reset)



         14.2.11 RTC_INT_EN
RTC interrupt enable register
Address offset: 0x0044
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
RTC_INT_EN                [3:0]    RW   0       When an event has its corresponding enable bit set, it will
                                                contribute to IRQ generation



         14.2.12 RTC_INT_SET
RTC interrupt set register
Address offset: 0x0048


                                                                                                     122
<PARSED TEXT FOR PAGE: 123 / 159>
                                                         Real-time counter and watchdog (RTC)


Reset value: 0x00000000

Field                     Bits    Type Reset   Description
RTC_INT_SET               [3:0]   RW   0       Allows to emulate events by software by writing ‘1’ to
                                               corresponding positions. This field is always read as 0.
                                               Automatically returns to 0 after a write request



         14.2.13 RTC_INT_CLEAR
RTC interrupt clear register
Address offset: 0x004C
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
RTC_INT_CLEAR             [3:0]   RW   0       Allows to erase event flags of RTC_INT_STATUS by writing ‘1’
                                               to corresponding positions. This field is always read as 0.
                                               Automatically returns to 0 after a write request




                                                                                                     123
<PARSED TEXT FOR PAGE: 124 / 159>
                                                            Universal asynchronous RX/TX (UART)




15 Universal asynchronous RX/TX (UART)
The UART peripheral provides asynchronous, full-duplex, serial communication over two wires. No
high-level protocol is defined, raw data can be sent or received through the block using its internal
FIFOs or with the help of the DMA peripheral.



15.1            Functional description
The UART module supports the following features:

   ●   Full-duplex communication

   ●   Configurable baud rate with internal clock divider

   ●   RX parity check and TX parity bit addition

   ●   DMA-enabled communication

   ●   Configurable data size (5 to 9 bits)

   ●   Loopback mode

   ●   ASCII mode

No flow control through CTS/RTS lines is supported.

Note that to use the UART in full duplex, both DIO.27 and DIO.28 should be configured in
Alternate 1 mode.




         Figure 23: UART peripheral schematic
                                                                                             124
<PARSED TEXT FOR PAGE: 125 / 159>
                                                          Universal asynchronous RX/TX (UART)



          15.1.1       Baud rate configuration
The baud rates of the UART peripheral and the external remote device must match for
communication to occur correctly. Possible baud rates are normalized to limit the number of
possible transmission rates. Typical baud rates are 9600, 19200, 57600, or 115200.

The primary clock source of the UART module is the APB_CLK. Its frequency should be known or
computed based on the system clocks configuration in order to correctly match the targeted baud
rate. The bit duration during the communication corresponds to 1/baud rate, hence the UART bit
clock frequency should be as close as possible to the target baud rate. The PRESCALER and
CLK_DIVIDER sub-blocks are combined to generate the UART bit clock. Generally, the
CLK_DIVIDER provides a sufficient range for the clock division ratio (integer between 1 and 4096)
and the UART_PRESCALER field can be left at 0. This field can be modified if a higher global division
ratio is needed.

The following formula can be used to derive the UART_PRESCALER and UART_CLK_DIV fields setting:
        BAUD_RATE = Round(APB_CLK_FREQUENCY / ((UART_PRESCALER + 1) * (UART_CLK_DIV + 1)))


          15.1.2       Protocol configuration
When a parity bit is appended to each transmitted/received data, the UART_PARITY_ENABLE bit
must be set to ‘1’. On the reception path, this enables detecting parity errors and removing the
parity bit added to each received data before storing it in the RX FIFO. On the transmission path,
enabling the parity bit allows automatic computing and appending a parity bit to each data
extracted from the TX FIFO and then transmitted.

The UART low-level protocol specifies that 1, 1.5 or 2 stop bits can be used to mark the end of a
data transfer. This peripheral supports cases with 1 or 2 stop bits, which must be configured in the
UART_STOP_LENGTH field.
The number of bits for all data transmitted or received can also be configured between 5 and 9. This
is achieved by writing the correct setting in UART_BIT_LENGTH.

In case 8-bit data are used, a specific ASCII mode can be enabled in the UART module by setting
UART_ASCII_MODE to ‘1’ and UART_BIT_LENGTH to 3. It allows reading and writing to the UART
FIFOs in a more efficient manner, as described below.


          15.1.3       Data transmit
When one or several data need to be transmitted over the UART_TX I/O line, the TX path must be
enabled by setting the UART_TX_ENABLE bit to ‘1’. As long as at least one data is available in the TX
FIFO, the transmission occurs and the UART_TX_BUSY flag is high.

Data are stored in the TX FIFO by writing them through the UART_TX_PUSH register:

   ●   In non-ASCII mode, only bits indexed 0 to 8 of the register are used, a single data is stored in
       the FIFO for each write access to the register .

   ●   In ASCII mode, four 8-bit data are written at a time in the FIFO by concatenating them in the
       UART_TX_PUSH register. For example, if bytes D0, D1, D2 and D3 need to be transmitted,
       then D0 can be mapped to UART_TX_PUSH[7:0], D1 to UART_TX_PUSH[15:8], D2 to

                                                                                               125
<PARSED TEXT FOR PAGE: 126 / 159>
                                                         Universal asynchronous RX/TX (UART)


       UART_TX_PUSH[23:16] and D4 to UART_TX_PUSH[31:24]. Other data can be provided with
       successive writes to the data register.

In ASCII mode, the bytes transmission order can be configured with the UART_TX_ENDIANNESS field.
If 0, LSB is transmitted first, i.e. given the previous example, the bytes will be transmitted in this
order: D0, D1, D2, D3. On the opposite, if 1, the bytes will be transmitted in this order: D3, D2, D1,
D0.

In addition, when in ASCII mode, the byte 0x00 is a particular value, generally marking the end of a
string. If a byte zero is written in the 4-byte word, the following bytes in the transmission order are
not transmitted. For example, if UART_TX_ENDIANNESS = ‘0’ and D1 = 0x00, then D2 and D3 will not
be transmitted over the UART_TX line.

The current number of data in the TX FIFO can be read in the UART_TX_FIFO_WORD_NB field. If
needed, the content of the TX FIFO can be dropped by writing ‘1’ into UART_CLEAR_TX_FIFO bit,
removing all words previously stored.


           15.1.4       Data receive
To receive data over the UART_RX I/O line, the RX path must be enabled by setting the
UART_RX_ENABLE bit to ‘1’. As long as the RX FIFO is not full, new data can be received and stored
in the RX FIFO. When a data reception is ongoing, the UART_RX_BUSY flag is high.

Data stored in the RX FIFO can be read back through the UART_RX_POP register:

   ●   In non-ASCII mode, only bits index 0 to 8 of the register are used and a single data is popped
       out of the FIFO for each read access to the register .

   ●   In ASCII mode, four 8-bit data are read at a time from the FIFO. If the bytes D0, D1, D2 and
       D3 are received in this order, they are all read at the same time from the UART_RX_POP
       register.

In ASCII mode, the bytes reception order can be configured with the UART_RX_ENDIANNESS field. If
0, the first byte D0 is mapped to the LSB of the data register, i.e. UART_RX_POP[7:0] and D3 is
mapped to UART_RX_POP[31:24]. On the opposite, if 1, D0 is mapped to UART_RX_POP[31:24] and
D3 is mapped to UART_RX_POP[7:0].

In addition, when in ASCII mode, the byte 0x00 is a particular value: when such byte is received, the
4-byte word is completed with zero-bytes. The next non-zero byte received will then be placed in a
new FIFO word.

The current number of data in the RX FIFO can be read in the UART_RX_FIFO_WORD_NB field. If
needed, the content of the RX FIFO can be dropped by writing ‘1’ into UART_CLEAR_RX_FIFO bit,
removing all words previously stored.


           15.1.5       Using the DMA peripheral
The DMA can be used to handle efficiently UART RX or TX transfers. Instead of managing the TX
FIFO level, the CPU can program and trigger data transfer from a memory location. The size of the
transfer is configured in the DMA module. Similarly, data reception can be directly stored into a




                                                                                               126
<PARSED TEXT FOR PAGE: 127 / 159>
                                                          Universal asynchronous RX/TX (UART)


reserved memory area and the CPU can be notified by an interrupt only once a pre-defined number
of data have been received.

To use this feature, the DMA channels must be first configured as described in the Direct memory
access controller (DMA) section. Then the UART_DMA_MODE bit must be written to ‘1’.
UART_TX_ENABLE and/or UART_RX_ENABLE bits must also be set to ‘1’ for the DMA transfers to
start.


           15.1.6       Loopback mode
The loopback mode is not a functional mode but can be enabled to test the TX to RX path. When
the UART_LOOPBACK bit is set to ‘1’, any data transmitted at the TX path output is received at the RX
input, as if UART_TX was connected to UART_RX.


           15.1.7       UART events and interrupt
There are three events which can assert the UART peripheral interrupt signal towards the CPU:

   ●    RX FIFO above threshold: this event is triggered each time a new data is stored in the RX
        FIFO and the number of words stored becomes strictly higher than the threshold defined in
        UART_IRQ_RX_THRESHOLD. This event contributes to the interrupt generation if the
        UART_IRQ_RX_ENABLE bit is set to ‘1’.
   ●    TX FIFO below threshold: this event is triggered each time a data is fetched from the TX FIFO
        for transmission and the number of words stored becomes lower than or equal to the
        threshold defined in UART_IRQ_TX_THRESHOLD. This event contributes to the interrupt
        generation if the UART_IRQ_TX_ENABLE bit is set to ‘1’.

   ●    UART error: this event is triggered whenever an RX or TX FIFO overflow occurs or if an RX
        data parity error is detected. This event contributes to the interrupt generation if the
        UART_IRQ_ERR_ENABLE bit is set to ‘1’.
When an UART error occurs, the corresponding error source is recorded and its flag is set to ‘1’ in
the UART_RX_FIFO_OVERFLOW, UART_TX_FIFO_OVERFLOW or UART_RX_PARITY_ERR bits. These flags
can be cleared to ‘0’ by writing a ‘1’ respectively into UART_CLEAR_RX_FIFO_OVERFLOW,
UART_CLEAR_TX_FIFO_OVERFLOW or UART_CLEAR_RX_PARITY_ERR bits.


15.2            Registers

           15.2.1       UART_VERSION
UART hardware version register
Address offset: 0x000
Reset value: 0x00010001

Field                        Bits   Type Reset   Description
UART_REV_MINOR               [15:0] RO    1      UART hardware revision minor
UART_REV_MAJOR               [31:16] RO   1      UART hardware revision major



                                                                                             127
<PARSED TEXT FOR PAGE: 128 / 159>
                                                          Universal asynchronous RX/TX (UART)




         15.2.2      UART_CTRL
UART control register
Address offset: 0x004
Reset value: 0x00260000

Field                     Bits     Type Reset   Description
UART_CLK_DIV              [11:0]   RW   0       UART clock divider. Generated clock frequency =
                                                APB_CLK_FREQ / ((1 + UART_PRESCALER) * (1 +
                                                UART_CLK_DIV))
UART_PRESCALER            [15:12] RW    0       UART clock precaler. Generated clock frequency =
                                                APB_CLK_FREQ / ((1 + UART_PRESCALER) * (1 +
                                                UART_CLK_DIV))
UART_PARITY_ENABLE        [16]     RW   0       UART parity bit enable. Should be ‘1’ if the parity bit is
                                                enabled
UART_BIT_LENGTH           [19:17] RW    3       RX/TX data length setting. Number_of_data_bits = 5 +
                                                UART_BIT_LENGTH
UART_STOP_LENGTH          [20]     RW   0       Number of stop bits setting. If ‘0’, 1 stop bit, else 2 stop bits
                                                are used in RX and TX
UART_ASCII_MODE           [21]     RW   1       ASCII mode enable. When used, a 32-bit FIFO interface is
                                                used and UART_BIT_LENGTH must be set to 3
UART_TX_ENDIANNESS        [22]     RW   0       When ASCII mode is set, specifies the TX byte order
UART_RX_ENDIANNESS        [23]     RW   0       When ASCII mode is set, specifies the RX byte order
UART_DMA_MODE             [24]     RW   0       DMA triggers enable bit
UART_RX_ENABLE            [25]     RW   0       RX path enable bit
UART_TX_ENABLE            [26]     RW   0       TX path enable bit
UART_LOOPBACK             [27]     RW   0       When ‘1’, RX path is looped with TX path, enabling UART
                                                software tests



         15.2.3      UART_IRQ_CTRL
UART IRQ enable register
Address offset: 0x0008
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
UART_IRQ_RX_ENABLE        [0]     RW    0       RX FIFO above threshold IRQ enable bit
UART_IRQ_TX_ENABLE        [1]     RW    0       TX FIFO below threshold IRQ enable bit
UART_IRQ_ERR_ENABLE       [2]     RW    0       RX or TX error IRQ enable bit
UART_IRQ_TX_THRESHOLD     [10:8] RW     0       TX FIFO threshold setting
UART_IRQ_RX_THRESHOLD     [13:11] RW    0       RX FIFO threshold setting



         15.2.4      UART_STATUS
UART status register
Address offset: 0x000C
Reset value: 0x00000000




                                                                                                         128
<PARSED TEXT FOR PAGE: 129 / 159>
                                                            Universal asynchronous RX/TX (UART)


Field                     Bits       Type Reset   Description
UART_RX_FIFO_WORD_NB      [3:0]   RO       0      Current number of words in the RX FIFO
UART_RX_FIFO_OVERFLOW     [8]     RO       0      RX FIFO overflow flag
UART_RX_PARITY_ERR        [9]     RO       0      RX parity error flag
UART_TX_FIFO_WORD_NB      [19:16] RO       0      Current number of words in the TX FIFO
UART_TX_FIFO_OVERFLOW     [24]    RO       0      TX FIFO overflow flag
UART_TX_BUSY              [25]    RO       0      TX busy flag
UART_RX_BUSY              [26]    RO       0      RX busy flag



         15.2.5      UART_CLEAR
UART status clear register
Address offset: 0x0010
Reset value: 0x00000000

Field                              Bits Typ Reset     Description
                                        e
UART_CLEAR_RX_FIFO                 [0]    RW 0        RX FIFO clear. This field is always read as 0. Automatically
                                                      returns to 0 after a write request
UART_CLEAR_TX_FIFO                 [1]    RW 0        TX FIFO clear. This field is always read as 0. Automatically
                                                      returns to 0 after a write request
UART_CLEAR_RX_FIFO_OVERFLOW        [2]    RWC 0       RX FIFO overflow flag clear. This field is always read as 0
UART_CLEAR_RX_PARITY_ERR           [3]    RWC 0       RX parity error flag clear. This field is always read as 0
UART_CLEAR_TX_FIFO_OVERFLOW        [4]    RWC 0       TX FIFO overflow flag clear. This field is always read as 0



         15.2.6      UART_RX_POP
RX read data register
Address offset: 0x0014
Reset value: 0x00000000

Field                     Bits       Type Reset   Description
UART_RX_DATA              [31:0]     RO    0      RX FIFO data. Reading this register pops 1 data (not ASCII
                                                  mode) or 4 bytes (ASCII mode) from the RX FIFO



         15.2.7      UART_TX_PUSH
TX write data register
Address offset: 0x0018
Reset value: 0x00000000

Field                     Bits       Type Reset   Description
UART_TX_DATA              [31:0]     RW    0      TX FIFO data. Writing to this register stores 1 data (not ASCII
                                                  mode) or 4 bytes (ASCII mode) to the TX FIFO. This field is
                                                  always read as 0




                                                                                                        129
<PARSED TEXT FOR PAGE: 130 / 159>
                                                                              I2C master (I2CM)




16 I2C master (I2CM)
The I2CM peripheral provides synchronous, half-duplex, serial communication over two wires and
compliant with the I2C bus specification. The block is master of the bus and controls the SCL clock
provided to one or multiple slave devices. Data bytes can be sent or received through the block
using its internal FIFOs or with the help of the DMA peripheral.



16.1            Functional description
The I2CM modxule supports the following features:

   ●   Half-duplex communication

   ●   Standard-mode, Fast-mode, Fast-mode plus

   ●   Configurable SCL frequency, internal clock divider

   ●   Multi-master support

   ●   7-bit or 10-bit slave addressing

   ●   50ns spike filter on I2CM I/Os

Clock stretching is not supported. I2C repeated start is not supported.




       Figure 24: I2CM peripheral schematic




                                                                                           130
<PARSED TEXT FOR PAGE: 131 / 159>
                                                                                 I2C master (I2CM)


Before transmitting or receiving data through the I2CM, the relevant DIOs must be configured to
I2C_SCL and I2C_SDA. The I2C bit clock frequency should be correctly set and the module can be
enabled by setting I2C_ENABLE to ‘1’.

This I2C master peripheral exchanges data bytes with slaves connected to the I 2C lines, based on
read or write transactions. During read transactions, a slave is expected to send data bytes after the
transmission of its address. For write transactions, only the master will send data bytes over the
SDA line. When an active transaction is ongoing, the I2C_BUSY flag bit is set to ‘1’.

The first byte sent to the RX FIFO to start a transaction contains the transaction length and the 10-
bit addressing mode flag. This byte is never sent on the I2C bus.


          16.1.1      SCL frequency configuration
The primary clock source of the I2CM module is the APB_CLK. Its frequency should be known or
computed based on the system clocks configuration. The I2C bit clock is generated from it through
a fixed divide-by-four PRESCALER, which is then divided again by a programmable CLK_DIVIDER
sub-block. The programmable division ratio ranges from 1 to 256 and is set in the I2C_CLK_DIV
field.

The following formula can be used to derive I2C_CLK_DIV:
        SCL_FREQUENCY = Round(APB_CLK_FREQUENCY / (4 * (I2C_CLK_DIV + 1)))


          16.1.2
                        2
                      I C write transactions
To send N data bytes to a slave in 7-bit addressing mode (slave address = “A6 A5 A4 A3 A2 A1 A0”
where Ai represents the ith bit of the address), the following sequence of bytes shall be written in
the TX FIFO:
    ●   Transaction length in bytes, including slave address → equals N+1
    ●   Slave address + R/W bit set to ‘0’ → equals “A6 A5 A4 A3 A2 A1 A0 0”
    ●   N consecutive data bytes, with a maximum of N=126 bytes

In case the 10-bit addressing mode is used (slave address = “A9 A8 A7 A6 A5 A4 A3 A2 A1 A0”), the
sequence becomes:
    ●   Transaction length in bytes, including slave address, MSB set to ‘1’ → equals N+2+128
    ●   Slave address MSB + R/W bit set to ‘0’ → “1 1 1 1 0 A9 A8 0”
    ●   Slave address LSB → “A7 A6 A5 A4 A3 A2 A1 A0”
    ●   N consecutive data bytes, with a maximum of N=125 bytes

A byte is stored in the TX FIFO each time the I2C_TX_PUSH register is written to. The CPU is
responsible for the TX FIFO flow control and avoiding any overflow or underflow, taking into
account that up to 4 bytes can be stored in this FIFO. A stored byte is popped out as soon as its
first bit needs to be sent on I2C_SDA.

The current number of data in the TX FIFO can be read in the I2C_TX_FIFO_WORD_NB field. If
needed, the content of the TX FIFO can be dropped by writing ‘1’ into I2C_CLEAR_TX_FIFO bit,
removing all words previously stored.



                                                                                              131
<PARSED TEXT FOR PAGE: 132 / 159>
                                                                                  I2C master (I2CM)


The I2CM module manages the START, STOP and ACK bits. When I2C_ACK_EN bit is set to ‘1’, the
acknowledge bit for all transmitted bytes is checked. If the slave has not acknowledged a byte, the
transaction is stopped and a TX error event is raised. If the bit is set to ‘0’, acknowledgments are not
checked.


           16.1.3
                        2
                       I C read transactions
To read N data bytes from a slave in 7-bit addressing mode (slave address = “A6 A5 A4 A3 A2 A1
A0” where Ai represents the ith bit of the address), the following sequence of bytes shall be written
in the TX FIFO:
     ●   Transaction length in bytes, including slave address → equals N+1
     ●   Slave address + R/W bit set to ‘1’ → equals “A6 A5 A4 A3 A2 A1 A0 1”

The sequence continues until N data bytes are received from the slave and stored in the RX FIFO.
Every data bytes are followed by an acknowledgment from the I2CM, except for the Nth byte which
is NACKed, indicating the end of the transaction.

In case the 10-bit addressing mode is used (slave address = “A9 A8 A7 A6 A5 A4 A3 A2 A1 A0”), the
sequence becomes:
     ●   Transaction length in bytes, including slave address → equals to N+2
     ●   Slave address MSB + R/W bit set to ‘1’ → “1 1 1 1 0 A9 A8 1”
     ●   Slave address LSB → “A7 A6 A5 A4 A3 A2 A1 A0”

As for the 7-bit addressing mode, the sequence continues until N data bytes are received from the
slave.

A byte is stored in the RX FIFO each time a full byte is read from the I2C_SDA line. A byte is
popped out from the FIFO each time the I2C_RX_POP register is read. The CPU is responsible for
the RX FIFO flow control and avoiding any overflow or underflow, taking into account that up to 4
bytes can be stored in this FIFO.

The current number of data in the RX FIFO can be read in the I2C_RX_FIFO_WORD_NB field. If
needed, the content of the RX FIFO can be dropped by writing ‘1’ into I2C_CLEAR_RX_FIFO bit,
removing all words previously stored.


           16.1.4      Using the DMA peripheral
The DMA can be used to handle efficiently I2CM RX or TX transfers. Instead of managing the TX
FIFO level, the CPU can program and trigger data transfer from a memory location. The size of the
transfer is configured in the DMA module. Similarly, data reception can be directly stored into a
reserved memory area and the CPU can be notified by an interrupt only once a pre-defined number
of data have been received.

To use this feature, the DMA channels must be first configured as described in the Direct memory
access controller (DMA) section. Then the I2C_DMA_MODE bit can be written to ‘1’. The I2C_ENABLE
bit must also be set to ‘1’ for the DMA transfers to start.



                                                                                                132
<PARSED TEXT FOR PAGE: 133 / 159>
                                                                                    I2C master (I2CM)



              16.1.5       Multi-master mode
When multiple masters are connected to the I2C bus, the I2C_MULTI_MASTER_EN bit should be set
to ‘1’. This allows checking collisions on SDA and SCL line, which generates a TX error event when it
happens. Otherwise, no collisions are checked.


              16.1.6       I2CM events and interrupt
There are three events which can assert the I2C peripheral interrupt signal towards the CPU:

   ●       RX FIFO above threshold: this event is triggered each time a new data is stored in the RX
           FIFO and the number of words stored becomes strictly higher than the threshold defined in
           I2C_IRQ_RX_THRESHOLD. This event contributes to the interrupt generation if the
           I2C_IRQ_RX_ENABLE bit is set to ‘1’.
   ●       TX FIFO below threshold: this event is triggered each time a data is fetched from the TX FIFO
           for transmission and the number of words stored becomes lower than or equal to the
           threshold defined in I2C_IRQ_TX_THRESHOLD. This event contributes to the interrupt
           generation if the I2C_IRQ_TX_ENABLE bit is set to ‘1’.

   ●       I2C error: this event contributes to the interrupt generation if the I2C_IRQ_ERR_ENABLE bit is
           set to ‘1’.

The I2C error event is triggered whenever one of these situations occurs:
       ●    A TX FIFO overflow or underflow.
       ●    An RX FIFO overflow.
       ●    A TX error, either when a transmitted byte is not acknowledged by a slave or in case an
            SDA/SCL collision is detected and I2C_MULTI_MASTER_EN = ‘1’.

When an I2C error occurs, the corresponding error source is recorded and its flag is set to ‘1’ in the
I2C_RX_FIFO_OVERFLOW, I2C_TX_FIFO_OVERFLOW , I2C_TX_FIFO_UNDERFLOW or I2C_TX_ERR bits.
These flags can be cleared to ‘0’ by writing a ‘1’ respectively into I2C_CLEAR_RX_FIFO_OVERFLOW,
I2C_CLEAR_TX_FIFO_OVERFLOW , I2C_CLEAR_TX_FIFO_UNDERFLOW or I2C_CLEAR_TX_ERR bits.


16.2               Registers

              16.2.1       I2C_VERSION
I2CM hardware version register
Address offset: 0x0000
Reset value: 0x00010000

Field                           Bits   Type Reset    Description
I2C_REV_MINOR                   [15:0] RO    0      I2CM hardware revision minor
I2C_REV_MAJOR                   [31:16] RO   1      I2CM hardware revision major




                                                                                                 133
<PARSED TEXT FOR PAGE: 134 / 159>
                                                                                     I2C master (I2CM)



         16.2.2         I2C_CTRL
I2C master control register
Address offset: 0x0004
Reset value: 0x00600000

Field                     Bits    Type Reset   Description
I2C_CLK_DIV               [7:0]   RW   0       I2CM clock division ratio = I2C_CLK_DIV + 1
I2C_IRQ_TX_THRESHOLD      [9:8]   RW   0       Generate a TX event when the TX FIFO level gets lower than
                                               this threshold
I2C_IRQ_RX_THRESHOLD      [11:10] RW   0       Generate an RX event when the RX FIFO level gets strictly
                                               higher than this threshold
I2C_ENABLE                [16]    RW   0       I2CM block enable
I2C_IRQ_TX_ENABLE         [17]    RW   0       When ‘1’, an IRQ is generated for TX event
I2C_IRQ_RX_ENABLE         [18]    RW   0       When ‘1’, an IRQ is generated for RX event
I2C_IRQ_ERR_ENABLE        [19]    RW   0       When ‘1’, an IRQ is generated for error event
I2C_DMA_MODE              [20]    RW   0       When ‘1’, enable DMA triggers
I2C_ACK_EN                [21]    RW   1       When ‘1’, check ACK/NACK response from slave
I2C_MULTI_MASTER_EN       [22]    RW   1       When ‘1’, check for SDA/SCL collision when I2C lines is driven
                                               by several masters, else collisions are not checked



         16.2.3         I2C_STATUS
I2C master status register
Address offset: 0x0008
Reset value: 0x00000000

Field                     Bits    Type Reset   Description
I2C_TX_FIFO_WORD_NB       [2:0]   RO   0       Current number of words stored in the TX FIFO
I2C_RX_FIFO_WORD_NB       [6:4]   RO   0       Current number of words stored in the RX FIFO
I2C_TX_ERR                [8]     RO   0       TX error flag
I2C_TX_FIFO_OVERFLOW      [9]     RO   0       TX FIFO overflow flag
I2C_TX_FIFO_UNDERFLOW     [10]    RO   0       TX FIFO underflow flag
I2C_RX_FIFO_OVERFLOW      [11]    RO   0       RX FIFO overflow flag
I2C_BUSY                  [12]    RO   0       I2C busy flag



         16.2.4         I2C_CLEAR
Events clear register
Address offset: 0x000C
Reset value: 0x00000000




                                                                                                    134
<PARSED TEXT FOR PAGE: 135 / 159>
                                                                                      I2C master (I2CM)


Field                      Bits   Type Reset   Description
I2C_CLEAR_TX_FIFO         [0]     RW   0       TX event clear. This field is always read as 0. Automatically
                                               returns to 0 after a write request
I2C_CLEAR_RX_FIFO         [1]     RW   0       RX event clear. This field is always read as 0. Automatically
                                               returns to 0 after a write request
I2C_CLEAR_TX_ERR          [8]     RWC 0        TX error flag clear. This field is always read as 0
I2C_CLEAR_TX_FIFO_OVERFLO [9]     RWC 0        TX FIFO overflow flag clear. This field is always read as 0
W
I2C_CLEAR_TX_FIFO_UNDERFL [10]    RWC 0        TX FIFO underflow flag clear. This field is always read as 0
OW
I2C_CLEAR_RX_FIFO_OVERFLO [11]    RWC 0        RX FIFO overflow flag clear. This field is always read as 0
W



         16.2.5      I2C_TX_PUSH
I2C master TX data register
Address offset: 0x0010
Reset value: 0x00000000

Field                      Bits   Type Reset   Description
I2C_TX_DATA               [7:0]   RW   0       TX FIFO data write. First word of the transaction specifies its
                                               length in bytes. This field is always read as 0



         16.2.6      I2C_RX_POP
I2C master RX data register
Address offset: 0x0014
Reset value: 0x00000000

Field                      Bits   Type Reset   Description
I2C_RX_DATA               [7:0]   RO   0       RX FIFO data read




                                                                                                      135
<PARSED TEXT FOR PAGE: 136 / 159>
                                                                                    SPI slave (SPIS)




17 SPI slave (SPIS)
The slave Serial Peripheral Interface (SPIS) peripheral allows synchronous, full-duplex, serial
communication over four wires:
    ● SPIS_SCK is the clock signal provided by the SPI master

       ●    SPIS_CSN is the selection signal, active-low
       ●    SPIS_MOSI corresponds to the serial data from the master to the slave
       ●    SPIS_MISO corresponds to the serial data from the slave to the master

This peripheral is designed to be driven by an external host SPI master.

Data can be sent or received through the block using its internal FIFOs or with the help of the DMA
peripheral.



17.1                Functional description
The SPIS module supports the following features:

   ●       Full-duplex communication, TX-only or RX-only

   ●       Configurable CPOL and CPHA

   ●       Configurable transaction bit length, up to 256 bits

   ●       Configurable default transmit data value

It supports having the SPIS_SCK clock active all the time or only while SPIS_CSN is low.




Figure 25: SPIS peripheral schematic

Before transmitting or receiving data through the SPIS, the relevant DIOs must be configured for
the SPIS_SCK, SPIS_CSN and SPIS_MOSI alternate functions. The SPIS_MISO is optional and can
be configured if data needs to be sent from the slave to the master.




                                                                                             136
<PARSED TEXT FOR PAGE: 137 / 159>
                                                                                   SPI slave (SPIS)


Some parts of the peripheral are clocked by the SPIS_SCK clock and others by the APB_CLK clock.
To ensure the communication is functional, the APB_CLK frequency must be higher than the
SPIS_SCK frequency divided by 4.

The module functionality is enabled by setting the SPIS_EN bit to ‘1’.

Data are exchanged based on SPI transactions. An SPI transaction is the period of time during
which the SPIS_CSN is driven low. The end of the transaction is defined by the rising edge of
SPIS_CSN. When an active transaction is on-going, the SPIS_BUSY flag bit is set to ‘1’.


            17.1.1       Protocol configuration
The SPI bus can be configured in data modes 0 to 3, which define the CPOL and CPHA settings, and
the same configuration must be used on both the slave and master sides. The SPIS can be
configured in one of these modes by setting the SPIS_CPOL and SPIS_CPHA register bits, according
to Table 20. In the table, the clock polarity indicates the state of SPIS_SCK on SPIS_CSN falling edge
which is also its default state outside transactions. The sampling edge corresponds to the SPIS_CLK
edge for which the data (MISO and MOSI) are stable and should be sampled.


 Data mode       CPOL         CPHA       Clock polarity   Sampling edge

   Mode 0            0          0             Low             Rising

   Mode 1            0          1             Low             Falling

   Mode 2            1          0            High             Falling

   Mode 3            1          1            High             Rising

Table 20: SPI mode configuration
Additionally, the SPIS_MISO polarity can be inverted by setting SPIS_MISO_CPHA_INV = ‘1’, which
can improve the timing margin towards the master in some cases.

When the SPIS is operating in half-duplex, i.e. RX-only or TX-only, the SPIS_MODE register can be
set to 0x1 or 0x2 respectively, simplifying the FIFOs management as the TX FIFO or the RX FIFO
will not be used.


            17.1.2       Data transfer
During a transaction, data bits are simultaneously received from the SPIS_MOSI and transmitted on
the SPIS_MISO lines. When SPIS_MODE = 0x0, both RX FIFO and TX FIFO are used to respectively
receive and transmit the data bits and interface with the CPU or the DMA.

The transaction size minus 1, in bits, must be configured in the SPIS_LEN register. If a transaction
lasts longer than the configured value, the extra bits transmitted are undefined and the extra bits
received are discarded.

It is worth noting that the first data bit of a transaction always start in a new word of the RX FIFO
or TX FIFO. Therefore, if the number of bits of a given transaction is not a multiple of 32, the last
word is padded with zeroes.




                                                                                              137
<PARSED TEXT FOR PAGE: 138 / 159>
                                                                                     SPI slave (SPIS)



       Transmitting data
When no specific data needs to be sent, the TX FIFO should be empty. During the next valid
transaction, the bits sent over SPIS_MISO are taken from the SPIS_TX_DEFAULT_VALUE register, as
if the TX FIFO would contain an infinite number of this 32-bit value.

On the other hand, when N bits need to be sent to the master, the TX FIFO must be filled with the
block of bits tx[0],tx[1],...tx[N-1] by writing successive groups of 32 bits into SPIS_TX_PUSH. The
order in which the data must be written depends on some configuration, as explained hereafter.

When N ≤ 32:

   ●    If SPIS_TX_MSB_ALIGN = ‘0’, tx[0],tx[1],...tx[N-1] must be written into SPIS_TX_PUSH[0:N-1].
        The (32 - N) MSBs of the 32-bit vector can be padded with zeroes.

   ●    If SPIS_TX_MSB_ALIGN = ‘1’, tx[0],tx[1],...tx[N-1] must be written into SPIS_TX_PUSH[32-
        N:31]. The (32 - N) LSBs of he 32-bit vector can be padded with zeroes.

In both cases, the SPIS_LSB_FIRST bit allows to choose whether the first transmitted bit of the
transaction is tx[0] (if ‘1’) or tx[N-1] (if ‘0’).

If N > 32, only the SPIS_LSB_FIRST configuration bit defines how TX data shall be loaded in the
FIFO.

   ●    If SPIS_LSB_FIRST = ‘1’, successive 32-bit groups of tx data should be written to
        SPIS_TX_PUSH, starting with bit tx[0]. The first group is then tx[0],tx[1],...tx[31] and the last
        group contains the last (N modulo 32) tx bits, completed with zeros on its MSBs.

   ●    If SPIS_LSB_FIRST = ‘0’, successive 32-bit groups of tx data should be written to
        SPIS_TX_PUSH, starting with bit tx[N-32]. The first group is then tx[N-32],tx[N-31],...tx[N-1]
        and the last group contains the first (N modulo 32) tx bits MSB aligned and completed with
        zeros on its LSBs.

The CPU is responsible for TX FIFO flow control and avoiding any overflow or underflow. The
current word level can be read from the SPIS_TX_FIFO_WORD_NB register and the
SPIS_TX_FIFO_EMPTY or SPIS_TX_FIFO_FULL flags can help in getting the TX FIFO status. The
SPIS_TX_IRQ event and SPIS peripheral IRQ can be used to manage more efficiently this FIFO.

       Receiving data
As for the transmitted bits, the SPIS_LSB_FIRST bit defines whether the first bit received after the
SPIS_CSN falling edge is the LSB (rx[0]) or the MSB (rx[N-1]) of an N-bit transaction. The RX FIFO is
filled with groups of 32 bits and a word can be popped out of it by reading the SPIS_RX_POP
register.

For transactions smaller than 32 bits, the SPIS_RX_MSB_ALIGN bit defines whether the received
data are placed on the MSBs or on the LSBs of the stored word.

The CPU is responsible for RX FIFO flow control and avoiding any overflow or underflow. The
current word level can be read from the SPIS_RX_FIFO_WORD_NB register and the
SPIS_RX_FIFO_EMPTY or SPIS_RX_FIFO_FULL flags can help in getting the RX FIFO status. The
SPIS_RX_IRQ event and SPIS peripheral IRQ can be used to manage more efficiently this FIFO.


                                                                                                 138
<PARSED TEXT FOR PAGE: 139 / 159>
                                                                                 SPI slave (SPIS)



         17.1.3       Using the DMA peripheral
The DMA can be used to handle efficiently SPI transfers. Instead of managing the TX FIFO level,
the CPU can program and trigger data transfer from a memory location. The size of the transfer is
configured in the DMA module. Similarly, data reception can be directly stored into a reserved
memory area and the CPU can be notified by an interrupt only once a pre-defined number of data
have been received.

To use this feature, the DMA channels must be first configured as described in the Direct memory
access controller (DMA) section. Then the SPIS_DMA_MODE bit can be written to ‘1’. The SPIS_EN bit
must also be set to ‘1’ for the DMA transfers to start.


         17.1.4       SPIS events and interrupt
There are four events which can assert the SPIS peripheral interrupt signal towards the CPU:

  ●   RX FIFO above threshold: this event is triggered each time a new data is stored in the RX
      FIFO and the number of words stored becomes strictly higher than the threshold defined in
      SPIS_IRQ_RX_THRESHOLD. This event contributes to the interrupt generation if the
      SPIS_RX_IRQ_EN bit is set to ‘1’.
  ●   TX FIFO below threshold: this event is triggered each time a data is fetched from the TX FIFO
      for transmission and the number of words stored becomes lower than or equal to the
      threshold defined in SPIS_IRQ_TX_THRESHOLD. This event contributes to the interrupt
      generation if the SPIS_TX_IRQ_EN bit is set to ‘1’.

  ●   RX error event is triggered whenever the RX FIFO undergo a data underflow (FIFO read while
      empty) or overflow (requiring to stored a new word but FIFO is full).

  ●   TX error event is triggered whenever the TX FIFO undergo a data underflow (FIFO data
      popped out while empty) or overflow (FIFO write while full).

The RX or TX error events contribute to the interrupt generation if the SPIS_ERR_IRQ_EN bit set to
‘1’.

When an event occurs, its corresponding flag set to ‘1’ in SPIS_STATUS. These flags are
SPIS_RX_IRQ, SPIS_TX_IRQ, SPIS_RX_ERROR and SPIS_TX_ERROR. They can be cleared to ‘0’ by
writing a ‘1’ respectively into SPIS_CLEAR_RX_IRQ, SPIS_CLEAR_TX_IRQ, SPIS_CLEAR_RX_ERROR
or SPIS_CLEAR_TX_ERROR bits.



17.2          Registers

         17.2.1       SPIS_VERSION
SPIS hardware version register
Address offset: 0x0000
Reset value: 0x00010000



                                                                                           139
<PARSED TEXT FOR PAGE: 140 / 159>
                                                                                          SPI slave (SPIS)


Field                        Bits    Type Reset   Description
SPIS_REV_MINOR               [15:0] RO    0       SPIS hardware revision minor
SPIS_REV_MAJOR               [31:16] RO   1       SPIS hardware revision major



           17.2.2       SPIS_CTRL
SPI slave control register
Address offset: 0x0004
Reset value: 0x00000103

Field                        Bits    Type Reset   Description
SPIS_IRQ_TX_THRESHOLD        [1:0]   RW   3       Generate a TX event when the TX FIFO level gets lower than
                                                  this threshold
SPIS_IRQ_RX_THRESHOLD        [5:4]   RW   0       Generate an RX event when the RX FIFO level gets strictly
                                                  higher than this threshold
SPIS_EN                      [8]     RW   1       SPI slave block enable
SPIS_TX_IRQ_EN               [9]     RW   0       When ‘1’, an IRQ is generated for TX event
SPIS_RX_IRQ_EN               [10]    RW   0       When ‘1’, an IRQ is generated for RX event
SPIS_ERR_IRQ_EN              [11]    RW   0       When ‘1’, an IRQ is generated for errors event
SPIS_DMA_MODE                [12]    RW   0       DMA triggers enable
SPIS_CPOL                    [16]    RW   0       CPOL configuration
SPIS_CPHA                    [17]    RW   0       CPHA configuration
SPIS_MISO_CPHA_INV           [18]    RW   0       When ‘1’, MISO polarity is inverted
SPIS_LSB_FIRST               [19]    RW   0       When ‘0’, the transaction MSB is received/transmitted first,
                                                  else this is the LSB
SPIS_MODE                    [21:20] RW   0       SPI working mode
                                                  0x0: Both RX and TX active
                                                  0x1: Only RX path active
                                                  0x2: Only TX path active
                                                  0x3: Reserved
SPIS_RX_MSB_ALIGN            [22]    RW   0       For transactions smaller than 32 bits, specifies where
                                                  received data are placed within the RX FIFO 32-bit word:
                                                  '0’: aligned on LSBs
                                                  '1’: aligned on MSBs
SPIS_TX_MSB_ALIGN            [23]    RW   0       For transactions smaller than 32 bits, specifies where
                                                  transmitted data are placed within the TX_FIFO 32-bit word:
                                                  '0’: aligned on LSBs
                                                  '1’: aligned on MSBs



           17.2.3       SPIS_LEN
Transaction length register
Address offset: 0x0008
Reset value: 0x0000003F

Field                        Bits    Type Reset   Description
SPIS_LEN                     [7:0]   RW   63      SPI transaction length = (1 + SPIS_LEN) bits




                                                                                                      140
<PARSED TEXT FOR PAGE: 141 / 159>
                                                                                            SPI slave (SPIS)



         17.2.4         SPIS_TX_DEFAULT_VALUE
TX default data register
Address offset: 0x000C
Reset value: 0x00000000

Field                       Bits     Type Reset   Description
SPIS_TX_DEFAULT_VALUE       [31:0]   RW    0      Value sent on SPI Rx/Tx when Tx FIFO is empty



         17.2.5         SPIS_STATUS
SPI slave status register
Address offset: 0x0010
Reset value: 0x00000000

Field                       Bits     Type Reset   Description
SPIS_TX_FIFO_WORD_NB        [2:0]   RO     0      Current number of words stored in the TX FIFO
SPIS_TX_FIFO_EMPTY          [4]     RO     0      TX FIFO empty flag
SPIS_TX_FIFO_FULL           [5]     RO     0      TX FIFO full flag
SPIS_TX_IRQ                 [6]     RO     0      TX event flag
SPIS_TX_ERROR               [8]     RO     0      TX error event flag
SPIS_RX_FIFO_WORD_NB        [18:16] RO     0      Current number of words stored in the RX FIFO
SPIS_RX_FIFO_EMPTY          [20]    RO     0      RX FIFO empty flag
SPIS_RX_FIFO_FULL           [21]    RO     0      RX FIFO full flag
SPIS_RX_IRQ                 [22]    RO     0      RX event flag
SPIS_RX_ERROR               [24]    RO     0      RX error event flag
SPIS_BUSY                   [31]    RO     0      Read to ‘1’ while a transaction is on-going



         17.2.6         SPIS_STATUS_CLEAR
SPI slave status clear register
Address offset: 0x0014
Reset value: 0x00000000

Field                       Bits     Type Reset   Description
SPIS_CLEAR_TX_IRQ           [0]      RWC   0      TX event flag clear. This field is always read as 0
SPIS_CLEAR_TX_ERROR         [8]      RWC   0      TX error event flag clear. This field is always read as 0
SPIS_CLEAR_RX_IRQ           [16]     RWC   0      RX event flag clear. This field is always read as 0
SPIS_CLEAR_RX_ERROR         [24]     RWC   0      RX error event flag clear. This field is always read as 0



         17.2.7         SPIS_TX_PUSH
Transmit data register
Address offset: 0x0018
Reset value: 0x00000000




                                                                                                          141
<PARSED TEXT FOR PAGE: 142 / 159>
                                                                                          SPI slave (SPIS)


Field                     Bits     Type Reset   Description
SPIS_TX_DATA              [31:0]   RW   0       SPI write data to TX FIFO. This field is always read as 0



         17.2.8     SPIS_RX_POP
Receive data register
Address offset: 0x001C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SPIS_RX_DATA              [31:0]   RO   0       SPI read data from RX FIFO




                                                                                                       142
<PARSED TEXT FOR PAGE: 143 / 159>
                                                                           SPI master / I2S (SPIMx)




18 SPI master / I S (SPIMx)
                                        2


The master Serial Peripheral Interface (SPIM) peripheral allows synchronous, full-duplex, serial
communication over four wires:
    ● SPIMx_SCK is the clock signal controlled by the SPI master

       ●    SPIMx_CSN_y (y = 0 to 3) is the selection signal, active-low
       ●    SPIMx_MOSI corresponds to the serial data from the master to the slave
       ●    SPIMx_MISO corresponds to the serial data from the slave to the master

The ColibryNPU has two instances of the SPIM, namely SPIM0 and SPIM1. SPIMx refers to any of
these instance.

The SPIMF peripheral, which is described in chapter 19, share the same core functionality than the
SPIM but also allows interfacing with a an external flash memory component. The
SPIM_FLASH_BRIDGE_EN register bit is provided for this purpose. For SPIM0 and SPIM1 peripherals,
it is recommended to disable this functionality and leave the bridge enable bit at ‘0’. After a reset,
this bit is automatically reset to ‘0’ for SPIM0 and SPIM1, even if the register field description
indicates a reset value at ‘1’.



18.1                Functional description
The SPIM module supports the following features:

   ●       Full-duplex communication or TX only

   ●       Up to four slaves managed through separate SPIM_CSN_y selection signals

   ●       Configurable SCK clock frequency, internal clock divider

   ●       Configurable CPOL and CPHA

   ●       Configurable transaction bit length, up to 256 bits

   ●       Optional per-transaction protocol configuration

   ●       I2S protocol support

Before transmitting or receiving data through the SPIM, the relevant DIOs must be configured to
SPIMx_SCK, SPIMx_CSN_y and SPIMx_MOSI alternate functions. The SPIMx_MISO is optional and
is not needed in TX only mode.

The SPIM_CSN_SEL field allows to select which SPIMx_CSN_y signal will be asserted low during the
transaction, by setting its value to y.

Data are exchanged based on SPI transactions. An SPI transaction is the period of time during
which an SPIM_CSN_y is driven low. The end of the transaction is defined by the rising edge of
SPIM_CSN_y. When an active transaction is on-going, the SPIM_BUSY flag bit is set to ‘1’.




                                                                                              143
<PARSED TEXT FOR PAGE: 144 / 159>
                                                                       SPI master / I2S (SPIMx)


 Transactions are initiated when the SPIM_EN bit is raised and the TX FIFO contains at least one
 word. The enable bit can be written directly or set to ‘1’ or ‘0’ independently by writing a ‘1’
 respectively into SPIM_EN_SET or SPIM_EN_CLEAR.




Figure 26: SPIM peripheral schematic


          18.1.1       SCK frequency configuration
 The primary clock source of the SPIM module is the APB_CLK. Its frequency can be known or
 computed based on the system clocks configuration. The SPIM bit clock is generated from it
 through a programmable CLK_DIVIDER sub-block. The programmable division ratio ranges from 1
 to 256 and is set in the SPIM_DIV field.

 The following formula can be used to derive SPIM_DIV:
        SCK_FREQUENCY = Round(APB_CLK_FREQUENCY / (SPIM_DIV + 1))


          18.1.2       Protocol configuration
 The SPI bus can be configured in data modes 0 to 3, which defines the CPOL and CPHA settings,
 and the same configuration should be used on both the slave and master sides. The SPIM can be
 configured in one of these modes by setting the SPIM_CPOL and SPIM_CPHA register bits, according
 to Table 20.

 Additionally, the SPIM_MISO polarity can be inverted by setting SPIM_MISO_CPHA_INV = ‘1’, which
 can improve the timing margin towards the slaves in some cases.

 In case the SPIM needs only to transmit data, the SPIM_TX_ONLY register can be set to ‘1’. In this
 case, the RX FIFO will not be used.




                                                                                           144
<PARSED TEXT FOR PAGE: 145 / 159>
                                                                            SPI master / I2S (SPIMx)


By default, the SPIM_CSN_y signal is driven low during a transaction and raised when no data
transfer occurs. For some applications, it can be useful to keep the selection signal low between
transactions, potentially enabling transactions of any size. This can be achieved by setting the
SPIM_CSN_KEEP_LOW bit to ‘1’. This bit can also be set to ‘1’ or ‘0’ independently by writing a ‘1’ into
SPIM_CSN_LOW_SET or SPIM_CSN_LOW_CLEAR, respectively.


            18.1.3       Data transfer
During a transaction, data bits are simultaneously received from the SPIM_MISO and transmitted
on the SPIM_MOSI lines. When SPIM_TX_ONLY = ‘0’, both RX FIFO and TX FIFO are used to
respectively receive and transmit the data bits and interface with the CPU or the DMA.

The transaction size minus 1, in bits, should be configured in the SPIM_LEN register.

It is worth noting that the first data bit of a transaction always starts in a new word of the RX FIFO
or TX FIFO, hence if the number of bits of a given transaction is not a multiple of 32, the last word
is padded with zeroes.

       Transmitting data
Data transfer is initiated by enabling the SPIM with SPIM_EN = ‘1’, followed by transaction data
pushed into the TX FIFO, or conversely the block can be enabled after having stored at least one
word in the TX FIFO. In the latter case, setting the SPIM_ACC_MODE bit to ‘1’ ensures that the
maximum of transaction data are available in the TX FIFO before starting the transfer.

When N bits need to be sent to the slave, the TX FIFO must be filled with the block of bits
tx[0],tx[1],...tx[N-1] by writing successive groups of 32 bits into SPIM_TX_PUSH. The order in which
the data must be written depends on some configuration, as explained hereafter.

When N ≤ 32:

   ●    If SPIM_TX_MSB_ALIGN = ‘0’, tx[0],tx[1],...tx[N-1] must be written into SPIM_TX_PUSH[0:N-1].
        The (32 - N) MSBs of the 32-bit vector can be padded with zeroes.

   ●    If SPIM_TX_MSB_ALIGN = ‘1’, tx[0],tx[1],...tx[N-1] must be written into SPIM_TX_PUSH[32-
        N:31]. The (32 - N) LSBs of he 32-bit vector can be padded with zeroes.

In both cases, the SPIM_LSB_FIRST bit allows to choose whether the first transmitted bit of the
transaction is tx[0] (if ‘1’) or tx[N-1] (if ‘0’).

If N > 32, only the SPIM_LSB_FIRST configuration bit defines how TX data shall be loaded in the
FIFO.

   ●    If SPIM_LSB_FIRST = ‘1’, successive 32-bit groups of tx data should be written to
        SPIM_TX_PUSH, starting with bit tx[0]. The first group is then tx[0],tx[1],...tx[31] and the last
        group contains the last (N modulo 32) tx bits, completed with zeros on its MSBs.

   ●    If SPIM_LSB_FIRST = ‘0’, successive 32-bit groups of tx data should be written to
        SPIM_TX_PUSH, starting with bit tx[N-32]. The first group is then tx[N-32],tx[N-31],...tx[N-1]
        and the last group contains the first (N modulo 32) tx bits MSB aligned and completed with
        zeros on its LSBs.




                                                                                                 145
<PARSED TEXT FOR PAGE: 146 / 159>
                                                                                                   SPI master / I2S (SPIMx)


The CPU is responsible for TX FIFO flow control and avoiding any overflow or underflow. The
current word level can be read from the SPIM_TX_FIFO_WORD_NB register and the
SPIM_TX_FIFO_EMPTY or SPIM_TX_FIFO_FULL flags can help in getting the TX FIFO status. The
SPIM_TX_IRQ event and SPIM peripheral IRQ can be used to manage more efficiently this FIFO.

     Receiving data
As for the transmitted bits, the SPIM_LSB_FIRST bit defines whether the first bit received after the
SPIM_CSN_y falling edge is the LSB (rx[0]) or the MSB (rx[N-1]) of an N-bit transaction. The RX
FIFO is filled with groups of 32 bits and a word can be popped out of it by reading the
SPIM_RX_POP register.
For transactions smaller than 32 bits, the SPIM_RX_MSB_ALIGN bit defines whether the received
data are placed on the MSBs or on the LSBs of the stored word.

The CPU is responsible for RX FIFO flow control and avoiding any overflow or underflow. The
current word level can be read from the SPIM_RX_FIFO_WORD_NB register and the
SPIM_RX_FIFO_EMPTY or SPIM_RX_FIFO_FULL flags can help in getting the RX FIFO status. The
SPIM_RX_IRQ event and SPIM peripheral IRQ can be used to manage more efficiently this FIFO.

It is recommended to manage the FIFO transaction-based, i.e. before starting a new transaction, the
RX FIFO should be empty. If the content of the RX FIFO related to an older transaction is not
needed by the application, it can simply be dropped by writing ‘1’ in the SPIM_RX_FLUSH bit.


             18.1.4                  Per-transaction configuration
The SPIM module allows providing a different configuration for every single SPI transaction by
writing first a 32-bit configuration word, or header, in the TX FIFO. This word contains field settings
for the next transaction according to the following mapping:

 0   1   2   3           4   5   6   7   8   9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
              SPIM_LEN




                                                           SPIM_RX_MSB_ALIGN
                                                                               SPIM_TX_MSB_ALIGN




                                                                                                   SPIM_MISO_CPHA_INV
                                                                                                                        SPIM_CPOL
                                                                                                                                    SPIM_CPHA


                                                                                                                                                SPIM_LSB_IFRST
                                                                                                                                                                 SPIM_TX_ONLY

                                                                                                                                                                                SPIM_CSN_SEL




This mode is enabled when SPIM_HEADER_EN bit is set to ‘1’. When enabled, the SPIM_CPOL,
SPIM_CPHA,      SPIM_LSB_FIRST, SPIM_CSN_SEL,         SPIM_TX_ONLY,     SPIM_TX_MSB_ALIGN,
SPIM_RX_MSB_ALIGN and SPIM_LEN register fields are ignored and replaced by those contained in
the configuration word.

Once the configuration word has been written in the TX FIFO through SPIM_TX_PUSH, the data bits
can be loaded. A new configuration word for another transaction can be pushed in the TX FIFO
after the previous transaction data have all been loaded.




                                                                                                                                                                                146
<PARSED TEXT FOR PAGE: 147 / 159>
                                                                         SPI master / I2S (SPIMx)



          18.1.5      Using the DMA peripheral
The DMA can be used to handle efficiently SPI transfers. Instead of managing the TX FIFO level,
the CPU can program and trigger data transfer from a memory location. The size of the transfer is
configured in the DMA module. Similarly, data reception can be directly stored into a reserved
memory area, and the CPU can be notified by an interrupt only once a pre-defined number of data
have been received.

To use this feature, the DMA channels must be first configured as described in the Direct memory
access controller (DMA) section. Then the SPIM_DMA_MODE bit can be written to ‘1’. The SPIM_EN bit
must also be set to ‘1’ for the DMA transfers to start.


          18.1.6
                        2
                      I S protocol support
Inter-IC Sound (I2S) is a simplex serial bus interface standard used for connecting digital audio
devices, typically a microphone sensor. The communication is synchronous with a separate clock
and data lines, making it very simmilar to the SPI protocol. The main difference lies in the
management of the CSN selection signal, which functions as an audio channel selector. Additionally,
the data transfer is treated as a continuous stream.

The SPIM module can function as an I 2S master, driving the WS and SCK lines. A typical I 2S data
transfer is represented in Error: Reference source not found.

SPIMx_CSN_y is used as the word select (WS) signal, SPIMx_SCK is the clock and the data line can
be either SPIMx_MOSI or SPIMx_MISO, depending on whether the module is used as an I2S
transmitter or receiver. When in receiver mode, the I2S_DIR_MODE bit must be set to ‘1’; otherwise
in transmitter mode I2S_DIR_MODE = ‘0’.




Figure 27: I2S protocol

I2S data are always transmitted MSB first, starting on the second rising edge after the WS transition.
In the figure, n-bit data words are transmitted in stereo mode. In the case of mono audio, only one
word out of two contains relevant data, however all words are stored or read from the FIFOs. Also
some bits within a word could only be padding. For example, d[1] and d[0] bits of each word could
be padding and d[2] is the actual data LSB. In any case, the transaction length, defined in the
SPIM_LEN field, should be set to n, the word size in bits.
The I2S mode of the SPIM can be enabled by setting the I2S_CHAN_MODE field to a value different
from 0x0. The most common configuration is I2S_CHAN_MODE = 0x3, for which the word selection



                                                                                              147
<PARSED TEXT FOR PAGE: 148 / 159>
                                                                            SPI master / I2S (SPIMx)


signal alternates between left and right audio channel. It can be noted that even if a FIFO overflow
or underflow occurs, the clock and WS signal are not interrupted.
To ensure the I2S mode works correctly, other register fields should be configured as follows, before
enabling the I2S mode:
       ● SPIM_CPOL = ‘0’ and SPIM_CPHA = ‘0’
       ● SPIM_CSN_KEEP_LOW = ‘0’

       ● SPIM_HEADER_EN = ‘0’

       ● SPIM_LSB_FIRST = ‘0’

       ● SPIM_TX_ONLY = ‘0’

       ● SPIM_ACC_MODE = ‘0’



Also SPIM_DIV should be configured according to the expected audio sampling ratio.


              18.1.7       SPIM events and interrupt
There are four events which can assert the SPIM peripheral interrupt signal towards the CPU:

   ●       RX FIFO above threshold: this event is triggered each time a new data is stored in the RX
           FIFO and the number of words stored becomes strictly higher than the threshold defined in
           SPIM_IRQ_RX_THRESHOLD. This event contributes to the interrupt generation if the
           SPIM_RX_IRQ_EN bit is set to ‘1’.
   ●       TX FIFO below threshold: this event is triggered each time a data is fetched from the TX FIFO
           for transmission and the number of words stored becomes lower than or equal to the
           threshold defined in SPIM_IRQ_TX_THRESHOLD. This event contributes to the interrupt
           generation if the SPIM_TX_IRQ_EN bit is set to ‘1’.

   ●       RX error event is triggered whenever the RX FIFO undergo a data underflow (FIFO read while
           empty) or overflow (requiring to stored a new word but FIFO is full).

   ●       TX error event is triggered whenever the TX FIFO undergo a data underflow (FIFO data
           popped out while empty) or overflow (FIFO write while full).

The RX or TX error events contribute to the interrupt generation if the SPIM_ERR_IRQ_EN bit set to
‘1’.

When an event occurs, its corresponding flag set to ‘1’ in SPIM_STATUS. These flags are
SPIM_RX_IRQ, SPIM_TX_IRQ, SPIM_RX_ERROR and SPIM_TX_ERROR. They can be cleared to ‘0’ by
writing a ‘1’ respectively into SPIM_CLEAR_RX_IRQ, SPIM_CLEAR_TX_IRQ, SPIM_CLEAR_RX_ERROR
or SPIM_CLEAR_TX_ERROR bits.



18.2               Registers

              18.2.1       SPIM_VERSION
SPIM hardware version register
Address offset: 0x0000


                                                                                                148
<PARSED TEXT FOR PAGE: 149 / 159>
                                                                             SPI master / I2S (SPIMx)


Reset value: 0x00010002

Field                     Bits    Type Reset   Description
SPIM_REV_MINOR            [15:0] RO    2       SPIM hardware revision minor
SPIM_REV_MAJOR            [31:16] RO   1       SPIM hardware revision major



         18.2.2        SPIM_CTRL
SPIM protocol control register
Address offset: 0x0004
Reset value: 0x00410300

Field                     Bits    Type Reset   Description
SPIM_DIV                  [7:0]   RW   0       SPIM clock division ratio = SPIM_DIV + 1
SPIM_IRQ_TX_THRESHOLD     [9:8]   RW   3       Generate a TX event when the TX FIFO level gets lower than
                                               this threshold
SPIM_IRQ_RX_THRESHOLD     [12:11] RW   0       Generate an RX event when the RX FIFO level gets strictly
                                               higher than this threshold
SPIM_RX_MSB_ALIGN         [14]    RW   0       For transactions smaller than 32 bits, specifies where
                                               received data are placed within the RX FIFO 32-bit word:
                                               '0’: aligned on LSBs
                                               '1’: aligned on MSBs
SPIM_TX_MSB_ALIGN         [15]    RW   0       For transactions smaller than 32 bits, specifies where
                                               transmitted data are placed within the TX_FIFO 32-bit word:
                                               '0’: aligned on LSBs
                                               '1’: aligned on MSBs
SPIM_EN                   [16]    RW   1       SPI master block enable
SPIM_TX_IRQ_EN            [17]    RW   0       When ‘1’, an IRQ is generated for TX event
SPIM_RX_IRQ_EN            [18]    RW   0       When ‘1’, an IRQ is generated for RX event
SPIM_ERR_IRQ_EN           [19]    RW   0       When ‘1’, an IRQ is generated for errors event
SPIM_DMA_MODE             [20]    RW   0       DMA triggers enable
SPIM_HEADER_EN            [21]    RW   0       Header-based transaction mode enable
SPIM_FLASH_BRIDGE_EN      [22]    RW   1       Flash bridge mode enable
SPIM_CSN_KEEP_LOW         [23]    RW   0       When ‘1’, the selected CSN signal is maintained low
SPIM_CPOL                 [24]    RW   0       CPOL configuration
SPIM_CPHA                 [25]    RW   0       CPHA configuration
SPIM_MISO_CPHA_INV        [26]    RW   0       When ‘1’, MISO polarity is inverted
SPIM_LSB_FIRST            [27]    RW   0       When ‘0’, the transaction MSB is received/transmitted first,
                                               else this is the LSB
SPIM_TX_ONLY              [28]    RW   0       When ‘1’, SPIM is in transmit only mode
SPIM_CSN_SEL              [30:29] RW   0       CSN output signal selection
SPIM_ACC_MODE             [31]    RW   0       When ‘1’, waits that the TX FIFO is full or contains all data to
                                               start a transaction



         18.2.3        SPIM_LEN
Transaction length register
Address offset: 0x0008
Reset value: 0x0000003F




                                                                                                       149
<PARSED TEXT FOR PAGE: 150 / 159>
                                                                             SPI master / I2S (SPIMx)


Field                      Bits    Type Reset   Description
SPIM_LEN                   [7:0]   RW   63      SPI transaction length = (1 + SPIM_LEN) bits



           18.2.4      SPIM_TRIG_CTRL
Trigger control register
Address offset: 0x000C
Reset value: 0x00000000

Field                      Bits    Type Reset   Description
SPIM_EN_SET                [0]     RWS 0        When written ‘1’, set SPIM_EN = 1. This field is always read as
                                                0
SPIM_EN_CLEAR              [1]     RWC 0        When written ‘1’, set SPIM_EN = 0. This field is always read as
                                                0
SPIM_CSN_LOW_SET           [2]     RWS 0        When written ‘1’, set SPIM_CSN_KEEP_LOW = 1. This field is
                                                always read as 0
SPIM_CSN_LOW_CLEAR         [3]     RWC 0        When written ‘1’, set SPIM_CSN_KEEP_LOW = 0. This field is
                                                always read as 0
SPIM_RX_FLUSH              [4]     RW   0       When written ‘1’, flush all words in the Rx FIFO. This field is
                                                always read as 0. Automatically returns to 0 after a write
                                                request



           18.2.5      SPIM_STATUS
SPIM status register
Address offset: 0x0010
Reset value: 0x00000000

Field                      Bits    Type Reset   Description
SPIM_TX_FIFO_WORD_NB       [2:0]   RO   0       Current number of words stored in the TX FIFO
SPIM_TX_FIFO_EMPTY         [4]     RO   0       TX FIFO empty flag
SPIM_TX_FIFO_FULL          [5]     RO   0       TX FIFO full flag
SPIM_TX_IRQ                [6]     RO   0       TX event flag
SPIM_TX_ERROR              [8]     RO   0       TX error event flag
SPIM_RX_FIFO_WORD_NB       [18:16] RO   0       Current number of words stored in the RX FIFO
SPIM_RX_FIFO_EMPTY         [20]    RO   0       RX FIFO empty flag
SPIM_RX_FIFO_FULL          [21]    RO   0       RX FIFO full flag
SPIM_RX_IRQ                [22]    RO   0       RX event flag
SPIM_RX_ERROR              [24]    RO   0       RX error event flag
SPIM_BUSY                  [31]    RO   0       Read to ‘1’ while a transaction is on-going



           18.2.6      SPIM_STATUS_CLEAR
SPIM status clear register
Address offset: 0x0014
Reset value: 0x00000000




                                                                                                      150
<PARSED TEXT FOR PAGE: 151 / 159>
                                                                              SPI master / I2S (SPIMx)


Field                     Bits     Type Reset   Description
SPIM_CLEAR_TX_IRQ         [0]      RWC   0      TX event flag clear. This field is always read as 0
SPIM_CLEAR_TX_ERROR       [8]      RWC   0      TX error event flag clear. This field is always read as 0
SPIM_CLEAR_RX_IRQ         [16]     RWC   0      RX event flag clear. This field is always read as 0
SPIM_CLEAR_RX_ERROR       [24]     RWC   0      RX error event flag clear. This field is always read as 0



         18.2.7       SPIM_TX_PUSH
TX data pushed register
Address offset: 0x0018
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SPIM_TX_DATA              [31:0]   RW    0      SPI write data to TX FIFO. This field is always read as 0



         18.2.8       SPIM_RX_POP
TX data popped register
Address offset: 0x001C
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
SPIM_RX_DATA              [31:0]   RO    0      SPI read data from RX FIFO



         18.2.9       I2S_CTRL
I2S mode control register
Address offset: 0x0020
Reset value: 0x00000000

Field                     Bits     Type Reset   Description
I2S_CHAN_MODE             [1:0]    RW    0      I2S mode enable:
                                                0x0: I2S mode disabled
                                                0x1: I2S mode enabled, CSN kept high
                                                0x2: I2S mode enabled, CSN kept low
                                                0x3: 0x1: I2S mode enabled, CSN alternating
I2S_DIR_MODE              [2]      RW    0      I2S direction when I2S mode is enabled:
                                                '0’: the SPIM is in TX (SD = MOSI)
                                                '1’: the SPIM is in RX (SD = MISO)




                                                                                                        151
<PARSED TEXT FOR PAGE: 152 / 159>
                                                                         SPI master flash (SPIMF)




19 SPI master flash (SPIMF)

19.1           Functional description
The SPIMF is similar to the SPIM peripheral and can act as a standard SPI master module. However,
it is specifically meant to interface with an external SPI-based flash memory.

The SPIMF_RSTN pin alternate function can be optionally used to generate an active-low reset
signal towards the external flash memory, if needed. When the SPIMF component is reset, the
SPIMF_RSTN signal is asserted low and rises after 30,000 APB_CLK clock cycles. This ensures that
the flash reset signal is active at least 1 ms with an APB clock frequency lower than 30 MHz.


          19.1.1       Flash bridge mode
When SPIM_FLASH_BRIDGE_EN register bit is set to ‘1’, reading from an external flash becomes
transparent and the flash space is accessed from the “EXT flash” memory space, as seen in Figure 4.
It can be noted that SPIMF_SCK, SPIMF_CSN_0, SPIMF_MOSI and SPIMF_MISO alternate
functions must be selected for the corresponding DIOs.

When the bridge function is activated, reading at address (0x2004000 + 4*k) allows to read four
bytes from the external flash located at its internal address (0x0 + 4*k). The SPIMF takes care of
sending the correct flash READ command with instruction code = 0x03. The external flash memory
component should support this command to ensure the bridge functionality.

The EXT flash bridge is functional with most flash memories in single data rate (SDR) mode, using
this SPIMF register configuration,:
     ● SPIM_LSB_FIRST = ‘0’

    ● SPIM_CPOL = ‘0’
    ● SPIM_CPHA = ‘0’




          19.1.2       Optimized reading from external flash
Enabling the flash bridge mode is not required to read data from an external flash; it only facilitates
the read accesses. A dedicated protocol could also be implemented, sending bytes through the
SPIMF as a regular SPI master. This may be useful to achieve full read performance, increasing the
SPIMF clock frequency or reading bytes in continuous bursts.

For example, some flash memories allows high read clock frequencies (> 50 MHz) but requires a
specific instruction and some dummy cycles between the instruction/address phase and the first
data.

Additionally, the bridge mode always sends a 32-bit instruction/address word for every 32-bit data
read access. Reading multiple 32-bit words at contiguous addresses in a single command word is
not possible using the bridge but can be specifically implemented, conveniently with the help of the
DMA module.



                                                                                               152
<PARSED TEXT FOR PAGE: 153 / 159>
                                                                        SPI master flash (SPIMF)



          19.1.3      Writing in external flash
Writing to an external flash through the SPIMF is also possible, but the module must be used as a
regular master SPI device; that is, the SPIM_FLASH_BRIDGE_EN should set to ‘0’. Writing to the EXT
flash space has no effect. The flash memory component requires specific commands and internal
register configuration to enable erasing or writing new data in its memory cells. Please refer to the
flash memory specification for details on the required programming and protocol.



19.2           Registers
The same register map as for the SPIM is used (refer to 18.2).




                                                                                             153
<PARSED TEXT FOR PAGE: 154 / 159>
                                                                         SPI direct access (SPID)




20 SPI direct access (SPID)
The SPI direct access (SPID) provides a straightforward means of reading from or writing to any
mapped 32-bit global address of the system. It it a slave SPI device typically driven by a host master
SPI. It supports read and write operations in burst mode, with or without address auto-increment.



20.1            Functional description
As shown in Figure 1, the SPID module can have a direct access to the AXI bus. It is able to read
from and write to peripheral registers and different memories. Consequently, this peripheral should
be used with care and enabled only when needed.

To use the SPID functionalities, the SPID_SCK, SPID_CSN, SPID_MOSI and SPID_MISO pin
alternate functions must be enabled. Additionally, the peripheral clock should be enabled in the
SCU_PERIPH_CLK_EN register. Conversely, the SPID has no effect if the alternate functions for the
block are not configured or if the clock of the block is disabled.


           20.1.1       SPID requirements
The SPID has a fixed protocol configuration for clock polarity and data sampling. Only data mode 2,
with CPOL = 1 and CPHA = 0 are supported (refer to section 17.1.1 for more information).

The maximum SPID_SCK input clock frequency is limited by the general I/O maximum toggling rate
and must be the frequency must be lower than the 90% of the configured APB_CLK frequency. The
clock can be free-running or only active while SPID_CSN is low.


           20.1.2       Address/data protocol
The SPI protocol is based on pre-defined transactions, which start when SPID_CSN goes low and
end when it returns to a high level. Each transaction contains:
     ●   2 command bits
     ●   30 bits of address
     ●   8 dummy read bits (for read operations)
     ●   n * 32 bits of data

Each field is sent with the MSB first.

The command bits are coded as follows:
     ●   Command = 0x0 → Write access, no address auto-increment
     ●   Command = 0x1 → Write access, +4 address auto-increment
     ●   Command = 0x2 → Read access, no address auto-increment
     ●   Command = 0x3 → Read access, +4 address auto-increment

The address corresponds to bits 2 to 31 of the global address space, assuming the two LSBs of the
address are 0b00. This means only 32-bit, word-aligned data can be read or written.


                                                                                              154
<PARSED TEXT FOR PAGE: 155 / 159>
                                                                       SPI direct access (SPID)


In case of a read command, 8 dummy bits are inserted to ensure the data is provided on time.

The data field corresponds to the read or written words. As long as the SPI_CSN pin is low and a
clock is provided, multiple words are read or written in sequence. Internally, the address
corresponding to the current word remains the same (no address auto-increment) or is
automatically incremented by 4, depending on the command used.

    Case of reading FIFO data
Generally a data FIFO read port is mapped to a specific register of a peripheral. To read multiple
data efficiently from such a FIFO, multiple reads at the same address are performed. Consequently,
to read several FIFO data in burst mode, the command 0x2 should be used.

By default, the next data of the FIFO is popped out right after a read access, which means that
reading N words in burst will actually result in N+1 words being popped out of the FIFO. To avoid
this unwanted side effect, the host SPI must indicate the last word that needs to be read. This is
achieved by sending all ‘1’ on the SPID_MOSI line for the last word read. An example is shown
below.

    Write transaction
Write transactions examples are provided in Figure 28 and Figure 29, respectively with and without
address auto-increment, for which two 32-bit data are written starting at address A. The total
number of clock cycles of the transactions is 96.




Figure 28: SPID write transaction with address auto-increment




                                                                                           155
<PARSED TEXT FOR PAGE: 156 / 159>
                                                                         SPI direct access (SPID)




Figure 29: SPID write transaction, no address auto-increment


    Read transaction
Read transactions examples are provided in Figure 30 and Figure 31, respectively with and without
address auto-increment, for which two 32-bit data are read starting at address A. The total number
of clock cycles of the transactions is 104.

Note 1: 8 dummy bits are inserted between the last address bit and the first data bit.
Note 2: During the 32 bits of the last data, the SPID_MOSI line is high to indicate this word is the
last one. This is only required when reading from a FIFO, otherwise the line is “don’t care”.




Figure 30: SPID read transaction with address auto-increment




                                                                                            156
<PARSED TEXT FOR PAGE: 157 / 159>
                                                                        SPI direct access (SPID)




Figure 31: SPID read transaction, no address auto-increment




 20.2          Registers
The SPID peripheral has a fixed configuration and does not include any register bank.




                                                                                         157
<PARSED TEXT FOR PAGE: 158 / 159>
                                                                          Debug module (DBGM)




21 Debug module (DBGM)
The DBGM provides debugging features of the RISC-V core and system, commonly controlled by a
GDB debugger for C/C++. It can be accessed over a JTAG interface via an external debugger device.
The following JTAG pins are provided as alternate functions:
     ● JTAG_TCK – input test clock, synchronizing the operations.

    ●   JTAG_TMS – input test mode select, which determines the next state.
    ●   JTAG_TRSTN – input test reset (optional), which can reset the TAP controller state.
    ●   JTAG_TDI – input test data
    ●   JTAG_TDO – output test data

This module has a reserved memory space reserved for its operations, mapped from address
0x2001000 to 0x2001FFF. The application should not try to read or write to this area.

The debug module is compliant with the RISC-V Debug Specification, version 0.13.2.




                                                                                              158
<PARSED TEXT FOR PAGE: 159 / 159>
                                                                  Package information




22 Package information
The ColibryNPU is available in a QFN56 7 x 7 mm package. The mechanical dimensions, as
specified in Figure 32, are given in millimeters.




       Figure 32: QFN56 package specification




                                                                               159
