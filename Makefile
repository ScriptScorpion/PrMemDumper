CC := g++
CCF := -O1 -std=c++17
SRC := proc_memory.cpp
main:
	@$(CC) $(CCF) $(SRC) -o prmemdumper
