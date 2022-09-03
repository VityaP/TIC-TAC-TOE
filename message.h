#ifndef MESSAGE_H
#define MESSAGE_H

#define Sizetext 127

typedef struct _msg{    
    int     id;
    int     movement;
    int     receiverPlayer;
    int     decision;
    int     status; //0 manual conection //1 waiting //2 in game //3 in DB but mo in game
    int     lose;   //0 manual //1 if lose
    int     win;
    int     winstat;
    int     losestat;
    int     playertype;
    int     action;
    void*   socket;
    char    text[Sizetext];
} message;

#define DELETE_PLAYER                  -1
#define REGISTER_PLAYER                 1
#define UPDATE_MOVE_ON_OPPONENTS_SIDE   2
#define CHECK_IF_OPPONENT_MAKE_MOVE     3
#define FIND_OPPONENT                   4
#define EXIT_GAME_EARLY                 5
#define SHOW_PERSONAL_STATISTICS        6
#define SHOW_GENERAL_STATISTICS         7
#define SHOW_DATABASE_ON_SERVERSIDE     8
#define PLAYER_WIN_GAME                 9

#define PROGRAM_RUN 0
#define PROGRAM_STOP 3

#endif