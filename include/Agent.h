#pragma once

#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>

#include "BoopGame.h"

using namespace std;

namespace boop
{
    // Constantes de recompensa para heurística de evaluación
    const int REWARD_BOOP_OPPONENT_OUT_OF_BOUNDS = 2;
    const int REWARD_BOOP_ME_OUT_OF_BOUNDS = -1;
    const int REWARD_BOOP_NORMAL = 1;
    const int REWARD_BOOP_OPPONENT_TO_ALIGNMENT = -8;
    const int REWARD_BOOP_ME_TO_ALIGNMENT = 5;

    // Constantes para minimax
    const int INF = 1000000;
    const int WIN_SCORE = 10000;
    const int LOSE_SCORE = -10000;

    // Estructura para guardar estado del tablero (backtracking)
    struct BoardState
    {
        vector<vector<Piece *>> grid;
        int p1_gatitos;
        int p1_gatos;
        int p2_gatitos;
        int p2_gatos;
        Player *currentPlayer;
        vector<Piece *> createdPieces; // Piezas creadas durante este movimiento

        BoardState() = default;
    };

    // Estructura para almacenar en tabla de transposición
    struct TranspositionEntry
    {
        int depth;
        int score;
        int alpha;
        int beta;
        enum class Flag
        {
            EXACT,
            LOWER_BOUND,
            UPPER_BOUND
        } flag;
    };

    // Clase principal del agente con minimax y tabla de transposición
    class BoopAgent
    {
    private:
        Player *me;
        Player *opponent;
        unordered_map<string, TranspositionEntry> transpositionTable;
        int nodesExplored;

        // Guarda estado actual del tablero
        BoardState saveState();

        // Restaura estado del tablero (modifica state para limpiar createdPieces)
        void restoreState(BoardState &state);

        // Genera hash del estado del tablero para tabla de transposición
        string hashBoard();

        // Evalúa el estado actual del tablero (función heurística)
        int evaluateBoard();

        // Minimax con alpha-beta pruning
        int minimax(int depth, int alpha, int beta, bool maximizing);

        // Obtiene movimientos legales ordenados por heurística
        vector<tuple<int, int, int, PieceType>> getOrderedMoves(bool forMe);

        // Ejecuta un movimiento y retorna las piezas removidas (para backtracking)
        // Si se pasa un BoardState, registra la pieza creada para eliminarla después
        vector<Piece *> makeMove(int row, int col, PieceType type, Player *player, BoardState *state = nullptr);

        // Deshace un movimiento
        void undoMove(int row, int col, const vector<Piece *> &removedPieces);

    public:
        BoopAgent(Player *me, Player *opponent);

        // Encuentra el mejor movimiento usando minimax con profundidad configurable
        tuple<int, int, PieceType, int> findBestMove(int maxDepth = 4);

        // Limpia la tabla de transposición
        void clearCache();

        // Obtiene estadísticas
        int getNodesExplored() const { return nodesExplored; }
        size_t getCacheSize() const { return transpositionTable.size(); }
    };

    // Funciones de utilidad legacy (compatibilidad)
    int evaluatePosition(int row, int col, PieceType placingType, Player *me, Player *opponent);
    vector<tuple<int, int, int>> getBestMoves(PieceType placingType, Player *me, Player *opponent);
    bool checkAlignment(int row, int col, Player *player, PieceType pieceType, int count);

} // namespace boop
