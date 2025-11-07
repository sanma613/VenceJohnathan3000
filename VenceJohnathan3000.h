#ifndef VENCEJOHNATHAN3000_H
#define VENCEJOHNATHAN3000_H

#include <vector>
#include <utility>
#include <optional>

// Forward declarations
class Piece;
class Player;
enum class PieceType;

// Función que verifica qué posiciones centrales están disponibles
// Retorna un vector de pares (row, col) con las posiciones centrales vacías
// Las posiciones centrales son: (2,2), (2,3), (3,2), (3,3)
std::vector<std::pair<int, int>> IsCenterAvailible(std::vector<std::vector<Piece*>> Grid);

// Función que encuentra si hay una jugada que haga tres en línea
// Retorna optional con la posición (row, col) si existe, o nullopt si no hay ninguna
std::optional<std::pair<int, int>> CanPromote(std::vector<std::vector<Piece*>> Grid, Player* player, PieceType piece_type);

#endif // VENCEJOHNATHAN3000_H
