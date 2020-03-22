/*
 * aID.h
 *
 *  Created on: 2016��1��4��
 *      Author: Administrator
 */

#ifndef AID_H_
#define AID_H_

#include "osCore.h"

/*******PDO Attribute ID value define as follow: ***************/
#define aID_PDO_Type_Server							0x000000 //
#define aID_PDO_Type_Device_Info					0x000001 //
#define	aID_PDO_Type_Port_List						0x000002
#define aID_PDO_Type_Port_Describe					0x000003
#define aID_PDO_Type_Scene							0x000004
#define	aID_PDO_Type_File							0x000005
#define	aID_PDO_Type_White_Table					0x000006
#define	aID_PDO_Type_Alarm_Record					0x000007
#define	aID_PDO_Type_History_Record					0x000008
#define	aID_PDO_Type_Beacon							0x00000F
#define aID_PDO_Type_Device_Indication				0x000010
#define aID_PDO_Type_Time							0x000011
#define aID_PDO_Type_Upgrade						0x000012
#define aID_PDO_TypeManufactrueID					0x0000FF

#define aID_PDO_Type_SubDevice						0x000100
#define aID_Gen_Type_LQI							0x000102


/*********PDO All Attribute option as follow *****************/
#define PDO_COMMON_OPTION							0x00
/*******************************************/

/************��Ҫ�豸����*****************************/
#define Application_ID_Gateway						0x000001
#define Application_ID_Device						0x000002
#define Application_ID_Route						0x000010

#define Application_ID_Key							0x001000

#define Application_ID_Switch						0x001010

#define Application_ID_Light						0x001020
#define Application_ID_DimmerLight					0x001021
#define Application_ID_ColorLight					0x001023

#define Application_ID_86Socket						0x001030
#define Application_ID_LineSocket					0x001031

#define Application_ID_WidowCovering				0x001041
#define Application_ID_Blind_WidowCovering			0x001042
#define Application_ID_PercentWidowCovering			0x001045

#define Application_ID_Locker						0x001080

#define Application_ID_IRBox						0x001100
////////////////////////////////////////////////////////
#define Application_ID_Fan							0x002100
#define Application_ID_WatterRefrigeration			0x002101

#define Application_ID_TempController				0x002200
#define Application_ID_AirConditioning				0x002201
#define Application_ID_FanMachineController			0x002202
#define Application_ID_DihumidificationController	0x002203
#define Application_ID_MainPower_Outlet				0x003000
#define Application_ID_MainPower_RCCB				0x003001
#define Application_ID_MainPower_AirCircuitBreaker	0x003002

///////////////////////////////////////////////////////////
#define Application_ID_FireAlarm					0x004000
#define Application_ID_SmokeAlarm					0x004001
#define Application_ID_PM25Alarm					0x004002

#define Application_ID_FloodAlarm					0x004010
#define Application_ID_WaterAlarm					0x004011

#define Application_ID_PoisonGasAlarm				0x004020
#define Application_ID_CarbonMonoxideAlarm			0x004021

#define Application_ID_CombustibelAlarm				0x004030

#define Application_ID_DoorMagneticAlarm			0x004100
#define Application_ID_BodyInfraredAlarm			0x004110
#define Application_ID_EmergencyButtonAlarm			0x004120

#define Application_ID_PowerAlarm					0x004200
#define Application_ID_AlarmDevice					0x004FFE
#define Application_ID_IAS_ZONE						0x004FFF
///////////////////////////////////////////////////////////
#define Application_ID_HumidityAndTemSensor			0x005000
#define Application_ID_TemperatureSensor			0x005001
#define Application_ID_HumiditySensor				0x005002

#define Application_ID_CarbonMonoxideSensor			0x005010
#define Application_ID_CarbonDioxideSensor			0x005011
#define Application_ID_OxygenSensor					0x005012
#define Application_ID_NitrogenSensor				0x005013
#define Application_ID_OxygenAndNitrogenSensor		0x005014
#define Application_ID_OzoneSensor					0x005015

#define Application_ID_NaturalGasSensor				0x005020
#define Application_ID_MethaneSensor				0x005021

#define Application_ID_FormaldehydeSensor			0x005030

#define Application_ID_LocationSensor				0x005100
#define Application_ID_DistanceSensor				0x005101
#define Application_ID_HeightSensor					0x005102
#define Application_ID_AltitudeSensor				0x005104
#define Application_ID_PressureSensor				0x005105

