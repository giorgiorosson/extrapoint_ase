#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "game.h"

#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "game.h"

// Definizione Variabili Globali
uint16_t board[RIGA_FIELD][COLONNA_FIELD] = {0};
volatile GameState currentGameState = GAME_RESET;

// Variabili gioco
Tetromino currentPiece;
int currentX = 0;
int currentY = 0;
int score = 0;
int highScore = 0;
int linesCleared = 0;

// Definizione dei 7 Tetromini (Spec. 3)
const Tetromino TETROMINOS[7] = {
    // I-Shape (Cyan)
    { { {-1, 0}, {0, 0}, {1, 0}, {2, 0} }, Cyan },
    // O-Shape (Yellow)
    { { {0, 0}, {1, 0}, {0, 1}, {1, 1} }, Yellow },
    // T-Shape (Magenta)
    { { {-1, 0}, {0, 0}, {1, 0}, {0, -1} }, Magenta },
    // J-Shape (Blue)
    { { {-1, -1}, {-1, 0}, {0, 0}, {1, 0} }, Blue },
    // L-Shape (Orange)
    { { {-1, 0}, {0, 0}, {1, 0}, {1, -1} }, Orange },
    // S-Shape (Green)
    { { {-1, 1}, {0, 1}, {0, 0}, {1, 0} }, Green },
    // Z-Shape (Red)
    { { {-1, 0}, {0, 0}, {0, 1}, {1, 1} }, Red }
};

void DrawBlock(int riga, int colonna, uint16_t colore) {
    int x_pixel = FIELD_X_START + (colonna * BLOCK_SIZE);
    int y_pixel = FIELD_Y_START + (riga * BLOCK_SIZE);
    int i;
    
    // Controlla limiti per sicurezza
    if(riga < 0 || riga >= RIGA_FIELD || colonna < 0 || colonna >= COLONNA_FIELD) return;

    for (i = 0; i < BLOCK_SIZE; i++) {
        LCD_DrawLine(x_pixel, y_pixel + i, x_pixel + BLOCK_SIZE - 1, y_pixel + i, colore);
    }
    // Bordo
    LCD_DrawLine(x_pixel, y_pixel, x_pixel + BLOCK_SIZE - 1, y_pixel, Black); 
    LCD_DrawLine(x_pixel, y_pixel + BLOCK_SIZE - 1, x_pixel + BLOCK_SIZE - 1, y_pixel + BLOCK_SIZE - 1, Black); 
    LCD_DrawLine(x_pixel, y_pixel, x_pixel, y_pixel + BLOCK_SIZE - 1, Black); 
    LCD_DrawLine(x_pixel + BLOCK_SIZE - 1, y_pixel, x_pixel + BLOCK_SIZE - 1, y_pixel + BLOCK_SIZE - 1, Black); 
}

void DrawGridOutline() {
    int width_pixel = COLONNA_FIELD * BLOCK_SIZE;
    int height_pixel = RIGA_FIELD * BLOCK_SIZE;
    LCD_DrawLine(FIELD_X_START - 1, FIELD_Y_START - 1, FIELD_X_START - 1, FIELD_Y_START + height_pixel, COLOR_GRID);
    LCD_DrawLine(FIELD_X_START + width_pixel, FIELD_Y_START - 1, FIELD_X_START + width_pixel, FIELD_Y_START + height_pixel, COLOR_GRID);
    LCD_DrawLine(FIELD_X_START - 1, FIELD_Y_START + height_pixel, FIELD_X_START + width_pixel, FIELD_Y_START + height_pixel, COLOR_GRID);
}

// Funzione per disegnare un pezzo intero in una posizione specifica
void DrawPiece(Tetromino p, int riga, int colonna) {
    int i;
    for(i=0; i<4; i++) {
        int absRow = riga + p.blocks[i].y;
        int absCol = colonna + p.blocks[i].x;
        
        // Disegna solo se è dentro i limiti visibili
        if (absRow >= 0 && absRow < RIGA_FIELD && absCol >= 0 && absCol < COLONNA_FIELD) {
            DrawBlock(absRow, absCol, p.color);
        }
    }
}

void ClearBoard(void) {
    int i, j;
    for(i=0; i<RIGA_FIELD; i++) {
        for(j=0; j<COLONNA_FIELD; j++) {
            board[i][j] = 0; // Pulisce memoria
            DrawBlock(i, j, COLOR_EMPTY); // Pulisce schermo (nero)
        }
    }
}

void ResetGame(void) {
    // Spec 2: Game starts with empty grid, score 0, lines 0
    ClearBoard();
    score = 0;
    linesCleared = 0;
    
    // NOTA: High Score non viene resettato qui (Spec 11), solo all'avvio scheda è 0
    
    // Aggiorna interfaccia punteggi (implementerai GUI_Text più avanti)
    // GUI_Text(..., "Score: 0");
}

// Genera un nuovo pezzo (Specifica 4)
void SpawnNewPiece() {
    int type = rand() % 7; // Numero casuale tra 0 e 6
    currentPiece = TETROMINOS[type];
    
    currentX = 4; // Centro orizzontale (colonne 0-9, centro è 4 o 5)
    currentY = 0; // In cima
    
    DrawPiece(currentPiece, currentY, currentX);
}