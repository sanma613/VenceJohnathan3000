#include "BoopGame.h"
#include "Agent.h"
#include <chrono>

using namespace boop;

// Implementación de métodos de Boop que no están inline en el header

bool Boop::placePiece(int row, int col, PieceType pieceType)
{
    if (gameOver)
    {
        std::cout << "El juego ha terminado!" << std::endl;
        return false;
    }

    if (!board.isEmpty(row, col))
    {
        std::cout << "Esa posición ya está ocupada!" << std::endl;
        return false;
    }

    if (pieceType == PieceType::GATITO && !currentPlayer->canPlaceGatito())
    {
        std::cout << "No tienes más gatitos disponibles!" << std::endl;
        return false;
    }
    else if (pieceType == PieceType::GATO && !currentPlayer->canPlaceGato())
    {
        std::cout << "No tienes gatos disponibles!" << std::endl;
        return false;
    }

    Piece *piece;
    if (pieceType == PieceType::GATITO)
    {
        piece = new Gatito({row, col}, currentPlayer);
        currentPlayer->useGatito();
    }
    else
    {
        piece = new Gato({row, col}, currentPlayer);
        currentPlayer->useGato();
    }

    board.placePiece(piece, row, col);

    std::vector<Piece *> boopedOut = board.boopPieces(row, col, piece);

    for (Piece *boopedPiece : boopedOut)
    {
        if (dynamic_cast<Gatito *>(boopedPiece))
        {
            boopedPiece->player->returnGatito();
        }
        else
        {
            boopedPiece->player->returnGato();
        }
        delete boopedPiece;
    }

    checkAndPromoteGatitos();
    checkVictory();

    if (!gameOver)
    {
        switchPlayer();
    }

    return true;
}

void Boop::checkAndPromoteGatitos()
{
    Player *players[] = {&player1, &player2};

    for (Player *player : players)
    {
        std::vector<std::vector<std::pair<int, int>>> lines = board.findLinesOfThree(player);

        for (const auto &line : lines)
        {
            bool allGatitos = true;
            for (const auto &pos : line)
            {
                Piece *piece = board.getPiece(pos.first, pos.second);
                if (!dynamic_cast<Gatito *>(piece))
                {
                    allGatitos = false;
                    break;
                }
            }

            if (allGatitos)
            {
                for (const auto &pos : line)
                {
                    Piece *piece = board.removePiece(pos.first, pos.second);
                    delete piece;
                }
                player->promoteGatitosToGato(3);
                std::cout << "¡" << player->name << " ha graduado a 3 gatitos!" << std::endl;
            }
        }
    }
}

void Boop::checkVictory()
{
    Player *players[] = {&player1, &player2};

    for (Player *player : players)
    {
        std::vector<std::vector<std::pair<int, int>>> lines = board.findLinesOfThree(player);

        for (const auto &line : lines)
        {
            bool allGatos = true;
            for (const auto &pos : line)
            {
                Piece *piece = board.getPiece(pos.first, pos.second);
                if (!dynamic_cast<Gato *>(piece))
                {
                    allGatos = false;
                    break;
                }
            }

            if (allGatos)
            {
                gameOver = true;
                winner = player;
                std::cout << "¡" << player->name << " ha ganado con 3 gatos adultos en línea!" << std::endl;
                return;
            }
        }
    }
}

std::tuple<int, int, PieceType> Boop::getPlayerInput()
{
    while (true)
    {
        std::cout << "\n"
                  << currentPlayer->name << ", es tu turno!" << std::endl;
        std::cout << "Ingresa tu movimiento en formato: fila,columna,tipo" << std::endl;
        std::cout << "Tipo: 'g' para gatito, 'G' para gato adulto" << std::endl;
        std::cout << "Ejemplo: 2,3,g" << std::endl;

        std::cout << "Tu movimiento: ";

        std::string input;
        std::getline(std::cin, input);

        std::stringstream ss(input);
        std::string rowStr, colStr, typeStr;

        if (std::getline(ss, rowStr, ',') && std::getline(ss, colStr, ',') && std::getline(ss, typeStr))
        {
            try
            {
                int row = std::stoi(rowStr);
                int col = std::stoi(colStr);

                // Eliminar espacios
                typeStr.erase(std::remove_if(typeStr.begin(), typeStr.end(), ::isspace), typeStr.end());

                PieceType pieceType;
                if (typeStr == "g")
                {
                    pieceType = PieceType::GATITO;
                }
                else if (typeStr == "G")
                {
                    pieceType = PieceType::GATO;
                }
                else
                {
                    std::cout << "Tipo de pieza inválido. Usa 'g' para gatito o 'G' para gato adulto" << std::endl;
                    continue;
                }

                return {row, col, pieceType};
            }
            catch (...)
            {
                std::cout << "Entrada inválida. Intenta de nuevo." << std::endl;
            }
        }
        else
        {
            std::cout << "Formato incorrecto. Usa: fila,columna,tipo" << std::endl;
        }
    }
}

