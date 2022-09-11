#!/usr/bin/env bash

until cd /opt/tmp
do
    echo "Waiting for docker-compose __Server actions.sh script__"
done
cd /opt/src
cp /opt/tmp/* /opt/src
/opt/cmake/bin/cmake /opt/src
make
./server_zmq_tic_tac_toe