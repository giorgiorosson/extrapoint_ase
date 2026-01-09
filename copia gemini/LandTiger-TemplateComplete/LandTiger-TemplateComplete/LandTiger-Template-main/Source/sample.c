/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: Main del Tetris Game per LandTiger
 * Note(s): Gestisce l'inizializzazione e il loop infinito
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.h"                    
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "button_EXINT/button.h" 
#include "game.h"                // Include le funzioni di inizializzazione gioco

#ifdef SIMULATOR
extern uint8_t ScaleFlag; 
#endif

int main (void) {
  	
    // 1. Inizializzazione Sistema e Clock
	SystemInit();  
    
    // 2. Inizializzazione Schermo LCD
  	LCD_Initialization();
    LCD_Clear(Black); // Sfondo nero iniziale
    
    // 3. Inizializzazione Grafica Gioco
    // Disegna la griglia vuota e le etichette (SCORE, LINES, ecc.)
    Game_Init(); 
	
    // 4. Inizializzazione Periferiche
    
    // Joystick (Porta 1)
	joystick_init(); 
    
    // Pulsanti (Porta 2)
    BUTTON_init();
	
	// RIT (Timer per polling tasti/joystick) - 50ms
    // Valore 0x004C4B40 = 50ms a 100MHz (o 25MHz simulati)
	init_RIT(0x004C4B40); 
	enable_RIT();
	
    // Timer0 (Motore del gioco e Seed casuale)
    // Impostiamo il timer per contare. 
    // Parametri: Timer0, Prescaler 0, MR0, Reset+Interrupt, Valore Match (25MHz = 1s)
	init_timer(TIMER0, 0, 0, 3, 25000000); 
	enable_timer(TIMER0);
    
    // 5. Configurazione Risparmio Energetico
	LPC_SC->PCON |= 0x1;		/* power-down mode */
	LPC_SC->PCON &= 0xFFFFFFFFD;						
		
    // 6. Loop Infinito
    // Il processore dorme finché non arriva un interrupt (Timer o RIT)
    while (1) {                           
		__ASM("wfi"); 
    }
}