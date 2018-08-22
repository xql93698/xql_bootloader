/**************************************************************************//**
 * @file     core_cm3.c
 * @brief    CMSIS Cortex-M3 Core Peripheral Access Layer Source File
 * @version  V1.30
 * @date     30. October 2009
 *����cortex_m3�ں˼����豸nvic��systick��
 *����cm3 cpu�Ĵ������ں�����ĺ�����
 *CMSIS�������㣺����������ʲ�Core Peripheral Access Layer��CPAL�����м�����ʲ�Middleware Access Layer��MWAL�����豸���ʲ㣨Device Peripheral Access Layer����
 *CPAL���ڷ����ں˵ļĴ������������NVIC������ϵͳ�ȡ��ò�����ARMʵ�ֵġ�
 *MWAL���ڶ��м���ķ��ʣ����ڸò㻹δʵ�֡���Ҳ��֪����ν���м����ʲô��������
 *DPAL���ڶ���һЩӲ���Ĵ����ĵ�ַ��һЩ������ʺ�������оƬ������ʵ�֡� 
 *CPAL���ʵ�־���Core_cm3.c�ļ���DPAL���ʵ�־���system_stm32f10x.c�ļ����ƺ���Ӧ�ü�������ĺ����⣩��
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

#include <stdint.h>

/*��Щ��ͬ�Ķ����Ǳ������Լ����Ķ���
�Ǳ�����Ĭ�϶���ģ����ڱ�����ʶ�𣬴�������������Եġ���������ı������ֲᡣ
����__DATE__, __LINE__������*/
/* define compiler specific symbols�����ǻ��ؼ���__ASM��__INLINE�ĺ궨�壬֧�ֲ�ͬ�ı����� */
#if defined ( __CC_ARM   )					      /*ʹ��ARM RealView����ʱ������궨��*/	
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */

#elif defined ( __ICCARM__ )						/*��iar����ʱʹ���������*/
  #define __ASM           __asm                                       /*!< asm keyword for IAR Compiler          */
  #define __INLINE        inline                                      /*!< inline keyword for IAR Compiler. Only avaiable in High optimization mode! */

#elif defined   (  __GNUC__  )						/*��gcc����ʱʹ���������*/
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */

#elif defined   (  __TASKING__  )					/*TASKING��ӦAltium Designer��ߴ��ı��������ͽ�Tasking*/
  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */

#endif


/* ###################  Compiler specific Intrinsics  ########################### */

#if defined ( __CC_ARM   ) /*------------------RealView Compiler  keil��������ʽ����ģ��-----------------*/
/* ARM armcc specific functions ��Keil��ʹ����__asm�ؼ��ֺ󣬱���������Ϊ�������ӷ���ָ�������Ҫ�Լ���д��������*/
/*Ҳ����ÿ�����������  bx lr*/
/**
 * @brief  Return the Process Stack Pointer
 * ��ȡ���̶�ջָ��PSP��
 * @return ProcessStackPointer
 * 
 * Return the actual process stack pointer
 */
__ASM uint32_t __get_PSP(void)
{
  mrs r0, psp
  bx lr
}

/**
 * @brief  Set the Process Stack Pointer
 * ���ý��̶�ջָ��PSP
 * @param  topOfProcStack  Process Stack Pointer
 *
 * Assign the value ProcessStackPointer to the MSP 
 * (process stack pointer) Cortex processor register
 */
__ASM void __set_PSP(uint32_t topOfProcStack)
{
  msr psp, r0
  bx lr
}

/**
 * @brief  Return the Main Stack Pointer
 *
 * @return Main Stack Pointer
 * ��ȡ����ջָ��MSP
 * Return the current value of the MSP (main stack pointer)
 * Cortex processor register
 */
__ASM uint32_t __get_MSP(void)
{
  mrs r0, msp
  bx lr
}

/**
 * @brief  Set the Main Stack Pointer
 *
 * @param  topOfMainStack  Main Stack Pointer
 * ��������ջָ��MSP
 * Assign the value mainStackPointer to the MSP 
 * (main stack pointer) Cortex processor register
 */
