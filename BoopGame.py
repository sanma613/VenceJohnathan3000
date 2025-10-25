from dataclasses import dataclass, field
from abc import ABC, abstractmethod
from typing import Optional, List, Tuple
from enum import Enum

class PieceType(Enum):
    GATITO = 1
    GATO = 2

@dataclass
class Player:
    name: str
    color: str  # 'orange' o 'gray'
    gatitos_disponibles: int = 8
    gatos_disponibles: int = 0
    
    def can_place_gatito(self) -> bool:
        return self.gatitos_disponibles > 0
    
    def can_place_gato(self) -> bool:
        return self.gatos_disponibles > 0
    
    def use_gatito(self):
        if self.can_place_gatito():
            self.gatitos_disponibles -= 1
    
    def use_gato(self):
        if self.can_place_gato():
            self.gatos_disponibles -= 1
    
    def return_gatito(self):
        self.gatitos_disponibles += 1
    
    def return_gato(self):
        self.gatos_disponibles += 1
    
    def promote_gatitos_to_gato(self, count: int = 3):
        """Convierte 3 gatitos en 1 gato"""

        self.gatitos_disponibles += count
        self.gatos_disponibles += 3

@dataclass
class Piece(ABC):
    position: Tuple[int, int]
    player: Player
    piece_type: Optional[PieceType] = None
    
    @property
    @abstractmethod
    def weight(self) -> int:
        pass
    


@dataclass
class Gatito(Piece):
    def __post_init__(self):
        self.piece_type = PieceType.GATITO
    
    @property
    def weight(self) -> int:
        return 1
    
    @property
    def can_boop(self) -> bool:
        return True

@dataclass
class Gato(Piece):
    def __post_init__(self):
        self.piece_type = PieceType.GATO
    
    @property
    def weight(self) -> int:
        return 2
    
    @property
    def can_boop(self) -> bool:
        return False  # Los gatos adultos no pueden ser empujados