void Boop::play()
{
    std::cout << "¡Bienvenido al juego Boop!" << std::endl;
    std::cout << "Objetivo: Forma una línea de 3 gatos adultos para ganar" << std::endl;
    std::cout << "Los gatitos en línea de 3 se convierten en gatos adultos" << std::endl;
    std::cout << "Los gatitos empujan a piezas adyacentes (boop!)" << std::endl;
    std::cout << "Los gatos adultos no pueden ser empujados" << std::endl;

    // Preguntar si quiere jugar contra IA
    std::cout << "\n¿Quieres jugar contra la IA? (s/n): ";
    std::string response;
    std::getline(std::cin, response);
    bool playAgainstAI = (response == "s" || response == "S");

    BoopAgent *agent = nullptr;
    int aiDepth = 4;

    if (playAgainstAI)
    {
        std::cout << "Profundidad de búsqueda de la IA (1-6, recomendado 4): ";
        std::string depthStr;
        std::getline(std::cin, depthStr);
        try
        {
            aiDepth = std::stoi(depthStr);
            if (aiDepth < 1)
                aiDepth = 1;
            if (aiDepth > 6)
                aiDepth = 6;
        }
        catch (...)
        {
            aiDepth = 4;
        }

        // Jugador 1 es humano, Jugador 2 es IA
        agent = new BoopAgent(&player2, &player1);
        std::cout << "\n¡Jugarás contra la IA (profundidad " << aiDepth << ")!" << std::endl;
        std::cout << "Tú eres: " << player1.name << " (" << player1.color << ")" << std::endl;
        std::cout << "IA es: " << player2.name << " (" << player2.color << ")" << std::endl;
    }

    while (!gameOver)
    {
        displayGameState();

        int row, col;
        PieceType pieceType;

        // Si es turno de la IA
        if (playAgainstAI && currentPlayer == &player2)
        {
            std::cout << "\n🤖 IA está pensando..." << std::endl;

            auto start = std::chrono::high_resolution_clock::now();

            int score;
            std::tie(row, col, pieceType, score) = agent->findBestMove(aiDepth);

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::cout << "IA eligió: (" << row << ", " << col << ") - "
                      << (pieceType == PieceType::GATITO ? "Gatito" : "Gato")
                      << " [Score: " << score << ", Nodos: " << agent->getNodesExplored()
                      << ", Tiempo: " << duration.count() << "ms]" << std::endl;
        }
        else
        {
            // Turno del jugador humano
            std::tie(row, col, pieceType) = getPlayerInput();
        }

        if (!placePiece(row, col, pieceType))
        {
            std::cout << "Movimiento inválido. Intenta de nuevo." << std::endl;
        }
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "¡JUEGO TERMINADO!" << std::endl;
    if (winner)
    {
        std::cout << "¡Felicitaciones " << winner->name << "! ¡Has ganado!" << std::endl;

        if (playAgainstAI)
        {
            if (winner == &player1)
            {
                std::cout << "🎉 ¡Venciste a la IA!" << std::endl;
            }
            else
            {
                std::cout << "🤖 La IA ganó esta vez." << std::endl;
            }
        }
    }
    std::cout << "========================================" << std::endl;
    board.display();

    if (agent)
    {
        delete agent;
    }
}

int main()
{
    Boop *game = Boop::getInstance();
    game->play();

    return 0;
}
