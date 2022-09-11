/*
This program is the private property of Victor Petrosyan. 
Any use without the consent of the author is prohibited.
*/

#include "functions.h"

int Find(std::map<int, Player>& players, int id){
    return players.count(id);
}

void PrintDatabase(std::map<int, Player>& players){
    std::cout << "|       ID      |    STATUS     |     TYPE      |   ID_OPPONENT |\n";
    for(const auto& tmp : players){
        auto id = tmp.first;
        auto player = tmp.second;
        std::cout << "|\t" << player.id << "\t|\t" << player.status << "\t|\t" << player.type << "\t|\t" << player.id_opponent << "\t|\n";
    }
}

bool DeletePlayers(std::map<int, Player>& players, int id){
    if(players.count(id) != 0){
        players[id].id = -1;
        players[id].id_opponent = -1;
        players[id].status = STATUS_PLAYER_DELETED;
        players[id].type = -1;
        return true;
    }
    return false;
}

bool ConnectPlayer(std::map<int, Player>& players, int id, int type){
    if((players.count(id) != 0) && (players[id].status == STATUS_IN_GAME)){
        return false;
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
                return true;
            }
        }
    }
    return false;
}

void DisconnectPlayer(std::map<int, Player>& players, int id){
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

int GetStatus(std::map<int, Player>& players, int id){
    if(players.count(id) != 0){
        return players[id].status;
    }
    return -1;
}

bool EnteringAtServer(std::map<int, Player>& players, int id, int type, int status){
    if(players.count(id) != 0){
        auto& player_ref = players[id];
        if(player_ref.status != STATUS_REGISTERED_BUT_NOT_IN_GAME){
            return false;
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
    return true;
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

int GetPositionAtLocalCell(const std::vector<char>& Array, int index){
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

int GetPositionAtGlobalCell(const std::vector<char>& Array, int index){
    for(size_t n = 0; n < 3 * 27; n = n + 27){
        for(size_t s = 0; s < 3 * 3; s = s + 3){
            for(size_t k = 0; k < 3 * 9; k = k + 9){
                for(size_t i = 1; i <= 3; i++){
                    if((i+k+s+n) == index){
                        return (3 * (n / 27) + (k / 9) + 1);
                    }
                }
            }
        }
    }
    return -2;
}

int ProcessAndVerifyMove(std::vector<char>& Array, int lastindex, int userindex, char what, const std::vector<int>& taken, int escape){
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

int CheckLocal(const std::vector<char>& Array, int lastindex, int userindex){
    int i = 1 + ((userindex - 1) % 3);
    int k = 9 * ((lastindex - 1) % 3);
    int s = 3 * ((userindex - 1) / 3);
    int n = 27 * ((lastindex - 1) / 3);
    return Array[i + k + s + n];
}

bool CheckLocalWin(const std::vector<char>& Array, int lastindex){
    if(CheckLocal(Array, lastindex, 1) != '.'){
        if((CheckLocal(Array, lastindex, 1) == CheckLocal(Array, lastindex, 2)) && 
           (CheckLocal(Array, lastindex, 2) == CheckLocal(Array, lastindex, 3))){
            return true;
        }
    }
    if(CheckLocal(Array, lastindex, 4) != '.'){
        if((CheckLocal(Array, lastindex, 4) == CheckLocal(Array, lastindex, 5)) && 
           (CheckLocal(Array, lastindex, 5) == CheckLocal(Array, lastindex, 6))){
            return true;
        }
    }
    if(CheckLocal(Array, lastindex, 7) != '.'){
        if((CheckLocal(Array, lastindex, 7) == CheckLocal(Array, lastindex, 8)) && 
           (CheckLocal(Array, lastindex, 8) == CheckLocal(Array, lastindex, 9))){
            return true;
        }
    }
    if(CheckLocal(Array, lastindex, 1) != '.'){
        if((CheckLocal(Array, lastindex, 1) == CheckLocal(Array, lastindex, 4)) && 
           (CheckLocal(Array, lastindex, 4) == CheckLocal(Array, lastindex, 7))){
            return true;
        }
    }
    if(CheckLocal(Array, lastindex, 2) != '.'){
        if((CheckLocal(Array, lastindex, 2) == CheckLocal(Array, lastindex, 5)) && 
           (CheckLocal(Array, lastindex, 5) == CheckLocal(Array, lastindex, 8))){
            return true;
        }
    }
    if(CheckLocal(Array, lastindex, 3) != '.'){
        if((CheckLocal(Array, lastindex, 3) == CheckLocal(Array, lastindex, 6)) && 
           (CheckLocal(Array, lastindex, 6) == CheckLocal(Array, lastindex, 9))){
            return true;
        }
    }
    if(CheckLocal(Array, lastindex, 1) != '.'){
        if((CheckLocal(Array, lastindex, 1) == CheckLocal(Array, lastindex, 5)) && 
           (CheckLocal(Array, lastindex, 5) == CheckLocal(Array, lastindex, 9))){
            return true;
        }
    }
    if(CheckLocal(Array, lastindex, 3) != '.'){
        if((CheckLocal(Array, lastindex, 3) == CheckLocal(Array, lastindex, 5)) && 
           (CheckLocal(Array, lastindex, 5) == CheckLocal(Array, lastindex, 7))){
            return true;
        }
    }

    return false;
}

void MarkGlobalWin(std::vector<char>& Array, int index, char what){
    if(Array[index] == '.'){
        Array[index] = what;
    }
}

int CheckGlobal(const std::vector<char>& Array, int index){
    return Array[index];
}

bool CheckGlobalWin(const std::vector<char>& Array){
    if(CheckGlobal(Array, 1) != '.'){
        if((CheckGlobal(Array, 1) == CheckGlobal(Array, 2)) && 
           (CheckGlobal(Array, 2) == CheckGlobal(Array, 3))){
            return true;
        }
    }
    if(CheckGlobal(Array, 4) != '.'){
        if((CheckGlobal(Array, 4) == CheckGlobal(Array, 5)) && 
           (CheckGlobal(Array, 5) == CheckGlobal(Array, 6))){
            return true;
        }
    }
    if(CheckGlobal(Array, 7) != '.'){
        if((CheckGlobal(Array, 7) == CheckGlobal(Array, 8)) && 
           (CheckGlobal(Array, 8) == CheckGlobal(Array, 9))){
            return true;
        }
    }
    if(CheckGlobal(Array, 1) != '.'){
        if((CheckGlobal(Array, 1) == CheckGlobal(Array, 4)) && 
           (CheckGlobal(Array, 4) == CheckGlobal(Array, 7))){
            return true;
        }
    }
    if(CheckGlobal(Array, 2) != '.'){
        if((CheckGlobal(Array, 2) == CheckGlobal(Array, 5)) && 
           (CheckGlobal(Array, 5) == CheckGlobal(Array, 8))){
            return true;
        }
    }
    if(CheckGlobal(Array, 3) != '.'){
        if((CheckGlobal(Array, 3) == CheckGlobal(Array, 6)) && 
           (CheckGlobal(Array, 6) == CheckGlobal(Array, 9))){
            return true;
        }
    }
    if(CheckGlobal(Array, 1) != '.'){
        if((CheckGlobal(Array, 1) == CheckGlobal(Array, 5)) && 
           (CheckGlobal(Array, 5) == CheckGlobal(Array, 9))){
            return true;
        }
    }
    if(CheckGlobal(Array, 3) != '.'){
        if((CheckGlobal(Array, 3) == CheckGlobal(Array, 5)) && 
           (CheckGlobal(Array, 5) == CheckGlobal(Array, 7))){
            return true;
        }
    }

    return false;
}