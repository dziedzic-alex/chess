#ifndef GAME_HPP
#define GAME_HPP
#include "player.hpp"

// class to create instance of a chess game 
class Game{
    Player p1;
    Player p2;
    Board board;

    public:
    Game(const std::string &whiteName, const std::string &redName)
        : p1("white", whiteName), p2("red", redName), board() {}
    
    // simulates chess game
    // ends when a player gets checkmated
    void conduct_game(){
        bool p1Victorius = false;
        std::cout << board << "\n";
        while (true){
            std::cout << p1.return_name() << "'s turn\n";
            p1.move_piece(board, p2);
            system("clear");
            std::cout << board << "\n";
            Piece *piece = p1.check_pawn_upgrade();
            if (piece){
                p1.upgrade_pawn(piece, board);
                system("clear");
                std::cout << board << "\n";
            }
            if (p2.check_mate(p1, board)){
                p1Victorius = true;
                break;
            }
            std::cout << p2.return_name() << "'s turn\n";
            p2.move_piece(board, p1);
            system("clear");
            std::cout << board << "\n";
            piece = p2.check_pawn_upgrade();
            if (piece){
                p2.upgrade_pawn(piece, board);
                system("clear");
                std::cout << board << "\n";
            }
            if (p1.check_mate(p2, board)){
                break;
            }
        }
        if (p1Victorius){
            std::cout << "CHECKMATE!\n" << p1.return_name() << " Wins!" << std::endl;
        } else{
            std::cout << "CHECKMATE!\n" << p2.return_name() << " Wins!" << std::endl;
        }
    }
};

#endif