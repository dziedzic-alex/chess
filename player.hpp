#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <string>
#include "pieces.hpp"
#include "board.hpp"


// class representing a player
// currently game only supports human players
// but support for ai players can be added later 
// and in that case this class will become a base class 
// from which human and ai player classes will be derived from
class Player{
    // each player has a set of pieces and a name
    PieceSet pieces;
    const std::string name;

    public:
    Player(const std::string &team, const std::string &name)
        : pieces{PieceSet(team)}, name{name} {}
    
    const std::string return_name() const{
        return name;
    }

    // checks if any of a players pawns have made it to other side of board
    // and are thus due for an upgrade
    // returns pointer to pawn due for uppgrade
    // if none due for upgrade returns nullptr
    Piece *check_pawn_upgrade() const{
        for (int i = 0; i < 8; i++){
            Piece *piece = pieces.return_piece(i);
            int xPos = piece->return_pos().first;
            if (piece->return_symbol() == 'P' && (xPos == 0 || xPos == 7)){
                return piece;
            }
        }
        return nullptr;
    }

    // prompts user for desired piece type to upgrade pawn to
    // and updates the players piece set and board based on this choice
    void upgrade_pawn(Piece *upgrade, Board &board){
        std::pair<int, int> pos = upgrade->return_pos();
        int xPos = pos.first;
        int yPos = pos.second;
        std::cout << "Congrats! You can upgrade your pawn to a Queen, Rook, Bishop, or Knight\n";
        while (true){
            std::cout << "Please enter a Q, R, B, or N representing your choice: ";
            char choice;
            std::cin >> choice;
            std::cin.ignore();
            if (choice == 'Q' || choice == 'q'){
                board.change_tile_symbol('Q', xPos, yPos);
            } else if (choice == 'R' || choice == 'r'){
                board.change_tile_symbol('R', xPos, yPos);
            } else if (choice == 'B' || choice == 'b'){
                board.change_tile_symbol('B', xPos, yPos);
            } else if (choice == 'N' || choice == 'n'){
                board.change_tile_symbol('N', xPos, yPos);
            } else {
                system("clear");
                std::cout << board;
                std::cout << "Not a valid upgrade input. Try Again!\n";
                continue;
            }
            pieces.upgrade_piece(upgrade, choice, pos);
            break;
        }
        
        
    }

    // simulates a chess move by a human player
    // prompts player for piece to move and where to move it to
    // and updates board and players' piece sets to reflect new piece position
    void move_piece(Board &board, Player &other){
        int x = -1;
        int y = -1;
        Piece *pieceToMove = nullptr;
        while (true){
            std::cout << "Please enter the coordinates of the piece you would like to move: ";
            std::string input;
            getline(std::cin, input);
            system("clear");
            std::cout << board << std::endl;
            x = std::stoi(std::string(1, input[0]));
            y = std::stoi(std::string(1, input[2]));
            pieceToMove = pieces.return_piece_at_pos(x, y);
            if (!pieceToMove){
                system("clear");
                std::cout << board;
                std::cout << "Try Again there is no piece there or it is the opponents piece!\n";
                continue;
            }
            std::cout << "Enter the coordinates you would like to move this piece to: ";
            getline(std::cin, input);
            x = std::stoi(std::string(1, input[0]));
            y = std::stoi(std::string(1, input[2]));
            if (!move_piece_possible(board, pieceToMove, other, x, y, true)){
                std::cout << "Try again!\n";
                continue;
            }
            break;
        }
        std::pair<int, int> currPos = pieceToMove->return_pos();
        pieceToMove->update_pos(x, y);
        Piece *opposingPiece = other.pieces.return_piece_at_pos(x, y);
        if (opposingPiece){
            opposingPiece->piece_taken();
        }
        board.update_board(x, y, currPos.first, currPos.second, pieceToMove->return_symbol());
    }

