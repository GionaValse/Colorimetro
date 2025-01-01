#include "clm.h"
#include "i2c.h"
#include "timer.h"

int iTime = 0;

/***	CLM_Init
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function initializes the colorimeter module by configuring the I2C interface and setting the integration time.
**      
**          
*/
void CLM_Init() {
    I2C_Init(400000); // 400kHz
    
    // Max RGBC Count = (256 - ATIME) × 1024 up to a maximum of 65535.
    CLM_Config(100); // Integration time: 100ms -> ATIME = 214 (0xD6)
}

/***	CLM_Config
**
**	Parameters:
**      int itime - Integration time in milliseconds.
**
**	Return Value:
**		
**
**	Description:
**		This function configures the colorimeter module with the specified integration time.
**      It sets up the ENABLE, ATIME, and CONTROL registers.
**      
**          
*/
void CLM_Config(int itime) {
    iTime = itime;
    
    // Setup ENABLE register
    I2C_MasterStart();
    I2C_MasterSend((clm_I2C_ADDR << 1) | i2c_MASTER_WRITE);
    I2C_MasterSend(0x80 | 0x00); // Command bit + ENABLE register
    I2C_MasterSend(0x03); // PON & AEN enabled
    I2C_MasterStop();
    TIMER2_DelayMS(10);
    
    // Setup ATIME register
    I2C_MasterStart();
    I2C_MasterSend((clm_I2C_ADDR << 1) | i2c_MASTER_WRITE);
    I2C_MasterSend(0x80 | 0x01); // Command bit + ATIME register
    I2C_MasterSend(256 - (itime / 2.4)); // ATIME = 256 - Integration Time / 2.4 ms
    I2C_MasterStop();
    TIMER2_DelayMS(10);
    
    // Setup CONTROL register (gain)
    I2C_MasterStart();
    I2C_MasterSend((clm_I2C_ADDR << 1) | i2c_MASTER_WRITE);
    I2C_MasterSend(0x80 | 0x0F); // Command bit + CONTROL register
    I2C_MasterSend(0x01); // Gain = 4x
    I2C_MasterStop();   
}

/***	CLM_GetID
**
**	Parameters:
**
**	Return Value:
**      unsigned char - The ID of the colorimeter.
**
**	Description:
**		This function reads and returns the ID of the colorimeter module.
**      
**          
*/
unsigned char CLM_GetID() {
    I2C_MasterStart();
    I2C_MasterSend((clm_I2C_ADDR << 1) | i2c_MASTER_WRITE); // Colorimeter address
    I2C_MasterSend(0x80 | clm_ID_ADDR);
    I2C_MasterRestart();
    I2C_MasterSend((clm_I2C_ADDR << 1) | i2c_MASTER_READ);
    unsigned char id = I2C_MasterReceive();
    I2C_MasterACK(1);
    I2C_MasterStop();
    
    return id;
}

/***	CLM_I2CGetColorData
**
**	Parameters:
**      unsigned char *colors - Pointer to an array to store the color data.
**
**	Return Value:
**		
**
**	Description:
**		This function reads the raw color data from the colorimeter module via I2C.
**      It stores the data in the provided array.
**      
**          
*/
void CLM_I2CGetColorData(unsigned char *colors) {
    I2C_MasterStart();
    I2C_MasterSend((clm_I2C_ADDR << 1) | i2c_MASTER_WRITE); // Colorimeter address
    I2C_MasterSend(0x80 | clm_CDATAL);
    I2C_MasterRestart();
    I2C_MasterSend((clm_I2C_ADDR << 1) | i2c_MASTER_READ);
    
    // Read color low and high data
    for (int i = 0; i < 8; i++) {
        colors[i] = I2C_MasterReceive();
        I2C_MasterACK(i >= 7 ? 1 : 0);
    }
    I2C_MasterStop();
}

/***	CLM_GetColorData
**
**	Parameters:
**      unsigned int *colors - Pointer to an array to store the normalized RGB color data.
**
**	Return Value:
**		
**
**	Description:
**		This function reads the raw color data from the colorimeter module and normalizes the RGB values.
**      It stores the normalized data in the provided array.
**      
**          
*/
void CLM_GetColorData(unsigned int *colors) {
    unsigned char values[8]; // 2c, 2r, 2g, 2b
    CLM_I2CGetColorData(values);
    
    int clear = (values[1] << 8) | values[0];
    
    // Normalize RGB
    if (clear == 0) {
        for (int i = 0; i < 3; i++)
            colors[i] = 0;
    } else {
        colors[0] = ((float) ((values[3] << 8) | values[2]) / clear) * 255;
        colors[1] = ((float) ((values[5] << 8) | values[4]) / clear) * 255;
        colors[2] = ((float) ((values[7] << 8) | values[6]) / clear) * 255;
    }
}
