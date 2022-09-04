
/*

This program is the private property of Viktor Petrosyan. 
Any use without the consent of the author is prohibited. 
The program began writing on January 4, 2019 
Last edit 26 January 2019 at 15: 49
*/
#include <string>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <time.h>
#include <sys/ioctl.h>
// #include <stropts.h>

#include "zmq.h"
#include "message.h"
#include "functions.h"

struct ScoreArray{
	int ID;
	int Score;
};


volatile sig_atomic_t flag = 0;

void block_func(int tmp){
	printf("Server blocked\n");
    flag = 1;
}

void unblock_func(int tmp){
	printf("Server unblocked\n");
    flag = 0;
}

void quit_server_func(int tmp){
	printf("\nServer quit\n");
	flag = 3;
}

void sort(struct ScoreArray* SortArray, int size){

	struct ScoreArray k;
	for(size_t i = 0; i < size - 1; i++){
		for(size_t j = i; j < size; j++){
			if ( SortArray[i].Score < SortArray[j].Score ){
				k = SortArray[i] ;
				SortArray[i] = SortArray[j];
				SortArray[j] = k;
			}
		}
	}
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

	players* list = NULL;

	int TYPE = 1;

	int j;

	int movmentArray[SizeArray];
	int statsGameWin[SizeArray];
	int statsGameLose[SizeArray];
	struct ScoreArray statisticArray[SizeArray];
	for(size_t i = 0; i < SizeArray; i++){
		movmentArray[i] = -2;
	}
	for(size_t i = 0; i < SizeArray; i++){
		statsGameLose[i] = 0;
	}
	for(size_t i = 0; i < SizeArray; i++){
		statsGameWin[i] = 0;
	}
	for(size_t i = 0; i < SizeArray; i++){
		statisticArray[i].ID = i;
		statisticArray[i].Score = 0;
	}


	void* respond = GetSocket(argc, argv);

	signal(SIGTSTP, block_func);
	signal(SIGCONT, unblock_func);
	signal(SIGINT, quit_server_func);
	int usl = 0;

	while(flag!=3){

		if(!flag){

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
					//To add player in database
					mes->lose = 0;
					if(EnteringTOserver(&list,mes->id,TYPE,mes->status)){

						mes->playertype = TYPE;
						
						if ( TYPE == 1 ) {
							TYPE = 2;
						}
						else{
							TYPE = 1;
						}
						
						Printdatabase(&list);

						printf("\n");

					}
					else{

						mes->win = 1;

						mes->status = Getstatus(&list,mes->id);

					}
					
					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply);  
					
				break;                                                       
		
				case UPDATE_MOVE_ON_OPPONENTS_SIDE :
					//To write move of current player in array of his opponent
					mes->lose = 0;

					if(Find(&list,mes->receiverPlayer)){

						if ( movmentArray[mes->id] != -5 ){

							mes->win = 0;

							movmentArray[mes->receiverPlayer] = mes->movement;

							//sprintf(mes->text,"succecfull");

						}
						else{

							mes->win = 1;

						}
					
					}
					else{
					
						//sprintf(mes->text,"no such gamer");
					
						printf("ID : %d  doesn't exist\n",mes->receiverPlayer);
					
					}
				
					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply); 
					
				break;

				case CHECK_IF_OPPONENT_MAKE_MOVE :
					// To check if another player make move and get movment value from array
					mes->lose = 0;

					if(Find(&list,mes->id)){

						if (movmentArray[mes->id] != -5 && movmentArray[mes->id] != -15){

							mes->win = 0;

							mes->movement = movmentArray[mes-> id];

							if ( movmentArray[mes-> id] != -2){

								movmentArray[mes-> id] = -2;

							}

						}
						else {

							mes->movement = movmentArray[mes-> id];

							if (movmentArray[mes->id] == -5){

								mes->win = 1;

							}
							else{
								mes->lose = 1;

							}

							movmentArray[mes-> id] = -2;

						}
				
					}
					else{
					
						//sprintf(mes->text,"no such gamer");
					
						printf("ID : %d doesn't exist\n",mes->id);
					
					}

					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply);  

				break;            

				case FIND_OPPONENT :
					//To find waiting player to start game player in database
					mes->lose = 0;
					Connect_player(&list,mes->id,mes->playertype);

					mes->receiverPlayer = OpponentID(&list,mes->id);

					if ( mes->receiverPlayer != -1){

						mes->status=2;

						//sprintf(mes->text,"successful");

						Printdatabase(&list);

						printf("\n");

					}

					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply);  
					
				break; 

				case EXIT_GAME_EARLY :
					//If you quit the game till it end

					mes->lose = 1;

					statsGameLose[mes->id]++;

					mes->receiverPlayer = OpponentID(&list,mes->id);

					statsGameWin[mes->receiverPlayer]++;

					movmentArray[mes->receiverPlayer] = -5;

					Disconnect_player(&list,mes->id);

					Disconnect_player(&list,mes->receiverPlayer);

					Printdatabase(&list);

					printf("\n");
					
					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply);  
					
				break;  

				case SHOW_PERSONAL_STATISTICS :
					//To print stat of current player

					if(Find(&list,mes->id)){

						mes->winstat = statsGameWin[mes->id];

						mes->losestat = statsGameLose[mes->id];

					}
					else{

						mes->winstat = statsGameWin[mes->id];

						mes->losestat = statsGameLose[mes->id];

					}

					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply);  
					
				break;   

				case SHOW_GENERAL_STATISTICS :
					//To print stats of all players in server

					for(size_t i = 0; i < SizeArray; i++){

						statisticArray[i].Score = 5*statsGameWin[statisticArray[i].ID] - 2*statsGameLose[statisticArray[i].ID];
					
					}

					sort(statisticArray,SizeArray);

					

					for(size_t i = 0; i < 5; i++){
					
						if (statisticArray[i].ID == mes->id){

							usl = 1;

						}

					}

					if (usl == 1){
					
						j = 0;

						for(int i = 0; j < 5; i = i +6){

							mes->text[i] = ' ';

							mes->text[i+1] = statisticArray[j].ID;

							mes->text[i+2] = ' ';

							mes->text[i+3] = statsGameWin[statisticArray[j].ID];

							mes->text[i+4] = ' ';

							mes->text[i+5] = statsGameLose[statisticArray[j].ID];
							
							j++;
						
						}

					}
					else{

						j = 0;

						for(int i = 0; j < 6; i = i +6){

							if (j != 5){

								mes->text[i] = ' ';

								mes->text[i+1] = statisticArray[j].ID;

								mes->text[i+2] = ' ';

								mes->text[i+3] = statsGameWin[statisticArray[j].ID];

								mes->text[i+4] = ' ';

								mes->text[i+5] = statsGameLose[statisticArray[j].ID];

							}
							else{
							
								mes->text[i] = ' ';

								mes->text[i+1] = mes->id;

								mes->text[i+2] = ' ';

								mes->text[i+3] = statsGameWin[mes->id];

								mes->text[i+4] = ' ';

								mes->text[i+5] = statsGameLose[mes->id];

							}
					
							j++;
						
						}

					}

					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply);  
					
				break;   

				case SHOW_DATABASE_ON_SERVERSIDE :
					//To print DB in server

					Printdatabase(&list);

					printf("\n");

					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply);  
					
				break;  

				case PLAYER_WIN_GAME :
					//If you win the game

					statsGameWin[mes->id]++;

					mes->receiverPlayer = OpponentID(&list,mes->id);

					statsGameLose[mes->receiverPlayer]++;

					movmentArray[mes->receiverPlayer] = -15;

					Disconnect_player(&list,mes->id);

					Disconnect_player(&list,mes->receiverPlayer);

					Printdatabase(&list);

					printf("\n");
					
					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply); 
					
				break; 

				case DELETE_PLAYER :
					//To delete player from database
					if (Delete_players(&list,mes->id) ){

						statsGameWin[mes->id] = 0;

						statsGameLose[mes->id] = 0;

						movmentArray[mes->id] = -2;

						//sprintf(mes->text,"gamer logoff");

					}
					else{

						//sprintf(mes->text,"No gamer with %d ID",mes->id);

					}
					
					memcpy(zmq_msg_data(&reply),mes,sizeof(message));
					
					zmq_msg_send(&reply, respond, 0);
					
					zmq_msg_close(&reply);
					
				break;        
					
			}
			
		}
	
	}

	zmq_close(respond);
	
	return 0;
}
