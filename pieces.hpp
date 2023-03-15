#ifndef PIECES_HPP
#define PIECES_HPP
#include <utility>
#include <vector>

enum State{alive, dead};

// Base class for a chess piece that the actual pieces in chess will be derived from
class Piece{
    int xPos;
    int yPos;
    State existence;
    char symbol;

    public:
    Piece()
        : xPos{0}, yPos{0}, existence{alive}, symbol{'*'}  {}

    // bool to indicate if its possible for a piece of this type to move to in such
    // a manner ex.rooks can only move straight and knights can only move in L-shape
    // virtual as each piece has different constraints on how it should move so 
    // every piece class should define their own
    virtual bool update_pos_possible(int x, int y)const = 0;

    // updates position of piece
    void update_pos(int x, int y){
        xPos = x;
        yPos = y;
    }
    
    // return x,y pair of position
    const std::pair<int, int> return_pos() const{
        return std::make_pair(xPos, yPos);
    }

    // update what symbol represents piece
    void update_symbol(char symb){
        symbol = symb;
    }

    // Returns symbol representing piece
    char return_symbol() const{
        return symbol;
    }

    // update existence piece attributes when it's taken
    void piece_taken(){
        existence = dead;
        update_symbol('*');
        update_pos(-10, -10);
    }
    
    // Return state(if piece has been taken or not yet)
    State return_state() const{
        return existence;
    }
};


class Pawn: public Piece{
    // pawn class must have team attribute because it is only piece
    // that can't move backwards so must know which "side" owns it
    const std::string team;
    public:
    // makes sure pawn is moving forward or diagnol by 1 space
    bool update_pos_possible(int x, int y)const override{
        const std::pair<int, int> pos = return_pos();
        int xDiff = x - pos.first;
        int yDiff = y - pos.second;
        if (team == "white"){
            if (xDiff >= 0){
                return false;
            } else if (xDiff < -1 || abs(yDiff) > 1){
                if (pos.first == 6 && xDiff == -2 && yDiff == 0){
                    return true;
                }
                return false;
            }
            return true;
        } else {
            if (xDiff <= 0){
                return false;
            } else if (xDiff > 1 || abs(yDiff) > 1){
                if (pos.first == 1 && xDiff == 2 && yDiff == 0){
                    return true;
                }
                return false;
            }
            return true;
        }
    }

    Pawn(std::string team, int x, int y)
        : team(team) {
        update_pos(x, y);
        update_symbol('P');
    }
    
};


class Rook: public Piece{
    public:
    // makes sure rook moves in a straight line
    bool update_pos_possible(int x, int y)const override{
        std::pair<int, int> pos = return_pos();
        int xDiff = x - pos.first;
        int yDiff = y - pos.second;
        if (abs(xDiff) > 0 && abs(yDiff) > 0){
            return false;
        } else if (xDiff == 0 && yDiff == 0){
            return false;
        }
        return true;
    }

    Rook(int x, int y){
        update_pos(x, y);
        update_symbol('R');
    }
};


class Knight: public Piece{
    public:
    // makes sure that knight moves in L-shape
    bool update_pos_possible(int x, int y)const override{
        std::pair<int, int> pos = return_pos();
        int xDiff = x - pos.first;
        int yDiff = y - pos.second;
        if ((abs(xDiff) == 2 && abs(yDiff) == 1) || ((abs(yDiff) == 2 && abs(xDiff) == 1))) {
            return true;
        }
        return false;
    }
    // given symbol 'N' because king has symbol 'K'
    Knight(int x, int y){
        update_pos(x, y);
        update_symbol('N');
    }
};


class Bishop: public Piece{
    public:
    // makes sure bishop moves diagnolly
    bool update_pos_possible(int x, int y)const override{
        std::pair<int, int> pos = return_pos();
        int xDiff = x - pos.first;
        int yDiff = y - pos.second;
        if (abs(xDiff) != 0 && abs(xDiff) == abs(yDiff)){
            return true;
        }
        return false;
    }
    Bishop(int x, int y){
        update_pos(x, y);
        update_symbol('B');
    }
};


