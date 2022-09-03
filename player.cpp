
/*
This program is the private property of Viktor Petrosyan. 
Any use without the consent of the author is prohibited. 
The program began writing on January 4, 2019 
Last edit 26 January 2019 at 15: 49
*/

#include <iostream>
#include <string>
#include <cctype>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <zmq.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/ioctl.h>
#include "functions.h"
#include "message.h"


std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::tolower(c); }
                  );
    return s;
}

volatile sig_atomic_t PROGRAM_ABORT_HANDLER = PROGRAM_RUN;


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
                    int*        s_move, 
                    int*        s_status, 
                    int*        s_receiver_player, 
                    int*        s_win, 
                    int*        s_lose, 
                    int*        s_win_stat, 
                    int*        s_lose_stat, 
                    int*        TYPE, 
                    char*       s_text){

    zmq_msg_t request;
    zmq_msg_init_size(&request, sizeof(message));
    memcpy(zmq_msg_data(&request), mes, sizeof(message));
    zmq_msg_send(&request, mes->socket, 0);
    zmq_msg_close(&request);

    zmq_msg_init(&request);
    zmq_msg_recv(&request, mes->socket, 0);
    mes = (message*) zmq_msg_data(&request);
    *s_move = mes->movement;
    *s_status = mes->status;
    *s_receiver_player = mes->receiverPlayer;
    *s_win = mes->win;
    *s_lose = mes->lose;
    *s_win_stat = mes->winstat;
    *s_lose_stat = mes->losestat;
    *TYPE = mes->playertype;
    for(size_t i = 0; i < Sizetext; i++){
        s_text[i] = mes->text[i];
    }
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
        if ( decision > 0 && decision < 7 ){
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
    if ( (std::stoi(argv[1]) < 0) || (std::stoi(argv[1]) > (SizeArray - 1)) ){
        std::cout << "ID should be from [0 ; " << SizeArray << "]\n";
        exit(0);
    }

    std::cout << "Starting...\n";
    int adress;
    std::cout << "Enter the address of server : ";
    std::cin >> adress;

    std::string txt = "tcp://localhost:" + std::to_string(adress);
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
                         int*        s_move, 
                         int*        s_status, 
                         int*        s_receiver_player, 
                         int*        s_win, 
                         int*        s_lose, 
                         int*        s_win_stat, 
                         int*        s_lose_stat, 
                         int*        TYPE, 
                         char*       s_text){

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

        if( str_tolower(input) == "exit" ){
            *s_lose = 0;
            while( *s_lose != 1){
                mes->action = EXIT_GAME_EARLY;
                SendAndRecieve(mes,
                               s_move,
                               s_status,
                               s_receiver_player,
                               s_win,
                               s_lose,
                               s_win_stat,
                               s_lose_stat,
                               TYPE,
                               s_text);
            }
            PrintMessageAndExitGame("You lost by resignation");
        }

        index = std::stoi(input);
        if ( (index > 0) && (index < 82) ){
            ok = true;
        }
    }
    return index;
}

int VerifyAndGetIndex_9(message*    mes, 
                        int*        s_move, 
                        int*        s_status, 
                        int*        s_receiver_player, 
                        int*        s_win, 
                        int*        s_lose, 
                        int*        s_win_stat, 
                        int*        s_lose_stat, 
                        int*        TYPE, 
                        char*       s_text){

    int index = -1;
    std::string input;
    bool ok = false;
    bool first_time = true;
    while( ok != true){
        if( first_time == false){
            std::cout << "Inccorect index. Please type again\n";
            std::cout << "Enter index [1 ... 9] \n";
        }
        first_time = false;
        std::cin >> input;

        if( str_tolower(input) == "exit" ){
            *s_lose = 0;
            while( *s_lose != 1){
                mes->action = EXIT_GAME_EARLY;
                SendAndRecieve(mes,
                               s_move,
                               s_status,
                               s_receiver_player,
                               s_win,
                               s_lose,
                               s_win_stat,
                               s_lose_stat,
                               TYPE,
                               s_text);
            }
            PrintMessageAndExitGame("You lost by resignation");
        }
        index = std::stoi(input);
        if ( (index > 0) && (index < 10) ){
            ok = true;
        }
    }
    return index;
}

