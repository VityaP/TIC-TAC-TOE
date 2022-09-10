/*
This program is the private property of Victor Petrosyan. 
Any use without the consent of the author is prohibited.
*/

#include <iostream>
#include <cstring>
#include <cctype>
#include <ctime>
#include <vector>
#include <algorithm>

#include <zmq.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "message.h"
#include "functions.h"

volatile sig_atomic_t PROGRAM_ABORT_HANDLER = PROGRAM_RUN;

std::string StrToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::tolower(c); }
                  );
    return s;
}

void GracefulQuit(int tmp){
	std::cout << "You lost by resignation\n";
	std::cout << "Quitting the game\n";

	PROGRAM_ABORT_HANDLER = PROGRAM_STOP;
}

void PrintMessageAndExitGame(const std::string& mes){
    std::cout << mes << "\n";
    std::cout << "Quitting the game\n";
    exit(0);
}

void SendAndRecieve(message*    mes,
                    int&        s_move,
                    int&        s_status,
                    int&        s_win,
                    int&        s_lose){
    zmq_msg_t request;
    zmq_msg_init_size(&request, sizeof(message));
    memcpy(zmq_msg_data(&request), mes, sizeof(message));
    zmq_msg_send(&request, mes->socket, 0);
    zmq_msg_close(&request);

    zmq_msg_init(&request);
    zmq_msg_recv(&request, mes->socket, 0);
    message* feefback = (message*) zmq_msg_data(&request);
    s_move = feefback->movement;
    s_status = feefback->status;
    s_win = feefback->win;
    s_lose = feefback->lose;
    mes->playertype = feefback->playertype;
    mes->opponentID = feefback->opponentID;
    zmq_msg_close(&request);
}

void ShowMenu(){
    puts("---------------------------MENU---------------------------\n");
    puts("           1 - To play with a certain player              \n");
    puts("             2 - To play with a random player             \n");
    puts("                                                          \n");
    puts("           3 - To view personal statistics                \n");
    puts("             4 - To view general statistics               \n");
    puts("           5 - To view database                           \n");
    puts("             6 - To delete your profile from DB           \n");
    puts("----------------------------------------------------------\n");
}

int GetMenuDecision(){
    int decision = 0;
    bool first_time = true;
    bool correct_input = false;
    std::string string_decision;
    while(correct_input != true){
        if(first_time == false){
            std::cout << "Inccorect entrance. Please type again\n";
        }
        first_time = false;
        ShowMenu();
        std::cout << "Enter your decision : \n";

        std::cin >> string_decision;
        decision = std::stoi(string_decision);
        if ( (decision > 0) && (decision < 7) ){
            correct_input = true;
        }
    }
    return decision;
}

void* GetSocket(int         argc, 
                char*       argv[]){
    void* context = zmq_ctx_new();
	if(context == NULL){
		exit(0);
    }
    void* request = zmq_socket(context, ZMQ_REQ);
	if(request == NULL){
		exit(0);
    }
    if (argc != 2){
        std::cout << "Terminating...\n";
        std::cout << "The application requires to pass exactly one command-line parameter - ID: natural number\n";
        std::cout << "Example: ./player 5\n";
        exit(0);
    }
    if ( std::stoi(argv[1]) < 0 ){
        std::cout << "ID should be > 0 \n";
        exit(0);
    }

    std::cout << "Starting...\n";
    std::string adress;
    std::cout << "Enter the address of server : ";
    std::cin >> adress;
    std::string txt = "tcp://localhost:" + adress;
    if (StrToLower(adress) == "docker"){
        txt = "tcp://zmq_server_cpp:4040";
    }

    
    zmq_connect(request, txt.c_str()); //creating socket
    return request;
}

void NotifyAboutStart(int opponent_id){
    std::cout << "----------------------------------------------------------\n";
    std::cout << "Player is found\n";
    std::cout << "You are playing against " << opponent_id << " player\n";
    std::cout << "3\n";
    sleep(1);
    std::cout << "2\n";
    sleep(1);
    std::cout << "1\n";
    sleep(1);
    std::cout << "Game Starts\n";
}

