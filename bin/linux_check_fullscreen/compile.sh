#/bin/bash
g++ main.cpp -o ../check_fullscreen -static-libgcc -O2 /usr/lib/gcc/i486-linux-gnu/4.4.1/libstdc++.a -I"../../build/includes/SFML-1.6/include" -L"../../lib" -lsfml-graphics -lsfml-window -lsfml-system
