#include <iostream>
#include <random>
#include <chrono>
#include "BoopGame.h"
#include "Agent.h"

using namespace std;
using namespace boop;

int main()
{
    // Obtener instancia singleton del juego
    Boop *game = Boop::getInstance();

    // Randomly populate board with Gatitos of both players or empty
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dist({20, 20, 60}); // weights 0.2,0.2,0.6

    for (int r = 0; r < game->board.size; ++r)
    {
        for (int c = 0; c < game->board.size; ++c)
        {
            int pick = dist(gen);
            if (pick == 0)
            {
                // place Gatito for player1
                if (game->board.isEmpty(r, c))
                {
                    Piece *p = new Gatito({r, c}, &game->player1);
                    game->board.placePiece(p, r, c);
                }
            }
            else if (pick == 1)
            {
                if (game->board.isEmpty(r, c))
                {
                    Piece *p = new Gatito({r, c}, &game->player2);
                    game->board.placePiece(p, r, c);
                }
            }
            else
            {
                // leave empty
            }
        }
    }

    game->displayGameState();

    cout << "\n========== EVALUACIÓN SIMPLE (Heurística) ==========\n";
    auto bestMoves = getBestMoves(PieceType::GATITO, &game->player1, &game->player2);

    cout << "Top 5 moves por heurística simple:\n";
    int limit = min(5, (int)bestMoves.size());
    for (int i = 0; i < limit; ++i)
    {
        auto [score, r, c] = bestMoves[i];
        cout << "  " << (i + 1) << ". Row=" << r << " Col=" << c << " Score=" << score << "\n";
    }

    // Crear agente BoopAgent
    BoopAgent agent(&game->player1, &game->player2);

    cout << "\n========== MINIMAX CON ALPHA-BETA Y TRANSPOSICIÓN ==========\n";
    cout << "Ejecutando búsqueda minimax con profundidad 4...\n";

    auto start = chrono::high_resolution_clock::now();

    int row, col, score;
    PieceType type;
    tie(row, col, type, score) = agent.findBestMove(4);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "\n✓ Minimax completado exitosamente!\n";
    cout << "Mejor movimiento encontrado:\n";
    cout << "  Posición: (" << row << ", " << col << ")\n";
    cout << "  Tipo: " << (type == PieceType::GATITO ? "Gatito" : "Gato") << "\n";
    cout << "  Score: " << score << "\n";
    cout << "  Nodos explorados: " << agent.getNodesExplored() << "\n";
    cout << "  Tiempo: " << duration.count() << " ms\n";

    return 0;
}
