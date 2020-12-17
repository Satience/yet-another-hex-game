/**
 * Author:    Artyom Timonin
 * Created:   23.10.2020
 *
 **/

#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include <set>
#include <list>

#include <cctype>
#include <cstdlib>

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

const int num_of_simulations = 250;

enum class cell_property {EMPTY, RED, BLUE};
enum class state {STARTGAME, GAMEINPROGRESS, ENDGAME};

class hexagon{
public:
    //default constructor
    hexagon(int i, int j, int n):number(n),coord_i(i),coord_j(j){
        property = cell_property::EMPTY;
        winrate = 0;
    }
    //default constructor without coordinates, if not needed
    hexagon(int n):number(n),coord_i(0),coord_j(0){
        property = cell_property::EMPTY;
        winrate = 0;
    }
    //deep-copy constructor
    hexagon(const hexagon& other):number(other.number),coord_i(other.coord_i),coord_j(other.coord_j){
        property = other.property;
        winrate = other.winrate;
    }
    ~hexagon(){}
    hexagon& operator=(const hexagon& other){
        this->property = other.property;
        return *this;
    }
    inline int GetNumber(){ return number; }
    inline cell_property GetProperty(){ return property; }
    inline void SetProperty(cell_property p){ property = p;}
    inline int GetWinrate(){ return winrate;}
    inline void SetWinrate(int w){ winrate = w;}

private:
    friend class Board;
    const int number;
    const int coord_i;
    const int coord_j;
    int winrate;
    cell_property property;
};
//a couple useful operators for debug purposes
ostream& operator<<(ostream& out, cell_property property){
    if(property == cell_property::BLUE) cout << BLUE << "BLU" << RESET;
    if(property == cell_property::RED) cout << RED << "RED" << RESET;
    if(property == cell_property::EMPTY) cout << "EMT";
    return out;
}

ostream& operator<<(ostream& out, vector<int> vector){
    for(auto i = vector.begin(); i != vector.end(); ++i)
        cout << *i << " ->";
    cout << endl;
    return out;
}

