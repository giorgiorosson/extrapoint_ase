/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Descriptions:        Gestione Input Utente (Pulsanti + Joystick) con Auto-Repeat
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../game.h"            
#include "../GLCD/GLCD.h"       

// Patch colori di sicurezza
#ifndef Red
  #define Red   0xF800
#endif
#ifndef Black
  #define Black 0x0000
#endif
#ifndef White
  #define White 0xFFFF
#endif

/* Variabili Globali Gestione De-Bouncing */
volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;
volatile int toRelease_down_0 = 0;
volatile int toRelease_down_1 = 0;
volatile int toRelease_down_2 = 0;

volatile int J_up = 0;
volatile int J_down = 0;
volatile int J_right = 0;
volatile int J_left = 0;
volatile int J_click = 0;

/******************************************************************************
** Function name:       RIT_IRQHandler
** Descriptions:        Polling ogni 50ms
******************************************************************************/
void RIT_IRQHandler(void) 
{           
    // ------------------------------------
    // GESTIONE KEY1 (Start / Pause)
    // ------------------------------------
    if(down_1 !=0) {            
        down_1++;
        if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){ 
            switch(down_1){
                case 2: // Short Press
                    if (currentGameState == GAME_RESET || currentGameState == GAME_OVER) {
                        srand(LPC_TIM0->TC); 
                        Game_Reset();      
                        SpawnNewPiece();   
                        currentGameState = GAME_PLAYING;
                    }
                    else if (currentGameState == GAME_PLAYING) {
                        currentGameState = GAME_PAUSED;
                        GUI_Text(160, 80, (uint8_t *)"PAUSED", Red, Black);
                    }
                    else if (currentGameState == GAME_PAUSED) {
                        currentGameState = GAME_PLAYING;
                        GUI_Text(160, 80, (uint8_t *)"      ", Black, Black); 
                    }
                    toRelease_down_1=1;
                    break;
                default: break;
            }
        }
        else {  
            if(toRelease_down_1) toRelease_down_1=0;           
            down_1=0;   
            NVIC_EnableIRQ(EINT1_IRQn);                  
            LPC_PINCON->PINSEL4 |= (1 << 22);                 
        }
    }   
    
    // ------------------------------------
    // GESTIONE KEY2 (Hard Drop)
    // ------------------------------------
    if(down_2 !=0) {            
        down_2++;
        if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){ 
            switch(down_2){
                case 2: 
                    if (currentGameState == GAME_PLAYING) HardDrop();
                    toRelease_down_2=1;
                    break;
                default: break;
            }
        }
        else {  
            if(toRelease_down_2) toRelease_down_2=0;   
            down_2=0;       
            NVIC_EnableIRQ(EINT2_IRQn);                  
            LPC_PINCON->PINSEL4 |= (1 << 24);                 
        }
    }   
    
    // ===============================================================
    // GESTIONE JOYSTICK (IMPLEMENTAZIONE MOVIMENTO LATERALE)
    // ===============================================================
    
   // --- JOYSTICK UP (Rotazione) ---
    if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 ) { 
        J_up++;
        switch(J_up){
            case 1: 
                if (currentGameState == GAME_PLAYING) RotatePiece();
                break;
        }
    } else { J_up=0; }

    // --- JOYSTICK DOWN (Soft Drop) ---
    if((LPC_GPIO1->FIOPIN & (1<<26)) == 0) {
        J_down++;
        switch(J_down){
            case 1: 
                if (currentGameState == GAME_PLAYING) MovePieceDown();
                break;
            default:
                // Scende veloce se tieni premuto
                 if (currentGameState == GAME_PLAYING) MovePieceDown();
                break;
        }
    } else { J_down=0; }

    // --- JOYSTICK RIGHT (Destra) ---
    if((LPC_GPIO1->FIOPIN & (1<<28)) == 0) {
        J_right++;
        switch(J_right){
            case 1: // Primo scatto immediato
                if (currentGameState == GAME_PLAYING) MovePieceRight();
                break;
            default:
                // Auto-Repeat: muovi ogni 2 cicli (più veloce)
                if ((J_right % 2) == 0) { 
                     if (currentGameState == GAME_PLAYING) MovePieceRight();
                }
                break;
        }
    } else { J_right=0; }
    
    // --- JOYSTICK LEFT (Sinistra) ---
    if((LPC_GPIO1->FIOPIN & (1<<27)) == 0) {
        J_left++;
        switch(J_left){
            case 1: 
                if (currentGameState == GAME_PLAYING) MovePieceLeft();
                break;
            default:
                // Auto-Repeat Sinistra
                if ((J_left % 2) == 0) { 
                     if (currentGameState == GAME_PLAYING) MovePieceLeft();
                }
                break;
        }
    } else { J_left=0; }
    
    /* GESTIONE KEY0 (Non usato) */
    if(down_0 !=0) { down_0++; if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){ if(down_0==2) toRelease_down_0=1; } else { if(toRelease_down_0) toRelease_down_0=0; down_0=0; NVIC_EnableIRQ(EINT0_IRQn); LPC_PINCON->PINSEL4 |= (1 << 20); } }

    /* Joystick Click (Non usato) */
    if((LPC_GPIO1->FIOPIN & (1<<25)) == 0) { J_click++; } else { J_click=0; }

    LPC_RIT->RICTRL |= 0x1; 
}