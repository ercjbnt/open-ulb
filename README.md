# open-ulb
An open source underwater locator beacon

This is designed to work with the 44kHz piezos sold at JLI electronics:
https://www.jlielectronics.com/diy-accessories/jli-pz001/



# TX\_Main — 44 kHz Ultrasonic Transmitter PCB


* **Piezo:** 44 kHz, \~5.8 nF
* **MCU:** ESP32 (replacing Teensy)
* **Adjustable bus voltage:** LT8362 SEPIC + AD5241 (I²C)
* **H-bridge:** 2× LTC4444-5 gate drivers + 4× FDD8780 MOSFETs
* **Matching:** Series-L network to resonate with the piezo near 44 kHz

**Voltage limit:** FDD8780 is 25 V max VDS. Firmware must cap VBUS ≤ \~21 V.

---

## Repo status (Rev-A)

* Schematics complete for TX power path, SEPIC supply, control header, and test points
* Nets and labels consistent: `SW_A`, `SW_B`, `VBUS`, `VDRV_12V`, `SW_SEPIC`, `CTRL_3V3`, `SCL`, `SDA`
* DNP options in place for tuning (see below)
* PCB layout not committed yet; placement guidance below
* Firmware pending: simple I²C driver for AD5241 and four PWM channels with dead-time on ESP32

**ERC/DRC:** clean except expected single-pin test-point warnings.
**Double-check before ordering:** SEPIC catch diode D4 must have its cathode on VBUS and anode on `SW_SEPIC`.

---

## Hardware blocks

### 1) H-bridge (transmitter)

* Gate drivers: 2× LTC4444-5 (U4/U5)
* Bootstrap per driver: 100 nF between BOOST–TS and B5819W diode from VDRV\_12V to BOOST
* VCC decoupling per driver: 0.1 µF + 1 µF to EP (GND) placed at the pins
* MOSFETs: 4× FDD8780 (TO-252-3)
* Gate resistors: 5.1 Ω in series at each gate, placed at the pads
* Gate pulldowns: 10 kΩ gate-to-source on the MOSFET side of the series resistor
* Bus decoupling near bridge: 1 µF ×2 + 0.1 µF + bulk 100 µF from VBUS to GND

### 2) Matching network and connector

* Series L: 1.0 mH + 1.0 mH in series (main)
* Trim inductors: 220 µH (DNP) + 33 µH (DNP) in series after the 1 mH pair
* Piezo header P1: PZ+ at the end of the L-chain, PZ− tied to `SW_B` (floating load)
* Snubber (DNP initially): 100 Ω + 1 nF in series across PZ+ to PZ−

### 3) SEPIC supply (adjustable VBUS)

* Controller: LT8362 (MSOP-16 with EP)
* Key pins and passives: INTVCC 1 µF; RT 45.3 kΩ (\~1 MHz); SS 47 nF; SYNC/MODE 100 kΩ to GND
* Compensation (VC): 36.5 kΩ in series with 680 pF to GND, plus 10 nF VC to GND
* Power train: L3 10 µH (VIN→SW), L4 10 µH (SW→VBUS), CDC 4.7 µF 50 V X7R (VIN↔VBUS), DFLS260 catch diode (anode SW, cathode VBUS), COUT 10 µF + 0.1 µF at diode cathode
* Feedback (adjustable): VBUS → 1.00 MΩ → FBX, and FBX → 82 kΩ → AD5241(W); AD5241(B) to GND, (A) NC
* Digital pot: AD5241BRZ100 on 3.3 V with 4.7 kΩ pull-ups on SDA and SCL
* Expected range: VBUS ≈ 10.4 V to 21.1 V (firmware must clamp ≤ 21 V)

### 4) Power and control I/O

* H2: VDRV\_12V and GND (drivers and LT8362 VIN)
* H3: VBUS and GND (bench input / measurement point)
* H1 (1×8): CTRL\_3V3, GND, PWM\_AH, PWM\_AL, PWM\_BH, PWM\_BL, SDA, SCL

---

## ESP32 pin suggestions

* PWM\_AH / PWM\_AL / PWM\_BH / PWM\_BL: GPIO 25 / 26 / 27 / 14
* SDA: GPIO 21
* SCL: GPIO 22
* CTRL\_3V3: ESP32 dev board 3V3 with common ground

---

## DNP list for Rev-A bring-up

* L1 220 µH and L2 33 µH (trim inductors)
* R9 100 Ω and C14 1 nF (snubber across PZ+–PZ−)

Populate trims only if frequency sweep indicates the resonance sits above 44 kHz; populate the snubber only if scope shows significant ringing/overshoot.

---

## Bring-up 

* Visual and continuity: confirm D4 orientation; exposed pads of LT8362 and both LTC4444s are tied to GND
* Power drivers only: apply VDRV\_12V; verify low quiescent current and no heating
* SEPIC alone (no piezo): VIN\_12V = 12 V; verify VBUS mid-range; adjust via I²C; keep ≤ 21 V
* H-bridge dry run: with VBUS 0–12 V, scope SW\_A and SW\_B; confirm complementary PWM and dead-time
* With piezo: start VBUS ≈ 12 V; sweep 41–49 kHz to find amplitude peak
* If peak > 44 kHz: stuff +220 µH; if still high: add +33 µH
* Only add the snubber if needed for damping or EMI

---

## Files and naming

* Schematic: `TX_Main` (EasyEDA)
* Important nets: `SW_A`, `SW_B`, `VBUS`, `VDRV_12V`, `SW_SEPIC`, `FBX`, `CTRL_3V3`
* Test points: `TP_VBUS`, `TP_SW_A`, `TP_SW_B`, `TP_PZ+`, `TP_GND`, `TP_SW_SEPIC`, `TP_FBX`, `TP_VC`, `TP_VIN`

---



