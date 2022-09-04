
/*
This program is the private property of Viktor Petrosyan. 
Any use without the consent of the author is prohibited. 
The program began writing on January 4, 2019 
Last edit 26 January 2019 at 15: 49
*/

#include <map>

#include "functions.h"
#include "message.h"

int Find(std::map<int, Player>& players, int id){
    return players.count(id);
}

void Printdatabase(std::map<int, Player>& players){
    printf("|       ID      |    STATUS     |     TYPE      |   ID_OPPONENT |\n");
    for(const auto& [id, player] : players){
        printf("|\t%d\t|\t%d\t|\t%d\t|\t%d\t|\n", player.id, player.status, player.type, player.id_opponent);
    }
}

int Delete_players(std::map<int, Player>& players, int id){
    if (players.count(id) != 0) {
        players[id].id = -1;
        players[id].id_opponent = -1;
        players[id].status = STATUS_PLAYER_DELETED;
        players[id].type = -1;
        return 1;
    }
    return 0;
}

int Connect_player(std::map<int, Player>& players, int id, int type){
    if( (players.count(id) != 0) && (players[id].status == STATUS_IN_GAME) ){
        return 0;
    }
    for(auto& [player_id, opponent] : players){
        if ((opponent.id != id) && (opponent.type != type)){
            if(opponent.status == STATUS_WAIT_FOR_OPPONENT){
                players[id].status = STATUS_IN_GAME;
                players[id].id_opponent = opponent.id;
                opponent.status = STATUS_IN_GAME;
                opponent.id_opponent = id;
                return 1;
            }
        }
    }
    return 0;
}

void Disconnect_player(std::map<int, Player>& players, int id){
    if( players.count(id) != 0 ){
        players[id].status = STATUS_REGISTERED_BUT_NOT_IN_GAME;
    }
}

int OpponentID(std::map<int, Player>& players, int id){
    if( (players.count(id) != 0) && (players[id].status == STATUS_IN_GAME) ){
        return players[id].id_opponent;
    }
    return -1;
}

int Getstatus(std::map<int, Player>& players, int id){
    if( players.count(id) != 0 ){
        return players[id].status;
    }    
    return -1;
}

int EnteringTOserver(std::map<int, Player>& players, int id, int type, int status){
    if( players.count(id) != 0 ){
        auto& player_ref = players[id];
        if( player_ref.status != STATUS_REGISTERED_BUT_NOT_IN_GAME){
            return 0;
        }
        player_ref.type = type;
        player_ref.id_opponent = 0;
        if ( status == STATUS_PLAYER_WITH_FRIEND) {
            player_ref.status = STATUS_IN_GAME;
        }
        else{
            player_ref.status = STATUS_WAIT_FOR_OPPONENT;
        }
    }
    else{
        Player tmp;
        tmp.id = id;
        tmp.type = type;
        if ( status == STATUS_PLAYER_WITH_FRIEND) {
            tmp.status = STATUS_IN_GAME;
        }
        else{
            tmp.status = STATUS_WAIT_FOR_OPPONENT;
        }
        players[id] = tmp;
    }
    return 1;
}





void PrintInfo( int * Array){
    for(size_t n = 0; n <= 54; n = n + 27){
        for(size_t s = 0; s <= 6; s = s + 3){
            for(size_t k = 0; k <= 18; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    printf(" %d ", Array[i+k+s+n] );
                }
                printf("\t");
            }
            printf("\n");
        }
        printf("\n");
    }
}

void PrintGame( char * Array){
    for(size_t n = 0; n <= 54; n = n + 27){
        for(size_t s = 0; s <= 6; s = s + 3){
            for(size_t k = 0; k <= 18; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    printf(" %c ", Array[i+k+s+n] );
                }
                printf("\t");
            }
            printf("\n");
        }
        printf("\n");
    }
}

int Position( char * Array , int index){
    for(size_t n = 0; n <= 54; n = n + 27){
        for(size_t s = 0; s <= 6; s = s + 3){
            for(size_t k = 0; k <= 18; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    if ( i+k+s+n == index ){
                        return ( i + s );
                    }
                }
            }
        }
    }
    return -2;
}

int PositionforCell( char * Array , int index){
    for(size_t n = 0; n <= 54; n = n + 27){
        for(size_t s = 0; s <= 6; s = s + 3){
            for(size_t k = 0; k <= 18; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    if ( i+k+s+n == index ){
                        if( k == 0 && n == 0){
                            return 1;
                        }
                        if( k == 9 && n == 0){
                            return 2;
                        }
                        if( k == 18 && n == 0){
                            return 3;
                        }
                        if( k == 0 && n == 27){
                            return 4;
                        }
                        if( k == 9 && n == 27){
                            return 5;
                        }
                        if( k == 18 && n == 27){
                            return 6;
                        }
                        if( k == 0 && n == 54){
                            return 7;
                        }
                        if( k == 9 && n == 54){
                            return 8;
                        }
                        if( k == 18 && n == 54){
                            return 9;
                        }
                    }
                }
            }
        }
    }
    return -2;
}

