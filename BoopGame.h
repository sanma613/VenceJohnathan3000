#ifndef BOOPGAME_H
#define BOOPGAME_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <sstream>
#include <algorithm>
#include <tuple>

using namespace std;

// Forward declarations
class Player;
class Piece;
class Gatito;
class Gato;

// Enumeración para tipos de pieza
enum class PieceType {
    GATITO = 1,
    GATO = 2
};

// Clase Player
class Player {
public:
    string name;
    string color;
    int gatitos_disponibles;
    int gatos_disponibles;

    Player(const string& n, const string& c);
    bool canPlaceGatito() const;
    bool canPlaceGato() const;
    void useGatito();
    void useGato();
    void returnGatito();
    void returnGato();
    void promoteGatitosToGato(int count = 3);
};

// Clase base abstracta Piece
class Piece {
public:
    pair<int, int> position;
    Player* player;
    PieceType piece_type;

    Piece(pair<int, int> pos, Player* p, PieceType type);
    virtual ~Piece() = default;
    virtual int weight() const = 0;
    virtual bool canBoop() const = 0;
    virtual char getSymbol() const = 0;
};

// Clase Gatito
class Gatito : public Piece {
public:
    Gatito(pair<int, int> pos, Player* p);
    int weight() const override;
    bool canBoop() const override;
    char getSymbol() const override;
};

// Clase Gato
class Gato : public Piece {
public:
    Gato(pair<int, int> pos, Player* p);
    int weight() const override;
    bool canBoop() const override;
    char getSymbol() const override;
};

// Clase Board
class Board {
public:
    int size;
    vector<vector<Piece*>> grid;

    Board(int s = 6);
    ~Board();
    bool isValidPosition(int row, int col) const;
    bool isEmpty(int row, int col) const;
    bool placePiece(Piece* piece, int row, int col);
    Piece* removePiece(int row, int col);
    Piece* getPiece(int row, int col) const;
    vector<pair<int, int>> getAdjacentPositions(int row, int col) const;
    vector<Piece*> boopPieces(int placedRow, int placedCol, Piece* placedPiece);
    vector<vector<pair<int, int>>> findLinesOfThree(Player* player) const;
    void display() const;
};

// Clase principal del juego Boop
class Boop {
public:
    Board board;
    Player player1;
    Player player2;
    Player* currentPlayer;
    bool gameOver;
    Player* winner;

    Boop();
    void switchPlayer();
    bool placePiece(int row, int col, PieceType pieceType);
    void checkAndPromoteGatitos();
    void checkVictory();
    void displayGameState() const;
    tuple<int, int, PieceType> getPlayerInput();
    void play();
};

// Implementaciones inline de Player
inline Player::Player(const string& n, const string& c) 
    : name(n), color(c), gatitos_disponibles(8), gatos_disponibles(0) {}

inline bool Player::canPlaceGatito() const {
    return gatitos_disponibles > 0;
}

inline bool Player::canPlaceGato() const {
    return gatos_disponibles > 0;
}

inline void Player::useGatito() {
    if (canPlaceGatito()) {
        gatitos_disponibles--;
    }
}

inline void Player::useGato() {
    if (canPlaceGato()) {
        gatos_disponibles--;
    }
}

inline void Player::returnGatito() {
    gatitos_disponibles++;
}

inline void Player::returnGato() {
    gatos_disponibles++;
}

inline void Player::promoteGatitosToGato(int count) {
    gatitos_disponibles += count;
    gatos_disponibles += 3;
}

// Implementaciones inline de Piece
inline Piece::Piece(pair<int, int> pos, Player* p, PieceType type) 
    : position(pos), player(p), piece_type(type) {}

// Implementaciones inline de Gatito
inline Gatito::Gatito(pair<int, int> pos, Player* p) 
    : Piece(pos, p, PieceType::GATITO) {}

inline int Gatito::weight() const {
    return 1;
}

inline bool Gatito::canBoop() const {
    return true;
}

inline char Gatito::getSymbol() const {
    return player->color == "orange" ? 'o' : 'x';
}

// Implementaciones inline de Gato
inline Gato::Gato(pair<int, int> pos, Player* p) 
    : Piece(pos, p, PieceType::GATO) {}

inline int Gato::weight() const {
    return 2;
}

inline bool Gato::canBoop() const {
    return false;
}

inline char Gato::getSymbol() const {
    return player->color == "orange" ? 'O' : 'X';
}

