
/************************************************************************/
/*               (C) Fujitsu Microelectronics Europe GmbH               */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user’s obligation to fully test the software in its        */
/* environment and to ensure proper functionality, qualification and    */
/* compliance with component specifications.                            */
/*                                                                      */
/* In the event the software deliverable includes the use of open       */
/* source components, the provisions of the governing open source       */
/* license agreement shall apply with respect to such software          */
/* deliverable.                                                         */
/* FME does not warrant that the deliverables do not infringe any       */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FME disclaims all  */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FME’s liability   */
/* is restricted to intention and gross negligence.                     */
/* FME is not liable for consequential damages.                         */
/*                                                                      */
/* (V1.3)                                                               */
/************************************************************************/

/* -----------------------------------------------------------------*/
/* History:                                                       	*/
/* Date        Version  Author  Description              						*/
// 14.09.2009  1.0      CHa     First release for FMA1125


#define TSC_ID				   0x68	  // I2C address of FMA1125 (0x68 or 0x69)

#define TSC_RESET_PIN PDR04_P3
#define TSC_RESET_DDR DDR04_D3

#define TSC_FILTER_EN    0x01   // Mode selection register values
#define TSC_APIS_1       0x02   // Mode 1 / only strongest sensor output is signaled
#define TSC_APIS_2       0x04   // Mode 2 / reports all outputs that exceeds pre-defined thresholds
#define TSC_APIS_3       0x08   // Mode 3 / reports two strongest outputs - for multitouch applications

#define	TSC_ALPHA_00                 0x00        // R/W Registers
#define	TSC_ALPHA_01                 0x01
#define	TSC_ALPHA_02                 0x02
#define	TSC_ALPHA_03                 0x03
#define	TSC_ALPHA_04                 0x04
#define	TSC_ALPHA_05                 0x05
#define	TSC_ALPHA_06                 0x06
#define	TSC_ALPHA_07                 0x07
#define	TSC_REF_DELAY                0x08
#define	TSC_BETA                     0x09
#define	TSC_AIC_WAIT                 0x0A
#define	TSC_STR_THRES_00             0x0B
#define	TSC_STR_THRES_01             0x0C
#define	TSC_STR_THRES_02             0x0D
#define	TSC_STR_THRES_03             0x0E
#define	TSC_STR_THRES_04             0x0F
#define	TSC_STR_THRES_05             0x10
#define	TSC_STR_THRES_06             0x11
#define	TSC_STR_THRES_07             0x12
#define TSC_FEATURE	                 0x13
#define	TSC_INT_TIME                 0x14
#define	TSC_IDLE_TIME	               0x15
#define	TSC_CONTROL_1	               0x16
#define	TSC_CONTROL_2	               0x17
#define	TSC_PA_DATA_OUT              0x18
#define	TSC_GPIO_DATA_OUT            0x19
#define	TSC_PA_DIR                   0x1A
#define	TSC_GPIO_DIR                 0x1B
#define	TSC_PA_CONF                  0x1C
#define	TSC_GPIO_CONF                0x1D
#define	TSC_CAL_INTERVAL             0x1E
#define	TSC_GINT_IRQ_MASK            0x1F
#define	TSC_GINT_IRQ_CLEAR           0x20
#define	TSC_PA_EINT_ENABLE           0x21
#define	TSC_GPIO_EINT_ENABLE         0x22
#define	TSC_FILTER_PERIOD            0x23
#define	TSC_FILTER_THRESH            0x24
#define	TSC_CONTROL_3                0x25
#define	TSC_GINT_IRQ_EDGE_ENABLE     0x26
#define	TSC_GPIO_INPUT_BOUNCE_CANCEL 0x27
#define	TSC_REGISTER_CHECK           0x28
#define	TSC_PA03_R_SEL               0x29
#define	TSC_PA47_R_SEL               0x2A
#define	TSC_REF_R_SEL                0x2B
#define	TSC_BETA_DISABLE             0x2C
#define	TSC_GPIO01_DIMMING_UNIT_PERIOD 0x2D
#define	TSC_GPIO23_DIMMING_UNIT_PERIOD 0x2E
#define	TSC_PA01_DIMMING_UNIT_PERIOD 0x2F
#define	TSC_PA23_DIMMING_UNIT_PERIOD 0x30
#define	TSC_GPIO0_DIMMING_CONTROL    0x31
#define	TSC_GPIO1_DIMMING_CONTROL    0x32
#define	TSC_GPIO2_DIMMING_CONTROL    0x33
#define	TSC_GPIO3_DIMMING_CONTROL    0x34
#define	TSC_PA0_DIMMING_CONTROL      0x35
#define	TSC_PA1_DIMMING_CONTROL      0x36
#define	TSC_PA2_DIMMING_CONTROL      0x37
#define	TSC_PA3_DIMMING_CONTROL      0x38
#define	TSC_GPIO03_DIMMING_MODE      0x39
#define	TSC_PA03_DIMMING_MODE        0x3A
#define	TSC_DIMMING_START            0x3B
#define	TSC_DIMMING_ENABLE           0x3C

#define	TSC_STRENGTH_00              0x50        // Read-Only Registers
#define	TSC_STRENGTH_01              0x51
#define	TSC_STRENGTH_02              0x52
#define	TSC_STRENGTH_03              0x53
#define	TSC_STRENGTH_04              0x54
#define	TSC_STRENGTH_05              0x55
#define	TSC_STRENGTH_06              0x56
#define	TSC_STRENGTH_07              0x57
#define	TSC_IMP_00                   0x58
#define	TSC_IMP_01                   0x59
#define	TSC_IMP_02                   0x5A
#define	TSC_IMP_03                   0x5B
#define	TSC_IMP_04                   0x5C
#define	TSC_IMP_05                   0x5D
#define	TSC_IMP_06                   0x5E
#define	TSC_IMP_07                   0x5F
#define	TSC_REF_IMP_00               0x60
#define	TSC_REF_IMP_01               0x61
#define	TSC_REF_IMP_02               0x62
#define	TSC_REF_IMP_03               0x63
#define	TSC_REF_IMP_04               0x64
#define	TSC_REF_IMP_05               0x65
#define	TSC_REF_IMP_06               0x66
#define	TSC_REF_IMP_07               0x67
#define	TSC_TOUCH_BYTE               0x68
#define	TSC_GINT_IRQ_PENDING         0x69
#define	TSC_PA_INPUT_DATA            0x6A
#define	TSC_GPIO_INPUT_DATA          0x6B
#define	TSC_PA_INPUT_BCU_DATA        0x6C
#define	TSC_GPIO_INPUT_BCU_DATA      0x6D
#define	TSC_PA_INPUT_FIFO_DATA       0x6E
#define	TSC_GPIO_INPUT_FIFO_DATA     0x6F

#define	TSC_CLK_EXT                  0xF7         // Write-Only Registers
#define	TSC_CLK_INT                  0xF8
#define	TSC_BIAS_OFF                 0xF9
#define	TSC_BIAS_ON                  0xFA
#define	TSC_LTB_ENABLE               0xFB
#define	TSC_WAKEUP_SLEEP             0xFC
#define	TSC_ENTER_SLEEP              0xFD
#define	TSC_COLD_RESET               0xFE
#define	TSC_WARM_RESET               0xFF

//extern const unsigned char tsc_init_data[];
//extern const char TSC_REG_NAMES_RW[61][20];
// extern const char TSC_REG_NAMES_RW[61][1];     // use this in case the plain-text name strings are not needed