int Add( char * Array , int lastindex , int userindex , char what , int * taken,int escape){

    int k , n;

    switch (lastindex){

        case 1:
            k = 0;
            n = 0;
            break;

        case 2:
            k = 9;
            n = 0;
            break;

        case 3:
            k = 18;
            n = 0;
            break;

        case 4:
            k = 0;
            n = 27;
            break;
        
        case 5:
            k = 9;
            n = 27;
            break;

        case 6:
            k = 18;
            n = 27;
            break;

        case 7:
            k = 0;
            n = 54;
            break;

        case 8:
            k = 9;
            n = 54;
            break;

        case 9:
            k = 18;
            n = 54;
            break;
    
        default:
            break;
    }

    if(Array[userindex+k+n] == '.'){

        Array[userindex+k+n] = what;

        return 0;
    
    }
    else{

        if ( taken[lastindex] == 9 ){

            printf("There are no free cells in the block. \nSelect any other index [1 ... 81] which is free\n");

            return 2;

        }

        if (escape){

            printf("This cell is taken. Choose another cell \n");

            printf("Enter index [1 ... 9] \n");

            return 1;
        }
        else{

            return 0;

        }

    }

}

int Check( char * Array , int lastindex , int userindex){

    int i , s;

    int k , n;

    switch (userindex){

        case 1:
            
            i = 1;
            s = 0;
            break;

        case 2:
            i = 2;
            s = 0;
            break;

        case 3:
            i = 3;
            s = 0;
            break;

        case 4:
            i = 1;
            s = 3;
            break;
        
        case 5:
            i = 2;
            s = 3;
            break;

        case 6:
            i = 3;
            s = 3;
            break;

        case 7:
            i = 1;
            s = 6;
            break;

        case 8:
            i = 2;
            s = 6;
            break;

        case 9:
            i = 3;
            s = 6;
            break;
    
        default:
            break;
    }

    switch (lastindex){

        case 1:
            k = 0;
            n = 0;
            break;

        case 2:
            k = 9;
            n = 0;
            break;

        case 3:
            k = 18;
            n = 0;
            break;

        case 4:
            k = 0;
            n = 27;
            break;
        
        case 5:
            k = 9;
            n = 27;
            break;

        case 6:
            k = 18;
            n = 27;
            break;

        case 7:
            k = 0;
            n = 54;
            break;

        case 8:
            k = 9;
            n = 54;
            break;

        case 9:
            k = 18;
            n = 54;
            break;
    
        default:
            break;
    }

    return Array[i+k+s+n] ;

}

