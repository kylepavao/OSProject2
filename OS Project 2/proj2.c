//Kyle Pavao Proj2.c file
//Serves as main kernel function

#include <stdio.h>
#include <stdlib.h>
#include <device/hid/keyboard.h>
#include "rpi-gpio.h"
#include <usbd/usbd.h>
#include <types.h>

/** GPIO register set */
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

/** System time variable */
volatile unsigned int tim;

u32 keyCount;      //Keeps track of the number of keys held
u32 keyAddress;    //Address at which main keyboard is found
u16 keyDown;       //Stores which key is down
int i;       //for indexing purposes
bool hasMods;      //Stores whether or not the keyboard has a modifier key down

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{ 
	struct KeyboardModifiers keyMod;         //declare this struct from keyboard.h
	gpio[GPIO_GPFSEL1] |= (1 << 18);         //enables gpio light bit
	UsbInitialise();                         //Loads up Usb capabilities           
	
	UsbCheckForChange();                     //Check USB for any changes in number of devices
	KeyboardCount();                         //Counts number of keyboards
	keyAddress = KeyboardGetAddress(0);      //get the address at which the keyboard can be found
	
	while(1) {                                                    //loop here forever
		KeyboardPoll(keyAddress);                                 //Poll keyboard
		keyMod = KeyboardGetModifiers(keyAddress);                //Get if modifiers were held down in last poll
		keyCount = KeyboardGetKeyDownCount(keyAddress);           //get number of keys held in last poll
		keyDown = KeyboardGetKeyDown(keyAddress, 0);              //get 1st key that is held down
		if (keyDown > 3 && keyDown < 40) {                        //if key held is a - z do following
			for (i = 0; i < 100000; i++);                         //we do this just in case a modifier was hit just after
			KeyboardPoll(keyAddress);                             //poll again for case above
			keyMod = KeyboardGetModifiers(keyAddress);            //check again for a modifier key
			if (keyMod.LeftControl == 1 || keyMod.LeftShift == 1 || keyMod.LeftAlt == 1 || keyMod.LeftGui == 1 || keyMod.RightControl == 1 || keyMod.RightShift == 1 || keyMod.RightAlt == 1 || keyMod.RightGui == 1) {    //checks our struct to see if any of the modifiers were held
				for(i = 0; i < 3; i++) {        
					dash();                                       //dash 3 times
				}
				for (i = 0; i < 4; i++) {
					dot();                                        //dot 4 times
				}
			} else {
				for (i = 0; i < 4; i++) {
					dot();                                        //else dot 4 times
				}
			}
		} else if (keyMod.LeftControl == 1 || keyMod.LeftShift == 1 || keyMod.LeftAlt == 1 || keyMod.LeftGui == 1 || keyMod.RightControl == 1 || keyMod.RightShift == 1 || keyMod.RightAlt == 1 || keyMod.RightGui == 1) {   //checks our struct to see if any of the modifiers were held
			for (i = 0; i < 100000; i++);                          //do this if letter key was hit just after
			KeyboardPoll(keyAddress);                              //poll again for case listed above
			keyDown = KeyboardGetKeyDown(keyAddress, 0);           //see if any keys were held
			if (keyDown > 3 && keyDown < 40) {	                   //if there was an A through Z held
				for(i = 0; i < 3; i++) {
					dash();                                        //dash 3 times
				}
				for (i = 0; i < 4; i++) {
					dot();                                         //dot 4 times
				}
			} else {
				for (i = 0; i < 3; i++) {
					dash();                                        //else dash 3 times
				}
			}			
		} else if (keyCount >= 1) {                               //if neither an A - Z or modifier was held do following
			for (i=0; i < 3; i++) {                               //alternate between dash and dot 3 times
				dash();
				dot();
			} 
		} 
	}
}

/* A simple function to implement the dots
 * the incrementing of time is what makes the 
 * dots work
*/
void dot() 
{
	gpio[GPIO_GPSET0] = (1 << 16);
			
	for(tim = 0; tim < 300000; tim++)
    ;
			
	gpio[GPIO_GPCLR0] = (1 << 16);
			
	for(tim = 0; tim < 500000; tim++)
    ;
			
	gpio[GPIO_GPSET0] = (1 << 16);
			
	for(tim = 0; tim < 300000; tim++)
    ;

}

/* A simple function to implement the dots
 * the incrementing of time is what makes the 
 * dots work
*/
void dash() 
{
			
	gpio[GPIO_GPCLR0] = (1 << 16);
			
	for(tim = 0; tim < 1200000; tim++)
    ;
			
	gpio[GPIO_GPSET0] = (1 << 16);
			
	for(tim = 0; tim < 500000; tim++)
    ;
}