// Implementaciones inline de Board
inline Board::Board(int s) : size(s) {
    grid.resize(size, vector<Piece*>(size, nullptr));
}

inline Board::~Board() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            delete grid[i][j];
        }
    }
}

inline bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < size && col >= 0 && col < size;
}

inline bool Board::isEmpty(int row, int col) const {
    return isValidPosition(row, col) && grid[row][col] == nullptr;
}

inline bool Board::placePiece(Piece* piece, int row, int col) {
    if (isEmpty(row, col)) {
        grid[row][col] = piece;
        piece->position = {row, col};
        return true;
    }
    return false;
}

inline Piece* Board::removePiece(int row, int col) {
    if (isValidPosition(row, col)) {
        Piece* piece = grid[row][col];
        grid[row][col] = nullptr;
        return piece;
    }
    return nullptr;
}

inline Piece* Board::getPiece(int row, int col) const {
    if (isValidPosition(row, col)) {
        return grid[row][col];
    }
    return nullptr;
}

inline vector<pair<int, int>> Board::getAdjacentPositions(int row, int col) const {
    vector<pair<int, int>> adjacent;
    int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    
    for (int i = 0; i < 8; i++) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];
        if (isValidPosition(newRow, newCol)) {
            adjacent.push_back({newRow, newCol});
        }
    }
    return adjacent;
}

inline vector<Piece*> Board::boopPieces(int placedRow, int placedCol, Piece* placedPiece) {
    vector<Piece*> boopedOut;
    vector<pair<int, int>> adjacentPositions = getAdjacentPositions(placedRow, placedCol);

    for (const auto& pos : adjacentPositions) {
        int adjRow = pos.first;
        int adjCol = pos.second;
        Piece* piece = getPiece(adjRow, adjCol);
        
        if (piece && piece->canBoop()) {
            int dr = adjRow - placedRow;
            int dc = adjCol - placedCol;
            int newRow = adjRow + dr;
            int newCol = adjCol + dc;

            if (isValidPosition(newRow, newCol) && isEmpty(newRow, newCol)) {
                removePiece(adjRow, adjCol);
                placePiece(piece, newRow, newCol);
            } else if (!isValidPosition(newRow, newCol)) {
                removePiece(adjRow, adjCol);
                boopedOut.push_back(piece);
            }
        }
    }
    return boopedOut;
}

inline vector<vector<pair<int, int>>> Board::findLinesOfThree(Player* player) const {
    vector<vector<pair<int, int>>> lines;
    int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            Piece* piece = getPiece(row, col);
            if (piece && piece->player == player) {
                for (int d = 0; d < 4; d++) {
                    vector<pair<int, int>> line = {{row, col}};
                    int dr = directions[d][0];
                    int dc = directions[d][1];

                    for (int i = 1; i < 3; i++) {
                        int newRow = row + i * dr;
                        int newCol = col + i * dc;
                        Piece* nextPiece = getPiece(newRow, newCol);
                        
                        if (nextPiece && nextPiece->player == player &&
                            nextPiece->piece_type == piece->piece_type) {
                            line.push_back({newRow, newCol});
                        } else {
                            break;
                        }
                    }

                    if (line.size() == 3) {
                        lines.push_back(line);
                    }
                }
            }
        }
    }
    return lines;
}

inline void Board::display() const {
    cout << "  ";
    for (int col = 0; col < size; col++) {
        cout << " " << col << " ";
    }
    cout << endl;

    for (int row = 0; row < size; row++) {
        cout << row << " ";
        for (int col = 0; col < size; col++) {
            Piece* piece = getPiece(row, col);
            if (piece == nullptr) {
                cout << " . ";
            } else {
                cout << " " << piece->getSymbol() << " ";
            }
        }
        cout << endl;
    }
}

// Implementaciones inline de Boop
inline Boop::Boop() : player1("Jugador 1", "orange"), player2("Jugador 2", "gray"),
         currentPlayer(&player1), gameOver(false), winner(nullptr) {}

inline void Boop::switchPlayer() {
    currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
}

inline void Boop::displayGameState() const {
    cout << "\n==============================" << endl;
    cout << "Turno de: " << currentPlayer->name << " (" << currentPlayer->color << ")" << endl;
    cout << "Gatitos disponibles: " << currentPlayer->gatitos_disponibles << endl;
    cout << "Gatos disponibles: " << currentPlayer->gatos_disponibles << endl;
    cout << "==============================" << endl;
    board.display();
    cout << "==============================" << endl;
}

#endif // BOOPGAME_H
