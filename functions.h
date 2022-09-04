#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <map>
#include "message.h"

#include <stdio.h>
#define SizeV 81


struct Player{
	int id       	= EMPTY_CELL;
	int id_opponent = EMPTY_CELL;
    int type		= EMPTY_CELL;
	int last_move 	= EMPTY_CELL;
    int status		= STATUS_MANUAL;
	int winRate  	= 0;
	int loseRate 	= 0;

	friend bool operator<(const Player& lhs, const Player& rhs){
		return lhs.id < rhs.id;
	}
};


extern int Find(std::map<int, Player>& players, int id);

extern void Printdatabase(std::map<int, Player>& players);

extern int Delete_players(std::map<int, Player>& players, int id);

extern void Disconnect_player(std::map<int, Player>& players, int id);

extern int Connect_player(std::map<int, Player>& players, int id, int type);

extern int OpponentID(std::map<int, Player>& players, int id);

extern int Getstatus(std::map<int, Player>& players, int id);

extern int EnteringTOserver(std::map<int, Player>& players, int id, int type, int status);





extern void PrintInfo( int * Array);

extern void PrintGame( char * Array);

extern int Position( char * Array , int index);

extern int PositionforCell( char * Array , int index);

extern int Add( char * Array , int lastindex , int userindex , char what , int * taken,int escape);

extern int Check( char * Array , int lastindex , int userindex);

extern int CheckWinlocal( char * Array , int lastindex );

extern void AddglobalWin( char * Array , int index , char what);

extern int CheckGlobal( char * Array , int index );

extern int CheckWinglobal( char * Array );

#endif 
/*FUNCTIONS_H*/