@dataclass
class Board:
    size: int = 6
    grid: List[List[Optional[Piece]]] = field(default_factory=lambda: [[None for _ in range(6)] for _ in range(6)])
    
    def is_valid_position(self, row: int, col: int) -> bool:
        return 0 <= row < self.size and 0 <= col < self.size
    
    def is_empty(self, row: int, col: int) -> bool:
        return self.is_valid_position(row, col) and self.grid[row][col] is None
    
    def place_piece(self, piece: Piece, row: int, col: int) -> bool:
        if self.is_empty(row, col):
            self.grid[row][col] = piece
            piece.position = (row, col)
            return True
        return False
    
    def remove_piece(self, row: int, col: int) -> Optional[Piece]:
        if self.is_valid_position(row, col):
            piece = self.grid[row][col]
            self.grid[row][col] = None
            return piece
        return None
    
    def get_piece(self, row: int, col: int) -> Optional[Piece]:
        if self.is_valid_position(row, col):
            return self.grid[row][col]
        return None
    
    def get_adjacent_positions(self, row: int, col: int) -> List[Tuple[int, int]]:
        """Obtiene todas las posiciones adyacentes (8 direcciones)"""
        directions = [(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1)]
        adjacent = []
        for dr, dc in directions:
            new_row, new_col = row + dr, col + dc
            if self.is_valid_position(new_row, new_col):
                adjacent.append((new_row, new_col))
        return adjacent
    
    def boop_pieces(self, placed_row: int, placed_col: int, placed_piece: Piece) -> List[Piece]:
        """Empuja las piezas adyacentes y retorna las que salen del tablero"""
        booped_out = []
        adjacent_positions = self.get_adjacent_positions(placed_row, placed_col)
        
        for adj_row, adj_col in adjacent_positions:
            piece = self.get_piece(adj_row, adj_col)
            if piece and piece.can_boop:
                # Calcular dirección del empuje
                dr = adj_row - placed_row
                dc = adj_col - placed_col
                new_row = adj_row + dr
                new_col = adj_col + dc
                
                # Verificar si la pieza puede moverse a la nueva posición
                if self.is_valid_position(new_row, new_col) and self.is_empty(new_row, new_col):
                    # Remover la pieza de su posición actual
                    self.remove_piece(adj_row, adj_col)
                    # Colocarla en la nueva posición
                    self.place_piece(piece, new_row, new_col)
                elif not self.is_valid_position(new_row, new_col):
                    # La pieza sale del tablero
                    self.remove_piece(adj_row, adj_col)
                    booped_out.append(piece)
                # Si la nueva posición está ocupada, la pieza se queda estática (no se mueve)
        
        return booped_out
    
    def find_lines_of_three(self, player: Player) -> List[List[Tuple[int, int]]]:
        """Encuentra todas las líneas de 3 piezas del mismo jugador"""
        lines = []
        directions = [(0, 1), (1, 0), (1, 1), (1, -1)]  # horizontal, vertical, diagonal
        
        for row in range(self.size):
            for col in range(self.size):
                piece = self.get_piece(row, col)
                if piece and piece.player == player:
                    for dr, dc in directions:
                        line = [(row, col)]
                        for i in range(1, 3):
                            new_row = row + i * dr
                            new_col = col + i * dc
                            next_piece = self.get_piece(new_row, new_col)
                            if (next_piece and next_piece.player == player and 
                                type(next_piece) == type(piece)):
                                line.append((new_row, new_col))
                            else:
                                break
                        
                        if len(line) == 3:
                            lines.append(line)
        
        return lines
    
    def display(self):
        """Muestra el tablero en la consola"""
        print("  ", end="")
        for col in range(self.size):
            print(f" {col} ", end="")
        print()
        
        for row in range(self.size):
            print(f"{row} ", end="")
            for col in range(self.size):
                piece = self.get_piece(row, col)
                if piece is None:
                    print(" . ", end="")
                elif isinstance(piece, Gatito):
                    symbol = "o" if piece.player.color == "orange" else "x"
                    print(f" {symbol} ", end="")
                elif isinstance(piece, Gato):
                    symbol = "O" if piece.player.color == "orange" else "X"
                    print(f" {symbol} ", end="")
            print()


