from typing import Optional, Tuple, Union
from BoopGame import Piece, Player, PieceType, Gatito, Gato

def IsCenterAvalible(Grid:list[list[Optional[Piece]]]):

    center_positions = [(2, 2), (2, 3), (3, 2), (3, 3)]
    
    available_centers = []
    for row, col in center_positions:
        if Grid[row][col] is None:
            available_centers.append((row, col))
    
    return available_centers if available_centers else False

def CanPromote(Grid: list[list[Optional[Piece]]], player: Player, piece_type: PieceType) -> Union[bool, Tuple[int, int]]:

    size = len(Grid)
    directions = [(0, 1), (1, 0), (1, 1), (1, -1)] 
    
    for row in range(size):
        for col in range(size):
            if Grid[row][col] is None:  
                for dr, dc in directions:
                    line_count = 1
                    
                    for i in range(1, 3):
                        check_row = row + i * dr
                        check_col = col + i * dc
                        
                        if not (0 <= check_row < size and 0 <= check_col < size):
                            break
                            
                        check_piece = Grid[check_row][check_col]
                        if (check_piece and 
                            check_piece.player == player and 
                            _is_same_piece_type(check_piece, piece_type)):
                            line_count += 1
                        else:
                            break
                    
                    for i in range(1, 3):
                        check_row = row - i * dr
                        check_col = col - i * dc
                        
                        if not (0 <= check_row < size and 0 <= check_col < size):
                            break
                            
                        check_piece = Grid[check_row][check_col]
                        if (check_piece and 
                            check_piece.player == player and 
                            _is_same_piece_type(check_piece, piece_type)):
                            line_count += 1
                        else:
                            break
                    
                    if line_count >= 3:
                        return (row, col)
    
    return False


def _is_same_piece_type(piece: Piece, piece_type: PieceType) -> bool:
    if piece_type == PieceType.GATITO:
        return isinstance(piece, Gatito)
    elif piece_type == PieceType.GATO:
        return isinstance(piece, Gato)
    return False


