/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Descriptions:        Gestione Interrupt Timer (Gravità Tetris)
** Correlated files:    timer.h, game.h
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../game.h" // Necessario per accedere a currentGameState e MovePieceDown

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
** Gestisce la discesa automatica (Gravità) del Tetris
******************************************************************************/
void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) 			 // MR0 (Match Register 0)
	{ 
		// SPECIFICA 5: Discesa Automatica (Gravity)
        // Il timer fa scendere il pezzo SOLO se il gioco è nello stato PLAYING.
        // Se è in PAUSA, RESET o GAME OVER, il timer scorre ma non muove nulla.
		if (currentGameState == GAME_PLAYING) {
			MovePieceDown(); // Funzione centralizzata in game.c
		}
		
		LPC_TIM0->IR = 1;			 // Clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		LPC_TIM0->IR = 2;			 // Clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		LPC_TIM0->IR = 4;			 // Clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		LPC_TIM0->IR = 8;			 // Clear interrupt flag 
	}
	
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
** Descriptions:		Timer/Counter 1 interrupt handler
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	if(LPC_TIM1->IR & 1) 	  	 // MR0
	{ 
		LPC_TIM1->IR = 1;			 // Clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		LPC_TIM1->IR = 2;			 // Clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		LPC_TIM1->IR = 4;			 // Clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		LPC_TIM1->IR = 8;			 // Clear interrupt flag 
	} 
	return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
** Descriptions:		Timer/Counter 2 interrupt handler
******************************************************************************/
void TIMER2_IRQHandler (void)
{
	if(LPC_TIM2->IR & 1) 	   	 // MR0
	{ 
		LPC_TIM2->IR = 1;			 // Clear interrupt flag
	}
	else if(LPC_TIM2->IR & 2){ // MR1
		LPC_TIM2->IR = 2;			 // Clear interrupt flag 
	}	
	else if(LPC_TIM2->IR & 4){ // MR2
		LPC_TIM2->IR = 4;			 // Clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 8){ // MR3
		LPC_TIM2->IR = 8;			 // Clear interrupt flag 
	} 
  return;
}

/******************************************************************************
** Function name:		Timer3_IRQHandler
** Descriptions:		Timer/Counter 3 interrupt handler
******************************************************************************/
void TIMER3_IRQHandler (void)
{
	if(LPC_TIM3->IR & 1) 		   // MR0
	{ 
		LPC_TIM3->IR = 1;			 // Clear interrupt flag
	}
	else if(LPC_TIM3->IR & 2){ // MR1
		LPC_TIM3->IR = 2;			 // Clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 4){ // MR2
		LPC_TIM3->IR = 4;			 // Clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 8){ // MR3
		LPC_TIM3->IR = 8;			 // Clear interrupt flag 
	} 
  return;
}