void ChooseAndVerifyOpponetsID(message*    mes, 
                               int*        s_move, 
                               int*        s_status, 
                               int*        s_receiver_player, 
                               int*        s_win, 
                               int*        s_lose, 
                               int*        s_win_stat, 
                               int*        s_lose_stat, 
                               int*        TYPE, 
                               char*       s_text){

    bool first_time = true;
    bool correct_input = false;
    std::string string_player;

    while( correct_input != true){
        if( first_time == false ){
            std::cout << "Inccorect ID. Please type again\n";
        }
        first_time = false;
        std::cout << "Enter ID of another player from [0 ; " << SizeArray << "]: ";
        std::cin >> string_player;
        if( str_tolower(string_player) == "exit" ){
            *s_lose = 0;
            while( *s_lose != 1){
                mes->action = EXIT_GAME_EARLY;
                SendAndRecieve(mes,
                               s_move,
                               s_status,
                               s_receiver_player,
                               s_win,
                               s_lose,
                               s_win_stat,
                               s_lose_stat,
                               TYPE,
                               s_text);
            }
            PrintMessageAndExitGame("You lost by resignation");
        }

        mes->receiverPlayer = std::stoi(string_player);
        if ( (mes->receiverPlayer > 0) && (mes->receiverPlayer < SizeArray) ){
            correct_input = true;
        }
    }
}

