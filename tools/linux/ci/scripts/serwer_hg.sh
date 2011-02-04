#!/bin/bash

CO_ILE_SPRAWDZAMY=10s

echo jestem serwer!

while [ 1 -eq 1 ]
do
  sleep $CO_ILE_SPRAWDZAMY
  ./agent.sh
done