int CheckWinlocal( char * Array , int lastindex ){

    int OK = 0;

    int u1 = 1;
    int u2 = 2;
    int u3 = 3;
    int u4 = 4;
    int u5 = 5;
    int u6 = 6;
    int u7 = 7;
    int u8 = 8;
    int u9 = 9;

    if (Check ( Array ,lastindex , u1 ) != '.'){

        if  ( Check ( Array ,lastindex , u1 ) == Check ( Array ,lastindex , u2 ) ) {

            OK = 1;
        
        }

        if  ( Check ( Array ,lastindex , u2 ) == Check ( Array ,lastindex , u3 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;

            }
        
        }
        else{

            OK = 0;

        }
    }
    else{

        OK = 0;

    }


    if (Check ( Array ,lastindex , u4 ) != '.'){

        if  ( Check ( Array ,lastindex , u4 ) == Check ( Array ,lastindex , u5 ) ) {

            OK = 1;
        
        }

        if  ( Check ( Array ,lastindex , u5 ) == Check ( Array ,lastindex , u6 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }
    }
    else{

        OK = 0;

    }


    if (Check ( Array ,lastindex , u7 ) != '.'){

        if  ( Check ( Array ,lastindex , u7 ) == Check ( Array ,lastindex , u8 ) ) {

            OK = 1;
        
        }

        if  ( Check ( Array ,lastindex , u8 ) == Check ( Array ,lastindex , u9 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (Check ( Array ,lastindex , u1 ) != '.'){

        if  ( Check ( Array ,lastindex , u1 ) == Check ( Array ,lastindex , u4 ) ) {

            OK = 1;
        
        }

        if  ( Check ( Array ,lastindex , u4 ) == Check ( Array ,lastindex , u7 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (Check ( Array ,lastindex , u2 ) != '.'){

        if  ( Check ( Array ,lastindex , u2 ) == Check ( Array ,lastindex , u5 ) ) {

            OK = 1;
        
        }

        if  ( Check ( Array ,lastindex , u5 ) == Check ( Array ,lastindex , u8 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (Check ( Array ,lastindex , u3 ) != '.'){

        if  ( Check ( Array ,lastindex , u3 ) == Check ( Array ,lastindex , u6 ) ) {

            OK = 1;
        
        }

        if  ( Check ( Array ,lastindex , u6 ) == Check ( Array ,lastindex , u9 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (Check ( Array ,lastindex , u1 ) != '.'){

        if  ( Check ( Array ,lastindex , u1 ) == Check ( Array ,lastindex , u5 ) ) {

            OK = 1;
        
        }

        if  ( Check ( Array ,lastindex , u5 ) == Check ( Array ,lastindex , u9 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (Check ( Array ,lastindex , u3 ) != '.'){

        if  ( Check ( Array ,lastindex , u3 ) == Check ( Array ,lastindex , u5 ) ) {

            OK = 1;
        
        }

        if  ( Check ( Array ,lastindex , u5 ) == Check ( Array ,lastindex , u7 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }

    return 0;

}

void AddglobalWin( char * Array , int index , char what){

    if(Array[index] == '.'){

        Array[index] = what;

    }
    
}

int CheckGlobal( char * Array , int index ){

    return Array[index];

}

int CheckWinglobal( char * Array ){

    int OK = 0;

    int u1 = 1;
    int u2 = 2;
    int u3 = 3;
    int u4 = 4;
    int u5 = 5;
    int u6 = 6;
    int u7 = 7;
    int u8 = 8;
    int u9 = 9;

    if (CheckGlobal ( Array , u1 ) != '.'){

        if  ( CheckGlobal ( Array , u1 ) == CheckGlobal ( Array , u2 ) ) {

            OK = 1;
        
        }

        if  ( CheckGlobal ( Array , u2 ) == CheckGlobal ( Array , u3 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;

            }
        
        }
        else{

            OK = 0;

        }
    }
    else{

        OK = 0;

    }


    if (CheckGlobal ( Array , u4 ) != '.'){

        if  ( CheckGlobal ( Array , u4 ) == CheckGlobal ( Array , u5 ) ) {

            OK = 1;
        
        }

        if  ( CheckGlobal ( Array , u5 ) == CheckGlobal ( Array , u6 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }
    }
    else{

        OK = 0;

    }


    if (CheckGlobal ( Array , u7 ) != '.'){

        if  ( CheckGlobal ( Array , u7 ) == CheckGlobal ( Array , u8 ) ) {

            OK = 1;
        
        }

        if  ( CheckGlobal ( Array , u8 ) == CheckGlobal ( Array , u9 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (CheckGlobal ( Array , u1 ) != '.'){

        if  ( CheckGlobal ( Array , u1 ) == CheckGlobal ( Array , u4 ) ) {

            OK = 1;
        
        }

        if  ( CheckGlobal ( Array , u4 ) == CheckGlobal ( Array , u7 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (CheckGlobal ( Array , u2 ) != '.'){

        if  ( CheckGlobal ( Array , u2 ) == CheckGlobal ( Array , u5 ) ) {

            OK = 1;
        
        }

        if  ( CheckGlobal ( Array , u5 ) == CheckGlobal ( Array , u8 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (CheckGlobal ( Array , u3 ) != '.'){

        if  ( CheckGlobal ( Array , u3 ) == CheckGlobal ( Array , u6 ) ) {

            OK = 1;
        
        }

        if  ( CheckGlobal ( Array , u6 ) == CheckGlobal ( Array , u9 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (CheckGlobal ( Array , u1 ) != '.'){

        if  ( CheckGlobal ( Array , u1 ) == CheckGlobal ( Array , u5 ) ) {

            OK = 1;
        
        }

        if  ( CheckGlobal ( Array , u5 ) == CheckGlobal ( Array , u9 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }


    if (CheckGlobal ( Array , u3 ) != '.'){

        if  ( CheckGlobal ( Array , u3 ) == CheckGlobal ( Array , u5 ) ) {

            OK = 1;
        
        }

        if  ( CheckGlobal ( Array , u5 ) == CheckGlobal ( Array , u7 ) ) {

            if (OK == 1){

                return 1;
            
            }
            else{

                OK = 0;
                
            }
        
        }
        else{

            OK = 0;

        }

    }
    else{

        OK = 0;

    }

    return 0;

}
