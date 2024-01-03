#!/bin/bash  

./remoteClient -i $1 -p $2 -d Server/floder1 &
./remoteClient -i $1 -p $2 -d Server &
./remoteClient -i $1 -p $2 -d Server/floder2 &
./remoteClient -i $1 -p $2 -d Server/floder3/floder3_3 &
./remoteClient -i $1 -p $2 -d Server/floder2/floder2_2 &
./remoteClient -i $1 -p $2 -d Server/floder1 &
./remoteClient -i $1 -p $2 -d Server/floder3 &
./remoteClient -i $1 -p $2 -d Server/floder3/floder3_2 &
./remoteClient -i $1 -p $2 -d Server/floder3/floder3_2 &
./remoteClient -i $1 -p $2 -d Server/floder3 &
