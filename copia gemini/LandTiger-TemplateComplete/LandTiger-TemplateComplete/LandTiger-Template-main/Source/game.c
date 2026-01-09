/*----------------------------------------------------------------------------
 * Name:    game.c
 * Purpose: Logica principale del Tetris (Movimento, Collisioni, Punteggio)
 *----------------------------------------------------------------------------*/

#include "LPC17xx.h"
#include "game.h"
#include "GLCD/GLCD.h"
#include <stdio.h>
#include <string.h>

// --- VARIABILI GLOBALI ---

uint16_t board[RIGA_FIELD][COLONNA_FIELD];
volatile GameState currentGameState = GAME_RESET;

int score = 0;
int highScore = 0;
int linesCleared = 0;

Tetromino currentPiece;
int currentX = 0;
int currentY = 0;

// Definizione dei 7 Tetromini (Specifica 3)
// Coordinate relative al centro (0,0)
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

// --- PROTOTIPI FUNZIONI PRIVATE ---
static bool CheckCollision(int px, int py, Tetromino p);
static void LockPiece(void);
static void RemoveLines(void);
static void UpdateHighscore(void);

// --- FUNZIONI GRAFICHE BASE ---

void DrawBlock(int r, int c, uint16_t color) {
    int x_pixel = FIELD_X_START + (c * BLOCK_SIZE);
    int y_pixel = FIELD_Y_START + (r * BLOCK_SIZE);
    int i;
    
    // Safety Check
    if (r < 0 || r >= RIGA_FIELD || c < 0 || c >= COLONNA_FIELD) return;

    for (i = 0; i < BLOCK_SIZE; i++) {
        LCD_DrawLine(x_pixel, y_pixel + i, x_pixel + BLOCK_SIZE - 1, y_pixel + i, color);
    }
    
    // Disegna bordo (nero se il blocco è pieno, o colore sfondo se cancelliamo)
    uint16_t borderColor = (color == COLOR_EMPTY) ? COLOR_EMPTY : Black;
    
    LCD_DrawLine(x_pixel, y_pixel, x_pixel + BLOCK_SIZE - 1, y_pixel, borderColor); 
    LCD_DrawLine(x_pixel, y_pixel + BLOCK_SIZE - 1, x_pixel + BLOCK_SIZE - 1, y_pixel + BLOCK_SIZE - 1, borderColor); 
    LCD_DrawLine(x_pixel, y_pixel, x_pixel, y_pixel + BLOCK_SIZE - 1, borderColor); 
    LCD_DrawLine(x_pixel + BLOCK_SIZE - 1, y_pixel, x_pixel + BLOCK_SIZE - 1, y_pixel + BLOCK_SIZE - 1, borderColor); 
}

void DrawGridOutline(void) {
    int width_pix = COLONNA_FIELD * BLOCK_SIZE;
    int height_pix = RIGA_FIELD * BLOCK_SIZE;
    
    // Linee bianche attorno al campo
    LCD_DrawLine(FIELD_X_START - 1, FIELD_Y_START - 1, FIELD_X_START - 1, FIELD_Y_START + height_pix, COLOR_GRID);
    LCD_DrawLine(FIELD_X_START + width_pix, FIELD_Y_START - 1, FIELD_X_START + width_pix, FIELD_Y_START + height_pix, COLOR_GRID);
    LCD_DrawLine(FIELD_X_START - 1, FIELD_Y_START + height_pix, FIELD_X_START + width_pix, FIELD_Y_START + height_pix, COLOR_GRID);
    
    // Interfaccia Testuale Statica
    GUI_Text(160, 40, (uint8_t *)"NEXT", White, Black); // Placeholder per Next Piece (opzionale)
    GUI_Text(160, 100, (uint8_t *)"SCORE", White, Black);
    GUI_Text(160, 160, (uint8_t *)"LINES", White, Black);
    GUI_Text(160, 220, (uint8_t *)"HI-SC", White, Black);
}

void DrawPiece(Tetromino p, int r, int c) {
    int i;
    for (i = 0; i < 4; i++) {
        int absR = r + p.blocks[i].y;
        int absC = c + p.blocks[i].x;
        if (absR >= 0) { // Disegna solo se dentro o sotto la top-line visibile
            DrawBlock(absR, absC, p.color);
        }
    }
}

