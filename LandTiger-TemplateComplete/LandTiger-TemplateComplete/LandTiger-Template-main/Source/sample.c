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
#include "button_EXINT/button.h" // Se usi i button interrupts diretti (opzionale se usi RIT)
#include "game.h"                // Include le tue funzioni di gioco

#ifdef SIMULATOR
extern uint8_t ScaleFlag; 
#endif

int main (void) {
  	
    // 1. Inizializzazione Sistema e Clock
	SystemInit();  
    
    // 2. Inizializzazione Schermo LCD
  	LCD_Initialization();
    LCD_Clear(Black); // Sfondo nero iniziale
    
    // 3. Inizializzazione Grafica Gioco (Griglia)
    DrawGridOutline(); 
	
    // 4. Inizializzazione Periferiche
    
    // Joystick
	joystick_init(); // Inizializza porte joystick
	
	// RIT (Timer per polling tasti/joystick) - 50ms
	init_RIT(0x004C4B40); 
	enable_RIT();
	
    // Timer0 (Motore del gioco e Seed casuale)
    // Impostiamo il timer per contare. 
    // Nota: Il valore di match (intervallo) sarà gestito dalla logica di gioco o fisso a 1s.
    // Qui lo inizializziamo a 1 secondo (25MHz) con interrupt abilitato.
	init_timer(TIMER0, 0, 0, 3, 25000000); // 3 = Interrupt + Reset on Match
	enable_timer(TIMER0);
    
    // Button Init (Opzionale se gestiti via RIT, ma utile per abilitare le porte)
    BUTTON_init();

    // 5. Inizializzazione Stato Gioco (Specifica 2)
    // Il gioco parte in stato RESET (schermata ferma, aspetta KEY1)
    currentGameState = GAME_RESET;
    score = 0;
    linesCleared = 0;
    highScore = 0;
    
    // Gestione Risparmio Energetico (Opzionale)
	LPC_SC->PCON |= 0x1;		/* power-down	mode */
	LPC_SC->PCON &= 0xFFFFFFFFD;						
		
    // 6. Loop Infinito
    while (1) {                           
		__ASM("wfi"); // Wait For Interrupt - Aspetta che succeda qualcosa (Timer o Tasto)
    }
}