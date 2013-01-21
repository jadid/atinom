
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "mb.h"
#include "mbport.h"

//*


#define TXDE	BIT(24)
#define RXDE	BIT(23)

#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4
#define REG_HOLDING_START 2000
#define REG_HOLDING_NREGS 130

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];


xTaskHandle hdl_modbus;

static portTASK_FUNCTION(task_modbus, pvParameters )
{
	//init_serial3();
	vTaskDelay(5000);
	vTaskDelay(3000);
	
	#ifdef DEBUG_MODBUS_SLAVE
	printf("init %s\r\n", __FUNCTION__);
	#endif
	
	eMBInit( MB_RTU, 0x01, 19200, 1, MB_PAR_NONE );
    
    //eMBEnable();
    vTaskDelay(500);
	
	for (;;)	{
	//	eMBPoll();
		vTaskDelay(1);

	}
}
void init_task_modbus(void) {
	xTaskCreate(task_modbus, ( signed portCHAR * ) "modbus",  (configMINIMAL_STACK_SIZE * 15) ,\
		 NULL, tskIDLE_PRIORITY + 2, ( xTaskHandle * ) &hdl_modbus );
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode ) {
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNRegs;
    ( void )eMode;
    return MB_ENOREG;
}

eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )	{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNCoils;
    ( void )eMode;
    return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )	{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}
