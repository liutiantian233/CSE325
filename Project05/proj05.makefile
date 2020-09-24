# Make Proj05
# Tianrui Liu

proj05: proj05.student.o
	g++ -std=c++11 -Wall -lpthread -o proj05 proj05.student.o
proj05.student.o: proj05.student.c
	g++ -std=c++11 -Wall -c proj05.student.c
