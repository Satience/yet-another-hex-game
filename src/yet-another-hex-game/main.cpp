/**
 * Author:    Artyom Timonin
 * Created:   23.10.2020
 *
 **/

#include <iostream>
#include <iomanip>
#include <vector>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;


#ifdef __APPLE__
const char BLUE[] = "\033[34m";//Blue color
const char RED[] = "\033[31m"; //Red color
const char RESET[] = "\033[0m";//Reset color manipulator
#endif

#ifdef _WIN32 //Poor Windows users cannot see colored output
const char BLUE[] = "\0"; //Blue color
const char RED[] = "\0";  //Red color
const char RESET[] = "\0";//Reset color manipulator
#endif

#ifdef __unix__
const char BLUE[] = "\033[34m";//Blue color
const char RED[] = "\033[31m"; //Red color
const char RESET[] = "\033[0m";//Reset color manipulator
#endif

enum class cell {EMPTY, RED, BLUE};

class hexagon{
public:
    hexagon(int i, int j, int n):number(n),coord_i(i),coord_j(j){
        prorerty = cell::EMPTY;
    }
    inline int GetNumber(){ return number; }
    inline cell GetProperty(){ return prorerty; }
    inline void SetProperty(cell p){ prorerty = p;}
private:
    friend class Board;
    const int number;
    const int coord_i;
    const int coord_j;
    cell prorerty;
};

ostream& operator<<(ostream& out, cell property){
    if(property == cell::BLUE) cout << BLUE << "BLU" << RESET;
    if(property == cell::RED) cout << RED << "RED" << RESET;
    return out;
}

class Board{
public:
    Board(const int s):board_size(s), vertices(s*s){
        edges = 0;
        adjacency_list = new vector<hexagon>[vertices];
        int hexagon_number = 0;

        //creating an array of vectors, each vector contains a vertice as first element
        for(int i = 0; i < board_size; ++i)
            for(int j = 0; j < board_size; ++j){
                hexagon tmp_hex(i,j,hexagon_number);
                adjacency_list[hexagon_number].push_back(tmp_hex);
                hexagon_number++;
            }
        /* next routine creates adjacency edge list
         * internally each hexagon has not only it's number
         * but also coordinates i,j:
         *      ___/0,2\
         *  ___/0,1\___/...
         * /0,0\___/1,1\
         * \___/1,0\___/...
         *     \___/2,0\
         *         \___/...
         * each hexagon has 6 neighbours, so the routine starts from
         * one neighbour and checks all the neighbours counterclockwise.
         * the routine checks neighbour coordinates for negative values
         * and board boundaries and, if the hexagon coordinates meet
         * the criteria, the routine adds an edge between two hexagons
         */
        hexagon_number = 0;
        for(int i = 0; i < board_size; ++i)
            for(int j = 0; j < board_size; ++j){
                if(IsHexagonExist(i,--j)) adjacency_list[hexagon_number].push_back(*adjacency_list[FindHexagonByCoords(i,j)].begin());
                if(IsHexagonExist(++i,j)) adjacency_list[hexagon_number].push_back(*adjacency_list[FindHexagonByCoords(i,j)].begin());
                if(IsHexagonExist(i,++j)) adjacency_list[hexagon_number].push_back(*adjacency_list[FindHexagonByCoords(i,j)].begin());
                if(IsHexagonExist(--i,++j)) adjacency_list[hexagon_number].push_back(*adjacency_list[FindHexagonByCoords(i,j)].begin());
                if(IsHexagonExist(--i,j)) adjacency_list[hexagon_number].push_back(*adjacency_list[FindHexagonByCoords(i,j)].begin());
                if(IsHexagonExist(i,--j)) adjacency_list[hexagon_number].push_back(*adjacency_list[FindHexagonByCoords(i,j)].begin());
                ++i;
                hexagon_number++;
            }
    }
    ~Board(){
        delete [] adjacency_list;
    }
    //checking if a neighbour hexagon exist on the board
    bool IsHexagonExist(const int i, const int j){
        if(i >= 0 && i < board_size && j >= 0 && j < board_size) return true;
                else return false;
    }
    //returns a hexagon number by it's internal coordinates i,j
    int FindHexagonByCoords(int coord_i, int coord_j){
        for(int hex_num = 0; hex_num < vertices; ++hex_num){
            if(adjacency_list[hex_num].begin()->coord_i == coord_i &&
                    adjacency_list[hex_num].begin()->coord_j == coord_j)
                return hex_num;
        }
        return -1;
    }

