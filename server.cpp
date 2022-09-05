
/*

This program is the private property of Viktor Petrosyan. 
Any use without the consent of the author is prohibited. 
The program began writing on January 4, 2019 
Last edit 26 January 2019 at 15: 49
*/
#include <map>
#include <string>
#include <iostream>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "zmq.h"
#include "message.h"
#include "functions.h"


volatile sig_atomic_t SERVER_ABORT_HANDLER = 0;

void ServerBlockFunction(int tmp){
	std::cout << "Server blocked\n";
    SERVER_ABORT_HANDLER = PROGRAM_PAUSE;
}

void ServerUnblockFunction(int tmp){
	std::cout << "Server unblocked\n";
    SERVER_ABORT_HANDLER = PROGRAM_RUN;
}

void ServerGracefulQuit(int tmp){
	std::cout << "Server quit\n";
	SERVER_ABORT_HANDLER = PROGRAM_STOP;
}

void* GetSocket(int         argc, 
                char*       argv[]){
	void* context = zmq_ctx_new();
	if(context == NULL){
		exit(0);
	}
	void* respond = zmq_socket(context, ZMQ_REP);
	if(respond == NULL){
		exit(0);
	}
	std::string txt;
	if(argc == 2){
		txt = "tcp://*:" + std::string(argv[1]);
	}
	else{
		txt = "tcp://*:4040";
	}
	if(zmq_bind(respond, txt.c_str()) == -1){
		exit(0);
	}
	return respond;
}

int main (int argc, char *argv[]) {
	zmq_msg_t reply;
	int TYPE = 1;

	std::map<int, Player> players;
	void* respond = GetSocket(argc, argv);

	signal(SIGTSTP, ServerBlockFunction);   //ctr + Z
	signal(SIGCONT, ServerUnblockFunction);
	signal(SIGINT, ServerGracefulQuit);     //ctr + c

	while(SERVER_ABORT_HANDLER != PROGRAM_STOP){
		if(SERVER_ABORT_HANDLER == PROGRAM_RUN){
			//recive information from player
			zmq_msg_t request;
			zmq_msg_init(&request);
			zmq_msg_recv(&request, respond, 0);
			message* mes = (message*) zmq_msg_data(&request);
			zmq_msg_init_size(&reply, sizeof(message));
			zmq_msg_close(&request);
			//recive information from player

			switch(mes->action){
				case REGISTER_PLAYER :
					mes->lose = NO;
					if(EnteringTOserver(players, mes->id, TYPE, mes->status)){
						mes->playertype = TYPE;
						if ( TYPE == 1 ) {
							TYPE = 2;
						}
						else{
							TYPE = 1;
						}
						Printdatabase(players);
						std::cout << "\n";
					}
					else{
						// ?????
						mes->win = YES;
						mes->status = Getstatus(players, mes->id);
					}
					break;                                                       
		
				case UPDATE_MOVE_ON_OPPONENTS_SIDE :
					//To write move of current player in array of his opponent
					mes->lose = NO;
					if(Find(players, mes->opponentID)){
						if ( players[mes->id].last_move != OPPONENT_LOSE ){
							mes->win = NO;
							players[mes->opponentID].last_move = mes->movement;
						}
						else{
							mes->win = YES;
						}
					}
					else{
						std::cout << "ID : " << mes->opponentID << "  doesn't exist\n";
					}
					break;

				case CHECK_IF_OPPONENT_MAKE_MOVE :
					// To check if another player make move and get move value from movmentArray
					mes->lose = NO;
					if(Find(players, mes->id)){
						if ((players[mes->id].last_move != OPPONENT_LOSE) && (players[mes->id].last_move != OPPONENT_WIN)){
							mes->win = NO;
							mes->movement = players[mes-> id].last_move;
							if ( players[mes-> id].last_move != EMPTY_CELL){
								players[mes-> id].last_move = EMPTY_CELL;
							}
						}
						else {
							mes->movement = players[mes-> id].last_move;
							if (players[mes->id].last_move == OPPONENT_LOSE){
								mes->win = YES;
							}
							else{
								mes->lose = YES;
							}
							players[mes-> id].last_move = EMPTY_CELL;
						}
					}
					else{
						std::cout << "ID : " << mes->id << "  doesn't exist\n";
					}
					break;            

				case FIND_OPPONENT :
					//To find waiting player to start game player in database
					mes->lose = NO;
					Connect_player(players, mes->id, mes->playertype);
					mes->opponentID = OpponentID(players, mes->id);
					if (mes->opponentID != -1){
						mes->status=STATUS_IN_GAME;
						Printdatabase(players);
						std::cout << "\n";
					}
					break; 

				case EXIT_GAME_EARLY :
					//If you quit the game till it end
					mes->lose = YES;
					mes->opponentID = OpponentID(players, mes->id);
					++players[mes->id].loseRate;
					++players[mes->opponentID].winRate;
					players[mes->opponentID].last_move = OPPONENT_LOSE;

					Disconnect_player(players, mes->id);
					Disconnect_player(players, mes->opponentID);
					Printdatabase(players);
					std::cout << "\n";
					break;  

				case SHOW_PERSONAL_STATISTICS :
					//To print stat of current player
					if(Find(players, mes->id)){
						mes->winstat = players[mes->id].winRate;
						mes->losestat = players[mes->id].loseRate;
					}
					else{
						// ???? what for if
					}
					break;   

				case SHOW_GENERAL_STATISTICS :
					//To print stats of all players in server
					break;   

				case SHOW_DATABASE_ON_SERVERSIDE :
					//To print DB in server
					Printdatabase(players);
					std::cout << "\n";
					break;  

				case PLAYER_WIN_GAME :
					//If you win the game
					++players[mes->id].winRate;
					mes->opponentID = OpponentID(players, mes->id);
					++players[mes->opponentID].loseRate;
					players[mes->opponentID].last_move = OPPONENT_WIN;

					Disconnect_player(players, mes->id);
					Disconnect_player(players, mes->opponentID);
					Printdatabase(players);
					std::cout << "\n";
					break; 

				case DELETE_PLAYER :
					//To delete player from database
					if (Delete_players(players, mes->id) ){
						players[mes->id].winRate = 0;
						players[mes->id].loseRate = 0;
						players[mes->id].last_move = EMPTY_CELL;
					}
					else{
						//sprintf(mes->text,"No gamer with %d ID",mes->id);
					}
					break;        
			}
			memcpy(zmq_msg_data(&reply), mes, sizeof(message));
			zmq_msg_send(&reply, respond, 0);
			zmq_msg_close(&reply);
		}
	}

	zmq_close(respond);
	
	return 0;
}
