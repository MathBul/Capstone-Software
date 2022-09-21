import chess
from stockfish import Stockfish
from time import sleep

IN_PROGRESS = True
TERMINATED = False
game_state = IN_PROGRESS

def main():
    stockfish = Stockfish(path="C:/Users/sirmu/Documents/Keenan/The_UVA_Archives/Capstone/stockfish_15_win_x64_avx2/stockfish_15_x64_avx2.exe")
    stockfish.update_engine_parameters({'Hash':256})
    stockfish.set_elo_rating(2500)
    player_color = input("Select a color (\"W\" or \"B\"): ")
    while player_color not in ["W", "B"]:
        player_color = input("Incorrect input. Select a color (\"W\" or \"B\"): ")
    if player_color == "W":
        while game_state == IN_PROGRESS:
            player_move(stockfish, player_color)
            check_game_state()
            stockfish_move(stockfish, player_color)
    else:
        while game_state == IN_PROGRESS:
            stockfish_move(stockfish, player_color)
            player_move(stockfish, player_color)
    print("Thanks for playing!")

def player_move(stockfish:Stockfish, player_color):
    print(stockfish.get_board_visual(perspective_white=(True if player_color=="W" else False)))
    valid_move = False
    while not valid_move:
        try:
            player_next_move = input("Select your next move: ")
            stockfish.make_moves_from_current_position([player_next_move])
        except ValueError:
            print("That move is invalid. Please try again.\n")
        else:
            valid_move = True
            return stockfish.get_fen_position()



def stockfish_move(stockfish:Stockfish, player_color):
    print(stockfish.get_board_visual(perspective_white=(True if player_color=="W" else False)))
    print("Stockfish making move...\n")
    stockfish_next_move = stockfish.get_best_move_time(1000)
    stockfish.make_moves_from_current_position([stockfish_next_move])
    print(f"Stockfish's move: {stockfish_next_move}")
    print(stockfish.get_evaluation())
    return stockfish.get_fen_position()

def check_game_state(board:chess.Board):
    if board.is_stalemate():
        print("Stalemate; game over")
        game_state = TERMINATED
    elif board.is_checkmate():
        print("Checkmate; game over")
        game_state = TERMINATED
    elif board.is_check():
        print("Check")
    else:
        pass

if __name__ == "__main__":
    main()