#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

void game_of_life(char **grid, int rows, int cols, int generations);
int count_live_neighbors(char **grid, int rows, int cols, int x, int y);
void write_output(char* filename,int*rows,int*cols,char **grid);
void read_input(char* filename,int*rows,int*cols,char **grid);