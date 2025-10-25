#!/usr/bin/env python3
"""
Ejemplo de uso de las funciones implementadas en VenceJohnathan2000.py
"""

from BoopGame import Boop, PieceType, Gatito, Gato, Player
from VenceJohnathan2000 import CanPromote, IsCenterAvalible

def test_can_promote():
    print("=== Test de CanPromote ===\n")
    
    # Crear un juego y configurar un escenario
    game = Boop()
    
    # Colocar dos gatitos del jugador 1 en línea
    gatito1 = Gatito((2, 1), game.player1)
    gatito2 = Gatito((2, 2), game.player1)
    
    game.board.place_piece(gatito1, 2, 1)
    game.board.place_piece(gatito2, 2, 2)
    
    print("Tablero de prueba:")
    game.board.display()
    print()
    
    # Probar CanPromote
    result = CanPromote(game.board.grid, game.player1, PieceType.GATITO)
    
    print("¿Puede hacer tres en línea con un gatito?")
    if result:
        print(f"✅ SÍ! Posición sugerida: {result}")
    else:
        print("❌ No puede hacer tres en línea")

def test_is_center_available():
    print("\n=== Test de IsCenterAvalible ===\n")
    
    game = Boop()
    
    # Tablero vacío
    print("Tablero vacío:")
    centers = IsCenterAvalible(game.board.grid)
    print(f"Centros disponibles: {centers}")
    
    # Ocupar una posición central
    gatito = Gatito((2, 2), game.player1)
    game.board.place_piece(gatito, 2, 2)
    
    print("\nDespués de ocupar (2,2):")
    game.board.display()
    centers = IsCenterAvalible(game.board.grid)
    print(f"Centros disponibles: {centers}")



def test_complete_scenario():
    print("\n=== Escenario Completo ===\n")
    
    game = Boop()
    
    # Configurar un escenario donde hay oportunidad de ganar
    game.player1.gatos_disponibles = 2
    
    gato1 = Gato((0, 0), game.player1)
    gato2 = Gato((1, 1), game.player1)
    
    game.board.place_piece(gato1, 0, 0)
    game.board.place_piece(gato2, 1, 1)
    
    print("Tablero con oportunidad de victoria:")
    game.board.display()
    print()
    
    # Verificar si puede ganar
    win_move = CanPromote(game.board.grid, game.player1, PieceType.GATO)
    promote_move = CanPromote(game.board.grid, game.player1, PieceType.GATITO)
    
    print("Análisis del tablero:")
    if win_move:
        print(f"🏆 ¡PUEDE GANAR! Colocar gato en {win_move}")
    
    if promote_move:
        print(f"💡 Puede hacer promoción con gatito en {promote_move}")
    
    # Mostrar centros disponibles
    centers = IsCenterAvalible(game.board.grid)
    print(f"📍 Centros disponibles: {centers}")
    

if __name__ == "__main__":
    test_can_promote()
    test_is_center_available()
    test_complete_scenario()
    
    print("\n" + "="*50)
