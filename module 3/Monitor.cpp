/**----------------------------------------------------------------------------
             \file Monitor.cpp
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1 Module 3                                       --
--                Microcontroller Firmware                                   --
--                      Monitor.cpp                                            --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
--               
--                
--  Designed by:  Tim Scherr
--  Revised by:  Student's name 
-- 
-- Version: 2.0
-- Date of current revision:  2016-09-29   
-- Target Microcontroller: Freescale MKL25ZVMT4 
-- Tools used:  ARM mbed compiler
--              ARM mbed SDK
--              Freescale FRDM-KL25Z Freedom Board
--               
-- 
   Functional Description: See below 
--
--      Copyright (c) 2015 Tim Scherr All rights reserved.
--
*/              

#include <stdio.h>
#include "shared.h"


/*******************************************************************************
* Set Display Mode Function
* Function determines the correct display mode.  The 3 display modes operate as 
*   follows:
*
*  NORMAL MODE       Outputs only mode and state information changes   
*                     and calculated outputs
*
*  QUIET MODE        No Outputs
*
*  DEBUG MODE        Outputs mode and state information, error counts,
*                    register displays, sensor states, and calculated output
*
*
* There is deliberate delay in switching between modes to allow the RS-232 cable 
* to be plugged into the header without causing problems. 
*******************************************************************************/
uint8_t itoa(int32_t input, uint8_t * pointer, uint32_t base_n);
uint8_t * reverse(uint8_t * source, size_t length);

/* Making r0-r15 global variables */


void set_display_mode(void)   
{
  UART_direct_msg_put("\r\nSelect Mode");
  UART_direct_msg_put("\r\n Hit NOR - Normal");
  UART_direct_msg_put("\r\n Hit QUI - Quiet");
  UART_direct_msg_put("\r\n Hit DEB - Debug" );
  UART_direct_msg_put("\r\n Hit V - Version#\r\n");
  UART_direct_msg_put("\r\n Hit R - Print Register");
  UART_direct_msg_put("\r\n Hit M - Memory section Display\r\n");
  UART_direct_msg_put("\r\nSelect:  ");
  
}


//*****************************************************************************/
/// \fn void chk_UART_msg(void) 
///
//*****************************************************************************/
void chk_UART_msg(void)    
{
   UCHAR j;
   while( UART_input() )      // becomes true only when a byte has been received
   {                                    // skip if no characters pending
      j = UART_get();                 // get next character

      if( j == '\r' )          // on a enter (return) key press
      {                // complete message (all messages end in carriage return)
         UART_msg_put("->");
         UART_msg_process();
      }
      else 
      {
         if ((j != 0x02) )         // if not ^B
         {                             // if not command, then   
            UART_put(j);              // echo the character   
         }
         else
         {
           ;
         }
         if( j == '\b' ) 
         {                             // backspace editor
            if( msg_buf_idx != 0) 
            {                       // if not 1st character then destructive 
               UART_msg_put(" \b");// backspace
               msg_buf_idx--;
            }
         }
         else if( msg_buf_idx >= MSG_BUF_SIZE )  
         {                                // check message length too large
            UART_msg_put("\r\nToo Long!");
            msg_buf_idx = 0;
         }
         else if ((display_mode == QUIET) && (msg_buf[0] != 0x02) && 
                  (msg_buf[0] != 'D') && (msg_buf[0] != 'N') && 
                  (msg_buf[0] != 'V') &&
                  (msg_buf_idx != 0))
         {                          // if first character is bad in Quiet mode
            msg_buf_idx = 0;        // then start over
         }
         else {                        // not complete message, store character
 
            msg_buf[msg_buf_idx] = j;
            msg_buf_idx++;
            if (msg_buf_idx > 2)
            {
               UART_msg_process();
            }
         }
      }
   }
}

