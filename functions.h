#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <map>
#include "message.h"

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





extern void PrintInfo(const std::vector<int>& Array);

extern void PrintGame(const std::vector<char>& Array);

extern int Position(const std::vector<char>& Array, int index);

extern int PositionforCell(const std::vector<char>& Array, int index);

extern int Add(std::vector<char>& Array, int lastindex, int userindex, char what, const std::vector<int>& taken, int escape);

extern int Check(const std::vector<char>& Array, int lastindex, int userindex);

extern int CheckWinlocal(const std::vector<char>& Array, int lastindex);

extern void AddglobalWin(std::vector<char>& Array, int index, char what);

extern int CheckGlobal(const std::vector<char>& Array, int index);

extern int CheckWinglobal(const std::vector<char>& Array);

#endif 
/*FUNCTIONS_H*/