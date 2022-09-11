/*
This program is the private property of Victor Petrosyan. 
Any use without the consent of the author is prohibited.
*/

#include "functions.h"

int Find(std::map<int, Player>& players, int id){
    return players.count(id);
}

void Printdatabase(std::map<int, Player>& players){
    std::cout << "|       ID      |    STATUS     |     TYPE      |   ID_OPPONENT |\n";
    for(const auto& tmp : players){
        auto id = tmp.first;
        auto player = tmp.second;
        std::cout << "|\t" << player.id << "\t|\t" << player.status << "\t|\t" << player.type << "\t|\t" << player.id_opponent << "\t|\n";
    }
}

int Delete_players(std::map<int, Player>& players, int id){
    if(players.count(id) != 0){
        players[id].id = -1;
        players[id].id_opponent = -1;
        players[id].status = STATUS_PLAYER_DELETED;
        players[id].type = -1;
        return 1;
    }
    return 0;
}

int Connect_player(std::map<int, Player>& players, int id, int type){
    if((players.count(id) != 0) && (players[id].status == STATUS_IN_GAME)){
        return 0;
    }
    for(auto& tmp : players){
        auto& player_id = tmp.first;
        auto& opponent = tmp.second;
        if((opponent.id != id) && (opponent.type != type)){
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
    if(players.count(id) != 0){
        players[id].status = STATUS_REGISTERED_BUT_NOT_IN_GAME;
    }
}

int OpponentID(std::map<int, Player>& players, int id){
    if((players.count(id) != 0) && (players[id].status == STATUS_IN_GAME)){
        return players[id].id_opponent;
    }
    return -1;
}

int Getstatus(std::map<int, Player>& players, int id){
    if(players.count(id) != 0){
        return players[id].status;
    }
    return -1;
}

int EnteringTOserver(std::map<int, Player>& players, int id, int type, int status){
    if(players.count(id) != 0){
        auto& player_ref = players[id];
        if(player_ref.status != STATUS_REGISTERED_BUT_NOT_IN_GAME){
            return 0;
        }
        player_ref.type = type;
        player_ref.id_opponent = 0;
        if(status == STATUS_PLAYER_WITH_FRIEND){
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
        if(status == STATUS_PLAYER_WITH_FRIEND){
            tmp.status = STATUS_IN_GAME;
        }
        else{
            tmp.status = STATUS_WAIT_FOR_OPPONENT;
        }
        players[id] = tmp;
    }
    return 1;
}



void PrintInfo(const std::vector<int>& Array){
    for(size_t n = 0; n < 3 * 27; n = n + 27){
        for(size_t s = 0; s < 3 * 3; s = s + 3){
            for(size_t k = 0; k < 3 * 9; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    std::cout << " " << Array[i+k+s+n] << " ";
                }
                std::cout << "\t";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

void PrintGame(const std::vector<char>& Array, const std::vector<char>& global_win, Colourize painter){
    for(size_t n = 0; n < 3 * 27; n = n + 27){
        for(size_t s = 0; s < 3 * 3; s = s + 3){
            for(size_t k = 0; k < 3 * 9; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    if(Array[i+k+s+n] == 'x'){
                        std::cout << painter.paint("RED", " X ");
                        continue;
                    }
                    if(Array[i+k+s+n] == '0'){
                        std::cout << painter.paint("BLUE", " O ");
                        continue;
                    }
                    int idx = 1;
                    while((i+k+s+n) > (9 * idx)){
                        ++idx;
                    }
                    if(global_win[idx] == 'x'){
                        std::cout << painter.paint("RED", " . ");
                        continue;
                    }
                    if(global_win[idx] == '0'){
                        std::cout << painter.paint("BLUE", " . ");
                        continue;
                    }
                    std::cout << painter.paint("ENDC", " . ");
                }
                std::cout << "\t";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

int Position(const std::vector<char>& Array, int index){
    for(size_t n = 0; n < 3 * 27; n = n + 27){
        for(size_t s = 0; s < 3 * 3; s = s + 3){
            for(size_t k = 0; k < 3 * 9; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    if((i + k + s + n) == index){
                        return (i + s);
                    }
                }
            }
        }
    }
    return -2;
}

int PositionforCell(const std::vector<char>& Array, int index){
    for(size_t n = 0; n <= 54; n = n + 27){
        for(size_t s = 0; s <= 6; s = s + 3){
            for(size_t k = 0; k <= 18; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    if ( i+k+s+n == index ){
                        return (3 * (n / 27) + (k / 9) + 1);
                    }
                }
            }
        }
    }
    return -2;
}

int Add(std::vector<char>& Array, int lastindex, int userindex, char what, const std::vector<int>& taken, int escape){
    int n = 27 * ((lastindex - 1) / 3);
    int k = 9 * ((lastindex - 1) % 3);

    if(Array[userindex + k + n] == '.'){
        Array[userindex+k+n] = what;
        return 0;
    }
    else{
        if(taken[lastindex] == 9){
            std::cout << "There are no free cells in the block. \n";
            std::cout << "Select any other index [1 ... 81] which is free\n";
            return 2;
        }
        if(escape != 0){
            std::cout << "This cell is taken. Choose another cell \n";
            std::cout << "Enter index [1 ... 9] \n";
            return 1;
        }
        else{
            return 0;
        }
    }
}

int Check(const std::vector<char>& Array, int lastindex, int userindex){
    int i = 1 + ((userindex - 1) % 3);
    int k = 9 * ((lastindex - 1) % 3);
    int s = 3 * ((userindex - 1) / 3);
    int n = 27 * ((lastindex - 1) / 3);
    return Array[i + k + s + n];
}

int CheckWinlocal(const std::vector<char>& Array, int lastindex){

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

    if(Check(Array, lastindex, u1) != '.'){
        if(Check(Array, lastindex, u1) == Check(Array, lastindex, u2)){
            OK = 1;
        }
        if(Check(Array, lastindex, u2) == Check(Array, lastindex, u3)){
            if(OK == 1){
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

    if(Check(Array, lastindex, u4) != '.'){
        if(Check(Array, lastindex, u4) == Check(Array, lastindex, u5)){
            OK = 1;
        }
        if(Check(Array, lastindex, u5) == Check(Array, lastindex, u6)){
            if(OK == 1){
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

    if(Check(Array, lastindex, u7) != '.'){
        if(Check(Array, lastindex, u7) == Check(Array, lastindex, u8)){
            OK = 1;
        }
        if(Check(Array, lastindex, u8) == Check(Array, lastindex, u9)){
            if(OK == 1){
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

    if(Check(Array, lastindex, u1) != '.'){
        if(Check(Array, lastindex, u1) == Check(Array, lastindex, u4)){
            OK = 1;
        }
        if(Check(Array, lastindex, u4) == Check(Array, lastindex, u7)){
            if(OK == 1){
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

    if(Check(Array, lastindex, u2) != '.'){
        if(Check(Array, lastindex, u2) == Check(Array, lastindex, u5)){
            OK = 1;
        }
        if(Check(Array, lastindex, u5) == Check(Array, lastindex, u8)){
            if(OK == 1){
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

    if(Check(Array, lastindex, u3) != '.'){
        if(Check(Array, lastindex, u3) == Check(Array, lastindex, u6)){
            OK = 1;
        }
        if(Check(Array, lastindex, u6) == Check(Array, lastindex, u9)){
            if(OK == 1){
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

    if(Check(Array, lastindex, u1) != '.'){
        if(Check(Array, lastindex, u1) == Check(Array, lastindex, u5)){
            OK = 1;
        }
        if(Check(Array, lastindex, u5) == Check(Array, lastindex, u9)){
            if(OK == 1){
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

    if(Check(Array, lastindex, u3) != '.'){
        if(Check(Array, lastindex, u3) == Check(Array, lastindex, u5)){
            OK = 1;
        }
        if(Check(Array, lastindex, u5) == Check(Array, lastindex, u7)){
            if(OK == 1){
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

void AddglobalWin(std::vector<char>& Array, int index, char what){
    if(Array[index] == '.'){
        Array[index] = what;
    }
}

int CheckGlobal(const std::vector<char>& Array, int index){
    return Array[index];
}

int CheckWinglobal(const std::vector<char>& Array){
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

    if(CheckGlobal(Array, u1) != '.'){
        if(CheckGlobal(Array, u1) == CheckGlobal(Array, u2)){
            OK = 1;
        }
        if(CheckGlobal(Array, u2) == CheckGlobal(Array, u3)){
            if(OK == 1){
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

    if(CheckGlobal(Array, u4) != '.'){
        if(CheckGlobal(Array, u4) == CheckGlobal(Array, u5)){
            OK = 1;
        }
        if(CheckGlobal(Array, u5) == CheckGlobal(Array, u6)){
            if(OK == 1){
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

    if(CheckGlobal(Array, u7) != '.'){
        if(CheckGlobal(Array, u7) == CheckGlobal(Array, u8)){
            OK = 1;
        }
        if(CheckGlobal(Array, u8) == CheckGlobal(Array, u9)){
            if(OK == 1){
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

    if(CheckGlobal(Array, u1) != '.'){
        if(CheckGlobal(Array, u1) == CheckGlobal(Array, u4)){
            OK = 1;
        }
        if(CheckGlobal(Array, u4) == CheckGlobal(Array, u7)){
            if(OK == 1){
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

    if(CheckGlobal(Array, u2) != '.'){
        if(CheckGlobal(Array, u2) == CheckGlobal(Array, u5)){
            OK = 1;
        }
        if(CheckGlobal(Array, u5) == CheckGlobal(Array, u8)){
            if(OK == 1){
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


    if(CheckGlobal(Array, u3) != '.'){
        if(CheckGlobal(Array, u3) == CheckGlobal(Array, u6)){
            OK = 1;        
        }
        if(CheckGlobal(Array, u6) == CheckGlobal(Array, u9)){
            if(OK == 1){
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

    if(CheckGlobal(Array, u1) != '.'){
        if(CheckGlobal(Array, u1) == CheckGlobal(Array, u5)){
            OK = 1;
        }
        if(CheckGlobal(Array, u5) == CheckGlobal(Array, u9)){
            if(OK == 1){
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

    if(CheckGlobal(Array, u3) != '.'){
        if(CheckGlobal(Array, u3) == CheckGlobal(Array, u5)){
            OK = 1;
        }
        if(CheckGlobal(Array, u5) == CheckGlobal(Array, u7)){
            if(OK == 1){
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