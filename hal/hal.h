#ifndef _HAL_H_
#define _HAL_H_

//#define CLI()         __set_PRIMASK(1)              // Disable Interrupts
//#define SEI()         __set_PRIMASK(0)              // Enable Interrupts
#define CLI()           ;              // Disable Interrupts
#define SEI()           ;              // Enable Interrupts

#define HAL_ENABLE_INTERRUPTS()         SEI()       // Enable Interrupts
#define HAL_DISABLE_INTERRUPTS()        CLI()       // Disable Interrupts
#define HAL_INTERRUPTS_ARE_ENABLED()    SEI()       // Enable Interrupts

#define HAL_ENTER_CRITICAL_SECTION()    CLI()
#define HAL_EXIT_CRITICAL_SECTION()     SEI()

#endif /* _HAL_H_ */
