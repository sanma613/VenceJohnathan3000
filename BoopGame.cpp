#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <sstream>
#include <algorithm>

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

    Player(const string& n, const string& c) 
        : name(n), color(c), gatitos_disponibles(8), gatos_disponibles(0) {}

    bool canPlaceGatito() const {
        return gatitos_disponibles > 0;
    }

    bool canPlaceGato() const {
        return gatos_disponibles > 0;
    }

    void useGatito() {
        if (canPlaceGatito()) {
            gatitos_disponibles--;
        }
    }

    void useGato() {
        if (canPlaceGato()) {
            gatos_disponibles--;
        }
    }

    void returnGatito() {
        gatitos_disponibles++;
    }

    void returnGato() {
        gatos_disponibles++;
    }

    void promoteGatitosToGato(int count = 3) {
        gatitos_disponibles += count;
        gatos_disponibles += 3;
    }
};

// Clase base abstracta Piece
class Piece {
public:
    pair<int, int> position;
    Player* player;
    PieceType piece_type;

    Piece(pair<int, int> pos, Player* p, PieceType type) 
        : position(pos), player(p), piece_type(type) {}

    virtual ~Piece() = default;

    virtual int weight() const = 0;
    virtual bool canBoop() const = 0;
    virtual char getSymbol() const = 0;
};

// Clase Gatito
class Gatito : public Piece {
public:
    Gatito(pair<int, int> pos, Player* p) 
        : Piece(pos, p, PieceType::GATITO) {}

    int weight() const override {
        return 1;
    }

    bool canBoop() const override {
        return true;
    }

    char getSymbol() const override {
        return player->color == "orange" ? 'o' : 'x';
    }
};

// Clase Gato
class Gato : public Piece {
public:
    Gato(pair<int, int> pos, Player* p) 
        : Piece(pos, p, PieceType::GATO) {}

    int weight() const override {
        return 2;
    }

    bool canBoop() const override {
        return true;
    }

    char getSymbol() const override {
        return player->color == "orange" ? 'O' : 'X';
    }
};

// Clase Board
class Board {
public:
    int size;
    vector<vector<Piece*>> grid;

    Board(int s = 6) : size(s) {
        grid.resize(size, vector<Piece*>(size, nullptr));
    }

