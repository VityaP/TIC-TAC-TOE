/*
This program is the private property of Victor Petrosyan. 
Any use without the consent of the author is prohibited.
*/

#include <map>
#include <cstring>
#include <iostream>

#include <zmq.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

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

int SwitchType(int type){
	if(type == 1){
		return 2;
	}
	else{
		return 1;
	}
}

void LogAboutFraud(int whoDoesNotExist, int hackerID, int switchCase){
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Problem at switch case : " << switchCase << "\n";
	std::cout << "ID : " << whoDoesNotExist << " doesn't exist\n";
	std::cout << "Attempts of unapproved behavior from a player with an ID : " << hackerID << "\n";
	std::cout << "-----------------------------------------------------------------------------\n\n";
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
			mes->lose = NO;
			mes->win  = NO;

			switch(mes->action){
				case REGISTER_PLAYER :
					if(mes->id <= 0){
						std::cout << "-----------------------------------------------------------------------------\n";
						std::cout << "ID should be > 0, but got: " << mes->id << "\n";
						std::cout << "Attempts of unapproved behavior from a player with an ID : " << mes->id << "\n";
						std::cout << "-----------------------------------------------------------------------------\n";
						mes->lose = YES;
						break;
					}
					
					if(EnteringAtServer(players, mes->id, TYPE, mes->status) == true){
						mes->playertype = TYPE;
						TYPE = SwitchType(TYPE);
						PrintDatabase(players);
						std::cout << "\n";
					}
					else{
						mes->lose = YES;
						// player already exist and online
						// mes->status = GetStatus(players, mes->id);
					}
					break;                                                       

				case UPDATE_MOVE_ON_OPPONENTS_SIDE :
					//To write move of current player in array of his opponent
					if(Find(players, mes->id) == false){
						LogAboutFraud(mes->id, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					// TODO verify mes->movement
					if(Find(players, mes->opponentID)){
						if(players[mes->id].last_move != OPPONENT_LOSE){
							mes->win = NO;
							players[mes->opponentID].last_move = mes->movement;
						}
						else{
							mes->win = YES;
						}
					}
					else{
						LogAboutFraud(mes->opponentID, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					break;

				case CHECK_IF_OPPONENT_MAKE_MOVE :
					// To check if another player make move and get move value from movmentArray
					if(Find(players, mes->id)){
						if ((players[mes->id].last_move != OPPONENT_LOSE) && (players[mes->id].last_move != OPPONENT_WIN)){
							mes->win = NO;
							mes->movement = players[mes->id].last_move;
							if(players[mes->id].last_move != EMPTY_CELL){
								players[mes->id].last_move = EMPTY_CELL;
							}
						}
						else {
							mes->movement = players[mes->id].last_move;
							if(players[mes->id].last_move == OPPONENT_LOSE){
								mes->win = YES;
							}
							else{
								mes->lose = YES;
							}
							players[mes->id].last_move = EMPTY_CELL;
						}
					}
					else{
						LogAboutFraud(mes->id, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					break;            

				case FIND_OPPONENT :
					//To find waiting player to start game player in database
					if(Find(players, mes->id) == false){
						LogAboutFraud(mes->id, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					// TODO verify mes->playertype
					ConnectPlayer(players, mes->id, mes->playertype);
					mes->opponentID = OpponentID(players, mes->id);
					if(mes->opponentID != NO_OPPONENT){
						mes->status = STATUS_IN_GAME;
						PrintDatabase(players);
						std::cout << "\n";
					}
					break; 

				case EXIT_GAME_EARLY :
					//If you quit the game till it end
					if(Find(players, mes->id) == false){
						LogAboutFraud(mes->id, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					mes->lose = YES;
					mes->opponentID = OpponentID(players, mes->id);

					++players[mes->id].loseRate;
					++players[mes->opponentID].winRate;
					players[mes->opponentID].last_move = OPPONENT_LOSE;

					DisconnectPlayer(players, mes->id);
					DisconnectPlayer(players, mes->opponentID);
					PrintDatabase(players);
					std::cout << "\n";
					break;  

				case SHOW_PERSONAL_STATISTICS :
					//To print stat of current player
					if(Find(players, mes->id)){
						mes->winstat = players[mes->id].winRate;
						mes->losestat = players[mes->id].loseRate;
					}
					else{
						LogAboutFraud(mes->id, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					break;   

				case SHOW_GENERAL_STATISTICS :
					//To print stats of all players in server
					break;   

				case SHOW_DATABASE_ON_SERVERSIDE :
					//To print DB in server
					if(Find(players, mes->id)){
						PrintDatabase(players);
						std::cout << "\n";
					}
					else{
						LogAboutFraud(mes->id, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					break;  

				case PLAYER_WIN_GAME :
					//If you win the game
					if(Find(players, mes->id)){
						mes->opponentID = OpponentID(players, mes->id);

						++players[mes->id].winRate;
						++players[mes->opponentID].loseRate;
						players[mes->opponentID].last_move = OPPONENT_WIN;

						DisconnectPlayer(players, mes->id);
						DisconnectPlayer(players, mes->opponentID);
						PrintDatabase(players);
						std::cout << "\n";
					}
					else{
						LogAboutFraud(mes->id, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					
					break; 

				case DELETE_PLAYER :
					//To delete player from database
					if (DeletePlayers(players, mes->id) != true){
						LogAboutFraud(mes->id, mes->id, mes->action);
						mes->lose = YES;
						break;
					}
					break;

				default:
					std::cout << "WARNING!!!\n";
					std::cout << "Unknown switch case value : " << mes->action << "\n";
					LogAboutFraud(mes->id, mes->id, mes->action);

			}
			memcpy(zmq_msg_data(&reply), mes, sizeof(message));
			zmq_msg_send(&reply, respond, 0);
			zmq_msg_close(&reply);
		}
	}

	zmq_close(respond);
	
	return 0;
}