__ASM void __set_MSP(uint32_t mainStackPointer)
{
  msr msp, r0
  bx lr
}

/**
 * @brief  Reverse byte order in unsigned short value
 * ��ת�������ֽ�˳����0xABCD��ת��õ�0xCDAB
 * @param   value  value to reverse
 * @return         reversed value
 *
 * Reverse byte order in unsigned short value
 */
__ASM uint32_t __REV16(uint16_t value)
{
  rev16 r0, r0
  bx lr
}

/**
 * @brief  Reverse byte order in signed short value with sign extension to integer
 *
 * @param   value  value to reverse
 * @return         reversed value
 * ��ת�ֽ�˳�򣬲���������չ��������__REV16�����õ��Ľ�����ٽ���һ�η�����չ��
 * ������������Ҫ�Ƿ�����д�С�˵��л�
 * Reverse byte order in signed short value with sign extension to integer
 */
__ASM int32_t __REVSH(int16_t value)
{
  revsh r0, r0
  bx lr
}


#if (__ARMCC_VERSION < 400000)		/*���Ǳ������汾������ô������֪��������������*/

/**
 * @brief  Remove the exclusive lock created by ldrex
 * �����LDREXָ����ɵĻ�������LDREX��STREX��Cortex����ʵ�ֻ�����ʣ������ٽ���Դ��ָ�
 * LDREXִ�к�ֻ�����������һ���洢ָ�STR��STREX������ִ�У������Ĵ洢ָ��ᱻ���أ�
 * ��CLREX������������������״̬�ı��
 * Removes the exclusive lock which is created by ldrex.
 */
__ASM void __CLREX(void)
{
  clrex
}

/*���´�����Ҫע���һ�㣬BASEPRI��PRIMASK��FAULTMASK��CONTROL��ֻ������Ȩģʽ�±��޸ġ�*/
/**
 * @brief  Return the Base Priority value
 * ��ȡBASEPRI�Ĵ�����ֵ�����ȼ��Ÿ��ڸüĴ������ж϶��ᱻ���Σ����ȼ���Խ�����ȼ�Խ�ͣ�
 * ��Ϊ��ʱ�������κ��ж�
 * @return BasePriority
 *
 * Return the content of the base priority register
 */
__ASM uint32_t  __get_BASEPRI(void)
{
  mrs r0, basepri
  bx lr
}

/**
 * @brief  Set the Base Priority value
 *����BASEPRI��ֵ�����ȼ��Ÿ��ڸüĴ������ж϶��ᱻ���Σ����ȼ���Խ�����ȼ�Խ�ͣ�
 * ��Ϊ��ʱ�������κ��ж�
 * @param  basePri  BasePriority
 *
 * Set the base priority register
 */
__ASM void __set_BASEPRI(uint32_t basePri)
{
  msr basepri, r0
  bx lr
}

/**
 * @brief  Return the Priority Mask value
 * ��ȡPRIMASK��ֵ
 * PRIMASK��һ��ֻ��һλ�ļĴ�������λʱ���ξ��󲿷ֵ��쳣�жϣ�ֻʣ��NMI��HardFault������Ӧ��
 * @return PriMask
 *
 * Return state of the priority mask bit from the priority mask register
 */
__ASM uint32_t __get_PRIMASK(void)
{
  mrs r0, primask
  bx lr
}

/**
 * @brief  Set the Priority Mask value
 * ����PRIMASK��ֵ
 * PRIMASK��һ��ֻ��һλ�ļĴ�������λʱ���ξ��󲿷ֵ��쳣�жϣ�ֻʣ��NMI��HardFault������Ӧ��
 * @param  priMask  PriMask
 *
 * Set the priority mask bit in the priority mask register
 */
__ASM void __set_PRIMASK(uint32_t priMask)
{
  msr primask, r0
  bx lr
}