int VerifyAndGetIndex_81(message*    mes,
                         int&        s_move,
                         int&        s_status,
                         int&        s_win,
                         int&        s_lose){

    int index = -1;
    std::string input;
    bool ok = false;
    bool first_time = true;
    while( ok != true){
        if (first_time == false){
            std::cout << "Inccorect index. Please type again\n";
        }
        first_time = false;
        std::cout << "Enter index [1 ... 81] \n";
        std::cin >> input;

        if(StrToLower(input) == "exit"){
            s_lose = NO;
            while(s_lose != YES){
                mes->action = EXIT_GAME_EARLY;
                SendAndRecieve(mes,
                               s_move,
                               s_status,
                               s_win,
                               s_lose);
            }
            PrintMessageAndExitGame("You lost by resignation");
        }

        index = std::stoi(input);
        if ((index > 0) && (index < 82)){
            ok = true;
        }
    }
    return index;
}

int VerifyAndGetIndex_9(message*    mes,
                        int&        s_move,
                        int&        s_status,
                        int&        s_win,
                        int&        s_lose){
    int index = -1;
    std::string input;
    bool ok = false;
    bool first_time = true;
    while(ok != true){
        if(first_time == false){
            std::cout << "Inccorect index. Please type again\n";
            std::cout << "Enter index [1 ... 9] \n";
        }
        first_time = false;
        std::cin >> input;

        if(StrToLower(input) == "exit"){
            s_lose = NO;
            while(s_lose != YES){
                mes->action = EXIT_GAME_EARLY;
                SendAndRecieve(mes,
                               s_move,
                               s_status,
                               s_win,
                               s_lose);
            }
            PrintMessageAndExitGame("You lost by resignation");
        }
        index = std::stoi(input);
        if ((index > 0) && (index < 10)){
            ok = true;
        }
    }
    return index;
}

void ChooseAndVerifyOpponetsID(message*    mes,
                               int&        s_move,
                               int&        s_status,
                               int&        s_win,
                               int&        s_lose){
    bool first_time = true;
    bool correct_input = false;
    std::string string_player;
    while(correct_input != true){
        if(first_time == false ){
            std::cout << "Inccorect ID. Please type again\n";
        }
        first_time = false;
        std::cout << "Enter ID of another player: ";
        std::cin >> string_player;
        if(StrToLower(string_player) == "exit"){
            s_lose = NO;
            while(s_lose != YES){
                mes->action = EXIT_GAME_EARLY;
                SendAndRecieve(mes,
                               s_move,
                               s_status,
                               s_win,
                               s_lose);
            }
            PrintMessageAndExitGame("You lost by resignation");
        }

        mes->opponentID = std::stoi(string_player);
        if(mes->opponentID > 0){
            correct_input = true;
        }
    }
}

void CheckGameCondition(int s_win, int s_lose){
    if(s_win == YES){
        PrintMessageAndExitGame("You won by resignation");
    }
    if(s_lose == YES){
        PrintMessageAndExitGame("You lost");
    }
}

