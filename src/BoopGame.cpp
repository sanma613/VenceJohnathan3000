#include "BoopGame.h"

// Inicialización de instancia estática del singleton
Boop *Boop::instance = nullptr;

// Implementación de getInstance
Boop *Boop::getInstance()
{
    if (instance == nullptr)
    {
        instance = new Boop();
    }
    return instance;
}

// Implementación de reset
void Boop::reset()
{
    // Limpiar el tablero
    for (int i = 0; i < board.size; i++)
    {
        for (int j = 0; j < board.size; j++)
        {
            delete board.grid[i][j];
            board.grid[i][j] = nullptr;
        }
    }

    // Resetear jugadores
    player1.gatitos_disponibles = 8;
    player1.gatos_disponibles = 0;
    player2.gatitos_disponibles = 8;
    player2.gatos_disponibles = 0;

    // Resetear estado del juego
    currentPlayer = &player1;
    gameOver = false;
    winner = nullptr;
}