/**
 * @brief  Return the Fault Mask value
 * ��ȡFAULTMASK��ֵ
 * FAULTMASKҲ��һ��ֻ��һλ�ļĴ�����Ϊ1ʱֻ��NMI������Ӧ�������쳣���ж�ȫ�������Ρ�
 * @return FaultMask
 *
 * Return the content of the fault mask register
 */
__ASM uint32_t  __get_FAULTMASK(void)
{
  mrs r0, faultmask
  bx lr
}

/**
 * @brief  Set the Fault Mask value
 *
 * ����FAULTMASK��ֵ
 * FAULTMASKҲ��һ��ֻ��һλ�ļĴ�����Ϊ1ʱֻ��NMI������Ӧ�������쳣���ж�ȫ�������Ρ�
 * @param  faultMask  faultMask value
 *
 * Set the fault mask register
 */
__ASM void __set_FAULTMASK(uint32_t faultMask)
{
  msr faultmask, r0
  bx lr
}

/**
 * @brief  Return the Control Register value
 * ��ȡCONTROL��ֵ���Ĵ���CONTROLֻ����λ��CONTROL[0]ѡ����Ȩ����0Ϊ��Ȩ����1Ϊ���û�����
 * CONTROL[1]����ѡ���ջָ�룬0ΪMSP��1ΪPSP��
 * @return Control value
 *
 * Return the content of the control register
 */
__ASM uint32_t __get_CONTROL(void)
{
  mrs r0, control
  bx lr
}

/**
 * @brief  Set the Control Register value
 * ����CONTROL��ֵ���Ĵ���CONTROLֻ����λ��CONTROL[0]ѡ����Ȩ����0Ϊ��Ȩ����1Ϊ���û�����
 * CONTROL[1]����ѡ���ջָ�룬0ΪMSP��1ΪPSP��
 * @param  control  Control value
 *
 * Set the control register
 */
__ASM void __set_CONTROL(uint32_t control)
{
  msr control, r0
  bx lr
}

#endif /* __ARMCC_VERSION  */ 



#elif (defined (__ICCARM__)) /*------------------ ICC Compiler -------------------*/
/* IAR iccarm specific functions */
#pragma diag_suppress=Pe940	/*   ��ֹ����ʱ����Pe940 ��Ϣ*/

/**
 * @brief  Return the Process Stack Pointer
 *
 * @return ProcessStackPointer
 *
 * Return the actual process stack pointer
 */
uint32_t __get_PSP(void)
{
  __ASM("mrs r0, psp");
  __ASM("bx lr");
}

/**
 * @brief  Set the Process Stack Pointer
 *
 * @param  topOfProcStack  Process Stack Pointer
 *
 * Assign the value ProcessStackPointer to the MSP 
 * (process stack pointer) Cortex processor register
 */
void __set_PSP(uint32_t topOfProcStack)
{
  __ASM("msr psp, r0");
  __ASM("bx lr");
}

/**
 * @brief  Return the Main Stack Pointer
 *
 * @return Main Stack Pointer
 *
 * Return the current value of the MSP (main stack pointer)
 * Cortex processor register
 */
uint32_t __get_MSP(void)
{
  __ASM("mrs r0, msp");
  __ASM("bx lr");
}

/**
 * @brief  Set the Main Stack Pointer
 *
 * @param  topOfMainStack  Main Stack Pointer
 *
 * Assign the value mainStackPointer to the MSP 
 * (main stack pointer) Cortex processor register
 */
void __set_MSP(uint32_t topOfMainStack)
{
  __ASM("msr msp, r0");
  __ASM("bx lr");
}

/**
 * @brief  Reverse byte order in unsigned short value
 * 
 * @param  value  value to reverse
 * @return        reversed value
 *
 * Reverse byte order in unsigned short value
 */
uint32_t __REV16(uint16_t value)
{
  __ASM("rev16 r0, r0");
  __ASM("bx lr");
}