int main(int        argc, 
         char*      argv[]){


    void*   request             = GetSocket(argc, argv);    
    int     decision            = GetMenuDecision();

    int     s_win               = NO;
    int     s_lose              = NO;
    int     s_status            = NO;
    int     s_move              = NO;

    message mes;
    mes.id                      = std::stoi(argv[1]);
    mes.socket                  = request;

    Colourize painter;
    
    switch(decision){
        case 1:
            mes.action = REGISTER_PLAYER;
            mes.status = STATUS_PLAYER_WITH_FRIEND;
            SendAndRecieve(&mes, 
                           s_move, 
                           s_status,
                           s_win, 
                           s_lose);
            break;

        case 2:
            mes.action = REGISTER_PLAYER;
            mes.status = 10 + decision;
            SendAndRecieve(&mes,
                           s_move,
                           s_status,
                           s_win,
                           s_lose);
            break;

        case 3:
            mes.action = SHOW_PERSONAL_STATISTICS;
            SendAndRecieve(&mes,
                           s_move,
                           s_status,
                           s_win,
                           s_lose);
            puts("------------------------Statistics------------------------\n");

            puts("----------------------------------------------------------\n");
            exit(0);
            break;

        case 4:
            mes.action = SHOW_GENERAL_STATISTICS;
            SendAndRecieve(&mes,
                           s_move,
                           s_status,
                           s_win,
                           s_lose);
            puts("------------------------Statistics------------------------\n");

            puts("----------------------------------------------------------\n");
            exit(0);            
            break;

        case 5:
            mes.action = SHOW_DATABASE_ON_SERVERSIDE;
            SendAndRecieve(&mes,
                           s_move,
                           s_status,
                           s_win,
                           s_lose);
            exit(0);
            break;

        case 6:
            mes.action = DELETE_PLAYER;
            SendAndRecieve(&mes,
                           s_move,
                           s_status,
                           s_win,
                           s_lose);
            exit(0);
            break;
    }

    if (s_status == STATUS_IN_GAME){
        PrintMessageAndExitGame("Sorry, this ID is in use now, choose another one");
    }


    if(decision == 1){
        ChooseAndVerifyOpponetsID(&mes,
                                  s_move,
                                  s_status,
                                  s_win,
                                  s_lose);

        //need to check it?
        mes.status = STATUS_IN_GAME;
    }
    if(decision == 2){
        std::cout << "Searching a player...\n";
        s_status = STATUS_MANUAL;
        while(s_status != STATUS_IN_GAME){
            mes.action = FIND_OPPONENT;
            mes.playertype = mes.playertype;
            SendAndRecieve(&mes,
                           s_move,
                           s_status,
                           s_win,
                           s_lose);
        }
    }

    NotifyAboutStart(mes.opponentID);

    int position;
    std::vector<int> info_data(SizeV + 1);
    std::vector<char> game_data(SizeV + 1, '.');
    std::vector<char> global_win(10, '.');
    std::vector<int> taken_array(10, 0);
    for(size_t i = 0; i < info_data.size(); ++i){
        info_data[i] = i;
    }

    PrintInfo(info_data);
    PrintGame(game_data, global_win, painter);

    int turn = 1;

    if(mes.playertype == 1){
        int index = VerifyAndGetIndex_81(&mes,
                                         s_move,
                                         s_status,
                                         s_win,
                                         s_lose);

        game_data[index] = 'x';
        ++taken_array[PositionforCell(game_data, index)];

        mes.action = UPDATE_MOVE_ON_OPPONENTS_SIDE;
        mes.movement = index;

        SendAndRecieve(&mes,
                       s_move,
                       s_status,
                       s_win,
                       s_lose);

        CheckGameCondition(s_win, s_lose);

        turn = 2;
        PrintGame(game_data, global_win, painter);
        position = Position(game_data, index);

        std::cout << "Opponent turn\n";

        signal(SIGTSTP, GracefulQuit); //ctr + Z
        signal(SIGINT, GracefulQuit); //ctr + c

        if(PROGRAM_ABORT_HANDLER == PROGRAM_STOP){
            s_lose = NO;
            while(s_lose != YES){
                mes.action = EXIT_GAME_EARLY;
                SendAndRecieve(&mes,
                               s_move,
                               s_status,
                               s_win,
                               s_lose);
            }
            PrintMessageAndExitGame("You lost by resignation");
        }

        while(PROGRAM_ABORT_HANDLER != PROGRAM_STOP){

            while(index != END_OF_GAME){

                CheckGameCondition(s_win, s_lose);

                if(turn == 1){

                    index = VerifyAndGetIndex_9(&mes,
                                                s_move,
                                                s_status,
                                                s_win,
                                                s_lose);

                    int x = -1;
                    int escape = 1;
                    while((x = Add(game_data, position, index, 'x', taken_array, escape)) != 0){

                        if(x == 1){
                            index = VerifyAndGetIndex_9(&mes,
                                                        s_move,
                                                        s_status,
                                                        s_win,
                                                        s_lose);
                        }
                        if(x == 2){
                            bool legal_move = false; 
                            while(legal_move == false){
                                index = VerifyAndGetIndex_81(&mes,
                                                             s_move,
                                                             s_status,
                                                             s_win,
                                                             s_lose);
                                if(game_data[index] == '.'){
                                    game_data[index] = 'x';
                                    position = PositionforCell(game_data, index);
                                    index = Position(game_data, index);
                                    legal_move = true;
                                }
                                else{
                                    std::cout << "This sell is taken too.\n";
                                }
                            }
                        }
                    }

                    ++taken_array[position];

                    if(CheckWinlocal( game_data, position) == true){
                        AddglobalWin(global_win,position, 'x');

                        if(CheckWinglobal(global_win) == true){
                            index = END_OF_GAME;
                            PrintGame(game_data, global_win, painter);

                            mes.action = PLAYER_WIN_GAME;
                            SendAndRecieve(&mes,
                                           s_move,
                                           s_status,
                                           s_win,
                                           s_lose);
                            PrintMessageAndExitGame("You won");
                        }
                    }

                    position = index;
                    PrintGame(game_data, global_win, painter);
                    mes.action = UPDATE_MOVE_ON_OPPONENTS_SIDE;
                    mes.movement = index;

                    SendAndRecieve(&mes,
                                   s_move,
                                   s_status,
                                   s_win,
                                   s_lose);

                    CheckGameCondition(s_win, s_lose);

                    turn = 2;
                    std::cout << "Opponent turn\n";
                }
                else {
                    s_move = EMPTY_CELL;
                    while(s_move == EMPTY_CELL){
                        mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
                        SendAndRecieve(&mes,
                                       s_move,
                                       s_status,
                                       s_win,
                                       s_lose);

                    }
                    CheckGameCondition(s_win, s_lose);

                    std::cout << "\n";
                    index = s_move;
                    int escape = 1;
                    if((index > 0) && (index < 10)){
                        Add(game_data, position, index, '0', taken_array, escape);
                    }
                    else{
                        game_data[index] = '0';
                        position = PositionforCell(game_data, index);
                        index = Position(game_data, index);
                        escape = 0;
                    }

                    ++taken_array[position];

                    if(CheckWinlocal( game_data , position ) == true){
                        AddglobalWin(global_win, position, '0');
                        if(CheckWinglobal(global_win) == true){
                            index = END_OF_GAME;
                            PrintGame(game_data, global_win, painter);
                            PrintMessageAndExitGame("You lost");
                            // ??
                            break;
                        }
                    }

                    position = index;
                    turn = 1;
                    PrintGame(game_data, global_win, painter);
                    std::cout << "Enter index [1 ... 9] \n";
                }
            }
        }
    }
    if(mes.playertype == 2){
        int index = -1;
        std::cout << "Opponent turn\n";

        s_move = EMPTY_CELL;
        while(s_move == EMPTY_CELL){
            mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
            SendAndRecieve(&mes,
                           s_move,
                           s_status,
                           s_win,
                           s_lose);
        }

        CheckGameCondition(s_win, s_lose);

        std::cout << "\n";
        index = s_move;
        game_data[index] = 'x';
        ++taken_array[PositionforCell(game_data, index)];

        turn = 2;
        PrintGame(game_data, global_win, painter);
        position = Position(game_data, index);

        std::cout << "Enter index [1 ... 9] \n"; 

        signal(SIGTSTP, GracefulQuit);//ctr + Z
        signal(SIGINT, GracefulQuit);//ctr + c

        if(PROGRAM_ABORT_HANDLER == PROGRAM_STOP){
            s_lose = NO;
            while(s_lose != YES){
                mes.action = EXIT_GAME_EARLY;
                SendAndRecieve(&mes,
                               s_move,
                               s_status,
                               s_win,
                               s_lose);
                
            }
            PrintMessageAndExitGame("You lost by resignation");
        }

        while(PROGRAM_ABORT_HANDLER != PROGRAM_STOP){

            while(index != END_OF_GAME){

                CheckGameCondition(s_win, s_lose);

                if(turn == 1){
                    s_move = EMPTY_CELL;
                    while(s_move == EMPTY_CELL){
                        mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
                        SendAndRecieve(&mes,
                                       s_move,
                                       s_status,
                                       s_win,
                                       s_lose);
                    }
                    CheckGameCondition(s_win, s_lose);

                    std::cout << "\n";
                    index = s_move;
                    int escape = 1;
                    if((index > 0) && (index < 10)){
                        Add(game_data, position, index, 'x', taken_array, escape);
                    }
                    else{
                        game_data[index] = 'x';
                        position = PositionforCell(game_data, index);
                        index = Position(game_data, index);
                        escape = 0;
                    }

                    ++taken_array[position];
                    if(CheckWinlocal(game_data, position) == true){
                        AddglobalWin(global_win,position, 'x');
                        if (CheckWinglobal(global_win) == true){
                            index = END_OF_GAME;
                            PrintGame(game_data, global_win, painter);
                            PrintMessageAndExitGame("You lost");
                        }
                    }

                    position = index;
                    turn = 2;
                    PrintGame(game_data, global_win, painter);
                    std::cout << "Enter index [1 ... 9] \n";
                }
                else {
                    index = VerifyAndGetIndex_9(&mes,
                                                s_move,
                                                s_status,
                                                s_win,
                                                s_lose);

                    int x = -1;
                    int escape = 1;
                    while((x = Add(game_data, position, index, '0', taken_array, escape)) != 0){
                        if(x == 1){
                            index = VerifyAndGetIndex_9(&mes,
                                                        s_move,
                                                        s_status,
                                                        s_win,
                                                        s_lose);
                        }
                        if(x == 2){
                            int legal_move = false; 
                            PrintInfo(info_data);
                            while(legal_move == false){
                                index = VerifyAndGetIndex_81(&mes,
                                                             s_move,
                                                             s_status,
                                                             s_win,
                                                             s_lose);
                                if(game_data[index] == '.'){
                                    game_data[index] = '0';
                                    position = PositionforCell(game_data, index);
                                    index = Position(game_data, index);
                                    legal_move = true;
                                    escape = 0;
                                }
                                else{
                                    std::cout << "This sell is taken too.\n";
                                }
                            }
                        }
                    }

                    ++taken_array[position];

                    if(CheckWinlocal(game_data, position) == true){
                        AddglobalWin(global_win, position, '0');
                        if(CheckWinglobal(global_win) == true){
                            index = END_OF_GAME;
                            PrintGame(game_data, global_win, painter);
                            std::cout << "You won \n";
                            std::cout << "Quitting the game\n";

                            mes.action = PLAYER_WIN_GAME;
                            
                            
                            SendAndRecieve(&mes,
                                           s_move,
                                           s_status,
                                           s_win,
                                           s_lose);
                            exit(0);
                        }
                    }

                    position = index;
                    PrintGame(game_data, global_win, painter);
                    mes.action = UPDATE_MOVE_ON_OPPONENTS_SIDE;
                    mes.movement = index;
                    
                    SendAndRecieve(&mes,
                                   s_move,
                                   s_status,
                                   s_win,
                                   s_lose);

                    CheckGameCondition(s_win, s_lose);

                    turn = 1;
                    std::cout << "Opponent turn\n";
                }
            }
        
        }
    }

    return 0;
}