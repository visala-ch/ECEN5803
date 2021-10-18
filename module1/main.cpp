/************************************************************************//**
* \file main.c
* \brief LAB EXERCISE 5.2 - SQUARE ROOT APPROXIMATION
*
* Write an assembly code subroutine to approximate the square root of an 
* argument using the bisection method. All math is done with integers, so the 
* resulting square root will also be an integer
******************************************************************************
* GOOD LUCK!
 ****************************************************************************/
#include "stdio.h"
 #include "stdint.h"
 #include "mbed.h"
 Serial pc(USBTX, USBRX);
 /** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
 #include "string.h"
 /** @endcond */
 
 /**
 * @brief 5863 Write a brief description of the function here
 *
 * Detailed description of the function after one line gap
 *
 * @note You can also add a note this way
 *
 * @param[in] 
 *  You can also start writing on a new line. 
 *  And continue on the next line. This is how you describe an input parameter
 *
 * @return
 *  This is how you describe the return value
 * 
 */
Timer timer;
__asm int my_sqrt(int x){
    
                    push    {r4-r7,lr}
                    ldr     r1, =0x00000000 
                    //ldr       r5, [r1]    //done=0    
                    
                    ldr     r2, =0x00000000
                    //ldr       r6, [r2]    //a
                    
                    ldr     r3, =0x00000100
                    //ldr       r6, [r3]    //b
                    
                    ldr     r4, =   0x00000000   
                    //ldr       r7, [r4]    //c =-1
                    
sqrtLoop    movs    r5, r4  //c_old = c
                    adds    r4, r2,r3  //c=a+b
                    asrs    r4,#1
                    movs    r6, r4  //storing c value in another reg
                    muls    r6, r4, r6 //mul,  r5=c*c
                    cmp     r6, r0  //comparing c*c and x
                    beq     done //if equal branch to done
                    blt     LT  //if less than branch to L2
                    mov     r3,r4 //if greater than set b=c
                    b           compare
                    
done            movs    r1,#1 //done=1
                    b           compare

LT              mov     r2,r4   //a=c
                    b           compare

compare     cmp     r1,#1 //if done =1
                    beq     end //end loop
                    bne     whileCond   //if not, go to whileCond

whileCond   cmp     r5, r4  //done!=1 && compare c to c_old
                    bne     sqrtLoop // if c!=c_old, go to sqrtLoop 
                    beq     end         // if done!=1 && c== c_old, end loop

end             movs    r0,r4 //move c to x
                    pop{r4-r7}
                    bx      lr  // {b lr ?? difference?) go back to the main branch

}


/*----------------------------------------------------------------------------
 MAIN function
 *----------------------------------------------------------------------------*/
 /**
 * @brief Main function
 *
 * Detailed description of the main
 */
/*int my_sqrt(int x){
int c_old, done = 0, a=0, b = 256, c = -1;
        do{
            c_old = c;
            c = (a+b)/2;
            if(c*c == x){
                    done = 1;
            }
            else if(c*c < x){
                a = c;
            }
            else{
                b = c;
            }
        }while((!done)&&(c!=c_old));
    //Write your code here
    return c;
    }       
*/

void printCpuCycles(double readValue, int number, int sqrt)
{
    double cpuCycles = 0;
     
    cpuCycles = (readValue * 48000000);   // read value * frequqncy of clock
    printf("Sqrt of %d using bisection menthod is %d \n\r", number, sqrt);
    printf("Total time of execution for sqrt of(%d)is %f \n\r", number, readValue);
    printf("Number of CPU cycles for sqrt(%d): %d\n\r\n\r", number, (int)cpuCycles); 
}


int main(void){
    volatile int r, j=0;
    int i; 
    double timer_read = 0.0;
    //double sum = 0;   

    pc.printf("Frequency of KL25Z (ARM Cortex M0+): 48 MHZ\n\r\n\r");
    
	  timer.reset();
    timer.start();
    r = my_sqrt(121);     // should be 11
	  timer.stop();
    timer_read = timer.read();
    printCpuCycles(timer_read, 121, r);  
    //sum = sum + timer_read;
    timer.reset();

		timer.reset();            
		timer.start();
    r = my_sqrt(4);     // should be 2
		timer_read = timer.read();
    printCpuCycles(timer_read, 4, r);  
    //sum = sum + timer_read;
    timer.reset();    

		timer.reset();    
		timer.start();
    r = my_sqrt(22);   			// should be 4
		timer.stop();
    timer_read = timer.read();
    printCpuCycles(timer_read, 22, r);  
    //sum = sum + timer_read;
    timer.reset();    
  //printf("%d",r);
		timer.reset();
		
		timer.start();
    r = my_sqrt(2);     // should be 1
		timer.stop();
		timer_read = timer.read();
    printCpuCycles(timer_read, 2, r);  
    //sum = sum + timer_read;
    timer.reset();    
  //printf("%d",r);
	
	for(i=0;i<10000;i++){
		r = my_sqrt(i);
		j+=r;
	}
  while(1);
  }

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************/