class Queen: public Piece{
    public:
    // makes sure that queen moves straight or diagnolly
    bool update_pos_possible(int x, int y)const override{
        std::pair<int, int> pos = return_pos();
        int xDiff = x - pos.first;
        int yDiff = y - pos.second;

        if ((abs(xDiff) != 0 && abs(yDiff) != 0) && abs(xDiff) != abs(yDiff)){
            return false;
        } else if (xDiff == 0 && yDiff == 0){
            return false;
        }
        return true;
    }
    Queen(int x, int y){
        update_pos(x, y);
        update_symbol('Q');
    }
};


class King: public Piece{
    public:
    // makes sure that king only moves 1 space in any direction
    bool update_pos_possible(int x, int y)const override{
        std::pair<int, int> pos = return_pos();
        int xDiff = x - pos.first;
        int yDiff = y - pos.second;
        
        if (abs(xDiff) > 1 || abs(yDiff) > 1){
            return false;
        } else if (xDiff == 0 && yDiff == 0){
            return false;
        }
        return true;
    }
    King(int x, int y){
        update_pos(x, y);
        update_symbol('K');
    }
};


// Combines piece types to form a standard set of pieces for a player
class PieceSet{
    std::vector<Piece *> pieces;

    public:

    // upgrades pawn
    // used when a player's pawn reaches the other side of board
    void upgrade_piece(Piece *upgrade, char choice, std::pair<int, int> pos){
        for (auto &piece: pieces){
            if (piece == upgrade){
                delete piece;
                if (choice == 'Q' || choice == 'q'){
                    piece = new Queen(pos.first, pos.second);
                } else if (choice == 'R' || choice == 'r'){
                    piece = new Rook(pos.first, pos.second);
                } else if (choice == 'B' || choice == 'b'){
                    piece = new Bishop(pos.first, pos.second);
                } else{
                    piece = new Knight(pos.first, pos.second);
                }
                break;
            }
        }
    }

    // returns whether any piece from the set is located at the specified position
    bool piece_at_pos(int x, int y) const{
        for (auto piece: pieces){
            if (std::make_pair(x,y) == piece->return_pos()){
                if (piece->return_state() == alive){
                    return true;
                }
            }
        }
        return false;
    }

    // returns pointer to piece in set if any piece at specified location
    // null pointer returned if no piece at position
    Piece *return_piece_at_pos(int x, int y) const{
        for (auto piece: pieces){
            if (std::make_pair(x,y) == piece->return_pos()){
                if (piece->return_state() == alive){
                    return piece;
                }
            }
        }
        return nullptr;
    }

    // return pointer of piece located at specified index in pieces vector
    // used mainly to speed up other functions because pieces are at certain indexes
    // for example can easily get pointer of king without looping through piece vector
    Piece *return_piece(size_t index) const{
        return pieces[index];
    }

    // constructor of piece set that creates a set with the standard quantity
    // and types of chess pieces
    // string passed in specifiy whether to make a set for "white" or "red" player
    // red equivalent to "black" in standard chess
    PieceSet(const std::string team){

        pieces.reserve(16);
        if (team == "red"){
            for (size_t col = 0; col < 8; col++){
                pieces.push_back(new Pawn(team, 1, col));
            }
            pieces.push_back(new Rook(0, 0));
            pieces.push_back(new Knight(0, 1));
            pieces.push_back(new Bishop(0, 2));
            pieces.push_back(new Queen(0, 3));
            pieces.push_back(new King(0, 4));
            pieces.push_back(new Bishop(0, 5));
            pieces.push_back(new Knight(0, 6));
            pieces.push_back(new Rook(0, 7));
        } else {
            for (size_t col = 0; col < 8; col++){
                pieces.push_back(new Pawn(team, 6, col));
            }
            pieces.push_back(new Rook(7, 0));
            pieces.push_back(new Knight(7, 1));
            pieces.push_back(new Bishop(7, 2));
            pieces.push_back(new Queen(7, 3));
            pieces.push_back(new King(7, 4));
            pieces.push_back(new Bishop(7, 5));
            pieces.push_back(new Knight(7, 6));
            pieces.push_back(new Rook(7, 7));
        }
    }

    // Destructor which deletes all pieces in piece set which were allocated onto heap
    // To avoid memory leaks
    ~PieceSet(){
        for (auto piece: pieces){
            delete piece;
        }
    }
};
#endif