//*****************************************************************************/
///  \fn void UART_msg_process(void) 
///UART Input Message Processing
//*****************************************************************************/
void UART_msg_process(void)
{
   UCHAR chr,err=0;
//   unsigned char  num;


   if( (chr = msg_buf[0]) <= 0x60 ) 
   {      // Upper Case
      switch( chr ) 
      {
         case 'D':
            if((msg_buf[1] == 'E') && (msg_buf[2] == 'B') && (msg_buf_idx == 3)) 
            {
               display_mode = DEBUG;
               UART_msg_put("\r\nMode=DEBUG\n");
               display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'N':
            if((msg_buf[1] == 'O') && (msg_buf[2] == 'R') && (msg_buf_idx == 3)) 
            {
               display_mode = NORMAL;
               UART_msg_put("\r\nMode=NORMAL\n");
               //display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'Q':
            if((msg_buf[1] == 'U') && (msg_buf[2] == 'I') && (msg_buf_idx == 3)) 
            {
               display_mode = QUIET;
               UART_msg_put("\r\nMode=QUIET\n");
               display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'V':
            display_mode = VERSION;
            UART_msg_put("\r\n");
            UART_msg_put( CODE_VERSION ); 
            UART_msg_put("\r\nSelect  ");
            display_timer = 0;
            break;
            
         case 'R':
            display_mode = REGISTER;
            UART_msg_put("\r\nMode=REGISTER\n");
            display_timer = 0;
            break;   
            
         default:
            err = 1;
      }
   }

   else 
   {                                 // Lower Case
      switch( chr ) 
      {
        default:
         err = 1;
      }
   }

   if( err == 1 )
   {
      UART_msg_put("\n\rError!");
   }   
   else if( err == 2 )
   {
      UART_msg_put("\n\rNot in DEBUG Mode!");
   }   
   else
   {
    msg_buf_idx = 0;          // put index to start of buffer for next message
      ;
   }
   msg_buf_idx = 0;          // put index to start of buffer for next message


}


//*****************************************************************************
///   \fn   is_hex
/// Function takes 
///  @param a single ASCII character and returns 
///  @return 1 if hex digit, 0 otherwise.
///    
//*****************************************************************************
UCHAR is_hex(UCHAR c)
{
   if( (((c |= 0x20) >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f'))  )
      return 1;
   return 0;
}

__asm int get_Reg1(){
       mov r0, r1
       bx lr
    }   
__asm int get_Reg2(){
       mov r0, r2
       bx lr
    } 
__asm int get_Reg3(){
       mov r0, r3
       bx lr
    } 
__asm int get_Reg4(){
       mov r0, r4
       bx lr
    } 
__asm int get_Reg5(){
       mov r0, r5
       bx lr
    } 
__asm int get_Reg6(){
       mov r0, r6
       bx lr
    } 
__asm int get_Reg7(){
       mov r0, r7
       bx lr
    } 
__asm int get_Reg8(){
       mov r0, r8
       bx lr
    }
__asm int get_Reg9(){
       mov r0, r9
       bx lr
    } 
__asm int get_Reg10(){
       mov r0, r10
       bx lr
    } 
__asm int get_Reg11(){
       mov r0, r11
       bx lr
    }
__asm int get_Reg12(){
       mov r0, r12
       bx lr
    } 
__asm int get_Reg13(){
       mov r0, r13
       bx lr
    }     
__asm int get_Reg14(){
       mov r0, r14
       bx lr
    }     
__asm int get_Reg15(){
       mov r0, r15
       bx lr
    }     
/*
*******************************************************************************
*   \fn  DEBUG and DIAGNOSTIC Mode UART Operation
*******************************************************************************/


void monitor(void)
{
uint32_t reg[16];

reg[1]=get_Reg1();
reg[2]=get_Reg2();
    reg[3]=get_Reg3();
    reg[4]=get_Reg4();
    reg[5]=get_Reg5();
    reg[6]=get_Reg6();
    reg[7]=get_Reg7();
    reg[8]=get_Reg8();
    reg[9]=get_Reg9();
    reg[10]=get_Reg10();
    reg[11]=get_Reg11();
    reg[12]=get_Reg12();
    reg[13]=get_Reg13();
    reg[14]=get_Reg14();
    reg[15]=get_Reg15();
/**********************************/
/*     Spew outputs               */
/**********************************/

   switch(display_mode)
   {
      case(QUIET):
         {
             UART_msg_put("\r\n ");
             display_flag = 0;
         }  
         break;
      case(VERSION):
         {
             display_flag = 0;
         }  
         break;         
      case(NORMAL):
         {
            if (display_flag == 1)
            {
               UART_msg_put("\r\nNORMAL ");
               UART_msg_put(" Flow: ");
               // ECEN 5803 add code as indicated
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Temp: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Freq: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               display_flag = 0;
            }
         }  
         break;
      case(DEBUG):
         {
            if (display_flag == 1)
            {
               UART_msg_put("\r\nDEBUG ");
               UART_msg_put(" Flow: ");
               // ECEN 5803 add code as indicated               
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Temp: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Freq: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               
               
 /****************      ECEN 5803 add code as indicated   ***************/             
               //  Create a display of  error counts, sensor states, and
               //  ARM Registers R0-R15
               
               //  Create a command to read a section of Memory and display it
               
               
               //  Create a command to read 16 words from the current stack 
               // and display it in reverse chronological order.
              
              
               // clear flag to ISR      
               display_flag = 0;
             }   
         }  
         break;
    case(REGISTER):
        if (display_flag == 1) {
      UART_msg_put("\r\nREGISTERS ---\r\n");
      
      reg[1]=get_Reg1();
      UART_direct_hex_put((reg[1]>>24));
      UART_direct_hex_put((reg[1]>>16));
      UART_direct_hex_put((reg[1]>>8));
      UART_direct_hex_put((reg[1]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[2]=get_Reg2();
      UART_direct_hex_put((reg[2]>>24));
      UART_direct_hex_put((reg[2]>>16));
      UART_direct_hex_put((reg[2]>>8));
      UART_direct_hex_put((reg[2]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[3]=get_Reg3();
      UART_direct_hex_put((reg[3]>>24));
      UART_direct_hex_put((reg[3]>>16));
      UART_direct_hex_put((reg[3]>>8));
      UART_direct_hex_put((reg[3]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[4]=get_Reg4();
      UART_direct_hex_put((reg[4]>>24));
      UART_direct_hex_put((reg[4]>>16));
      UART_direct_hex_put((reg[4]>>8));
      UART_direct_hex_put((reg[4]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[5]=get_Reg5();
      UART_direct_hex_put((reg[5]>>24));
      UART_direct_hex_put((reg[5]>>16));
      UART_direct_hex_put((reg[5]>>8));
      UART_direct_hex_put((reg[5]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[6]=get_Reg6();
      UART_direct_hex_put((reg[6]>>24));
      UART_direct_hex_put((reg[6]>>16));
      UART_direct_hex_put((reg[6]>>8));
      UART_direct_hex_put((reg[6]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[7]=get_Reg7();
      UART_direct_hex_put((reg[7]>>24));
      UART_direct_hex_put((reg[7]>>16));
      UART_direct_hex_put((reg[7]>>8));
      UART_direct_hex_put((reg[7]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[8]=get_Reg8();
      UART_direct_hex_put((reg[8]>>24));
      UART_direct_hex_put((reg[8]>>16));
      UART_direct_hex_put((reg[8]>>8));
      UART_direct_hex_put((reg[8]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[9]=get_Reg9();
      UART_direct_hex_put((reg[9]>>24));
      UART_direct_hex_put((reg[9]>>16));
      UART_direct_hex_put((reg[9]>>8));
      UART_direct_hex_put((reg[9]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[10]=get_Reg10();
      UART_direct_hex_put((reg[10]>>24));
      UART_direct_hex_put((reg[10]>>16));
      UART_direct_hex_put((reg[10]>>8));
      UART_direct_hex_put((reg[10]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[11]=get_Reg11();
      UART_direct_hex_put((reg[11]>>24));
      UART_direct_hex_put((reg[11]>>16));
      UART_direct_hex_put((reg[11]>>8));
      UART_direct_hex_put((reg[11]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[12]=get_Reg12();
      UART_direct_hex_put((reg[12]>>24));
      UART_direct_hex_put((reg[12]>>16));
      UART_direct_hex_put((reg[12]>>8));
      UART_direct_hex_put((reg[12]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[13]=get_Reg13();
      UART_direct_hex_put((reg[13]>>24));
      UART_direct_hex_put((reg[13]>>16));
      UART_direct_hex_put((reg[13]>>8));
      UART_direct_hex_put((reg[13]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[14]=get_Reg14();
      UART_direct_hex_put((reg[14]>>24));
      UART_direct_hex_put((reg[14]>>16));
      UART_direct_hex_put((reg[14]>>8));
      UART_direct_hex_put((reg[14]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
    reg[15]=get_Reg15();
      UART_direct_hex_put((reg[15]>>24));
      UART_direct_hex_put((reg[15]>>16));
      UART_direct_hex_put((reg[15]>>8));
      UART_direct_hex_put((reg[15]&0xFF));
      UART_direct_msg_put(("\r\n"));
    
      //uint8_t i;
      /*for (i = 0; i < 16; i++) {
        reg[i]=get_Reg();
        UART_direct_hex_put((reg[i]>>24));
        UART_direct_hex_put((reg[i]>>16));
        UART_direct_hex_put((reg[i]>>8));
        UART_direct_hex_put((reg[i]&0xFF));
        UART_direct_msg_put(("\r\n"));
    }
    itoa(reg[i], convert, 10);
    UART_msg_put(" ");
    UART_msg_put((const char*)convert);
     */ 
      display_flag = 0;
    }
    break;
    
      default:
      {
         UART_msg_put("Mode Error");
      }  
   }
}  

uint8_t itoa(int32_t input, uint8_t * pointer, uint32_t base_n)
{
  if (pointer == NULL || base_n < 2 || base_n > 16) {
    return 0;
  }

  uint8_t len = 0, val;

  /* check for sign */
  if(input < 0) {
    input *= -1;
    *pointer = '-';
    len++;
  }

  /* this uses modulus and divide to find the value in each place */
  do {
    val = input % base_n;
    *(pointer + len++) = val < 10 ? '0' + val : 'A' + val - 10;
    input /= base_n;
  } while(input != 0);

  /* reverse the numbers */
  if(*pointer == '-') {
    reverse(pointer + 1, len - 1);
  } else {
    reverse(pointer, len);
  }

  *(pointer + len++) = '\0';

  return len;
}

uint8_t * reverse(uint8_t * source, size_t length)
{
  if (source == NULL) {
    return NULL;
  }

  uint8_t temp_val;
  int32_t i;
  for(i = 0; i < length / 2; i++)
    {
      temp_val = *(source + i);
      *(source + i) = *(source + length - 1 - i);
      *(source + length - 1 - i) = temp_val;
    }

  return source;
}