class Board{
public:
    Board(const int s):board_size(s), vertices(s*s){
        adjacency_list = new vector<hexagon>[vertices];
        int hexagon_number = 0;

        //creating an array of vectors, each vector contains a vertice as first element
        for(int i = 0; i < board_size; ++i)
            for(int j = 0; j < board_size; ++j){
                hexagon tmp_hex(i,j,hexagon_number);
                adjacency_list[hexagon_number].push_back(tmp_hex);
                ++hexagon_number;
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
                ++hexagon_number;
            }
    }
    //deep-copy constructor
    Board(const Board& other):board_size(other.board_size), vertices(other.vertices){// deep copy constructor
        adjacency_list = new vector<hexagon>[vertices];
        //creating an array of vectors, each vector contains a vertice as first element
        for(int hexagon_number = 0; hexagon_number < vertices; ++hexagon_number){
            for(std::vector<hexagon>::iterator i = other.adjacency_list[hexagon_number].begin(); i != other.adjacency_list[hexagon_number].end(); ++i){
                hexagon tmp_hex(*i);
                adjacency_list[hexagon_number].push_back(tmp_hex);
            }
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
    void PrintBoardCoords() const{
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

                if(adjacency_list[offset].begin()->GetProperty() == cell_property::EMPTY)
                cout << "/" << setw(3) <<adjacency_list[offset].begin()->number << "\\";
                else cout << "/" << adjacency_list[offset].begin()->property << "\\";

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
    //Winrate Map printing routine
    void PrintBoardWinrates() const{
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
                cout << "/" << setw(3) << adjacency_list[offset].begin()->GetWinrate() << "\\";
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
    void MakeAMove(const int hexagon_number, const cell_property property){
        for(std::vector<hexagon>::iterator i = adjacency_list[hexagon_number].begin(); i != adjacency_list[hexagon_number].end(); ++i){
            for(std::vector<hexagon>::iterator j = adjacency_list[i->GetNumber()].begin(); j != adjacency_list[i->GetNumber()].end(); ++j){
                if(j->GetNumber() == hexagon_number) j->SetProperty(property);
            }
        }
    }
    bool IsReachable(const int src, const int destination, cell_property property){
        int source = src;
        // Base case
        if (source == destination)
            return true;
        // Mark all the vertices as not visited
        bool *visited = new bool[vertices]; //array of bools
        for (int i = 0; i < vertices; ++i)
            visited[i] = false;
        // Create a queue for BFS
        list<int> queue;

        // Mark the current node as visited and enqueue it
        visited[source] = true;
        queue.push_back(source);

        // it will be used to get all adjacent vertices of a vertex
        vector<hexagon>::iterator i;
        while (!queue.empty())
        {
            // Dequeue a vertex from queue and print it
            source = queue.front();
            queue.pop_front();

            // Get all adjacent vertices of the dequeued vertex s
            // If a adjacent has not been visited, then mark it visited
            // and enqueue it
            for (i = ++adjacency_list[source].begin(); i != adjacency_list[source].end(); ++i){
                if(i->GetProperty() == property){
                    // If this adjacent node is the destination node, then
                    // return true
                    if ( i->GetNumber() == destination)
                        return true;
                    // Else, continue to do BFS
                    if (!visited[i->GetNumber()])
                    {
                        visited[i->GetNumber()] = true;
                        queue.push_back(i->GetNumber());
                    }
                }
            }
        }
        delete [] visited;
        // If BFS is complete without visiting d
        return false;
    }
    cell_property WhoIsWon(){
        //check if a stone is placed near any border,
        //and if yes, proceed to the path search algorithm
        vector<int> *north_border = new vector<int>;
        vector<int> *south_border = new vector<int>;
        vector<int> *east_border = new vector<int>;
        vector<int> *west_border = new vector<int>;

        bool stone_at_border = false;
        for(int i = 0; i < board_size; ++i){ //north border
            if(GetAdjacencyList()[i].begin()->GetProperty() != cell_property::EMPTY) {stone_at_border = true; north_border->push_back(i);}
        }
        for(int i = vertices - board_size; i < vertices; ++i){ //south border
            if(GetAdjacencyList()[i].begin()->GetProperty() != cell_property::EMPTY) {stone_at_border = true; south_border->push_back(i);}
        }
        for(int i = 0; i <= vertices - board_size; i += board_size){ //west border
            if(GetAdjacencyList()[i].begin()->GetProperty() != cell_property::EMPTY) {stone_at_border = true; west_border->push_back(i);}
        }
        for(int i = board_size - 1; i < vertices; i += board_size){ //east border
            if(GetAdjacencyList()[i].begin()->GetProperty() != cell_property::EMPTY) {stone_at_border = true; east_border->push_back(i);}
        }
        if(!stone_at_border) return cell_property::EMPTY;
        //path search algorithm, player's path is checked first
        if(!north_border->empty())
            if(!south_border->empty()){
                for(std::vector<int>::iterator s = north_border->begin(); s != north_border->end(); ++s){// for every source...
                    for(std::vector<int>::iterator d = south_border->begin(); d != south_border->end(); ++d){//...check every destination
                        if(IsReachable(*s, *d, GetAdjacencyList()[*s].begin()->GetProperty())){
                            //cout << "Player has won!" << endl;
                            return GetAdjacencyList()[*s].begin()->GetProperty();
                        }
                    }
                }
            }
        if(!west_border->empty())
            if(!east_border->empty()){
                for(std::vector<int>::iterator s = west_border->begin(); s != west_border->end(); ++s){// for every source...
                    for(std::vector<int>::iterator d = east_border->begin(); d != east_border->end(); ++d){//...check every destination
                        if(IsReachable(*s, *d, GetAdjacencyList()[*s].begin()->GetProperty())){
                            //cout << "CPU has won!" << endl;
                            return GetAdjacencyList()[*s].begin()->GetProperty();
                        }
                    }
                }
            }
        delete north_border;
        delete south_border;
        delete west_border;
        delete east_border;

        return cell_property::EMPTY; //no winner yet
    }
    void EraseProperties(vector<int>& hexes_to_clear){
        for(std::vector<int>::iterator i = hexes_to_clear.begin(); i != hexes_to_clear.end(); ++i){
            adjacency_list[*i].begin()->SetProperty(cell_property::EMPTY);
        }
    }
    void EraseProperties(int hex_to_clear){
        adjacency_list[hex_to_clear].begin()->SetProperty(cell_property::EMPTY);
    }
    inline int GetVertices(){ return vertices; }
    inline vector<hexagon>* GetAdjacencyList(){ return adjacency_list; }

private:
    const int board_size;
    const int vertices;
    vector<hexagon>* adjacency_list;
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
    Player():player_color(cell_property::EMPTY){}
    ~Player(){}
    inline cell_property GetPlayerColor(){ return player_color; }
    inline void SetPlayerColor(cell_property color){ player_color = color; }
private:
    cell_property player_color;
};

class Game{
public:
    //default constructor
    Game(Board& user_board, Player& user):game_board(user_board),game_state(state::STARTGAME){
        if(user.GetPlayerColor() == cell_property::BLUE)
            cpu_player_property = cell_property::RED;
        else
            cpu_player_property = cell_property::BLUE;
        game_state = state::GAMEINPROGRESS;
    }
    //updates Game's board with User board
    void UpdateGame(Board& user_board){
        for(int hexagon_number = 0; hexagon_number < user_board.GetVertices(); ++hexagon_number){
            game_board.GetAdjacencyList()[hexagon_number].clear();
            for(std::vector<hexagon>::iterator i = user_board.GetAdjacencyList()[hexagon_number].begin(); i != user_board.GetAdjacencyList()[hexagon_number].end(); ++i){
                hexagon tmp_hex(*i);
                game_board.GetAdjacencyList()[hexagon_number].push_back(tmp_hex);
            }
        }
    }
    void DoTheMath(Board& user_board){
        UpdateGame(user_board);
        //check if human player is won, and, if not, continue to play
        if(user_board.WhoIsWon() != cell_property::EMPTY){
            game_state = state::ENDGAME;
        }
        if(game_state == state::GAMEINPROGRESS){
            //cpu player choosing the next hexagon to play
            vector<int> template_hexes_to_calc;
            for (int hexagon_number = 0; hexagon_number < game_board.GetVertices(); ++hexagon_number){
                if(game_board.GetAdjacencyList()[hexagon_number].begin()->GetProperty() == cell_property::EMPTY){
                    template_hexes_to_calc.push_back(hexagon_number);
                }
            }

            for(int i = 0; i < static_cast<int>(template_hexes_to_calc.size()) ; ++i){
                int hex_to_check = template_hexes_to_calc[i];
                game_board.MakeAMove(hex_to_check, cpu_player_property); // make a CPU move...
                cell_property color_to_move = SwapPlayerColor(cpu_player_property); // ...and turn to a player's side

                int wins = 0;
                for(int sim = 0; sim < num_of_simulations; ++sim){
                    vector<int> empty_hexes = template_hexes_to_calc;
                    empty_hexes.erase(empty_hexes.begin() + i);
                    game_board.EraseProperties(empty_hexes);
                    while(empty_hexes.size()){
                        int rand_hex_index = rand() % empty_hexes.size();
                        game_board.MakeAMove(empty_hexes[rand_hex_index], color_to_move);
                        empty_hexes.erase(empty_hexes.begin() + rand_hex_index);
                        color_to_move = SwapPlayerColor(color_to_move);
                    }
                    if(game_board.WhoIsWon() == cpu_player_property){
                        ++wins;
                        //cout << "CPU WINS" << endl;
                    }
                    else{
                        //cout << "CPU LOSES" << endl;
                    }
                }
                //cout << "hex " << hex_to_check << " winrate " << wins*100/num_of_simulations << endl;
                game_board.GetAdjacencyList()[hex_to_check].begin()->SetWinrate(wins*100/num_of_simulations);
                game_board.EraseProperties(hex_to_check);
            }
            //generate a winrate map, useful for debug purposes
            //game_board.PrintBoardWinrates();
            //pick a best winrate
            int highest_possible_winrate = 0,
                highest_possible_winrate_hex;
            for(int i = 0; i < game_board.GetVertices(); ++i){
                if(game_board.GetAdjacencyList()[i].begin()->GetWinrate() > highest_possible_winrate){
                    highest_possible_winrate = game_board.GetAdjacencyList()[i].begin()->GetWinrate();
                    highest_possible_winrate_hex = i;
                }
            }
            //finally, a CPU move
            user_board.MakeAMove(highest_possible_winrate_hex, cpu_player_property);
            if(user_board.WhoIsWon() != cell_property::EMPTY){
                game_state = state::ENDGAME;
            }
        }
        if(game_state == state::ENDGAME){
            UpdateGame(user_board);
            if(game_board.WhoIsWon() == cpu_player_property)
                cout << "CPU has won!" << endl;
            else cout << "Player has won!" << endl;
        }
    }
    state GetGameState(){ return game_state;}
    inline cell_property SwapPlayerColor(cell_property property){ if(property == cell_property::BLUE) return cell_property::RED; else return cell_property::BLUE; }
    ~Game(){}
private:
    Board game_board;
    state game_state;
    cell_property cpu_player_property;
};



int main()
{
    /* The program represents the 11 by 11 hex game board.
     * The pure Monte-Carlo algorithm is pretty dumb, because
     * statistically at the very beginning of the game there
     * probability of winning the game is still close to 50%,
     * so CPU may make a several "non-sense" moves. This takes
     * place only on large boards, like 11 by 11. Really good the
     * CPU behaves on smaller 5 by 5 boards, where the game
     * situation develops faster. Overall the whole tactics look
     * rather defensive than offensive.
    */
    srand(time(0));
    Board hex(11);
    Player human_player, cpu_player;
    //cout << hex << endl; //printout of the board graph adjacency list

    string player_input;
    bool color_valid = false;
    while(!color_valid){
        cout << "Choose your color! (R or B) " << endl;
        cin >> player_input;
        switch (toupper(*player_input.begin())) {
        case 'R': human_player.SetPlayerColor(cell_property::RED); color_valid = true; break;
        case 'B': human_player.SetPlayerColor(cell_property::BLUE); color_valid = true; break;
        default : cout << "Wrong color entered..." << endl; break;
        }
        player_input.clear();
    }
    //Here the Game object is being created
    Game hexgame(hex, human_player);

    while(1){
        hex.PrintBoardCoords();
        if(hexgame.GetGameState() == state::ENDGAME) break;
        cout << "Make your move by choosing the hexagon number, or press Q to quit the game!" << endl;
        cout << "Your Move - ";
        cin >> player_input;
        cout << endl;
        switch (toupper(*player_input.begin())) {
        case 'Q' : return 0;
        default: {
            int player_move = 0;
            try {
                player_move = stoi(player_input);
            }  catch (std::invalid_argument) {
                cout << player_input << " is not a number!" << endl;
                break;
            }
            if(player_move >= 0 && player_move < hex.GetVertices()){
                hex.MakeAMove(player_move, human_player.GetPlayerColor());
                cout << "Thinking..." << endl;
            }
            else cout << "the hexagon " << player_input << " is out of the board!" << endl;
            player_input.clear();
            break;
            }
        }
        hexgame.DoTheMath(hex);
    }
    return 0;
}
