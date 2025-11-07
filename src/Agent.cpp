#include "Agent.h"
#include <algorithm>
#include <sstream>
#include <cmath>

namespace boop
{
    // ============== Funciones de utilidad legacy ==============

    bool checkAlignment(int row, int col, Player *player, PieceType pieceType, int count)
    {
        Boop *game = Boop::getInstance();
        Board &board = game->board;
        int n = board.size;

        int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

        for (int d = 0; d < 4; ++d)
        {
            int dr = directions[d][0];
            int dc = directions[d][1];
            int total = 1;

            int r = row + dr, c = col + dc;
            while (r >= 0 && r < n && c >= 0 && c < n)
            {
                Piece *p = board.getPiece(r, c);
                if (p && p->player == player && p->piece_type == pieceType)
                {
                    ++total;
                }
                else
                    break;
                r += dr;
                c += dc;
            }

            r = row - dr;
            c = col - dc;
            while (r >= 0 && r < n && c >= 0 && c < n)
            {
                Piece *p = board.getPiece(r, c);
                if (p && p->player == player && p->piece_type == pieceType)
                {
                    ++total;
                }
                else
                    break;
                r -= dr;
                c -= dc;
            }

            if (total >= count)
                return true;
        }
        return false;
    }

    int evaluatePosition(int row, int col, PieceType placingType, Player *me, Player *opponent)
    {
        Boop *game = Boop::getInstance();
        Board &board = game->board;
        int n = board.size;

        int options[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
        int boop_score = 0;

        for (int k = 0; k < 8; ++k)
        {
            int dr = options[k][0];
            int dc = options[k][1];
            int ni = row + dr;
            int nj = col + dc;

            if (ni >= 0 && ni < n && nj >= 0 && nj < n)
            {
                Piece *neighbor = board.getPiece(ni, nj);
                if (!neighbor)
                    continue;

                if (placingType == PieceType::GATITO && neighbor->piece_type == PieceType::GATO)
                    continue;

                int dest_row = ni + dr;
                int dest_col = nj + dc;

                if (!(dest_row >= 0 && dest_row < n && dest_col >= 0 && dest_col < n))
                {
                    if (neighbor->player == opponent)
                    {
                        boop_score += REWARD_BOOP_OPPONENT_OUT_OF_BOUNDS;
                    }
                    else
                    {
                        boop_score += REWARD_BOOP_ME_OUT_OF_BOUNDS;
                    }
                    continue;
                }

                if (board.getPiece(dest_row, dest_col) != nullptr)
                    continue;

                bool is_aligned = checkAlignment(dest_row, dest_col, neighbor->player, neighbor->piece_type, 3);

                if (is_aligned && neighbor->player == me)
                {
                    boop_score += REWARD_BOOP_ME_TO_ALIGNMENT;
                }
                else if (is_aligned && neighbor->player == opponent)
                {
                    boop_score += REWARD_BOOP_OPPONENT_TO_ALIGNMENT;
                }
                else
                {
                    boop_score += REWARD_BOOP_NORMAL;
                }
            }
        }
        return boop_score;
    }

    vector<tuple<int, int, int>> getBestMoves(PieceType placingType, Player *me, Player *opponent)
    {
        Boop *game = Boop::getInstance();
        Board &board = game->board;
        int n = board.size;

        vector<tuple<int, int, int>> result;

        for (int row = 0; row < n; ++row)
        {
            for (int col = 0; col < n; ++col)
            {
                if (board.isEmpty(row, col))
                {
                    int score = evaluatePosition(row, col, placingType, me, opponent);
                    result.emplace_back(score, row, col);
                }
            }
        }

        sort(result.begin(), result.end(), [](const tuple<int, int, int> &a, const tuple<int, int, int> &b)
             { return get<0>(a) > get<0>(b); });

        return result;
    }

    // ============== Implementación de BoopAgent ==============

    BoopAgent::BoopAgent(Player *me, Player *opponent)
        : me(me), opponent(opponent), nodesExplored(0)
    {
    }

    BoardState BoopAgent::saveState()
    {
        Boop *game = Boop::getInstance();
        BoardState state;

        // Copiar grid (solo referencias)
        state.grid.resize(game->board.size);
        for (int i = 0; i < game->board.size; i++)
        {
            state.grid[i].resize(game->board.size);
            for (int j = 0; j < game->board.size; j++)
            {
                state.grid[i][j] = game->board.grid[i][j];
            }
        }

        // Copiar recursos de jugadores
        state.p1_gatitos = game->player1.gatitos_disponibles;
        state.p1_gatos = game->player1.gatos_disponibles;
        state.p2_gatitos = game->player2.gatitos_disponibles;
        state.p2_gatos = game->player2.gatos_disponibles;
        state.currentPlayer = game->currentPlayer;

        return state;
    }

    void BoopAgent::restoreState(BoardState &state)
    {
        Boop *game = Boop::getInstance();

        // Primero restaurar grid con las referencias originales
        for (int i = 0; i < game->board.size; i++)
        {
            for (int j = 0; j < game->board.size; j++)
            {
                game->board.grid[i][j] = state.grid[i][j];
            }
        }

        // Eliminar todas las piezas creadas durante este movimiento
        // Como restauramos el grid ANTES, estas piezas ya no están en el tablero
        for (Piece *p : state.createdPieces)
        {
            if (p != nullptr)
            {
                delete p;
            }
        }
        state.createdPieces.clear(); // Limpiar el vector después de eliminar

        // Restaurar recursos
        game->player1.gatitos_disponibles = state.p1_gatitos;
        game->player1.gatos_disponibles = state.p1_gatos;
        game->player2.gatitos_disponibles = state.p2_gatitos;
        game->player2.gatos_disponibles = state.p2_gatos;
        game->currentPlayer = state.currentPlayer;
    }

    string BoopAgent::hashBoard()
    {
        Boop *game = Boop::getInstance();
        stringstream ss;

        for (int i = 0; i < game->board.size; i++)
        {
            for (int j = 0; j < game->board.size; j++)
            {
                Piece *p = game->board.getPiece(i, j);
                if (!p)
                {
                    ss << ".";
                }
                else
                {
                    char c = p->getSymbol();
                    ss << c;
                }
            }
        }

        // Agregar información de recursos
        ss << "|" << game->player1.gatitos_disponibles << "," << game->player1.gatos_disponibles;
        ss << "|" << game->player2.gatitos_disponibles << "," << game->player2.gatos_disponibles;
        ss << "|" << (game->currentPlayer == &game->player1 ? "1" : "2");

        return ss.str();
    }

    int BoopAgent::evaluateBoard()
    {
        Boop *game = Boop::getInstance();
        int score = 0;

        // Bonus por piezas en el tablero
        int myPieces = 0, oppPieces = 0;
        for (int i = 0; i < game->board.size; i++)
        {
            for (int j = 0; j < game->board.size; j++)
            {
                Piece *p = game->board.getPiece(i, j);
                if (p)
                {
                    if (p->player == me)
                    {
                        myPieces++;
                        // Bonus por piezas centrales
                        int distCenter = abs(i - 2) + abs(j - 2);
                        score += (6 - distCenter) * 2;

                        // Bonus extra por gatos (más valiosos)
                        if (p->piece_type == PieceType::GATO)
                            score += 10;
                    }
                    else
                    {
                        oppPieces++;
                        int distCenter = abs(i - 2) + abs(j - 2);
                        score -= (6 - distCenter) * 2;
                        if (p->piece_type == PieceType::GATO)
                            score -= 10;
                    }
                }
            }
        }

        // Penalización por diferencia de piezas
        score += (myPieces - oppPieces) * 15;

        // Bonus por recursos disponibles
        score += me->gatitos_disponibles * 5;
        score += me->gatos_disponibles * 20;
        score -= opponent->gatitos_disponibles * 5;
        score -= opponent->gatos_disponibles * 20;

        // Detectar alineaciones de 2 (amenazas)
        for (int i = 0; i < game->board.size; i++)
        {
            for (int j = 0; j < game->board.size; j++)
            {
                Piece *p = game->board.getPiece(i, j);
                if (p)
                {
                    if (checkAlignment(i, j, p->player, p->piece_type, 2))
                    {
                        if (p->player == me)
                            score += 30; // Amenaza de victoria
                        else
                            score -= 30; // Amenaza enemiga
                    }
                }
            }
        }

        return score;
    }

    vector<tuple<int, int, int, PieceType>> BoopAgent::getOrderedMoves(bool forMe)
    {
        Boop *game = Boop::getInstance();
        Player *player = forMe ? me : opponent;
        vector<tuple<int, int, int, PieceType>> moves;

        // Generar movimientos con Gatitos
        if (player->canPlaceGatito())
        {
            for (int i = 0; i < game->board.size; i++)
            {
                for (int j = 0; j < game->board.size; j++)
                {
                    if (game->board.isEmpty(i, j))
                    {
                        int score = evaluatePosition(i, j, PieceType::GATITO, me, opponent);
                        moves.emplace_back(score, i, j, PieceType::GATITO);
                    }
                }
            }
        }

        // Generar movimientos con Gatos
        if (player->canPlaceGato())
        {
            for (int i = 0; i < game->board.size; i++)
            {
                for (int j = 0; j < game->board.size; j++)
                {
                    if (game->board.isEmpty(i, j))
                    {
                        int score = evaluatePosition(i, j, PieceType::GATO, me, opponent);
                        moves.emplace_back(score + 5, i, j, PieceType::GATO); // Bonus por ser gato
                    }
                }
            }
        }

        // Ordenar por score descendente
        sort(moves.begin(), moves.end(), [](const auto &a, const auto &b)
             { return get<0>(a) > get<0>(b); });

        return moves;
    }

    vector<Piece *> BoopAgent::makeMove(int row, int col, PieceType type, Player *player, BoardState *state)
    {
        Boop *game = Boop::getInstance();

        // Crear y colocar pieza
        Piece *newPiece = nullptr;
        if (type == PieceType::GATITO)
        {
            newPiece = new Gatito({row, col}, player);
            player->useGatito();
        }
        else
        {
            newPiece = new Gato({row, col}, player);
            player->useGato();
        }

        game->board.placePiece(newPiece, row, col);

        // Registrar pieza creada si tenemos un estado
        if (state != nullptr)
        {
            state->createdPieces.push_back(newPiece);
        }

        // Ejecutar boops y retornar piezas eliminadas
        vector<Piece *> boopedOut = game->board.boopPieces(row, col, newPiece);

        // Retornar recursos de piezas eliminadas
        for (Piece *p : boopedOut)
        {
            if (p->piece_type == PieceType::GATITO)
                p->player->returnGatito();
            else
                p->player->returnGato();

            // Si estamos en minimax (state != nullptr), NO eliminar las piezas
            // porque necesitamos poder restaurar el estado original
            // Solo eliminar cuando es un movimiento real (state == nullptr)
            if (state == nullptr)
            {
                delete p;
            }
        }

        return boopedOut; // Retorna piezas expelled (sin eliminar si es minimax)
    }

    void BoopAgent::undoMove(int row, int col, const vector<Piece *> &removedPieces)
    {
        (void)removedPieces; // Suprimir warning

        // No hacemos nada aquí porque usamos saveState/restoreState
        // que restaura TODO el estado del tablero completo
        // La pieza colocada será manejada por restoreState
        (void)row;
        (void)col;
    }

    int BoopAgent::minimax(int depth, int alpha, int beta, bool maximizing)
    {
        nodesExplored++;
        Boop *game = Boop::getInstance();

        // Consultar tabla de transposición
        string hash = hashBoard();
        auto it = transpositionTable.find(hash);
        if (it != transpositionTable.end() && it->second.depth >= depth)
        {
            TranspositionEntry &entry = it->second;
            if (entry.flag == TranspositionEntry::Flag::EXACT)
                return entry.score;
            if (entry.flag == TranspositionEntry::Flag::LOWER_BOUND && entry.score > alpha)
                alpha = entry.score;
            if (entry.flag == TranspositionEntry::Flag::UPPER_BOUND && entry.score < beta)
                beta = entry.score;
            if (alpha >= beta)
                return entry.score;
        }

        // Condición de término: profundidad 0 o juego terminado
        if (depth == 0)
        {
            return evaluateBoard();
        }

        // Verificar victoria (alguien tiene 3 en línea)
        auto myLines = game->board.findLinesOfThree(me);
        if (!myLines.empty())
            return WIN_SCORE + depth; // Preferir victorias más rápidas

        auto oppLines = game->board.findLinesOfThree(opponent);
        if (!oppLines.empty())
            return LOSE_SCORE - depth;

        // Obtener movimientos ordenados
        auto moves = getOrderedMoves(maximizing);

        // Si no hay movimientos, retornar evaluación
        if (moves.empty())
        {
            return evaluateBoard();
        }

        int bestScore = maximizing ? -INF : INF;
        TranspositionEntry::Flag flag = TranspositionEntry::Flag::UPPER_BOUND;

        for (const auto &move : moves)
        {
            int score, row, col;
            PieceType type;
            tie(score, row, col, type) = move;

            // Guardar estado
            BoardState state = saveState();

            // Hacer movimiento (pasamos el estado para registrar la pieza creada)
            Player *player = maximizing ? me : opponent;
            vector<Piece *> removed = makeMove(row, col, type, player, &state);

            // Llamada recursiva
            int eval = minimax(depth - 1, alpha, beta, !maximizing);

            // Deshacer movimiento (esto eliminará la pieza creada automáticamente)
            restoreState(state);

            // Las piezas removidas ya fueron eliminadas en makeMove
            // removed está vacío o contiene piezas ya eliminadas

            // Actualizar mejor score y alpha/beta
            if (maximizing)
            {
                if (eval > bestScore)
                {
                    bestScore = eval;
                    flag = TranspositionEntry::Flag::EXACT;
                }
                alpha = max(alpha, bestScore);
            }
            else
            {
                if (eval < bestScore)
                {
                    bestScore = eval;
                    flag = TranspositionEntry::Flag::EXACT;
                }
                beta = min(beta, bestScore);
            }

            // Poda alpha-beta
            if (beta <= alpha)
            {
                flag = maximizing ? TranspositionEntry::Flag::LOWER_BOUND : TranspositionEntry::Flag::UPPER_BOUND;
                break;
            }
        }

        // Guardar en tabla de transposición
        transpositionTable[hash] = {depth, bestScore, alpha, beta, flag};

        return bestScore;
    }

    tuple<int, int, PieceType, int> BoopAgent::findBestMove(int maxDepth)
    {
        nodesExplored = 0;
        int bestScore = -INF;
        int bestRow = -1, bestCol = -1;
        PieceType bestType = PieceType::GATITO;

        auto moves = getOrderedMoves(true);

        for (const auto &move : moves)
        {
            int score, row, col;
            PieceType type;
            tie(score, row, col, type) = move;

            // Guardar estado
            BoardState state = saveState();

            // Hacer movimiento (pasamos el estado para registrar la pieza creada)
            vector<Piece *> removed = makeMove(row, col, type, me, &state);

            // Evaluar con minimax
            int eval = minimax(maxDepth - 1, -INF, INF, false);

            // Deshacer (esto eliminará la pieza creada automáticamente)
            restoreState(state);
            // removed ya está vacío o contiene piezas ya eliminadas

            // Actualizar mejor movimiento
            if (eval > bestScore)
            {
                bestScore = eval;
                bestRow = row;
                bestCol = col;
                bestType = type;
            }
        }

        return make_tuple(bestRow, bestCol, bestType, bestScore);
    }

    void BoopAgent::clearCache()
    {
        transpositionTable.clear();
    }

} // namespace boop