int main(int        argc, 
         char*      argv[]){


    void*   request             = GetSocket(argc, argv);    
    int     decision            = GetMenuDecision();

    int     TYPE                = 0;
    int     s_win               = 0;
    int     s_lose              = 0;
    int     s_status            = 0;
    int     s_move              = 0;
    int     s_win_stat          = 0;
    int     s_lose_stat         = 0;
    int     s_receiver_player   = 0;
    char    s_text[Sizetext];

    message mes;
    mes.id                      = std::stoi(argv[1]);
    mes.socket                  = request;
    
    switch (decision){
        case 1:
            mes.action = REGISTER_PLAYER;


            mes.status = 10 + decision;
            SendAndRecieve(&mes, 
                           &s_move, 
                           &s_status, 
                           &s_receiver_player, 
                           &s_win, 
                           &s_lose, 
                           &s_win_stat, 
                           &s_lose_stat, 
                           &TYPE, 
                           s_text);
            break;

        case 2:
            mes.action = REGISTER_PLAYER;
            
            
            mes.status = 10 + decision;
            SendAndRecieve(&mes,
                           &s_move,
                           &s_status,
                           &s_receiver_player,
                           &s_win,
                           &s_lose,
                           &s_win_stat,
                           &s_lose_stat,
                           &TYPE,
                           s_text);
            break;

        case 3:
            mes.action = SHOW_PERSONAL_STATISTICS;
            SendAndRecieve(&mes,
                           &s_move,
                           &s_status,
                           &s_receiver_player,
                           &s_win,
                           &s_lose,
                           &s_win_stat,
                           &s_lose_stat,
                           &TYPE,
                           s_text);
            std::cout << "ID "    << std::stoi(argv[1]) 
                      << " won "  << s_win_stat  << " times and" 
                      << " lost " << s_lose_stat << " times\n";
            exit(0);
            break;

        case 4:
            mes.action = SHOW_GENERAL_STATISTICS;
            SendAndRecieve(&mes,
                           &s_move,
                           &s_status,
                           &s_receiver_player,
                           &s_win,
                           &s_lose,
                           &s_win_stat,
                           &s_lose_stat,
                           &TYPE,
                           s_text);
            puts("------------------------Statistics------------------------\n");
			for(int i = 0, j = 0; j < 5; i = i + 6){
                std::cout << "ID\t" << s_text[i+1] << "\t won " << s_text[i+3] << " times and lost " << s_text[i+5] << " times\n";
				++j;
			}
            puts("----------------------------------------------------------\n");
            exit(0);            
            break;

        case 5:
            mes.action = SHOW_DATABASE_ON_SERVERSIDE;
            SendAndRecieve(&mes,
                           &s_move,
                           &s_status,
                           &s_receiver_player,
                           &s_win,
                           &s_lose,
                           &s_win_stat,
                           &s_lose_stat,
                           &TYPE,
                           s_text);
            exit(0);
            break;

        case 6:
            mes.action = DELETE_PLAYER;
            SendAndRecieve(&mes,
                           &s_move,
                           &s_status,
                           &s_receiver_player,
                           &s_win,
                           &s_lose,
                           &s_win_stat,
                           &s_lose_stat,
                           &TYPE,
                           s_text);
            exit(0);
            break;
    }

    if (s_status == 2){
        PrintMessageAndExitGame("Sorry, this ID is in use now, choose another one");
    }

    std::string  string_player;
    if (TYPE == 1){
        if( decision == 1){
            ChooseAndVerifyOpponetsID(&mes,
                                      &s_move,
                                      &s_status,
                                      &s_receiver_player,
                                      &s_win,
                                      &s_lose,
                                      &s_win_stat,
                                      &s_lose_stat,
                                      &TYPE,
                                      s_text);

            //need to check it?
            mes.status = 2;
        }
        if( decision == 2){
            std::cout << "Searching a player...\n";
            s_status = 0;
            while( s_status != 2){
                mes.action = FIND_OPPONENT;
                mes.receiverPlayer = -2;
                
                mes.playertype = TYPE;
                SendAndRecieve(&mes,
                                &s_move,
                                &s_status,
                                &s_receiver_player,
                                &s_win,
                                &s_lose,
                                &s_win_stat,
                                &s_lose_stat,
                                &TYPE,
                                s_text);
            }
            mes.receiverPlayer = s_receiver_player;
        }

        NotifyAboutStart(mes.receiverPlayer);

        int position;
        int* info_data = (int* ) malloc( ( SizeV + 1 ) * sizeof(int));
        char* game_data = (char* ) malloc( ( SizeV + 1 ) * sizeof(char));
        char* global_win = (char* ) malloc( ( 10 ) * sizeof(char));
        int* taken_array = (int* ) malloc( ( 10 ) * sizeof(int));

        for(size_t i = 0; i <= SizeV; ++i){
            info_data[i] = i;
            game_data[i] = '.';
        }

        for(size_t i = 0; i < 10; ++i){
            global_win[i] = '.';
            taken_array[i] = 0;
        }

        PrintInfo(info_data);
        PrintGame(game_data);

        int turn = 1;

        int index = VerifyAndGetIndex_81(&mes,
                                         &s_move,
                                         &s_status,
                                         &s_receiver_player,
                                         &s_win,
                                         &s_lose,
                                         &s_win_stat,
                                         &s_lose_stat,
                                         &TYPE,
                                         s_text);

        game_data[index] = 'x';
        ++taken_array[PositionforCell(game_data, index)];

        mes.action = UPDATE_MOVE_ON_OPPONENTS_SIDE;
        mes.movement = index;
        if ( decision == 1 ) {
            mes.receiverPlayer = std::stoi(string_player);
        }
        else{
            mes.receiverPlayer = s_receiver_player;
        }
        
        SendAndRecieve(&mes,
                       &s_move,
                       &s_status,
                       &s_receiver_player,
                       &s_win,
                       &s_lose,
                       &s_win_stat,
                       &s_lose_stat,
                       &TYPE,
                       s_text);

        if (s_win == 1){
            PrintMessageAndExitGame("You won by resignation");
        }
        if (s_lose == 1){
            PrintMessageAndExitGame("You lost");
        }

        PrintGame(game_data);

        turn = 2;
        position = Position(game_data, index);

        std::cout << "Opponent turn\n";

        signal(SIGTSTP, GracefulQuit);//ctr + Z
        signal(SIGINT, GracefulQuit);//ctr + c

        if ( PROGRAM_ABORT_HANDLER == PROGRAM_STOP){

            s_lose = 0;
            while( s_lose != 1){
                mes.action = EXIT_GAME_EARLY;
                SendAndRecieve(&mes,
                               &s_move,
                               &s_status,
                               &s_receiver_player,
                               &s_win,
                               &s_lose,
                               &s_win_stat,
                               &s_lose_stat,
                               &TYPE,
                               s_text);
            }

            PrintMessageAndExitGame("You lost by resignation");
        }

        while(PROGRAM_ABORT_HANDLER != PROGRAM_STOP){

            while ( index != 100 ){

                if (s_win == 1){
                    PrintMessageAndExitGame("You won by resignation");
                }
                if (s_lose == 1){
                    PrintMessageAndExitGame("You lost");
                }

                if (turn == 1) {

                    index = VerifyAndGetIndex_9(&mes,
                                                &s_move,
                                                &s_status,
                                                &s_receiver_player,
                                                &s_win,
                                                &s_lose,
                                                &s_win_stat,
                                                &s_lose_stat,
                                                &TYPE,
                                                s_text);

                    int x = -1;
                    int escape = 1;
                    while ( (x = Add(game_data, position, index, 'x', taken_array, escape) ) ){

                        if ( x == 1){
                            index = VerifyAndGetIndex_9(&mes,
                                                        &s_move,
                                                        &s_status,
                                                        &s_receiver_player,
                                                        &s_win,
                                                        &s_lose,
                                                        &s_win_stat,
                                                        &s_lose_stat,
                                                        &TYPE,
                                                        s_text);
                        }
                        if ( x == 2){
                            bool legal_move = false; 
                            while(legal_move == false){

                                index = VerifyAndGetIndex_81(&mes,
                                                                &s_move,
                                                                &s_status,
                                                                &s_receiver_player,
                                                                &s_win,
                                                                &s_lose,
                                                                &s_win_stat,
                                                                &s_lose_stat,
                                                                &TYPE,
                                                                s_text);

                                if (game_data[index] == '.'){
                                    game_data[index] = 'x';
                                    position = PositionforCell(game_data,index);
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

                    if ( CheckWinlocal( game_data, position) ){
                        AddglobalWin(global_win,position, 'x');

                        if (CheckWinglobal(global_win) == true){
                            index = 100;
                            PrintGame(game_data);
                            std::cout << "You won\n";
                            std::cout << "Quitting the game\n";
                            mes.action = PLAYER_WIN_GAME;
                            
                            
                            SendAndRecieve(&mes,
                                            &s_move,
                                            &s_status,
                                            &s_receiver_player,
                                            &s_win,
                                            &s_lose,
                                            &s_win_stat,
                                            &s_lose_stat,
                                            &TYPE,
                                            s_text);
                            exit(0);
                            // Stupid???
                            break;
                        }
                    }

                    position = index;
                    PrintGame(game_data);
                    mes.action = UPDATE_MOVE_ON_OPPONENTS_SIDE;
                    mes.movement = index;
                    if ( decision == 1 ) {
                        mes.receiverPlayer = std::stoi(string_player);
                    }
                    else{
                        mes.receiverPlayer = s_receiver_player;
                    }

                    
                    
                    SendAndRecieve(&mes,
                                    &s_move,
                                    &s_status,
                                    &s_receiver_player,
                                    &s_win,
                                    &s_lose,
                                    &s_win_stat,
                                    &s_lose_stat,
                                    &TYPE,
                                    s_text);

                    if (s_win == 1){
                        PrintMessageAndExitGame("You won by resignation");
                    }
                    if (s_lose == 1){
                        PrintMessageAndExitGame("You lost");
                    }

                    turn = 2;
                    std::cout << "Opponent turn\n";
                }
                else {

                    mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
                    mes.movement = -2;
                    
                    
                    SendAndRecieve(&mes,
                                    &s_move,
                                    &s_status,
                                    &s_receiver_player,
                                    &s_win,
                                    &s_lose,
                                    &s_win_stat,
                                    &s_lose_stat,
                                    &TYPE,
                                    s_text);


                    while( s_move == -2){
                        mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
                        
                        
                        SendAndRecieve(&mes,
                                        &s_move,
                                        &s_status,
                                        &s_receiver_player,
                                        &s_win,
                                        &s_lose,
                                        &s_win_stat,
                                        &s_lose_stat,
                                        &TYPE,
                                        s_text);

                    }
                    if (s_win == 1){
                        PrintMessageAndExitGame("You won by resignation");
                    }
                    if (s_lose == 1){
                        PrintMessageAndExitGame("You lost");
                    }

                    std::cout << "\n";
                    index = s_move;
                    int escape = 1;
                    if ( (index > 0) && (index < 10) ){
                        Add(game_data, position, index, '0', taken_array, escape);
                    }
                    else{
                        game_data[index] = '0';
                        position = PositionforCell(game_data,index);
                        index = Position(game_data , index);
                        escape = 0;
                    }

                    ++taken_array[position];

                    if ( CheckWinlocal( game_data , position ) ){
                        AddglobalWin(global_win, position, '0');
                        if ( CheckWinglobal(global_win) ){
                            index = 100;

                            PrintGame(game_data);
                            PrintMessageAndExitGame("You lost");
                            // ??
                            break;
                        }
                    }

                    position = index;
                    turn = 1;
                    PrintGame(game_data);
                    std::cout << "Enter index [1 ... 9] \n";
                }
            }
        }
    }
    if (TYPE == 2){
        if( decision == 1){
            ChooseAndVerifyOpponetsID(&mes,
                                      &s_move,
                                      &s_status,
                                      &s_receiver_player,
                                      &s_win,
                                      &s_lose,
                                      &s_win_stat,
                                      &s_lose_stat,
                                      &TYPE,
                                      s_text);

            //need to check it?
            mes.status = 2;
        }
        if( decision == 2){
            std::cout << "Searching a player...\n";
            s_status = 0;
            while( s_status != 2){
                mes.action = FIND_OPPONENT;
                mes.receiverPlayer = -2;
                
                mes.playertype = TYPE;
                SendAndRecieve(&mes,
                                &s_move,
                                &s_status,
                                &s_receiver_player,
                                &s_win,
                                &s_lose,
                                &s_win_stat,
                                &s_lose_stat,
                                &TYPE,
                                s_text);
            }
            mes.receiverPlayer = s_receiver_player;
        }

        if ( decision == 1 ) {
            mes.receiverPlayer = std::stoi(string_player);
        }
        else{
            mes.receiverPlayer = s_receiver_player;
        }

        NotifyAboutStart(mes.receiverPlayer);

        
        int position;
        int* info_data = (int* ) malloc( ( SizeV + 1 ) * sizeof(int));
        char* game_data = (char* ) malloc( ( SizeV + 1 ) * sizeof(char));
        char* global_win = (char* ) malloc( ( 10 ) * sizeof(char));
        int* taken_array = (int* ) malloc( ( 10 ) * sizeof(int));

        for(size_t i = 0; i <= SizeV; i++){
            info_data[i] = i ;
            game_data[i] = '.' ;
        }

        for(size_t i = 0; i < 10; i++){
            global_win[i] = '.';
            taken_array[i] = 0;
        }

        PrintInfo(info_data);
        PrintGame(game_data);

        int index = -1;
        int turn = 1;

        std::cout << "Opponent turn\n";

        mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
        mes.movement = -2;
        s_move = -7;
        
        
        SendAndRecieve(&mes,&s_move,&s_status,&s_receiver_player,&s_win,&s_lose,&s_win_stat,&s_lose_stat,&TYPE,s_text);

        while( s_move == -2){
            mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
            
            
            SendAndRecieve(&mes,&s_move,&s_status,&s_receiver_player,&s_win,&s_lose,&s_win_stat,&s_lose_stat,&TYPE,s_text);
        }

        if (s_win == 1){
            PrintMessageAndExitGame("You won by resignation");
        }
        if (s_lose == 1){
            PrintMessageAndExitGame("You lost");
        }

        std::cout << "\n";
        index = s_move;
        game_data[index] = 'x';
        ++taken_array[ PositionforCell(game_data, index) ];
        turn = 2;
        PrintGame(game_data);
        position = Position(game_data, index);
        std::cout << "Enter index [1 ... 9] \n"; 

        signal(SIGTSTP, GracefulQuit);//ctr + Z
        signal(SIGINT, GracefulQuit);//ctr + c

        if ( PROGRAM_ABORT_HANDLER == PROGRAM_STOP){
            s_lose = 0;
            while( s_lose != 1){
                mes.action = EXIT_GAME_EARLY;
                SendAndRecieve(&mes,
                               &s_move,
                               &s_status,
                               &s_receiver_player,
                               &s_win,
                               &s_lose,
                               &s_win_stat,
                               &s_lose_stat,
                               &TYPE,
                               s_text);
                
            }
            exit(0);
        }

        while(PROGRAM_ABORT_HANDLER!=PROGRAM_STOP){

            while ( index != 100 ){

                if (s_win == 1){
                    PrintMessageAndExitGame("You won by resignation");
                }
                if (s_lose == 1){
                    PrintMessageAndExitGame("You lost");
                }

                if (turn == 1) {
                    mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
                    mes.movement = -2;
                    s_move = -7;
                    
                    
                    SendAndRecieve(&mes,&s_move,&s_status,&s_receiver_player,&s_win,&s_lose,&s_win_stat,&s_lose_stat,&TYPE,s_text);

                    while( s_move == -2){
                        mes.action = CHECK_IF_OPPONENT_MAKE_MOVE;
                        
                        
                        SendAndRecieve(&mes,
                                        &s_move,
                                        &s_status,
                                        &s_receiver_player,
                                        &s_win,
                                        &s_lose,
                                        &s_win_stat,
                                        &s_lose_stat,
                                        &TYPE,
                                        s_text);
                    }

                    if (s_win == 1){
                        PrintMessageAndExitGame("You won by resignation");
                    }
                    if (s_lose == 1){
                        PrintMessageAndExitGame("You lost");
                    }

                    std::cout << "\n";

                    index = s_move;
                    int escape = 1;
                    if ( (index > 0) && (index < 10) ){
                        Add( game_data , position , index , 'x',taken_array,escape);
                    }
                    else{
                        game_data[index] = 'x';
                        position = PositionforCell(game_data,index);
                        index = Position(game_data , index);
                        escape = 0;
                    }

                    ++taken_array[position];
                    if ( CheckWinlocal(game_data, position) ){

                        AddglobalWin(global_win,position, 'x');
                        if ( CheckWinglobal(global_win) ){
                            index = 100;
                            PrintGame(game_data);
                            PrintMessageAndExitGame("You lost");
                            // ????
                            break;
                        }
                    }

                    position = index;
                    turn = 2;
                    PrintGame(game_data);
                    std::cout << "Enter index [1 ... 9] \n";
                }
                else {

                    index = VerifyAndGetIndex_9(&mes,
                                                &s_move,
                                                &s_status,
                                                &s_receiver_player,
                                                &s_win,
                                                &s_lose,
                                                &s_win_stat,
                                                &s_lose_stat,
                                                &TYPE,
                                                s_text);

                    int x = -1;
                    int escape = 1;
                    while ( (x = Add(game_data, position, index, '0', taken_array, escape) ) ){
                        if ( x == 1){
                            index = VerifyAndGetIndex_9(&mes,
                                                        &s_move,
                                                        &s_status,
                                                        &s_receiver_player,
                                                        &s_win,
                                                        &s_lose,
                                                        &s_win_stat,
                                                        &s_lose_stat,
                                                        &TYPE,
                                                        s_text);
                        }
                        if ( x == 2){

                            int legal_move = false; 
                            PrintInfo(info_data);
                            while(legal_move == false){

                                index = VerifyAndGetIndex_81(&mes,
                                                                &s_move,
                                                                &s_status,
                                                                &s_receiver_player,
                                                                &s_win,
                                                                &s_lose,
                                                                &s_win_stat,
                                                                &s_lose_stat,
                                                                &TYPE,
                                                                s_text);

                                if (game_data[index] == '.'){
                                    game_data[index] = '0';
                                    position = PositionforCell(game_data,index);
                                    index = Position(game_data , index);
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

                    if ( CheckWinlocal( game_data , position ) ){
                        AddglobalWin(global_win, position, '0');
                        if ( CheckWinglobal(global_win) ){
                            index = 100;
                            PrintGame(game_data);
                            std::cout << "You won \n";
                            std::cout << "Quitting the game\n";

                            mes.action = PLAYER_WIN_GAME;
                            
                            
                            SendAndRecieve(&mes,
                                            &s_move,
                                            &s_status,
                                            &s_receiver_player,
                                            &s_win,
                                            &s_lose,
                                            &s_win_stat,
                                            &s_lose_stat,
                                            &TYPE,
                                            s_text);
                            exit(0);
                            // ???
                            break;
                        }
                    }

                    position = index;
                    PrintGame(game_data);
                    mes.action = UPDATE_MOVE_ON_OPPONENTS_SIDE;
                    mes.movement = index;
                    if ( decision == 1 ) {
                        mes.receiverPlayer = std::stoi(string_player);
                    }
                    else{
                        mes.receiverPlayer = s_receiver_player;
                    }

                    
                    
                    SendAndRecieve(&mes,
                                    &s_move,
                                    &s_status,
                                    &s_receiver_player,
                                    &s_win,
                                    &s_lose,
                                    &s_win_stat,
                                    &s_lose_stat,
                                    &TYPE,
                                    s_text);

                    if (s_win == 1){
                        PrintMessageAndExitGame("You won by resignation");
                    }
                    if (s_lose == 1){
                        PrintMessageAndExitGame("You lost");
                    }

                    turn = 1;
                    std::cout << "Opponent turn\n";
                }
            }
        
        }
    }

    return 0;
}