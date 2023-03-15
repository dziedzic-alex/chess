#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <iostream>

enum TileOwner{white, red, nobody};

// class that represents a single tile on the chess board
class Tile{
    TileOwner owner;
    char symbol;

    public:
    Tile()
        : owner{nobody}, symbol{'*'}{}
    
    // changes the "owner" of a tile to white red or nobody
    // primarily used to update which team is occupying a tile when a piece is moved
    // if tile is vacant team can be "nobody"
    void change_owner(const TileOwner newOwner, char newSymbol){
        owner = newOwner;
        symbol = newSymbol;
    }

    // returns the current owner or team occupying tile
    TileOwner return_owner() const{
        return owner;
    }

    // changes the symbol that the tile is represented by
    // used to update the board when pieces are moved
    void change_symbol(char newSymbol){
        symbol = newSymbol;
    }

    // return the symbol of the tile
    // used to check if a piece is occupying a tile
    char return_symbol() const{
        return symbol;
    }
};


// class that represents visual state of chess board
class Board{
    // 2d board
    std::vector<std::vector<Tile>> board;
    public:
    // initializes chess board to default state
    Board()
        : board{std::vector<std::vector<Tile>>(8, std::vector<Tile>(8))}{
            board[0][0].change_owner(red, 'R');
            board[0][1].change_owner(red, 'N');
            board[0][2].change_owner(red, 'B');
            board[0][3].change_owner(red, 'Q');
            board[0][4].change_owner(red, 'K');
            board[0][5].change_owner(red, 'B');
            board[0][6].change_owner(red, 'N');
            board[0][7].change_owner(red, 'R');

            for (size_t i = 0; i < 8; i++){
                board[1][i].change_owner(red, 'P');
            }

            for (size_t i = 0; i < 8; i++){
                board[6][i].change_owner(white, 'P');
            }

            board[7][0].change_owner(white, 'R');
            board[7][1].change_owner(white, 'N');
            board[7][2].change_owner(white, 'B');
            board[7][3].change_owner(white, 'Q');
            board[7][4].change_owner(white, 'K');
            board[7][5].change_owner(white, 'B');
            board[7][6].change_owner(white, 'N');
            board[7][7].change_owner(white, 'R');
        }

    // update board after moving piece
    void update_board(int newX, int newY, int oldX, int oldY, char pieceType){
        TileOwner owner = board[oldX][oldY].return_owner();
        board[oldX][oldY].change_owner(nobody, '*');
        board[newX][newY].change_owner(owner, pieceType);
    }

    // change tile symbol on board at specified position
    void change_tile_symbol(char symbol, int x, int y){
        board[x][y].change_symbol(symbol);
    }

    // return tile symbol on board at specified position
    char return_symbol(int x, int y) const{
        return board[x][y].return_symbol();
    }
    
    // return player occupying tile on board or if it is vacant
    TileOwner return_owner(int x, int y) const{
        return board[x][y].return_owner();
    }
    
    // return if there is a piece within a straight line region formed by 
    // the current and desired position to move to
    // created to speed up checking if a piece is in a straight region
    // as its quicker than looping through piece set for every piece in region
    // as this has a maximum of 8 checks(8x8 board) compared to 128(16*8)
    bool piece_blocking_straight_move(int x, int y, int currX, int currY) const{
        int xDiff = x - currX;
        int yDiff = y - currY;
        
        // 4 control statments for if region from currX to desired x is moving forward,
        // backward, left, or right
        if (xDiff > 0){
            currX++;
            while (currX != x){
                if (board[currX][currY].return_symbol() != '*'){
                    return true;
                }
                currX++;
            }
            return false;
        } else if (xDiff < 0){
            currX--;
            while (currX != x){
                if (board[currX][currY].return_symbol() != '*'){
                    return true;
                }
                currX--;
            }
            return false;
        } else if (yDiff > 0){
            currY++;
            while (currY != y){
                if (board[currX][currY].return_symbol() != '*'){
                    return true;
                }
                currY++;
            }
            return false;
        } else{
            currY--;
            while (currY != y){
                if (board[currX][currY].return_symbol() != '*'){
                    return true;
                }
                currY--;
            }
            return false;
        }
    }

    // return if there is a piece within a diagnol region formed by 
    // the current and desired position to move to
    // created to speed up checking if a piece is in a diagnol region
    // as its quicker than looping through piece set for every piece in region
    // as this has a maximum of 8 checks(8x8 board) compared to 128(16*8)
    bool piece_blocking_diagnol_move(int x, int y, int currX, int currY) const{
        int xDiff = x - currX;
        int yDiff = y - currY;

        // 4 control statments for if region from currX to desired x is moving forward right,
        // forward left, backward right, or  backward left
        if (xDiff < 0 && yDiff < 0){
            currX--;
            currY--;
            while (currX != x){
                if (board[currX][currY].return_symbol() != '*'){
                    return true;
                }
                currX--;
                currY--;
            }
            return false;
        } else if (xDiff < 0 && yDiff > 0){
            currX--;
            currY++;
            while (currX != x){
                if (board[currX][currY].return_symbol() != '*'){
                    return true;
                }
                currX--;
                currY++;
            }
            return false;
        } else if (xDiff > 0 && yDiff < 0){
            currX++;
            currY--;
            while (currX != x){
                if (board[currX][currY].return_symbol() != '*'){
                    return true;
                }
                currX++;
                currY--;
            }
            return false;
        } else{
            currX++;
            currY++;
            while (currX != x){
                if (board[currX][currY].return_symbol() != '*'){
                    return true;
                }
                currX++;
                currY++;
            }
            return false;
        }

    }

    // prints the board to terminal
    void print_board () const{
        const std::string RED_TEXT = "\033[31m";
        const std::string RESET_COLOR = "\033[0m";
        std::cout << "   ";
        for (size_t col = 0; col < board[0].size(); col++){
            std::cout << col << " ";
        }
        std::cout << "\n";
        for (size_t row = 0; row < board.size(); row++){
            std::cout << row << "  ";
            for (size_t col = 0; col < board[0].size(); col++){
                if(board[row][col].return_owner() == red){
                    std::cout << RED_TEXT << board[row][col].return_symbol() << " " << RESET_COLOR;
                } else {
                    std::cout << board[row][col].return_symbol() << " ";
                } 
            }
            std::cout << "\n";
        }
    }
};


// overloaded output operator to make convient board printing
std::ostream & operator<<(std::ostream &os, const Board &board){
    board.print_board();
    return os;
}

#endif