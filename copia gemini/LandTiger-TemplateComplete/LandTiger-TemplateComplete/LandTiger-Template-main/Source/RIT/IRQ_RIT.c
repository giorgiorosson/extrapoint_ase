/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Descriptions:        Gestione Input Utente (Pulsanti + Joystick) per Tetris
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../game.h"            // Include le funzioni di gioco (Move, Rotate, HardDrop, ecc.)
#include "../GLCD/GLCD.h"       // Per testi a schermo (PAUSED)

// Patch colori di sicurezza (nel caso GLCD.h non li esporti correttamente)
#ifndef Red
  #define Red   0xF800
#endif
#ifndef Black
  #define Black 0x0000
#endif
#ifndef White
  #define White 0xFFFF
#endif

/* Variabili Globali Gestione De-Bouncing (Standard del Template) */
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
volatile int J_up_left = 0;
volatile int J_up_right = 0;
volatile int J_down_left = 0;
volatile int J_down_right = 0;

/******************************************************************************
** Function name:       RIT_IRQHandler
** Descriptions:        Polling ogni 50ms per leggere tasti e joystick
******************************************************************************/
void RIT_IRQHandler(void) 
{           
    // ------------------------------------
    // GESTIONE KEY0 (INT0) - Non usato nel Tetris
    // ------------------------------------
    if(down_0 !=0) {            
        down_0++;
        if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){ 
            switch(down_0) {
                case 2:             
                    toRelease_down_0 = 1;
                    break;
                default:
                    break;
            }
        }
        else {  
            if(toRelease_down_0){
                toRelease_down_0=0;
            }
            down_0=0;           
            NVIC_EnableIRQ(EINT0_IRQn);                  
            LPC_PINCON->PINSEL4    |= (1 << 20);                 
        }
    }

    // ---------------------------------------------------------------
    // GESTIONE KEY1 (Specifica 2: Start / Pause / Resume)
    // ---------------------------------------------------------------
    if(down_1 !=0) {            
        down_1++;
        if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){ // Tasto KEY1 premuto
            switch(down_1){
                case 2: // Short Press
                    
                    // --- LOGICA DI GIOCO START/PAUSA ---
                    
                    // CASO 1: AVVIO GIOCO
                    if (currentGameState == GAME_RESET || currentGameState == GAME_OVER) {
                        // Genera Random Seed basato sul timer attuale
                        srand(LPC_TIM0->TC); 
                        
                        Game_Reset();      // Resetta grid e score
                        SpawnNewPiece();   // Crea primo pezzo
                        
                        currentGameState = GAME_PLAYING;
                    }
                    // CASO 2: METTI IN PAUSA
                    else if (currentGameState == GAME_PLAYING) {
                        currentGameState = GAME_PAUSED;
                        GUI_Text(160, 80, (uint8_t *)"PAUSED", Red, Black);
                    }
                    // CASO 3: RIPRENDI DALLA PAUSA
                    else if (currentGameState == GAME_PAUSED) {
                        currentGameState = GAME_PLAYING;
                        GUI_Text(160, 80, (uint8_t *)"      ", Black, Black); // Cancella scritta
                    }
                    // -----------------------------------
                    
                    toRelease_down_1=1;
                    break;
                default:
                    break;
            }
        }
        else {  /* button released */
            if(toRelease_down_1){
                toRelease_down_1=0;
            }           
            down_1=0;   
            NVIC_EnableIRQ(EINT1_IRQn);                  
            LPC_PINCON->PINSEL4    |= (1 << 22);                 
        }
    }   
    
    // ------------------------------------
    // GESTIONE KEY2 (Specifica 7: Hard Drop)
    // ------------------------------------
    if(down_2 !=0) {            
        down_2++;
        if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){ 
            switch(down_2){
                case 2: // Short Press
                    // Esegui Hard Drop solo se si sta giocando
                    if (currentGameState == GAME_PLAYING) {
                        HardDrop();
                    }
                    
                    toRelease_down_2=1;
                    break;
                default:
                    break;
            }
        }
        else {  
            if(toRelease_down_2){
                toRelease_down_2=0;
            }   
            down_2=0;       
            NVIC_EnableIRQ(EINT2_IRQn);                  
            LPC_PINCON->PINSEL4    |= (1 << 24);                 
        }
    }   
    
    // ===============================================================
    // GESTIONE JOYSTICK (Specifica 5)
    // ===============================================================
    
    /* Joystick UP - ROTAZIONE */
    if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 ) { 
        J_up++;
        switch(J_up){
            case 1: 
                // Esegui Rotazione
                if (currentGameState == GAME_PLAYING) RotatePiece();
                break;
            default:
                // Auto-repeat delay potrebbe essere aggiunto qui
                break;
        }
    } else { J_up=0; }

    /* Joystick DOWN - SOFT DROP (Discesa veloce) */
    if((LPC_GPIO1->FIOPIN & (1<<26)) == 0) {
        J_down++;
        switch(J_down){
            case 1: 
                // Prima pressione: muovi giù
                if (currentGameState == GAME_PLAYING) MovePieceDown();
                break;
            default:
                // Se tenuto premuto, scende veloce (ogni 50ms)
                 if (currentGameState == GAME_PLAYING) MovePieceDown();
                break;
        }
    } else { J_down=0; }

    /* Joystick RIGHT - MUOVI A DESTRA */
    if((LPC_GPIO1->FIOPIN & (1<<28)) == 0) {
        J_right++;
        switch(J_right){
            case 1: 
                if (currentGameState == GAME_PLAYING) MovePieceRight();
                break;
             // Opzionale: gestire movimento continuo se tenuto premuto
            default:
                break;
        }
    } else { J_right=0; }
    
    /* Joystick LEFT - MUOVI A SINISTRA */
    if((LPC_GPIO1->FIOPIN & (1<<27)) == 0) {
        J_left++;
        switch(J_left){
            case 1: 
                if (currentGameState == GAME_PLAYING) MovePieceLeft();
                break;
            default:
                break;
        }
    } else { J_left=0; }
    
    /* Joystick CLICK (Select) - Non usato o Opzionale */
    if((LPC_GPIO1->FIOPIN & (1<<25)) == 0) {
        J_click++;
        // ...
    } else { J_click=0; }
    
    /* Reset flag interrupt RIT */
    LPC_RIT->RICTRL |= 0x1; 
}