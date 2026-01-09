#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdlib.h> 
#include "LPC17xx.h"

// --- Costanti ---
#define RIGA_FIELD 20
#define COLONNA_FIELD 10
#define BLOCK_SIZE 12   
#define FIELD_X_START 20    
#define FIELD_Y_START 20

// Colori
#define COLOR_EMPTY  0x0000   
#define COLOR_GRID   0xFFFF   
#define Orange       0xFD20   

// --- Strutture Dati ---
typedef struct {
    int8_t x;
    int8_t y;
} Point;

typedef struct {
    Point blocks[4]; 
    uint16_t color;  
} Tetromino;

// Stati del gioco (Spec. 2)
typedef enum {
    GAME_RESET,   // Stato iniziale: griglia vuota, punteggio 0, attesa di Start
    GAME_PLAYING, // Gioco in corso
    GAME_PAUSED,  // Gioco in pausa
    GAME_OVER     // Partita finita
} GameState;

// --- Variabili Globali Esterne ---
extern uint16_t board[RIGA_FIELD][COLONNA_FIELD];
extern const Tetromino TETROMINOS[7]; 

// Variabili di Stato
extern volatile GameState currentGameState; // volatile perché modificata negli interrupt
extern Tetromino currentPiece;
extern int currentX; 
extern int currentY; 

// Variabili Punteggio (Spec. 2)
extern int score;
extern int highScore;
extern int linesCleared;

// --- Prototipi Funzioni ---
void DrawBlock(int riga, int colonna, uint16_t colore);
void DrawGridOutline(void);
void DrawPiece(Tetromino p, int riga, int colonna);
void ClearBoard(void);      // Pulisce la matrice logica e grafica
void ResetGame(void);       // Inizializza variabili per nuova partita
void SpawnNewPiece(void);   // Genera pezzo casuale (Spec. 4)

#endif