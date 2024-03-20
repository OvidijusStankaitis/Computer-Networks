#!/bin/bash

if [ "$1" == "run" ]; then
    clear
    javac PokalbiuKlientas.java
    java PokalbiuKlientas 20000
    clear

elif [ "$1" == "clean" ]; then
    rm *.class
    clear
fi