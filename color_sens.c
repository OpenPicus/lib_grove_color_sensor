/** \file color_sensor.c
 *  \brief Grove devices support library 
 */

/**
\addtogroup Grove devices
@{
*/
/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        serial_lcd.c
 *  Dependencies:    OpenPicus libraries
 *  Module:          FlyPort WI-FI - FlyPort ETH
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Davide Vicca	     1.0     01/11/2013		   First release  
 *  
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by 
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *  
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to 
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code. 
 *  OpenPicus software is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. 
 * 
 * 
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/


#include "taskFlyport.h"
#include "grovelib.h"
#include "color_sens.h"


struct Interface *attachSensorToI2CBus(void *,int,int);
int set_register(BYTE,BYTE,BYTE);
BYTE read_register(BYTE,BYTE);


/**
 * struct ColorSens - The structure for ColorSens grove sensor 
 */
struct ColorSens
{
	const void *class;
	struct Interface *inter;
	BYTE devaddress; 
	BYTE gain; 
	BYTE integr;
};



/**
 * static void init (struct ColorSens *self)
 * \param *self - pointer to the Color Sensor Grove device class.
 * \return - None
*/
static int init(struct ColorSens* self)
{
	if(set_register(self->devaddress,REG_CTL,0x01))//switch the color sensor on
		return -1;		
	if(set_register(self->devaddress,REG_TIMING,0x10))
		return -1;		
	return set_register(self->devaddress,REG_GAIN,self->gain);
	
}	

/**
 * static int ColorSens_read(struct ColorSens* self)
 * \param *self - pointer to the Color Sensor Grove device class.
 * \return - status of the operation
*/
static int ColorSens_read(struct ColorSens* self)
{
	if(set_register(self->devaddress,REG_CTL,START))
		return -1;		
	Delay10us(self->integr*100);
	if(set_register(self->devaddress,REG_CTL,STOP))
		return -1;		
	BYTE data = read_register(self->devaddress,REG_CTL);
	Delay10us(10);
	if(!(data & 0x10))//ADC_VAL
		return -1;
	else
		return 0;
}	

/**
 * static void *ColorSens_ctor (void * _self, va_list *app) -ColorSens grove device Constructor  
 * \param *_self - pointer to the ColorSens grove device class.
 * \param *app
 <UL>
	<LI><B param 1 -I2C device address</LI> 
	<LI><B param 2 -Device Gain setting</LI> 
	<LI><B param 1 -Output Prescaler</LI> 
 </UL>
* \return - Pointer to the ColorSens devices instantiated
 <UL>
	<LI><Breturn = Pointer to the I2C interface created:</B> the operation was successful.</LI> 
	<LI><B>return = NULL:</B> the operation was unsuccessful.</LI> 
 </UL>

*/
static void *ColorSens_ctor (void * _self, va_list *app)
{
	struct ColorSens *self = _self;
	self->devaddress =  va_arg(*app, const BYTE);
	BYTE gain = va_arg(*app, const BYTE);
	BYTE prescaler = va_arg(*app, const BYTE);
	self->gain = gain | prescaler;
	self->inter = NULL;
	return self;
}

/**
 * static void ColorSens_dtor (void * _sensor)- ColorSens grove device Destructor  
 * \param *_sensor - pointer to the ColorSens grove device class.
 * \return - None
*/
static void ColorSens_dtor (void * _sensor)
{

}


/**
 * static void* ColorSens_attach (void * _board,void *_sensor,int n) - attach a ColorSens grove device to the GroveNest I2C port  
 * \param *_board - pointer to the GroveNest 
 * \param *_sensor - pointer to the ColorSens grove device class.
 * \param n - port number which ColorSens device is connected to
 * \return 
 <UL>
	<LI><Breturn = Pointer to the I2C interface created:</B> the operation was successful.</LI> 
	<LI><B>return = NULL:</B> the operation was unsuccessful.</LI> 
 </UL>
 */
static void *ColorSens_attach (void * _board,void *_sensor,int ic2bus)
{
	struct ColorSens *sensor = _sensor;
	int add = sensor->devaddress;
	sensor->inter = attachSensorToI2CBus(_board,ic2bus,add);
	return sensor->inter;
}


/**
 *  static int ColorSens_configure (void * _self, va_list *app) -  Configure the ColorSens grove device
 * \param *_self - pointer to the device 
 * \param *app - none 
 * \return:
 	<LI><Breturn = 0:</B>when the Serial LCD device is properly configured </LI> 
 	<LI><Breturn = -1:</B>when the operation was unsucceful (the Serial LCD device will not work) </LI> 
 </UL>
 */
static int ColorSens_config (void * _self, va_list *app)
{
	struct ColorSens *self = _self;
	return init(self);
}


/**
 * static float ColorSens_set(void * _self,va_list *app) -  write into the ColorSens device.
 * \param *_self - pointer to the device 
 * \param *app - Output Color type (BLUE,RED,GREEN and CLEAR)
 *\return the status of the operation:
  	<LI><Breturn = 0:</B>when the command was properly interpreted </LI> 
 	<LI><Breturn = -1:</B>when the command sent was unknown </LI> 
*/
static float ColorSens_get (void *_self,va_list *app)
{
	struct ColorSens *self = _self;
	self->integr = va_arg(*app, const BYTE);
	BYTE param = va_arg(*app, const BYTE);
	if(!ColorSens_read(self))
	{
		int datal,datah;
		switch(param)
		{
			case 0://blu registers
				datal = read_register(self->devaddress,REG_BLUE_LOW);
				datah = read_register(self->devaddress,REG_BLUE_HIGH);
			break;
			case 1://red registers
				datal = read_register(self->devaddress,REG_RED_LOW);
				datah = read_register(self->devaddress,REG_RED_HIGH);
			break;
			case 2://green registers
				datal = read_register(self->devaddress,REG_GREEN_LOW);
				datah = read_register(self->devaddress,REG_GREEN_HIGH);
			break;
			case 3://clear light registers
				datal = read_register(self->devaddress,REG_CLEAR_LOW);
				datah = read_register(self->devaddress,REG_CLEAR_HIGH);
			break;
		}
	datah = datah <<8 |datal;	
	return (float)datah; 

	}
	flag = 1;//error
	set_register(self->devaddress,REG_CTL,0x00);
	return -1;
}



static const struct SensorClass _ColorSens =
{	
	sizeof(struct ColorSens),
	ColorSens_ctor,
	ColorSens_dtor,
	ColorSens_attach,
	ColorSens_config,
	0,
	ColorSens_get
};

const void *ColorSens = &_ColorSens;

