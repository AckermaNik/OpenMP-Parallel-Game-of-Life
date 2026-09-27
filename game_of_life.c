#ifndef DECLARATIONS_H  // Checstep if DECLARATIONS_H is not defined
#define DECLARATIONS_H  // Define DECLARATIONS_H

#include "declarations.h"


void read_input(char* filename,int*rows,int*cols,char **grid){
    char c;
    int i,j;


    FILE *input = fopen(filename, "r");
    if (!input) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fscanf(input,"%d %d",rows,cols);
    (*grid) = (char *)malloc((*rows) * (*cols) * sizeof(char));

    if(!(*grid)){
        perror("Couldn't allocate memory for grid");
        exit(EXIT_FAILURE);
    }

    for ( i = 0; i < *rows; i++) {
        for ( j = 0; j < *cols; j++) {
            c = fgetc(input);
            while(c=='\n' || c=='|' || c=='\r'){
                c=fgetc(input);
            }
            (*grid)[i*(*cols)+j]=c;
        }
    }
    fclose(input);
}

void write_output(char* filename,int*rows,int*cols,char **grid){
    int i,j;
    char c;

    FILE *output = fopen(filename, "w");
    if (!output) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fprintf(output, "%d %d\n", *rows, *cols);

    for ( i = 0; i < *rows; i++) {
        fputc('|',output);
        for ( j = 0; j < *cols; j++) {           
            fputc((*grid)[i * (*cols) + j], output);
            fputc('|',output);
        }
        fputc('\n',output);
    }
    fclose(output);
}

int count_live_neighbors(char **grid, int rows, int cols, int x, int y) {
    int count=0,i=0,j=0,step=1,down=0,up=0;

    // for(j=0;j<cols;j++){
    //     if((x * cols) + j!= (x * cols) + y && (*grid)[(x * cols) + j]=='*'){
    //         count++;
    //     }
    // }


    // for(i=0;i<rows;i++){
    //     if((i * cols) + y!=(x * cols) + y && (*grid)[(i * cols) + y]=='*'){
    //         count++;
    //     }
    // }

    //for(step=1;step<=cols-1;step++){

        if(x+step<rows){
            if((*grid)[(x+step)* cols + y ]=='*') count++;
        }

        if(x-step>=0){
            if((*grid)[(x-step)* cols + y ]=='*') count++;
        }

        if(y+step<cols){
            if((*grid)[x*cols + (y+step) ]=='*') count++;
        }

        if(y-step>=0){
            if((*grid)[x*cols + (y-step) ]=='*') count++;
        }

        /*down and left*/
        if(x+step<rows && y-step>=0){
            if((*grid)[(x+step)* cols + (y-step) ]=='*') count++;
        }

        /*up and left*/
        if(x-step>=0 && y-step>=0){
            if((*grid)[(x-step)* cols + (y-step) ]=='*') count++;
        }

        /*down and right*/
        if(y+step<cols && x+step<rows){
            if((*grid)[(x+step)* cols + (y+step) ]=='*') count++;
        }

        /*up and right*/
        if(x-step>=0 && y+step<cols){
            if((*grid)[(x-step)* cols + (y+step) ]=='*') count++;
        }
    //}

    // if(count>0) printf("count:%d\n", count);
    
    return count;

}


void game_of_life(char **grid, int rows, int cols, int generations) {

    int gen,alive_neighbors=0,i,j;
    char*new_grid=(char *)malloc((rows) * (cols) * sizeof(char));
    char *temp;


    for (gen = 0; gen < generations; gen++) {

            #pragma omp parallel for collapse(2) default (none) \
            shared (grid,new_grid,rows,cols) private(i,j,alive_neighbors) /*it unifies the two for loops to 1 and runs all iterations in parallel not just the outer loop*/
            
            for ( i = 0; i < rows; i++) {
                for ( j = 0; j < cols; j++) {
                    alive_neighbors = count_live_neighbors(grid, rows, cols, i, j);
                    if ((*grid)[i * cols + j] == '*') {
                        if (alive_neighbors < 2 || alive_neighbors > 3)
                            new_grid[i * cols + j] = ' ';
                        else
                            new_grid[i * cols + j] = '*';
                    } else {
                            if (alive_neighbors == 3)
                                new_grid[i * cols + j] = '*';
                            else
                                new_grid[i * cols + j] = ' ';
                    }
                    
                }   
     
        } 

        //memcpy(*grid, new_grid, rows * cols * sizeof(char));
        temp = *grid;
        *grid = new_grid;
        new_grid = temp;
        // printf("////////gen: %d",gen);
    }
}

int main(int argc, char* argv[]){

    int gens,rows=0,collumns=0;
    char *grid=NULL,*input;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file> <generations> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    gens=atoi(argv[2]);
    input=argv[1];
    read_input(input,&rows,&collumns,&grid);
    //printf("count: %d\n",count_live_neighbors(&grid,rows,collumns,0,6));
    game_of_life(&grid,rows,collumns,gens);
    write_output(argv[3],&rows,&collumns,&grid);

    return EXIT_SUCCESS;
}

#endif