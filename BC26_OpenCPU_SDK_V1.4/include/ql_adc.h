/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2019
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   ql_adc.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   The module defines the information, and APIs related to the ADC function.We support two ways to get ADC data:
 *    1. Message loop triggers callback.
 *    2. single read .
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
#ifndef __QL_ADC_H__
#define __QL_ADC_H__

typedef enum
{
    PIN_ADC0,
    PIN_ADC_MAX = 2
}Enum_ADCPin;


#if 1  //Here is the loop interface
/*****************************************************************
* Function:     Ql_ADC_Register 
* 
* Description:
*               This function registers an ADC with the ADC pin name,
*               callback function and the customized parameter.
*
* Parameters:
*               adcPin:
*                   ADC pin name, one value of Enum_ADCPin.
*
*               callback_adc:
*                   callback function, which will be invoked when the ADC result comes out.
*
*               customParam:
*                   A customized parameter, which can be used in the ADC interrupt handler. 
*                   It may be NULL if no customized parameter needs to be passed in.
*
*               adcValue:
*                   the average voltage value for the specified sampling times. 
*                   The range is 0~1400mV. Please also see "Ql_ADC_Init".
* Return:        
*               QL_RET_OK, this function succeeds.
*               QL_RET_ERR_NOSUPPORTPIN, the input IO is invalid. 
*               other place. For example this IO has been using as GPIO.
*               QL_RET_ERR_ADC_ALREADY_REGISTERED, this ADC pin is registered already
*****************************************************************/
typedef void (*Callback_ADC)(Enum_ADCPin adcPin, u32 adcValue, void *customParam);
s32 Ql_ADC_Register(Enum_ADCPin adcPin, Callback_ADC callback_adc, void *customParam);

/*****************************************************************
* Function:     Ql_ADC_Init 
* 
* Description:
*               This function configures the sampling parameters, 
*               including sampling count and the interval.
*
*               Note:
*               The ADC sampling result will be reported in the period of 
*               (count * interval)(ms).
*               For example, if Ql_ADC_Init(PIN_ADC0, 5, 200), then the 
*               Callback_ADC function will be triggered in (5*200)ms=1s periodically.
* Parameters:
*               adcPin:
*                   ADC pin name, one value of Enum_ADCPin
*
*               count:
*                   Sampling times for each ADC value, the minimum is 1.
*
*               interval:
*                   Interval of each internal sampling, unit is ms. 
*                   the minimum is 10(ms).
*
*               For example, if Ql_ADC_Init(PIN_ADC0, 5, 200), then
*               |--200ms--->|--200ms--->|--200ms--->|--200ms--->|--200ms--->|...
*               Start---->sample1---->sample2---->sample3---->sample4---->sample5. Then report the average value by callback.
* Return:        
*               QL_RET_OK, this function succeeds.
*               QL_RET_ERR_PARAM, parameter is error.
*               QL_RET_ERR_NOSUPPORTPIN, the input pin is invalid. 
*               QL_RET_ERR_ADC_NOT_REGISTERED, the ADC not registered.
*
*****************************************************************/
s32 Ql_ADC_Init(Enum_ADCPin adcPin, u32 count, u32 interval);

/*****************************************************************
* Function:     Ql_ADC_Sampling 
* 
* Description:
*               this function switches on/off ADC sample.
*
* Parameters:
*               enable:
*                   sample control, TRUE : start to sample.
*                                   FALSE: stop sampling.
*                 
* Return:        
*               QL_RET_OK, this function succeeds.
*               QL_RET_ERR_NOSUPPORTPIN, the input pin is invalid.
*               QL_RET_ERR_ADC_NOT_REGISTERED, the ADC not register
*               QL_RET_ERR_ADC_SAMPLING_ALREADY,the ADC sampling has been started already
*****************************************************************/
s32 Ql_ADC_Sampling(Enum_ADCPin adcPin, bool enable);
#endif //Here is the loop interface


#if 1//Single read ADC value interface

typedef enum {
    QL_ADC_STATUS_INVALID_PARAMETER = -4,      /**< Invalid parameter */
    QL_ADC_STATUS_ERROR_BUSY = -3,             /**< ADC is busy */
    QL_ADC_STATUS_ERROR_CHANNEL = -2,          /**< ADC channel number error */
    QL_ADC_STATUS_ERROR = -1,                  /**< ADC error */
    QL_ADC_STATUS_OK = 0                       /**< ADC ok */
}ql_adc_status_t;


/*****************************************************************
* Function:   Ql_ADC_Read 
* 
* Description:
*               this function single read ADC values.
*
* Parameters:
*               adcPin:
*                    only supports PIN_ADC0.
*             adc_vol:
*                     ADC values ,The range is 0~1400mV
*
*                 
* Return:        
*             refer to the enum of ql_adc_status_t.
*****************************************************************/
s32 Ql_ADC_Read(Enum_ADCPin adcPin, u32 *adc_vol);

#endif //Single read ADC value interface


#endif  //__QL_ADC_H__
