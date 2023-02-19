///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2023, Nawaf Alsrehin
////////////////////////////////////////////////////////////////////////////////
   
////////////////////////////////////////////////////////////////////////////////
// Main File:        magic_square.c
// This File:        magic_square.c
// Other Files:
// Semester:         CS 354 Spring 2023
// Instructor:       Nawaf Alsrehin
//
// Author:           Cory Sterner
// Email:            csterner@wisc.edu
// CS Login:         sterner
// GG#:              (your Canvas GG number)
//                   (See https://canvas.wisc.edu/groups for your GG number)
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   Fully acknowledge and credit all sources of help,
//                   including family, friencs, classmates, tutors,
//                   Peer Mentors, TAs, and Instructor.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   Avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *DELIM = ",";

// Structure that represents a magic square
typedef struct {
    int size;           // dimension of the square
    int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Prompts the user for the magic square's size, reads it,
 * checks if it's an odd number >= 3 (if not display the required
 * error message and exit), and returns the valid number.
 */
int getSize() {
    return 0;   
} 
typedef struct{
	int row;
	int col;
} Location;
void freeSquareMemory(MagicSquare *square){
	for (int i = 0; i < square->size; i++){
		free(*((square->magic_square) + i));
	}
	free(square->magic_square);
}
void allocateSquareMemory(MagicSquare *square){
	square->magic_square = (int**)malloc(sizeof(int*) * square->size);
	if ((square->magic_square) == NULL){
		free(square->magic_square);
		printf("Memory could not be allocated\n");
		exit(1);
	}
	for (int i = 0; i < square->size; i++){
		*((square->magic_square) + i)=(int*)malloc(sizeof(int*) * square->size);
		if (*((square->magic_square) + i) == NULL){
			freeSquareMemory(square);
			printf("Memory could not be allocated\n");
			exit(1);
		}
	}
}

int getValue(int** square, int row, int col){
	return *(*(square + row) + col);
}
Location getNextLoc(MagicSquare square, Location currLoc){
	//Set the column location
	Location loc = currLoc;
	if (loc.col == (square.size - 1)){
		loc.col = 0;
	}
	else{
		loc.col = loc.col + 1;
	}

	//Set the row location
	if (loc.row == 0){
		loc.row = square.size - 1;
	}
	else{
		loc.row = loc.row - 1; 
	}

	//Move down a row if there is already a value present
	if (getValue(square.magic_square, loc.row, loc.col) != 0){
		loc = currLoc;
		if (loc.row ==  square.size - 1){
			loc.row = 0;
		}
		else{
			loc.row =  loc.row +  1;
		}
	}
	return loc;
}
/* Makes a magic square of size n using the alternate 
 * Siamese magic square algorithm from assignment and 
 * returns a pointer to the completed MagicSquare struct.
 *
 * n the number of rows and columns
 */
MagicSquare *generateMagicSquare(int n) {
	static MagicSquare square;
      	square.size = n;
	allocateSquareMemory(&square);	
	
	//Set initial location
	Location loc = {0, (square.size - 1)/2};

	//Loop over locations and create square	
	for (int i = 1; i <= (square.size * square.size); i++){
		*(*(square.magic_square + loc.row) + loc.col) = i;
		loc = getNextLoc(square, loc);
	}

	return &square;    
} 

/*   
 * Opens a new file (or overwrites the existing file)
 * and writes the square in the specified format.
 *
 * magic_square the magic square to write to a file
 * filename the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *square, char *filename) {
	FILE *fp = fopen(filename, "w");
	//Make sure file correctly opens
	if (fp == NULL){
		printf("Can't open file for writing.\n");
		freeSquareMemory(square);
		exit(1);
	}

	//Print all the Magic Square to the specificed file
	for (int row = 0; row < square->size; row++){
		for (int col = 0; col < square->size; col++){
			fprintf(fp, "%d", getValue(square->magic_square, row, col));
			if (col < square->size - 1){
				fputs(DELIM, fp);
			}
		}
		if (row < square->size - 1){
			fputs("\n", fp);
		}
	}
	if (fclose(fp) != 0){
		printf("Error while closing the file.\n");
		freeSquareMemory(square);
		exit(1);
	}
}

/* TODO:
 * Generates a magic square of the user specified size and
 * output the quare to the output filename
 * 
 * Be su
 *
 * Add description of required CLAs here
 */
int main(int argc, char **argv) {
       	// Check input arguments to get output filename
	if (argc != 2) {
		printf("Usage: ./myMagicSquare <output_filename>\n");
		exit(1);
	}
	
	// Get magic square's size from user
	int size;
	printf("Enter magic square's size (odd integer >=3)\n");
	scanf("%d", &size);
	if ((size % 2) == 0){
		printf("Magic square size must be odd.\n");
		exit(1);
	}
	if (size < 3){
		printf("Magic square size must be >= 3.\n");
		exit(1);
	}	
	// 	 Generate the magic square by correctly interpreting 
	//       the algorithm(s) in the write-up or by writing or your own.  
	//       You must confirm that your program produces a 
	//       Magic Sqare as described in the linked Wikipedia page.
	MagicSquare *squarePtr = generateMagicSquare(size);

	// Output the magic square
	fileOutputMagicSquare(squarePtr, *(argv + 1));

	freeSquareMemory(squarePtr);
	return 0;
} 

// S23