/**
 * @brief  Reverse bit order of value
 *	λ�ķ�ת����һ��32λ����̶����ƣ��ڷ�ת180��
 * @param  value  value to reverse
 * @return        reversed value
 *
 * Reverse bit order of value
 */
uint32_t __RBIT(uint32_t value)
{
  __ASM("rbit r0, r0");
  __ASM("bx lr");
}

/**
 * @brief  LDR Exclusive (8 bit)
 *
 * @param  *addr  address pointer
 * @return        value of (*address)
 * �����ֽڵ��Ĵ����������ں��б�����ָ���ĵ�ַ���뻥�����״̬
 * Exclusive LDR command for 8 bit values)
 */
uint8_t __LDREXB(uint8_t *addr)
{
  __ASM("ldrexb r0, [r0]");
  __ASM("bx lr"); 
}

/**
 * @brief  LDR Exclusive (16 bit)
 *
 * @param  *addr  address pointer
 * @return        value of (*address)
 * ���ذ��ֵ��Ĵ����������ں��б�����ָ���ĵ�ַ���뻥�����״̬
 * Exclusive LDR command for 16 bit values
 */
uint16_t __LDREXH(uint16_t *addr)
{
  __ASM("ldrexh r0, [r0]");
  __ASM("bx lr");
}

/**
 * @brief  LDR Exclusive (32 bit)
 *
 * @param  *addr  address pointer
 * @return        value of (*address)
 * �����ֵ��Ĵ����������ں��б�����ָ���ĵ�ַ���뻥�����״̬
 * Exclusive LDR command for 32 bit values
 */
uint32_t __LDREXW(uint32_t *addr)
{
  __ASM("ldrex r0, [r0]");
  __ASM("bx lr");
}

/**
 * @brief  STR Exclusive (8 bit)
 *
 * @param  value  value to store
 * @param  *addr  address pointer
 * @return        successful / failed
 * ���Ҫ���ʵĵ�ַ�Ƿ���뻥�����״̬������ǣ���洢�Ĵ������ֽ�
 * Exclusive STR command for 8 bit values
 */
uint32_t __STREXB(uint8_t value, uint8_t *addr)
{
  __ASM("strexb r0, r0, [r1]");
  __ASM("bx lr");
}

/**
 * @brief  STR Exclusive (16 bit)
 *
 * @param  value  value to store
 * @param  *addr  address pointer
 * @return        successful / failed
 * ���Ҫ���ʵĵ�ַ�Ƿ���뻥�����״̬������ǣ���洢�Ĵ����İ���
 * Exclusive STR command for 16 bit values
 */
uint32_t __STREXH(uint16_t value, uint16_t *addr)
{
  __ASM("strexh r0, r0, [r1]");
  __ASM("bx lr");
}

/**
 * @brief  STR Exclusive (32 bit)
 *
 * @param  value  value to store
 * @param  *addr  address pointer
 * @return        successful / failed
 * ���Ҫ���ʵĵ�ַ�Ƿ���뻥�����״̬������ǣ���洢�Ĵ�������
 * Exclusive STR command for 32 bit values
 */
uint32_t __STREXW(uint32_t value, uint32_t *addr)
{
  __ASM("strex r0, r0, [r1]");
  __ASM("bx lr");
}

#pragma diag_default=Pe940		/*�ָ�����ʱ���� Pe826,Pe940 ��ϢΪĬ��״̬*/


#elif (defined (__GNUC__)) /*------------------ GNU Compiler ---------------------*/
/* GNU gcc specific functions */

/**
 * @brief  Return the Process Stack Pointer
 *
 * @return ProcessStackPointer
 *
 * Return the actual process stack pointer
 */
uint32_t __get_PSP(void) __attribute__( ( naked ) );
uint32_t __get_PSP(void)
{
  uint32_t result=0;

  __ASM volatile ("MRS %0, psp\n\t" 
                  "MOV r0, %0 \n\t"
                  "BX  lr     \n\t"  : "=r" (result) );
  return(result);
}

