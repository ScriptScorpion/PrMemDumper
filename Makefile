CC := g++
CCF := -O1 -std=c++17
FILE1 := proc_memory.cpp
FILE2 := proc_memory_write.cpp
main:
	@$(CC) $(CCF) $(FILE1) -o prmemdumper
	@$(CC) $(CCF) $(FILE2) -o prmemwriter
