#ifndef GAME_H
#define GAME_H

#include "LPC17xx.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h> // Per rand/srand

// --- SPECIFICA 1: Campo di Gioco 20x10 ---
#define RIGA_FIELD      20
#define COLONNA_FIELD   10
#define BLOCK_SIZE      12          // Dimensione in pixel di un singolo blocco
#define FIELD_X_START   20          // Posizione X inizio griglia su LCD (Offset Sinistro)
#define FIELD_Y_START   30          // Posizione Y inizio griglia su LCD (Offset Alto)

// --- Colori (RGB565) ---
// Definiamo i colori se non presenti in GLCD.h
#ifndef Black
#define Black           0x0000      
#endif
#ifndef White
#define White           0xFFFF      
#endif
#ifndef Red
#define Red             0xF800
#endif
#ifndef Green
#define Green           0x07E0
#endif
#ifndef Blue
#define Blue            0x001F
#endif
#ifndef Yellow
#define Yellow          0xFFE0
#endif
#ifndef Magenta
#define Magenta         0xF81F
#endif
#ifndef Cyan
#define Cyan            0x07FF
#endif
#define Orange          0xFD20      // Colore per L-Shape
#define Grey            0x8410      // Colore per High Score / Testi secondari

// Colori Logici
#define COLOR_EMPTY     Black       // Sfondo del campo (vuoto)
#define COLOR_GRID      White       // Colore bordi campo

// --- Strutture Dati (SPECIFICA 3) ---

// Punto relativo (coordinate rispetto al centro del pezzo)
typedef struct {
    int8_t x;
    int8_t y;
} Point;

// Struttura Tetromino
typedef struct {
    Point blocks[4];    // I 4 blocchi che formano il pezzo
    uint16_t color;     // Colore del pezzo
} Tetromino;

// Stati della Macchina a Stati del Gioco (SPECIFICA 2)
typedef enum {
    GAME_RESET,     // Stato iniziale: punteggi 0, griglia vuota, attesa KEY1
    GAME_PLAYING,   // Gioco attivo
    GAME_PAUSED,    // Gioco in pausa
    GAME_OVER       // Partita finita (Attesa KEY1 per restart)
} GameState;

// --- Variabili Globali Esterne (condivise tra i moduli) ---

// Matrice logica del campo: 0 = vuoto, Altro = Colore occupato
extern uint16_t board[RIGA_FIELD][COLONNA_FIELD];

// Stato del gioco e Punteggi
extern volatile GameState currentGameState;
extern int score;           // Punteggio corrente (Spec. 9)
extern int highScore;       // Record (Spec. 11)
extern int linesCleared;    // Linee completate

// Pezzo Corrente
extern Tetromino currentPiece;
extern int currentX;        // Posizione X del pivot nel campo (0..9)
extern int currentY;        // Posizione Y del pivot nel campo (0..19)

// Array dei 7 Tetromini base
extern const Tetromino TETROMINOS[7];

// --- Prototipi Funzioni (Implementazione in game.c) ---

// Gestione Ciclo di Vita
void Game_Init(void);           // Inizializzazione grafica statica e variabili
void Game_Reset(void);          // Resetta per nuova partita (Punteggi a 0, Griglia vuota) (Spec. 2)
void Game_Over(void);           // Gestisce la fine partita (Salvataggio High Score) (Spec. 10, 11)

// Logica di Gioco Core
void SpawnNewPiece(void);       // Genera nuovo pezzo casuale (Spec. 4)
void RotatePiece(void);         // Ruota pezzo corrente (Spec. 5)
void MovePieceLeft(void);       // Muove a sinistra (Spec. 5)
void MovePieceRight(void);      // Muove a destra (Spec. 5)
void MovePieceDown(void);       // Movimento verso il basso (Timer/Soft Drop) (Spec. 5)
void HardDrop(void);            // Caduta istantanea (KEY2) (Spec. 7)

// Grafica
void DrawGridOutline(void);     // Disegna contorno campo
void DrawBlock(int r, int c, uint16_t color); // Disegna singolo blocco (o cancella se Black)
void DrawPiece(Tetromino p, int r, int c);    // Disegna un pezzo intero
void RefreshScore(void);        // Aggiorna punteggi a video (Score, Lines, High Score)

#endif