    ~Board() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                delete grid[i][j];
            }
        }
    }

    bool isValidPosition(int row, int col) const {
        return row >= 0 && row < size && col >= 0 && col < size;
    }

    bool isEmpty(int row, int col) const {
        return isValidPosition(row, col) && grid[row][col] == nullptr;
    }

    bool placePiece(Piece* piece, int row, int col) {
        if (isEmpty(row, col)) {
            grid[row][col] = piece;
            piece->position = {row, col};
            return true;
        }
        return false;
    }

    Piece* removePiece(int row, int col) {
        if (isValidPosition(row, col)) {
            Piece* piece = grid[row][col];
            grid[row][col] = nullptr;
            return piece;
        }
        return nullptr;
    }

    Piece* getPiece(int row, int col) const {
        if (isValidPosition(row, col)) {
            return grid[row][col];
        }
        return nullptr;
    }

    vector<pair<int, int>> getAdjacentPositions(int row, int col) const {
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

    vector<Piece*> boopPieces(int placedRow, int placedCol, Piece* placedPiece) {
        vector<Piece*> boopedOut;
        vector<pair<int, int>> adjacentPositions = getAdjacentPositions(placedRow, placedCol);

        for (const auto& pos : adjacentPositions) {
            int adjRow = pos.first;
            int adjCol = pos.second;
            Piece* piece = getPiece(adjRow, adjCol);
            
            if (piece && piece->canBoop() && placedPiece->weight() >= piece->weight()) {
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

    vector<vector<pair<int, int>>> findLinesOfThree(Player* player) const {
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

    void display() const {
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

    Boop() : player1("Jugador 1", "orange"), player2("Jugador 2", "gray"),
             currentPlayer(&player1), gameOver(false), winner(nullptr) {}

    void switchPlayer() {
        currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
    }

    bool placePiece(int row, int col, PieceType pieceType) {
        if (gameOver) {
            cout << "El juego ha terminado!" << endl;
            return false;
        }

        if (!board.isEmpty(row, col)) {
            cout << "Esa posición ya está ocupada!" << endl;
            return false;
        }

        if (pieceType == PieceType::GATITO && !currentPlayer->canPlaceGatito()) {
            cout << "No tienes más gatitos disponibles!" << endl;
            return false;
        } else if (pieceType == PieceType::GATO && !currentPlayer->canPlaceGato()) {
            cout << "No tienes gatos disponibles!" << endl;
            return false;
        }

        Piece* piece;
        if (pieceType == PieceType::GATITO) {
            piece = new Gatito({row, col}, currentPlayer);
            currentPlayer->useGatito();
        } else {
            piece = new Gato({row, col}, currentPlayer);
            currentPlayer->useGato();
        }

        board.placePiece(piece, row, col);

        vector<Piece*> boopedOut = board.boopPieces(row, col, piece);

        for (Piece* boopedPiece : boopedOut) {
            if (dynamic_cast<Gatito*>(boopedPiece)) {
                boopedPiece->player->returnGatito();
            } else {
                boopedPiece->player->returnGato();
            }
            delete boopedPiece;
        }

        checkAndPromoteGatitos();
        checkVictory();

        if (!gameOver) {
            switchPlayer();
        }

        return true;
    }

    void checkAndPromoteGatitos() {
        Player* players[] = {&player1, &player2};
        
        for (Player* player : players) {
            vector<vector<pair<int, int>>> lines = board.findLinesOfThree(player);
            
            for (const auto& line : lines) {
                bool allGatitos = true;
                for (const auto& pos : line) {
                    Piece* piece = board.getPiece(pos.first, pos.second);
                    if (!dynamic_cast<Gatito*>(piece)) {
                        allGatitos = false;
                        break;
                    }
                }

                if (allGatitos) {
                    for (const auto& pos : line) {
                        Piece* piece = board.removePiece(pos.first, pos.second);
                        delete piece;
                    }
                    player->promoteGatitosToGato(3);
                    cout << "¡" << player->name << " ha graduado a 3 gatitos!" << endl;
                }
            }
        }
    }

    void checkVictory() {
        Player* players[] = {&player1, &player2};
        
        for (Player* player : players) {
            vector<vector<pair<int, int>>> lines = board.findLinesOfThree(player);
            
            for (const auto& line : lines) {
                bool allGatos = true;
                for (const auto& pos : line) {
                    Piece* piece = board.getPiece(pos.first, pos.second);
                    if (!dynamic_cast<Gato*>(piece)) {
                        allGatos = false;
                        break;
                    }
                }

                if (allGatos) {
                    gameOver = true;
                    winner = player;
                    cout << "¡" << player->name << " ha ganado con 3 gatos adultos en línea!" << endl;
                    return;
                }
            }
        }
    }

    void displayGameState() const {
        cout << "\n==============================" << endl;
        cout << "Turno de: " << currentPlayer->name << " (" << currentPlayer->color << ")" << endl;
        cout << "Gatitos disponibles: " << currentPlayer->gatitos_disponibles << endl;
        cout << "Gatos disponibles: " << currentPlayer->gatos_disponibles << endl;
        cout << "==============================" << endl;
        board.display();
        cout << "==============================" << endl;
    }

    tuple<int, int, PieceType> getPlayerInput() {
        while (true) {
            cout << "\n" << currentPlayer->name << ", es tu turno!" << endl;
            cout << "Ingresa tu movimiento en formato: fila,columna,tipo" << endl;
            cout << "Tipo: 'g' para gatito, 'G' para gato adulto" << endl;
            cout << "Ejemplo: 2,3,g" << endl;
            cout << "Tu movimiento: ";

            string input;
            getline(cin, input);

            stringstream ss(input);
            string rowStr, colStr, typeStr;

            if (getline(ss, rowStr, ',') && getline(ss, colStr, ',') && getline(ss, typeStr)) {
                try {
                    int row = stoi(rowStr);
                    int col = stoi(colStr);
                    
                    // Eliminar espacios
                    typeStr.erase(remove_if(typeStr.begin(), typeStr.end(), ::isspace), typeStr.end());

                    PieceType pieceType;
                    if (typeStr == "g") {
                        pieceType = PieceType::GATITO;
                    } else if (typeStr == "G") {
                        pieceType = PieceType::GATO;
                    } else {
                        cout << "Tipo de pieza inválido. Usa 'g' para gatito o 'G' para gato adulto" << endl;
                        continue;
                    }

                    return {row, col, pieceType};
                } catch (...) {
                    cout << "Entrada inválida. Intenta de nuevo." << endl;
                }
            } else {
                cout << "Formato incorrecto. Usa: fila,columna,tipo" << endl;
            }
        }
    }

    void play() {
        cout << "¡Bienvenido al juego Boop!" << endl;
        cout << "Objetivo: Forma una línea de 3 gatos adultos para ganar" << endl;
        cout << "Los gatitos en línea de 3 se convierten en gatos adultos" << endl;
        cout << "Los gatitos empujan a piezas adyacentes (boop!)" << endl;
        cout << "Los gatos adultos no pueden ser empujados" << endl;

        while (!gameOver) {
            displayGameState();
            auto [row, col, pieceType] = getPlayerInput();

            if (!placePiece(row, col, pieceType)) {
                cout << "Movimiento inválido. Intenta de nuevo." << endl;
            }
        }

        cout << "\n========================================" << endl;
        cout << "¡JUEGO TERMINADO!" << endl;
        if (winner) {
            cout << "¡Felicitaciones " << winner->name << "! ¡Has ganado!" << endl;
        }
        cout << "========================================" << endl;
        board.display();
    }
};

int main() {
    Boop game;
    game.play();
    return 0;
}