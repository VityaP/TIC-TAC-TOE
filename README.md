# Команды в стартовом терминале
## 1. `docker network create zmq`
## 2. `docker-compose up --build`
### 3. Создаем два дополнительных терминала для игроков, так как в активном терминале в дальнейшем будут писаться логи сервера

# Команды в терминале первого игрока
## 4. `docker exec -it zmq_first_player bash`
## 5. `./player_zmq_tic_tac_toe` $ID$
## 6. $Важно!!!$ На запрос 
## `"Enter the address of server :" ` $вводим$ `docker`

# Команды в терминале второго игрока
## 7. `docker exec -it zmq_second_player bash`
## 8. `./player_zmq_tic_tac_toe` $ID$
## 9. $Важно!!!$ На запрос 
## `"Enter the address of server :" ` $вводим$ `docker`