@dataclass
class Boop:
    board: Board = field(default_factory=Board)
    player1: Player = field(default_factory=lambda: Player("Jugador 1", "orange"))
    player2: Player = field(default_factory=lambda: Player("Jugador 2", "gray"))
    current_player: Optional[Player] = None
    game_over: bool = False
    winner: Optional[Player] = None
    
    def __post_init__(self):
        self.current_player = self.player1
    
    def switch_player(self):
        self.current_player = self.player2 if self.current_player == self.player1 else self.player1
    
    def place_piece(self, row: int, col: int, piece_type: PieceType) -> bool:
        """Coloca una pieza en el tablero y ejecuta la lógica del juego"""
        if self.game_over:
            print("El juego ha terminado!")
            return False
        
        if not self.board.is_empty(row, col):
            print("Esa posición ya está ocupada!")
            return False
        
        # Verificar si el jugador puede colocar el tipo de pieza solicitado
        if piece_type == PieceType.GATITO and not self.current_player.can_place_gatito():
            print("No tienes más gatitos disponibles!")
            return False
        elif piece_type == PieceType.GATO and not self.current_player.can_place_gato():
            print("No tienes gatos disponibles!")
            return False
        
        # Crear la pieza
        if piece_type == PieceType.GATITO:
            piece = Gatito((row, col), self.current_player)
            self.current_player.use_gatito()
        else:
            piece = Gato((row, col), self.current_player)
            self.current_player.use_gato()
        
        # Colocar la pieza en el tablero
        self.board.place_piece(piece, row, col)
        
        # Ejecutar el boop (empujar piezas adyacentes)
        booped_out = self.board.boop_pieces(row, col, piece)
        
        # Devolver las piezas que salieron del tablero a sus jugadores
        for booped_piece in booped_out:
            if isinstance(booped_piece, Gatito):
                booped_piece.player.return_gatito()
            else:
                booped_piece.player.return_gato()
        
        # Verificar líneas de 3 gatitos para promoción
        self.check_and_promote_gatitos()
        
        # Verificar condiciones de victoria
        self.check_victory()
        
        if not self.game_over:
            self.switch_player()
        
        return True
    
    def check_and_promote_gatitos(self):
        """Verifica y promueve líneas de 3 gatitos a gatos adultos"""
        for player in [self.player1, self.player2]:
            lines = self.board.find_lines_of_three(player)
            
            for line in lines:
                # Verificar si todos son gatitos
                pieces = [self.board.get_piece(row, col) for row, col in line]
                if all(isinstance(piece, Gatito) for piece in pieces):
                    # Remover los gatitos del tablero
                    for row, col in line:
                        self.board.remove_piece(row, col)
                    
                    # Promover gatitos a gato
                    player.promote_gatitos_to_gato(3)
                    
                    print(f"¡{player.name} ha graduado a 3 gaatitos!")
    
    def check_victory(self):
        """Verifica las condiciones de victoria"""
        for player in [self.player1, self.player2]:
            lines = self.board.find_lines_of_three(player)
            
            for line in lines:
                # Verificar si todos son gatos adultos
                pieces = [self.board.get_piece(row, col) for row, col in line]
                if all(isinstance(piece, Gato) for piece in pieces):
                    self.game_over = True
                    self.winner = player
                    print(f"¡{player.name} ha ganado con 3 gatos adultos en línea!")
                    return
    
    def display_game_state(self):
        """Muestra el estado actual del juego"""
        print("\n" + "="*30)
        print(f"Turno de: {self.current_player.name} ({self.current_player.color})")
        print(f"Gatitos disponibles: {self.current_player.gatitos_disponibles}")
        print(f"Gatos disponibles: {self.current_player.gatos_disponibles}")
        print("="*30)
        self.board.display()
        print("="*30)
    
    def get_player_input(self) -> Tuple[int, int, PieceType]:
        """Obtiene la entrada del jugador"""
        while True:
            try:
                print(f"\n{self.current_player.name}, es tu turno!")
                print("Ingresa tu movimiento en formato: fila,columna,tipo")
                print("Tipo: 'g' para gatito, 'G' para gato adulto")
                print("Ejemplo: 2,3,g")
                
                input_str = input("Tu movimiento: ").strip()
                parts = input_str.split(',')
                
                if len(parts) != 3:
                    print("Formato incorrecto. Usa: fila,columna,tipo")
                    continue
                
                row = int(parts[0])
                col = int(parts[1])
                piece_char = parts[2].strip()
                
                if piece_char == 'g':
                    piece_type = PieceType.GATITO
                elif piece_char == 'G':
                    piece_type = PieceType.GATO
                else:
                    print("Tipo de pieza inválido. Usa 'g' para gatito o 'G' para gato adulto")
                    continue
                
                return row, col, piece_type
                
            except (ValueError, IndexError):
                print("Entrada inválida. Intenta de nuevo.")
    
    def play(self):
        """Bucle principal del juego"""
        print("¡Bienvenido al juego Boop!")
        print("Objetivo: Forma una línea de 3 gatos adultos para ganar")
        print("Los gatitos en línea de 3 se convierten en gatos adultos")
        print("Los gatitos empujan a piezas adyacentes (boop!)")
        print("Los gatos adultos no pueden ser empujados")
        
        while not self.game_over:
            self.display_game_state()
            row, col, piece_type = self.get_player_input()
            
            if not self.place_piece(row, col, piece_type):
                print("Movimiento inválido. Intenta de nuevo.")
                continue
        
        print("\n" + "="*40)
        print("¡JUEGO TERMINADO!")
        if self.winner:
            print(f"¡Felicitaciones {self.winner.name}! ¡Has ganado!")
        print("="*40)
        self.board.display()


# Función para iniciar el juego
def main():
    game = Boop()
    game.play()

if __name__ == "__main__":
    main()

