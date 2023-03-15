#include "game.hpp"
using namespace std;

int main(){
    const string RED_TEXT = "\033[31m";
    const string RESET_COLOR = "\033[0m";
    cout << "Welcome to Chess by Larry Tingles!\nWhen entering coordinates please use either one of these two formats: \"0 0\" or \"0,0\"\n";
    cout << "First let me acquire your names!\n";
    cout << "Player 1 (white) name: ";
    string p1Name;
    getline(cin, p1Name);
    system("clear");
    cout << RED_TEXT << "Player 2 (red) name: " << RESET_COLOR;
    string p2Name;
    getline(cin, p2Name);
    system("clear");
    Game game = Game(p1Name, p2Name);
    game.conduct_game();
}