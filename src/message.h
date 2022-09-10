/*
This program is the private property of Victor Petrosyan. 
Any use without the consent of the author is prohibited.
*/

#ifndef MESSAGE_H
#define MESSAGE_H

struct message{    
    int     id;
    int     movement;
    int     opponentID;
    int     status;
    int     lose;   //0 manual //1 if lose
    int     win;
    int     winstat;
    int     losestat;
    int     playertype;
    int     action;
    void*   socket;
};

#define DELETE_PLAYER                       -1
#define REGISTER_PLAYER                      1
#define UPDATE_MOVE_ON_OPPONENTS_SIDE        2
#define CHECK_IF_OPPONENT_MAKE_MOVE          3
#define FIND_OPPONENT                        4
#define EXIT_GAME_EARLY                      5
#define SHOW_PERSONAL_STATISTICS             6
#define SHOW_GENERAL_STATISTICS              7
#define SHOW_DATABASE_ON_SERVERSIDE          8
#define PLAYER_WIN_GAME                      9

#define PROGRAM_RUN                          0
#define PROGRAM_PAUSE                        1
#define PROGRAM_STOP                         3

#define OPPONENT_WIN                       -15
#define OPPONENT_LOSE                       -5
#define EMPTY_CELL                          -2

#define STATUS_PLAYER_DELETED               -1
#define STATUS_MANUAL                        0
#define STATUS_WAIT_FOR_OPPONENT             1
#define STATUS_IN_GAME                       2
#define STATUS_REGISTERED_BUT_NOT_IN_GAME    3
#define STATUS_PLAYER_WITH_FRIEND           11

#define NO                                   0
#define YES                                  1

#define END_OF_GAME                        100

#endif