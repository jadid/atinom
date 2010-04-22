/*
 * By Lars Englund 2007
 * lars . englund at gmail . com
 *
 * Cypress Wireless USB module (CYWM6934 and CYWM6935 (CYWUSB6934, CYWUSB6935)) demo, see project page for more information:
 * http://code.google.com/p/cywusb
 *
 * $Id$
 */

/*
From CYWM693x datasheet:

	Table 7-1. CYWUSB6932/CYWUSB6934 Register Map[2]
	Register Name Mnemonic Address Page Default Access
	Revision ID REG_ID 0x00 8 0x07 RO
	Control REG_CONTROL 0x03 8 0x00 RW
	Data Rate REG_DATA_RATE 0x04 9 0x00 RW
	Configuration REG_CONFIG 0x05 9 0x01 RW
	SERDES Control REG_SERDES_CTL 0x06 10 0x03 RW
	Receive SERDES Interrupt Enable REG_RX_INT_EN 0x07[1] 11 0x00 RW
	Receive SERDES Interrupt Status REG_RX_INT_STAT 0x08[1] 12 0x00 RO
	Receive SERDES Data A REG_RX_DATA_A 0x09[1] 13 0x00 RO
	Receive SERDES Valid A REG_RX_VALID_A 0x0A[1] 13 0x00 RO
	Receive SERDES Data B REG_RX_DATA_B 0x0B[1] 13 0x00 RO
	Receive SERDES Valid B REG_RX_VALID_B 0x0C[1] 13 0x00 RO
	Transmit SERDES Interrupt Enable REG_TX_INT_EN 0x0D 14 0x00 RW
	Transmit SERDES Interrupt Status REG_TX_INT_STAT 0x0E 15 0x00 RO
	Transmit SERDES Data REG_TX_DATA 0x0F 16 0x00 RW
	Transmit SERDES Valid REG_TX_VALID 0x10 16 0x00 RW
	PN Code REG_PN_CODE 0x18–0x11 16 0x1E8B6A3DE0E9B222 RW
	Threshold Low REG_THRESHOLD_L 0x19[1] 17 0x08 RW
	Threshold High REG_THRESHOLD_H 0x1A[1] 17 0x38 RW
	Wake Enable REG_WAKE_EN 0x1C 17 0x00 RW
	Wake Status REG_WAKE_STAT 0x1D 18 0x01 RO
	Analog Control REG_ANALOG_CTL 0x20 18 0x04 RW
	Channel REG_CHANNEL 0x21 19 0x00 RW
	Receive Signal Strength Indicator REG_RSSI 0x22[1] 19 0x00 RO
	PA Bias REG_PA 0x23 19 0x00 RW
	Crystal Adjust REG_CRYSTAL_ADJ 0x24 20 0x00 RW
	VCO Calibration REG_VCO_CAL 0x26 20 0x00 RW
	Reg Power Control REG_PWR_CTL 0x2E 20 0x00 RW
	Carrier Detect REG_CARRIER_DETECT 0x2F 21 0x00 RW
	Clock Manual REG_CLOCK_MANUAL 0x32 21 0x00 RW
	Clock Enable REG_CLOCK_ENABLE 0x33 21 0x00 RW
	Synthesizer Lock Count REG_SYN_LOCK_CNT 0x38 21 0x64 RW
	Manufacturing ID REG_MID 0x3C–0x3F 21 – RO
*/

#define REG_WRITE			0x80

#define REG_ID				0x00
#define REG_CONTROL			0x03
#define REG_CONFIG			0x05
#define REG_SERDES_CTL		0x06
#define REG_MID				0x3C // 0x3C–0x3F

#define REG_RX_INT_EN		0x07
#define REG_RX_INT_STAT		0x08
#define REG_RX_DATA_A		0x09
#define REG_RX_VALID_A		0x0A
#define REG_RX_DATA_B		0x0B
#define REG_RX_VALID_B		0x0C

#define REG_TX_INT_EN		0x0D
#define REG_TX_INT_STAT		0x0E
#define REG_TX_DATA			0x0F
#define REG_TX_VALID		0x10

#define REG_ANALOG_CTL		0x20
#define REG_CHANNEL			0x21
#define REG_RSSI			0x22
#define REG_PA				0x23
#define REG_VCO_CAL			0x26
#define REG_PWR_CTL			0x2E
#define REG_CARRIER_DETECT	0x2F
#define REG_CLOCK_MANUAL	0x32
#define REG_CLOCK_ENABLE	0x33
