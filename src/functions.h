/*
This program is the private property of Victor Petrosyan. 
Any use without the consent of the author is prohibited.
*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <map>
#include <vector>
#include <cstring>
#include <iostream>
#include <utility>
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

class Colourize{
public:
    Colourize(){
        colours["ENDC"]     = "\033[0m";
        colours["RED"]      = "\033[91m";
        colours["GREEN"]    = "\033[92m";
        colours["YELLOW"]   = "\033[93m";
        colours["BLUE"]     = "\033[94m";
        colours["VIOLET"]   = "\033[95m";
    }
    std::string paint(std::string colour, std::string text){
        return colours[colour] + text + colours["ENDC"];
    }
private:
    std::map<std::string, std::string> colours;
};

extern int Find(std::map<int, Player>& players, int id);

extern void PrintDatabase(std::map<int, Player>& players);

extern bool DeletePlayers(std::map<int, Player>& players, int id);

extern void DisconnectPlayer(std::map<int, Player>& players, int id);

extern bool ConnectPlayer(std::map<int, Player>& players, int id, int type);

extern int OpponentID(std::map<int, Player>& players, int id);

extern int GetStatus(std::map<int, Player>& players, int id);

extern bool EnteringAtServer(std::map<int, Player>& players, int id, int type, int status);





extern void PrintInfo(const std::vector<int>& Array);

extern void PrintGame(const std::vector<char>& Array, const std::vector<char>& global_win, Colourize painter);

extern int GetPositionAtLocalCell(const std::vector<char>& Array, int index);

extern int GetPositionAtGlobalCell(const std::vector<char>& Array, int index);

extern int ProcessAndVerifyMove(std::vector<char>& Array, int lastindex, int userindex, char what, const std::vector<int>& taken, int escape);

static int CheckLocal(const std::vector<char>& Array, int lastindex, int userindex);

extern bool CheckLocalWin(const std::vector<char>& Array, int lastindex);

extern void MarkGlobalWin(std::vector<char>& Array, int index, char what);

static int CheckGlobal(const std::vector<char>& Array, int index);

extern bool CheckGlobalWin(const std::vector<char>& Array);

#endif 
/*FUNCTIONS_H*/