void RefreshScore(void) {
    char buffer[20];
    
    sprintf(buffer, "%06d", score);
    GUI_Text(160, 120, (uint8_t *)buffer, White, Black);
    
    sprintf(buffer, "%03d", linesCleared);
    GUI_Text(160, 180, (uint8_t *)buffer, White, Black);
    
    sprintf(buffer, "%06d", highScore);
    GUI_Text(160, 240, (uint8_t *)buffer, Grey, Black);
}

// --- LOGICA DI GIOCO ---

// Inizializza la grafica all'avvio
void Game_Init(void) {
    DrawGridOutline();
    RefreshScore();
}

// Specifica 2: Reset del gioco
void Game_Reset(void) {
    int i, j;
    // Pulisci matrice logica e schermo
    for (i = 0; i < RIGA_FIELD; i++) {
        for (j = 0; j < COLONNA_FIELD; j++) {
            board[i][j] = 0;
            DrawBlock(i, j, COLOR_EMPTY);
        }
    }
    
    score = 0;
    linesCleared = 0;
    // HighScore NON si resetta (Spec 11)
    
    RefreshScore();
}

// Specifica 4: Generazione nuovo pezzo
void SpawnNewPiece(void) {
    int type = rand() % 7;
    currentPiece = TETROMINOS[type];
    
    currentX = 4; // Centro (Colonna 4)
    currentY = 0; // In cima
    
    // Specifica 10: Game Over se il nuovo pezzo collide subito
    if (CheckCollision(currentX, currentY, currentPiece)) {
        // Se collide appena nato -> Perso!
        Game_Over();
    } else {
        DrawPiece(currentPiece, currentY, currentX);
    }
}

// Controllo collisioni (bordi e blocchi esistenti)
static bool CheckCollision(int px, int py, Tetromino p) {
    int i;
    for (i = 0; i < 4; i++) {
        int absX = px + p.blocks[i].x;
        int absY = py + p.blocks[i].y;
        
        // 1. Collisione con i muri laterali e fondo
        if (absX < 0 || absX >= COLONNA_FIELD || absY >= RIGA_FIELD) {
            return true;
        }
        
        // 2. Collisione con blocchi già piazzati (solo se Y >= 0)
        if (absY >= 0 && board[absY][absX] != 0) {
            return true;
        }
    }
    return false;
}

// Specifica 5: Movimento Sinistra
void MovePieceLeft(void) {
    if (currentGameState != GAME_PLAYING) return;
    
    // Cancella
    Tetromino empty = currentPiece;
    empty.color = COLOR_EMPTY;
    DrawPiece(empty, currentY, currentX);
    
    if (!CheckCollision(currentX - 1, currentY, currentPiece)) {
        currentX--;
    }
    
    // Ridisegna
    DrawPiece(currentPiece, currentY, currentX);
}

// Specifica 5: Movimento Destra
void MovePieceRight(void) {
    if (currentGameState != GAME_PLAYING) return;
    
    // Cancella
    Tetromino empty = currentPiece;
    empty.color = COLOR_EMPTY;
    DrawPiece(empty, currentY, currentX);
    
    if (!CheckCollision(currentX + 1, currentY, currentPiece)) {
        currentX++;
    }
    
    // Ridisegna
    DrawPiece(currentPiece, currentY, currentX);
}

// Specifica 5: Rotazione (90 gradi orario)
void RotatePiece(void) {
    if (currentGameState != GAME_PLAYING) return;
    
    Tetromino rotated = currentPiece;
    int i;
    
    // Rotazione matematica: x' = -y, y' = x
    for (i = 0; i < 4; i++) {
        int oldX = currentPiece.blocks[i].x;
        int oldY = currentPiece.blocks[i].y;
        rotated.blocks[i].x = -oldY;
        rotated.blocks[i].y = oldX;
    }
    
    // Cancella vecchio
    Tetromino empty = currentPiece;
    empty.color = COLOR_EMPTY;
    DrawPiece(empty, currentY, currentX);
    
    // Controlla se il pezzo ruotato è valido
    if (!CheckCollision(currentX, currentY, rotated)) {
        currentPiece = rotated; // Applica rotazione
    }
    // Wall Kicks (Opzionale ma utile): Se ruotando esci, prova a spostarti di 1
    else if (!CheckCollision(currentX - 1, currentY, rotated)) {
        currentX--;
        currentPiece = rotated;
    }
    else if (!CheckCollision(currentX + 1, currentY, rotated)) {
        currentX++;
        currentPiece = rotated;
    }
    
    // Ridisegna
    DrawPiece(currentPiece, currentY, currentX);
}