    // return if it is possible for a player to move specified piece to the specified x y position
    // if not possible errorMsg bool specifies whether to print why to cout
    bool move_piece_possible(Board &board, Piece *move, Player &other, int x, int y, bool errorMsg){
        if (!move){
            return false;
        }
        if (!(x >= 0 && x <= 7) || !(y >= 0 && y <= 7)){
            if(errorMsg){
                std::cout << "This move places piece off the board! ";
            }
            return false;
        } else if (!move->update_pos_possible(x, y)){
            // checks if piece type can move in such a manner ex. knights can only move in L-shape
            if(errorMsg){
                std::cout << "You simply can't do that! ";
            }
            return false;
        } else if (piece_in_way(other, board, move, x, y)){
            if(errorMsg){
                std::cout << "There is a piece blocking your path. ";
            }
            return false;
        } else if (move->return_symbol() == 'P' && y != move->return_pos().second){
            // ensures that if a pawn is moving diagnolly then it is taking an opponents piece
            TileOwner owner = board.return_owner(move->return_pos().first, move->return_pos().second);
            if (owner == board.return_owner(x, y) || board.return_owner(x, y) == nobody){
                if (errorMsg){
                    std::cout << "Cheeky! Pawns can't move like that. ";
                }
                return false;
            } 
        }
        // if opponent has piece at square then take it off board so it cause a false check error
        Piece *pieceAtPos = other.pieces.return_piece_at_pos(x,y);
        if (pieceAtPos){
            pieceAtPos->update_pos(-10,-10);
        }
        // simulate board change if move completed and ensure that it does not cause a check
        board.change_tile_symbol('*', move->return_pos().first, move->return_pos().second);
        char currTileSymbol = board.return_symbol(x, y);
        board.change_tile_symbol(move->return_symbol(), x, y);
        std::pair<int, int> kingPos = pieces.return_piece(12)->return_pos();
        int kingX = kingPos.first;
        int kingY = kingPos.second;
        if (move->return_symbol() == 'K'){
            kingX = x;
            kingY = y;
        }
        if (check(other, board, kingX, kingY)){
            // make sure move does not cause a check
            if(errorMsg){
                std::cout << "Invalid! You are still under check or place yourself under check with this move";
            }
            if (pieceAtPos){
                pieceAtPos->update_pos(x, y);
            }
            // set board back to what it was origianlly
            board.change_tile_symbol(move->return_symbol(), move->return_pos().first, move->return_pos().second);
            board.change_tile_symbol(currTileSymbol, x, y);
            return false;
        }
        // place opponents piece back at position as "check for check" is complete
        if (pieceAtPos){
            pieceAtPos->update_pos(x, y);
        }
        // set board back to what it was originally
        board.change_tile_symbol(move->return_symbol(), move->return_pos().first, move->return_pos().second);
        board.change_tile_symbol(currTileSymbol, x, y);
        return true; 
    }

    // returns if a player is under check
    bool check(const Player &other, Board &board, int kingX, int kingY){
        for (size_t i = 0; i < 16; i++){
            // loop through opponents pieces and see if they can move to king position
            Piece *piece = other.pieces.return_piece(i);
            if (piece->update_pos_possible(kingX, kingY) && !other.piece_in_way(*this, board, piece, kingX, kingY)
                && piece->return_state() == alive){
                return true;
            }
        }
        return false;
    }

    // Returns a vector of piece pointers that contains the opponents pieces that are threatening your king
    // Used in checkmate function to help determine if player can escape checkmate
    std::vector<Piece *> pieces_threatening_king(const Player &other, Board &board, int kingX, int kingY){
        std::vector<Piece *> threateningPieces;
        for (size_t i = 0; i < 16; i++){
            // loop through opponents pieces and see if they can take your king
            Piece *piece = other.pieces.return_piece(i);
            if (piece->update_pos_possible(kingX, kingY) && !other.piece_in_way(*this, board, piece, kingX, kingY)
                && piece->return_state() == alive){
                threateningPieces.push_back(piece);
            }
        }
        return threateningPieces;
    }
    