/**
 * @brief  Set the Process Stack Pointer
 *
 * @param  topOfProcStack  Process Stack Pointer
 *
 * Assign the value ProcessStackPointer to the MSP 
 * (process stack pointer) Cortex processor register
 */
void __set_PSP(uint32_t topOfProcStack) __attribute__( ( naked ) );
void __set_PSP(uint32_t topOfProcStack)
{
  __ASM volatile ("MSR psp, %0\n\t"
                  "BX  lr     \n\t" : : "r" (topOfProcStack) );
}

/**
 * @brief  Return the Main Stack Pointer
 *
 * @return Main Stack Pointer
 *
 * Return the current value of the MSP (main stack pointer)
 * Cortex processor register
 */
uint32_t __get_MSP(void) __attribute__( ( naked ) );
uint32_t __get_MSP(void)
{
  uint32_t result=0;

  __ASM volatile ("MRS %0, msp\n\t" 
                  "MOV r0, %0 \n\t"
                  "BX  lr     \n\t"  : "=r" (result) );
  return(result);
}

/**
 * @brief  Set the Main Stack Pointer
 *
 * @param  topOfMainStack  Main Stack Pointer
 *
 * Assign the value mainStackPointer to the MSP 
 * (main stack pointer) Cortex processor register
 */
void __set_MSP(uint32_t topOfMainStack) __attribute__( ( naked ) );
void __set_MSP(uint32_t topOfMainStack)
{
  __ASM volatile ("MSR msp, %0\n\t"
                  "BX  lr     \n\t" : : "r" (topOfMainStack) );
}

/**
 * @brief  Return the Base Priority value
 *
 * @return BasePriority
 *
 * Return the content of the base priority register
 */
uint32_t __get_BASEPRI(void)
{
  uint32_t result=0;
  
  __ASM volatile ("MRS %0, basepri_max" : "=r" (result) );
  return(result);
}

/**
 * @brief  Set the Base Priority value
 *
 * @param  basePri  BasePriority
 *
 * Set the base priority register
 */
void __set_BASEPRI(uint32_t value)
{
  __ASM volatile ("MSR basepri, %0" : : "r" (value) );
}

/**
 * @brief  Return the Priority Mask value
 *
 * @return PriMask
 *
 * Return state of the priority mask bit from the priority mask register
 */
uint32_t __get_PRIMASK(void)
{
  uint32_t result=0;

  __ASM volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}

/**
 * @brief  Set the Priority Mask value
 *
 * @param  priMask  PriMask
 *
 * Set the priority mask bit in the priority mask register
 */
void __set_PRIMASK(uint32_t priMask)
{
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) );
}

/**
 * @brief  Return the Fault Mask value
 *
 * @return FaultMask
 *
 * Return the content of the fault mask register
 */
uint32_t __get_FAULTMASK(void)
{
  uint32_t result=0;
  
  __ASM volatile ("MRS %0, faultmask" : "=r" (result) );
  return(result);
}

/**
 * @brief  Set the Fault Mask value
 *
 * @param  faultMask  faultMask value
 *
 * Set the fault mask register
 */
void __set_FAULTMASK(uint32_t faultMask)
{
  __ASM volatile ("MSR faultmask, %0" : : "r" (faultMask) );
}

/**
 * @brief  Return the Control Register value
* 
*  @return Control value
 *
 * Return the content of the control register
 */
uint32_t __get_CONTROL(void)
{
  uint32_t result=0;

  __ASM volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}

/**
 * @brief  Set the Control Register value
 *
 * @param  control  Control value
 *
 * Set the control register
 */
void __set_CONTROL(uint32_t control)
{
  __ASM volatile ("MSR control, %0" : : "r" (control) );
}


/**
 * @brief  Reverse byte order in integer value
 *
 * @param  value  value to reverse
 * @return        reversed value
 *
 * Reverse byte order in integer value
 */