// Specifica 5: Discesa (chiamata dal Timer o Soft Drop)
void MovePieceDown(void) {
    if (currentGameState != GAME_PLAYING) return;
    
    // Cancella vecchio
    Tetromino empty = currentPiece;
    empty.color = COLOR_EMPTY;
    DrawPiece(empty, currentY, currentX);
    
    if (!CheckCollision(currentX, currentY + 1, currentPiece)) {
        currentY++;
        DrawPiece(currentPiece, currentY, currentX);
    } else {
        // Toccato il fondo o un altro pezzo
        DrawPiece(currentPiece, currentY, currentX); // Ridisegna per sicurezza
        LockPiece(); // Fissa il pezzo
    }
}

// Specifica 7: Hard Drop
void HardDrop(void) {
    if (currentGameState != GAME_PLAYING) return;
    
    // Cancella posizione corrente
    Tetromino empty = currentPiece;
    empty.color = COLOR_EMPTY;
    DrawPiece(empty, currentY, currentX);
    
    // Scendi finché non collidi
    while (!CheckCollision(currentX, currentY + 1, currentPiece)) {
        currentY++;
    }
    
    // Disegna e Fissa
    DrawPiece(currentPiece, currentY, currentX);
    LockPiece();
}

// Fissa il pezzo nella griglia board[][]
static void LockPiece(void) {
    int i;
    for (i = 0; i < 4; i++) {
        int absX = currentX + currentPiece.blocks[i].x;
        int absY = currentY + currentPiece.blocks[i].y;
        
        // Specifica 10: Se un blocco si ferma sopra il campo (Y < 0), Game Over
        if (absY < 0) {
            Game_Over();
            return;
        }
        
        if (absY < RIGA_FIELD && absX >= 0 && absX < COLONNA_FIELD) {
            board[absY][absX] = currentPiece.color;
        }
    }
    
    // Specifica 9: Punti per piazzamento (+10)
    score += 10;
    
    RemoveLines();    // Specifica 8
    RefreshScore();   // Aggiorna GUI
    SpawnNewPiece();  // Specifica 4
}

// Specifica 8: Cancellazione Linee e Specifica 9: Punteggio
static void RemoveLines(void) {
    int linesRemovedThisTurn = 0;
    int r, c, k;
    
    // Scansiona dall'alto verso il basso (o viceversa)
    for (r = RIGA_FIELD - 1; r >= 0; r--) {
        bool full = true;
        for (c = 0; c < COLONNA_FIELD; c++) {
            if (board[r][c] == 0) {
                full = false;
                break;
            }
        }
        
        if (full) {
            linesRemovedThisTurn++;
            
            // Sposta tutto giù di una riga
            for (k = r; k > 0; k--) {
                for (c = 0; c < COLONNA_FIELD; c++) {
                    board[k][c] = board[k-1][c];
                    // Aggiorna grafica riga per riga
                    DrawBlock(k, c, board[k][c]);
                }
            }
            // Pulisci riga 0
            for (c = 0; c < COLONNA_FIELD; c++) {
                board[0][c] = 0;
                DrawBlock(0, c, COLOR_EMPTY);
            }
            
            // Dato che abbiamo spostato giù, dobbiamo ricontrollare la riga corrente 'r'
            // (che ora contiene quella che era sopra)
            r++; 
        }
    }
    
    // Calcolo Punteggio (Spec 9)
    if (linesRemovedThisTurn > 0) {
        linesCleared += linesRemovedThisTurn;
        if (linesRemovedThisTurn == 4) {
            score += 600; // Tetris!
        } else {
            score += (100 * linesRemovedThisTurn);
        }
    }
}

// Specifica 10 e 11: Game Over
void Game_Over(void) {
    currentGameState = GAME_OVER;
    UpdateHighscore();
    GUI_Text(90, 140, (uint8_t *)"GAME OVER", Red, Yellow);
}

void UpdateHighscore(void) {
    if (score > highScore) {
        highScore = score;
    }
    RefreshScore();
}