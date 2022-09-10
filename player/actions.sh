#!/usr/bin/env bash

until cd /opt/tmp
do
    echo "Waiting for docker-compose"
done
cd /opt/src
cp /opt/tmp/* /opt/src
/opt/cmake/bin/cmake /opt/src
make
./player_zmq_tic_tac_toe 1