uint32_t __REV(uint32_t value)
{
  uint32_t result=0;
  
  __ASM volatile ("rev %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}

/**
 * @brief  Reverse byte order in unsigned short value
 *
 * @param  value  value to reverse
 * @return        reversed value
 *
 * Reverse byte order in unsigned short value
 */
uint32_t __REV16(uint16_t value)
{
  uint32_t result=0;
  
  __ASM volatile ("rev16 %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}

/**
 * @brief  Reverse byte order in signed short value with sign extension to integer
 *
 * @param  value  value to reverse
 * @return        reversed value
 *
 * Reverse byte order in signed short value with sign extension to integer
 */
int32_t __REVSH(int16_t value)
{
  uint32_t result=0;
  
  __ASM volatile ("revsh %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}

/**
 * @brief  Reverse bit order of value
 *
 * @param  value  value to reverse
 * @return        reversed value
 *
 * Reverse bit order of value
 */
uint32_t __RBIT(uint32_t value)
{
  uint32_t result=0;
  
   __ASM volatile ("rbit %0, %1" : "=r" (result) : "r" (value) );
   return(result);
}

/**
 * @brief  LDR Exclusive (8 bit)
 *
 * @param  *addr  address pointer
 * @return        value of (*address)
 *
 * Exclusive LDR command for 8 bit value
 */
uint8_t __LDREXB(uint8_t *addr)
{
    uint8_t result=0;
  
   __ASM volatile ("ldrexb %0, [%1]" : "=r" (result) : "r" (addr) );
   return(result);
}

/**
 * @brief  LDR Exclusive (16 bit)
 *
 * @param  *addr  address pointer
 * @return        value of (*address)
 *
 * Exclusive LDR command for 16 bit values
 */
uint16_t __LDREXH(uint16_t *addr)
{
    uint16_t result=0;
  
   __ASM volatile ("ldrexh %0, [%1]" : "=r" (result) : "r" (addr) );
   return(result);
}

/**
 * @brief  LDR Exclusive (32 bit)
 *
 * @param  *addr  address pointer
 * @return        value of (*address)
 *
 * Exclusive LDR command for 32 bit values
 */
uint32_t __LDREXW(uint32_t *addr)
{
    uint32_t result=0;
  
   __ASM volatile ("ldrex %0, [%1]" : "=r" (result) : "r" (addr) );
   return(result);
}

/**
 * @brief  STR Exclusive (8 bit)
 *
 * @param  value  value to store
 * @param  *addr  address pointer
 * @return        successful / failed
 *
 * Exclusive STR command for 8 bit values
 */
uint32_t __STREXB(uint8_t value, uint8_t *addr)
{
   uint32_t result=0;
  
   __ASM volatile ("strexb %0, %2, [%1]" : "=r" (result) : "r" (addr), "r" (value) );
   return(result);
}

/**
 * @brief  STR Exclusive (16 bit)
 *
 * @param  value  value to store
 * @param  *addr  address pointer
 * @return        successful / failed
 *
 * Exclusive STR command for 16 bit values
 */
uint32_t __STREXH(uint16_t value, uint16_t *addr)
{
   uint32_t result=0;
  
   __ASM volatile ("strexh %0, %2, [%1]" : "=r" (result) : "r" (addr), "r" (value) );
   return(result);
}

/**
 * @brief  STR Exclusive (32 bit)
 *
 * @param  value  value to store
 * @param  *addr  address pointer
 * @return        successful / failed
 *
 * Exclusive STR command for 32 bit values
 */
uint32_t __STREXW(uint32_t value, uint32_t *addr)
{
   uint32_t result=0;
  
   __ASM volatile ("strex %0, %2, [%1]" : "=r" (result) : "r" (addr), "r" (value) );
   return(result);
}


#elif (defined (__TASKING__)) /*------------------ TASKING Compiler ---------------------*/
/* TASKING carm specific functions */

/*
 * The CMSIS functions have been implemented as intrinsics in the compiler.
 * Please use "carm -?i" to get an up to date list of all instrinsics,
 * Including the CMSIS ones.
 */

#endif