    // returns if a player is checkmated(game over)
    bool check_mate(Player &other, Board &board){
        Piece *king = pieces.return_piece(12);
        std::pair<int, int> kingPos = king->return_pos();
        int kingX = kingPos.first;
        int kingY = kingPos.second;

        // can't be checkmated if not under check already
        if (check(other, board, kingX, kingY)){
            // first check if you can move king to escape check
            if (move_piece_possible(board, king, other, kingX - 1, kingY - 1, false)){
                return false;
            } else if (move_piece_possible(board, king, other, kingX - 1, kingY, false)){
                return false;
            } else if (move_piece_possible(board, king, other, kingX - 1, kingY + 1, false)){
                return false;
            } else if (move_piece_possible(board, king, other, kingX, kingY - 1, false)){
                return false;
            } else if (move_piece_possible(board, king, other, kingX, kingY + 1, false)){
                return false;
            } else if (move_piece_possible(board, king, other, kingX + 1, kingY - 1, false)){
                return false;
            } else if (move_piece_possible(board, king, other, kingX + 1, kingY, false)){
                return false;
            } else if (move_piece_possible(board, king, other, kingX + 1, kingY + 1, false)){
                return false;
            }

            // next check if player can move one of other pieces to escape check
            std::vector<Piece *> threateningPieces = pieces_threatening_king(other, board, kingX, kingY);
            // if cant move king to escape check and king threatened by more than 1 piece then checkmate
            if (threateningPieces.size() > 1){
                return true;
            } else {
                // see if player can move one of its pieces to kill piece threatening king
                std::pair<int, int> threateningPiecePos = threateningPieces[0]->return_pos();
                int threateningX = threateningPiecePos.first;
                int threateningY = threateningPiecePos.second;
                for (size_t i = 0; i < 16; i++){
                    Piece *piece = pieces.return_piece(i);
                    if (piece->return_state() == dead){
                        continue;
                    }
                    if (move_piece_possible(board, piece, other, threateningX, threateningY, false)){
                        return false;
                    }
                }
                // check if player can move one of its pieces to block threatening piece
                char typeThreateningPiece = threateningPieces[0]->return_symbol();
                // pawn or knight impossible to block
                if (typeThreateningPiece == 'P' || typeThreateningPiece == 'N'){
                    return true;
                } else if (typeThreateningPiece == 'R'){
                    // check if rook threatening king can be blocked
                    for (size_t i = 0; i < 16; i++){
                        Piece *piece = pieces.return_piece(i);
                        if (piece->return_state() == dead){
                            continue;
                        }
                        if (piece_block_row_check(board, piece, other, kingX, kingY, threateningX, threateningY)){
                            return false;
                        }
                    }
                    return true;
                } else if (typeThreateningPiece == 'B'){
                    for (size_t i = 0; i < 16; i++){
                        // check if bishop threatening king can be blocked
                        Piece *piece = pieces.return_piece(i);
                        if (piece->return_state() == dead){
                            continue;
                        }
                        if (piece_block_diagnol_check(board, piece, other, kingX, kingY, threateningX, threateningY)){
                            return false;
                        }
                    }
                    return true;
                } else {
                    // opponent queen: can move diagnol or straight so two control levels
                    int xDiff = kingX - threateningX;
                    int yDiff = kingY - threateningY;
                    if (xDiff == 0 || yDiff == 0 ){
                        // check if queen threatening king with straight move can be blocked
                        for (size_t i = 0; i < 16; i++){
                            Piece *piece = pieces.return_piece(i);
                            if (piece->return_state() == dead){
                                continue;
                            }
                            if (piece_block_row_check(board, piece, other, kingX, kingY, threateningX, threateningY)){
                                return false;
                            }
                        }
                        return true;
                    } else {
                        // check if queen threatening king with diagnol move can be blocked
                        for (size_t i = 0; i < 16; i++){
                            Piece *piece = pieces.return_piece(i);
                            if (piece->return_state() == dead){
                                continue;
                            }
                            if (piece_block_diagnol_check(board, piece, other, kingX, kingY, threateningX, threateningY)){
                                return false;
                            }
                        }
                        return true;
                    }
                }
            }
        } else{
            // should not reach here but if checkmate not proven yet then assumed no checkmate
            return false;
        }
    }