#define Application_ID_AirPressureSensor			0x005110
#define Application_ID_WatterPressureSensor			0x005111
#define Application_ID_SimpleSensor					0x005112

#define Application_ID_GravitySensor				0x005120

#define Application_ID_IlluminanceSensor			0x005130

#define Application_ID_SpeedSensor					0x005140
#define Application_ID_AccelerationSensor			0x005141

#define Application_ID_WindSpeedSensor				0x005150

#define Application_ID_FlowSensor					0x005160

#define Application_ID_ElectricityMeter				0x005170
#define Application_ID_WattHourSensor				0x005171
#define Application_ID_VoltageSensor				0x005172
#define Application_ID_AmmeterSensor				0x005173



///////////////////////////////////////////////////////////
#define Application_ID_RFASKRemoteCtrl			0x007000
#define Application_ID_UART						0x007010

#define Application_ID_Unkown					0xFFFFFFFF
/*************ͨ������*********************************/
#define aID_Gen_Option							0x00
////////////////////////////////////////////////////////////
#define aID_Gen_Type_KeyValue					0x009000

#define aID_Gen_Type_Switch						0x009020

#define aID_Gen_Type_Level						0x009030
#define aID_Gen_Type_Red						0x009031
#define aID_Gen_Type_Green						0x009032
#define aID_Gen_Type_Blue						0x009033
#define aID_Gen_Type_Yellow						0x009034
#define aID_Gen_Type_White						0x00903F
#define aID_Gen_Type_Color						0x009040

#define aID_Gen_Type_WindowSwitch				0x009050
#define aID_Gen_Type_WindowPercent				0x009051
#define aID_Gen_Type_BlindsWindow				0x009052

#define aID_Gen_Type_IRCode						0x009055
#define aID_Gen_Type_HXDIRCode					0x009056
#define aID_Gen_Type_RFCode						0x00905A

#define aID_Gen_Type_FanSwitch					0x009060
#define aID_Gen_Type_FanSpeedPercent			0x009061
#define aID_Gen_Type_FanHorizontalForward	 	0x009062
#define aID_Gen_Type_FanVerticalForward	 		0x009063
#define aID_Gen_Type_FanWaterSwitch				0x009068

#define aID_Gen_Type_Locker						0x009080
#define aID_Gen_Type_Locker_RFID				0x009081
#define aID_Gen_Type_Locker_PSW					0x009082
#define aID_Gen_Type_Locker_FINGER				0x009083


#define aID_Gen_Type_FireAlarm					0x00C000
#define aID_Gen_Type_SmokeAlarm					0x00C001
#define aID_Gen_Type_PM25Alarm					0x00C002

#define aID_Gen_Type_FloodAlarm					0x00C010
#define aID_Gen_Type_WaterAlarm					0x00C011

#define aID_Gen_Type_PoisonGasAlarm				0x00C020
#define aID_Gen_Type_CarbonMonoxideAlarm		0x00C021

#define aID_Gen_Type_CombustibleAlarm			0x00C030

#define aID_Gen_Type_MenciAlarm					0x00C100
#define aID_Gen_Type_HumenIRAlarm				0x00C110
#define aID_Gen_Type_EmergencyButton			0x00C120
#define aID_Gen_Type_LowPowerAlarm				0x00C130

#define aID_Gen_Type_DisassembleAlarm			0x00C400
#define aID_Gen_Type_Alarm_Blink				0x00CFFD
#define aID_Gen_Type_Alarm_Voice				0x00CFFE
#define aID_Gen_Type_IAS_ZON_Alarm				0x00CFFF


#define aID_Gen_Type_TemperatureMeasurement			0x00D000
#define aID_Gen_Type_HumidityMeasurement			0x00D001
#define aID_Gen_Type_Illuminance_Measurement		0x00D003

#define aID_Gen_Type_PressureMeasurement				0x00D105
#define aID_Gen_Type_WeightMeasurement					0x00D120

#define aID_Gen_Type_TensionMeasurement					0x00D130

#define aID_Gen_Type_FlowMeasurement					0x00D160
#define aID_Gen_Type_EnergyMeasurement					0x00D170

#define aID_Gen_Type_PowerMeasurement					0x00D174
#define aID_Gen_Type_VoltageMeasurement					0x00D178
#define aID_Gen_Type_CurrentMeasurement					0x00D17C
#define aID_Gen_Type_Freq						0x00D180

#define aID_Gen_Type_Unkown						0x00FFFF

uint8 pluto_get_data_type(uint32 aID);

#endif /* AID_H_ */
