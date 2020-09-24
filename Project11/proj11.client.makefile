# Make Proj11
# Tianrui Liu

proj11: proj11.client.o
	g++ -std=c++11 -Wall proj11.client.o -o proj11
proj11.client.o: proj11.client.c
	g++ -std=c++11 -Wall -c proj11.client.c