    //Fancy Hex board printout routine
    void PrintBoardCoords(){
        int start_hex = board_size - 1;
        int row_size = 1;
        //header
        for(int space = 0; space < board_size * 3 + board_size - 15; ++space) cout << " ";
        cout << "Player Side ___ CPU Side" << endl;
        //header end
        for(int row = 1; row < board_size * 2; ++row){
            int offset = start_hex;

            //filling the space to the left of the board
            int space_border = (board_size - row_size) * 4;
            for(int space = 0; space < space_border; ++space) {
                if(row > board_size && space_border - space == 4) cout << "\\";
                else if(space_border - space > 3) cout << " ";
                else cout << "_";
            }
            //filling routine end

            //hexagon printing routine
            for(int hex_num = 0; hex_num < row_size; ++hex_num){

                if(adjacency_list[offset].begin()->GetProperty() == cell::EMPTY)
                cout << "/" << setw(3) <<adjacency_list[offset].begin()->number << "\\";
                else cout << "/" << adjacency_list[offset].begin()->prorerty << "\\";

                if((row_size - hex_num) > 1) {offset += board_size + 1; cout << "___";}
                else if(row < board_size) cout << "___";
                else if(row > board_size) cout << "___/";
            }
            if(row < board_size){ ++row_size; --start_hex;}
            else {--row_size; start_hex += board_size;}
            cout << endl;
            //hexagon printing routine end
        }

        //footer
        for(int space = 0; space < board_size * 3 + board_size - 13; ++space) cout << " ";
        cout << "CPU Side \\___/ Player Side" << endl;
        //footer end
    }

    inline int GetVertices(){ return vertices; }
    inline int GetEdges(){ return edges; }
    inline vector<hexagon>* GetAdjacencyList(){ return adjacency_list; }

private:
    const int board_size;
    const int vertices;
    vector<hexagon>* adjacency_list;
    int edges;
};

//Graph adjacency list printout operator, useful for debug purposes
ostream& operator<<(ostream& out, Board& B){
    for(int i = 0; i < B.GetVertices(); ++i){
        cout << "Adjacency list of vertice ";
        for(vector<hexagon>::iterator iter = B.GetAdjacencyList()[i].begin(); iter != B.GetAdjacencyList()[i].end(); ++iter){
            cout << iter->GetNumber() << " -> ";
        }
        cout << endl;
    }
    return out;
}

class Player{
public:
    Player():player_color(cell::EMPTY){}
    ~Player(){}
    inline cell GetPlayerColor(){ return player_color; }
    inline void SetPlayerColor(cell color){ player_color = color; }
private:
    cell player_color;
};


int main()
{
    /* The program represents the 11 by 11 hex game board.
     * There is no CPU player logic, because this topic
     * hasn't been covered in the course yet. The most
     * "advanced" logic I can implement right now is a random
     * play for a CPU player...
     * At least human player can choose his color and make
     * some moves on the board, all necessary checks for user
     * input are implemented.
     * The Board class can generate a hex board of any size,
     * but currently it's not recommended to generate a board
     * of size less than 4, because current formatting will fail
     * (needs to be fixed later).
     * Also, try to run this program under Linux or MacOS, you will
     * see the fancy colored output!
    */
    Board hex(11);
    Player player;
    //cout << hex << endl; //printout of the board graph adjacency list

    string player_input;
    bool color_valid = false;
    while(!color_valid){
        cout << "Choose your color! (R or B) " << endl;
        cin >> player_input;
        switch (*player_input.begin()) {
        case 'R': player.SetPlayerColor(cell::RED); color_valid = true; break;
        case 'B': player.SetPlayerColor(cell::BLUE); color_valid = true; break;
        default : cout << "Wrong color entered..." << endl; break;
        }
        player_input.clear();
    }
    while(1){
        cout << "Make your move by choosing the hexagon number, or press Q to quit the game!" << endl << endl;
        hex.PrintBoardCoords();
        cin >> player_input;
        switch (*player_input.begin()) {
        case 'Q' : return 0;
        default: {
            int player_move = 0;
            try {
                player_move = stoi(player_input);
            }  catch (std::invalid_argument) {
                cout << player_input << " is not a number!" << endl;
                break;
            }
            if(player_move >= 0 && player_move < hex.GetVertices())
                hex.GetAdjacencyList()[player_move].begin()->SetProperty(player.GetPlayerColor());
            else cout << "the hexagon " << player_input << " is out of the board!" << endl;
            player_input.clear();
            break;
            }
        }
    }

    return 0;
}