    // returns if specified piece can move to a square to block a piece threatening king in same row as king
    bool piece_block_row_check(Board &board, Piece *move, Player &other, int kingX, int kingY, int threatX, int threatY){
        int xDiff = kingX - threatX;
        int yDiff = kingY - threatY;

        // 4 control flow statements for if threatening piece is in front of king, behind king,
        // to the left of king, or to the right of king
        if (xDiff > 0) {
            threatX++;
            while (threatX != kingX){
                if (move_piece_possible(board, move, other, threatX, threatY, false)){
                    return true;
                }
                threatX++;
            }
            return false;
        } else if (xDiff < 0){
            threatX--;
            while (threatX != kingX){
                if (move_piece_possible(board, move, other, threatX, threatY, false)){
                    return true;
                }
                threatX--;
            }
            return false;
        } else if (yDiff > 0){
            threatY++;
            while (threatY != kingY){
                if (move_piece_possible(board, move, other, threatX, threatY, false)){
                    return true;
                }
                threatY++;
            }
            return false;
        } else {
            threatY--;
            while (threatY != kingY){
                if (move_piece_possible(board, move, other, threatX, threatY, false)){
                    return true;
                }
                threatY--;
            }
            return false;
        }
    }

    // returns if specified piece can move to a square to block a piece threatening king diagnoly
    bool piece_block_diagnol_check(Board &board, Piece *move, Player &other, int kingX, int kingY, int threatX, int threatY){
        int xDiff = kingX - threatX;
        int yDiff = kingY - threatY;

        // 4 control flow statements for if threatening piece is in front left of king, front right,
        // back left, or back right
        if (xDiff > 0 && yDiff > 0){
            threatX++;
            threatY++;
            while (threatX != kingX){
                if (move_piece_possible(board, move, other, threatX, threatY, false )){
                    return true;
                }
                threatX++;
                threatY++;
            }
            return false;
        } else if (xDiff > 0 && yDiff < 0){
            threatX++;
            threatY--;
            while (threatX != kingX){
                if (move_piece_possible(board, move, other, threatX, threatY, false )){
                    return true;
                }
                threatX++;
                threatY--;
            }
            return false;
        } else if (xDiff < 0 && yDiff < 0){
            threatX--;
            threatY--;
            while (threatX != kingX){
                if (move_piece_possible(board, move, other, threatX, threatY, false )){
                    return true;
                }
                threatX--;
                threatY--;
            }
            return false;
        } else {
            threatX--;
            threatY++;
            while (threatX != kingX){
                if (move_piece_possible(board, move, other, threatX, threatY, false )){
                    return true;
                }
                threatX--;
                threatY++;
            }
            return false;
        }
    }
    
    // returns if there is a piece blocking a desired move path for a piece
    bool piece_in_way(const Player &other, const Board &board, Piece *move, int x, int y) const{
        std::pair<int,int> currPos = move->return_pos();
        int currX = currPos.first;
        int currY = currPos.second;

        int xDiff = x - currX;
        int yDiff = y - currY;

        char type = move->return_symbol();

        // if player already has a piece at desired position for moving piece 
        // then piece in way as player can't take their own piece
        if (pieces.piece_at_pos(x, y)){
            return true;
        }

        if (type == 'P'){
            // pawn can't move forward a square if opponent has a piece there
            if (y == currY){
                if (other.pieces.piece_at_pos(x, y)){
                    return true;
                }
            }
            // pawn can always take opponents piece diagnolly
            return false;
        } else if (type == 'R'){
            return board.piece_blocking_straight_move(x, y, currX, currY);
        } else if (type == 'B'){
            return board.piece_blocking_diagnol_move(x, y, currX, currY);
        } else if (type == 'Q'){
            // queen can move diagnolly or straight
            if (xDiff == 0 || yDiff == 0){
                return board.piece_blocking_straight_move(x, y, currX, currY);
            } else{
                return board.piece_blocking_diagnol_move(x, y, currX, currY);
            }
        } else{
            return false;
        }
    }
};
#endif