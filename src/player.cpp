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

message mes;

std::string StrToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::tolower(c); }
                  );
    return s;
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

void GracefulQuit(int tmp){
	std::cout << "Program forced stopped\n";

    int tmp1, tmp2, tmp3;
    int s_lose = NO;
    while(s_lose != YES){
        mes.action = EXIT_GAME_EARLY;
        SendAndRecieve(&mes,
                       tmp1,
                       tmp2,
                       tmp3,
                       s_lose);
    }
    PrintMessageAndExitGame("You lost by resignation");
    exit(0);
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

void MakeMoveScenario(std::vector<char>& game_data, std::vector<int>& taken_array, std::vector<char>& global_win, std::vector<int>&  info_data, int& position, char what){
    Colourize painter;
    int s_move, s_status, s_win, s_lose;
    int index = VerifyAndGetIndex_9(&mes,
                                    s_move,
                                    s_status,
                                    s_win,
                                    s_lose);

    
    int x = ProcessAndVerifyMove(game_data, position, index, what, taken_array);
    bool isAbilityToChoose81 = false;
    while(x != MOVE_VERIFIED){
        if(x == MOVE_CHOOSE_9){
            index = VerifyAndGetIndex_9(&mes,
                                        s_move,
                                        s_status,
                                        s_win,
                                        s_lose);
        }
        if(x == MOVE_CHOOSE_81){
            bool legal_move = false;
            isAbilityToChoose81 = true;
            PrintInfo(info_data);
            while(legal_move == false){
                index = VerifyAndGetIndex_81(&mes,
                                                s_move,
                                                s_status,
                                                s_win,
                                                s_lose);
                if(game_data[index] == '.'){
                    position = GetPositionAtGlobalCell(game_data, index);
                    legal_move = true;
                }
                else{
                    std::cout << "This sell is taken too.\n";
                }
            }
        }
        if(isAbilityToChoose81 == true){
            x = ProcessAndVerifyMove(game_data, position, GetPositionAtLocalCell(game_data, index), what, taken_array);
        }
        else{
            x = ProcessAndVerifyMove(game_data, position, index, what, taken_array);
        }
    }

    ++taken_array[position];

    if(CheckLocalWin(game_data, position) == true){
        MarkGlobalWin(global_win, position, what);
        if(CheckGlobalWin(global_win) == true){
            index = END_OF_GAME;
            mes.action = PLAYER_WIN_GAME;
            SendAndRecieve(&mes,
                            s_move,
                            s_status,
                            s_win,
                            s_lose);
            PrintGame(game_data, global_win, painter);
            PrintMessageAndExitGame("You won");
        }
    }

    PrintGame(game_data, global_win, painter);
    if(isAbilityToChoose81 == true){
        position = GetPositionAtLocalCell(game_data, index);
        mes.movement = index;
    }
    else{
        position = index;
        mes.movement = index;
    }
    mes.action = UPDATE_MOVE_ON_OPPONENTS_SIDE;
    
    SendAndRecieve(&mes,
                    s_move,
                    s_status,
                    s_win,
                    s_lose);

    CheckGameCondition(s_win, s_lose);
}

void WaitForOpponentsMoveScenario(std::vector<char>& game_data, std::vector<int>& taken_array, std::vector<char>& global_win, int& position, char what){
    Colourize painter;
    int s_move, s_status, s_win, s_lose;
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

    int index = s_move;
    if((index > 0) && (index < 10)){
        ProcessAndVerifyMove(game_data, position, index, what, taken_array);
    }
    else{
        game_data[index] = what;
        position = GetPositionAtGlobalCell(game_data, index);
        index = GetPositionAtLocalCell(game_data, index);
    }

    if(CheckLocalWin(game_data, position) == true){
        MarkGlobalWin(global_win, position, what);
        if(CheckGlobalWin(global_win) == true){
            index = END_OF_GAME;
            PrintGame(game_data, global_win, painter);
            PrintMessageAndExitGame("You lost");
        }
    }

    ++taken_array[position];
    position = index;
    PrintGame(game_data, global_win, painter);
}

int main(int        argc, 
         char*      argv[]){


    void*   request             = GetSocket(argc, argv);
    int     decision            = GetMenuDecision();

    int     s_win               = NO;
    int     s_lose              = NO;
    int     s_status            = NO;
    int     s_move              = NO;

    mes.id                      = std::stoi(argv[1]);
    mes.socket                  = request;

    Colourize painter;
    signal(SIGTSTP, GracefulQuit); //ctr + Z
    signal(SIGINT, GracefulQuit);  //ctr + c
    
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
    if(s_lose == YES){
        PrintMessageAndExitGame("Sorry, this ID is in use now, choose another one");
    }

    if(decision == 1){
        ChooseAndVerifyOpponetsID(&mes,
                                  s_move,
                                  s_status,
                                  s_win,
                                  s_lose);

        // TODO what if another player don't want to play???
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
    std::vector<int>  info_data(SizeV + 1);
    std::vector<char> game_data(SizeV + 1, '.');
    std::vector<char> global_win(10, '.');
    std::vector<int>  taken_array(10, 0);
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
        ++taken_array[GetPositionAtGlobalCell(game_data, index)];

        mes.action = UPDATE_MOVE_ON_OPPONENTS_SIDE;
        mes.movement = index;
        SendAndRecieve(&mes,
                       s_move,
                       s_status,
                       s_win,
                       s_lose);
        CheckGameCondition(s_win, s_lose);
        position = GetPositionAtLocalCell(game_data, index);
        PrintGame(game_data, global_win, painter);

        turn = 2;
        std::cout << "Opponent turn\n";
        while(index != END_OF_GAME){
            CheckGameCondition(s_win, s_lose);
            if(turn == 1){
                MakeMoveScenario(game_data, taken_array, global_win, info_data, position, 'x');
                turn = 2;
                std::cout << "Opponent turn\n";
            }
            else {
                WaitForOpponentsMoveScenario(game_data, taken_array, global_win, position, '0');
                turn = 1;
                std::cout << "Enter index [1 ... 9] \n";
            }
        }
    }
    if(mes.playertype == 2){
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

        int index = s_move;
        game_data[index] = 'x';
        position = GetPositionAtLocalCell(game_data, index);
        ++taken_array[GetPositionAtGlobalCell(game_data, index)];
        PrintGame(game_data, global_win, painter);

        turn = 2;
        std::cout << "Enter index [1 ... 9] \n"; 
        while(index != END_OF_GAME){
            CheckGameCondition(s_win, s_lose);
            if(turn == 1){
                WaitForOpponentsMoveScenario(game_data, taken_array, global_win, position, 'x');
                turn = 2;
                std::cout << "Enter index [1 ... 9] \n";
            }
            else {
                MakeMoveScenario(game_data, taken_array, global_win, info_data, position, '0');
                turn = 1;
                std::cout << "Opponent turn\n";
            }
        }
    }

    return 0;
}