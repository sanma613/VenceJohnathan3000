#ifndef BOOPGAME_H
#define BOOPGAME_H

#include <vector>
#include <string>
#include <utility>

// Forward declarations
class Player;
class Piece;
class Gatito;
class Gato;
class Board;
class Boop;

// Enumeración para tipos de pieza
enum class PieceType {
    GATITO = 1,
    GATO = 2
};

// Clase Player
class Player {
public:
    std::string name;
    std::string color;
    int gatitos_disponibles;
    int gatos_disponibles;

    Player(const std::string& n, const std::string& c);

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
    std::pair<int, int> position;
    Player* player;
    PieceType piece_type;

    Piece(std::pair<int, int> pos, Player* p, PieceType type);
    virtual ~Piece() = default;

    virtual int weight() const = 0;
    virtual bool canBoop() const = 0;
    virtual char getSymbol() const = 0;
};

// Clase Gatito
class Gatito : public Piece {
public:
    Gatito(std::pair<int, int> pos, Player* p);

    int weight() const override;
    bool canBoop() const override;
    char getSymbol() const override;
};

// Clase Gato
class Gato : public Piece {
public:
    Gato(std::pair<int, int> pos, Player* p);

    int weight() const override;
    bool canBoop() const override;
    char getSymbol() const override;
};

// Clase Board
class Board {
public:
    int size;
    std::vector<std::vector<Piece*>> grid;

    Board(int s = 6);
    ~Board();

    bool isValidPosition(int row, int col) const;
    bool isEmpty(int row, int col) const;
    bool placePiece(Piece* piece, int row, int col);
    Piece* removePiece(int row, int col);
    Piece* getPiece(int row, int col) const;
    std::vector<std::pair<int, int>> getAdjacentPositions(int row, int col) const;
    std::vector<Piece*> boopPieces(int placedRow, int placedCol, Piece* placedPiece);
    std::vector<std::vector<std::pair<int, int>>> findLinesOfThree(Player* player) const;
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
    std::tuple<int, int, PieceType> getPlayerInput();
    void play();
};

#endif // BOOPGAME_H
