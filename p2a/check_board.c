///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Debra Deppeler & Nawaf Alsrehin
// Posting or sharing this file is prohibited, including any changes/additions.
//
// We have provided comments and structure for this program to help you get 
// started.  Later programs will not provide the same level of commenting,
// rather you will be expected to add same level of comments to your work.
// 09/20/2021 Revised to free memory allocated in get_board_size function.
// 01/24/2022 Revised to use pointers for CLAs
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        check_board.c 
// This File:        check_board.c    
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

char *DELIM = ",";  // commas ',' are a common delimiter character for data strings

/* COMPLETED (DO NOT EDIT):       
 * Read the first line of input file to get the size of that board.
 * 
 * PRE-CONDITION #1: file exists
 * PRE-CONDITION #2: first line of file contains valid non-zero integer value
 *
 * fptr: file pointer for the board's input file
 * size: a pointer to an int to store the size
 *
 * POST-CONDITION: the integer whos address is passed in as size (int *) 
 * will now have the size (number of rows and cols) of the board being checked.
 */
void get_board_size(FILE *fptr, int *size) {      
	char *line1 = NULL;
	size_t len = 0;

	if ( getline(&line1, &len, fptr) == -1 ) {
		printf("Error reading the input file.\n");
		free(line1);
		exit(1);
	}

	char *size_chars = NULL;
	size_chars = strtok(line1, DELIM);
	*size = atoi(size_chars);

	// free memory allocated for reading first link of file
	free(line1);
	line1 = NULL;
}
/* Returns 1 if the given value is in the valid range and 
 * 0 otherwise
 *
 * Pre-conditions: Size of board has been determined
 *
 * val: board value to check
 * size: size of the board
 *
 * retval: 1 - if value is in valid range
 * 	   0 - otherwise
 */
int in_range(int val, int size){
	if (val < 0) return 0;
	if (val > size) return 0;
	return 1;

}
/* Returns if the value is already in the passed in value
 * key array. 
 *
 * val: value to check for duplicates against
 * keyArr: 2D array of key pairs for values already in the row/column
 * index: Index of the row or column to check against
 *
 * retval: 1 - if value is not a duplicate
 * 	   0 - otherwise
 *
 * Post-condition: If not a duplicate then the value will be added to
 * the key array for the given index
 */ 
int duplicate_validation(int val, int** keyArr, int index){
	if (val ==  0){
		return 1;
	}else if (*(*(keyArr + index) + val)){
		return 0;
	}else{
		*(*(keyArr + index) + val) = 1;
		return 1;	
	}
}
/* Creates the 1D children of the parent pointer
 *
 * Pre-condition: Memory already allocated for parent pointer
 *
 * array: array to add children to
 * size: amount of children to create
 *
 * retval: 1 - if memory successfully allocated
 * 	   0 - otherwise
 * 
 * Post-condition: array is populated with pointers to 1D arrays
 */
int create_children(int **array, int size){
	for (int row = 0; row < size; row ++){
		*(array + row) = (int*)malloc(sizeof(int) * size);
		if (*(array + row) == NULL) {
			return 0;
		}
	}
	return 1;
}
/* Frees all children of parent pointer
 *
 * array: parent pointer
 * size: amount of children to free
 *
 * Post-condition: Dynamic memory freed for children
 */
void free_children(int **array, int size){
	for (int row = 0; row < size; row ++){
		free(*(array + row));
		*(array + row) = NULL;
	}
}
/* Helper cleanup function for valid_board in order to exit the
 * fucntion cleanly
 *
 * rowKey: row key pointer
 * colKey: column key pointer
 * size: size of the board
 *
 * Post-condition: Parent and child array dynamic memory is freed
 */
void cleanup_valid_board(int** rowKey, int** colKey, int size){
	//Free children pointers 
	free_children(rowKey, size);
	free_children(colKey, size);

	//Free parent pointer
	free(rowKey);
	free(colKey);
	rowKey = NULL;
	colKey = NULL;
}

/* 
 * Returns 1 if and only if the board is in a valid Sudoku board state.
 * Otherwise returns 0.
 * 
 * A valid row or column contains only blanks or the digits 1-size, 
 * with no duplicate digits, where size is the value 1 to 9.
 * 
 * Note: p2A requires only that each row and each column are valid.
 * 
 * board: heap allocated 2D array of integers 
 * size:  number of rows and columns in the board
 */
int valid_board(int **board, int size) {
	//Check for valid size
	if (size<1) return 0;
	
	//Create array keys to keep track of vals
	int** rowKey=(int**)malloc(sizeof(int*) * size);
	int** colKey=(int**)malloc(sizeof(int*) * size);
	if (rowKey == NULL || colKey == NULL){
		free(rowKey);
		free(colKey);
		return 0;
	}

	//Create children arrays and check for success
	if (!create_children(rowKey, size) || !create_children(colKey, size)){
		cleanup_valid_board(rowKey, colKey, size);
		return 0;
	}

	//Loop over values and validate
	int currVal;
	for (int row=0; row<size; row++){
		for (int col=0; col<size; col++){
			currVal=*(*(board + row) + col);

			//Check if value in valid range
			if (!in_range(currVal, size)){
				cleanup_valid_board(rowKey, colKey, size);
				return 0;
			}

			//Check if the value has already been found on the
			//current row
			if (!duplicate_validation(currVal, rowKey, row)){
				cleanup_valid_board(rowKey, colKey, size);
				return 0;
			}
			
			//Check if the value has already been found on the 
			//curren column	
			if (!duplicate_validation(currVal, colKey, col)){
				cleanup_valid_board(rowKey, colKey, size);
				return 0;
			}
		}
	}
	//Free dynamically allocated memory
	cleanup_valid_board(rowKey, colKey, size);

	return 1;   
}    
/*  COMPLETE THE MAIN FUNCTION
 * This program prints "valid" (without quotes) if the input file contains
 * a valid state of a Sudoku puzzle board wrt to rows and columns only.
 *
 * A single CLA is required, which is the name of the file 
 * that contains board data is required.
 *
 * argc: the number of command line args (CLAs)
 * argv: the CLA strings, includes the program name
 *
 * Returns 0 if able to correctly output valid or invalid.
 * Only exit with a non-zero result if unable to open and read the file given.
 */
int main( int argc, char **argv ) {              
	//Check if number of command-line arguments is correct.
	if (argc != 2){
		printf("Usage: ./check_board <input_filename>\n");
		exit(1);	
	}
	// Open the file and check if it opened successfully.
	FILE *fp = fopen(*(argv + 1), "r");
	if (fp == NULL) {
		printf("Can't open file for reading.\n");
		exit(1);
	}

	// Declare local variables.
	int size;

	// Call get_board_size to read first line of file as the board size.
	get_board_size(fp, &size);

	// Dynamically allocate a 2D array for given board size.
	int** board=(int**)malloc(sizeof(int*) * size);
	if (board == NULL || !create_children(board, size)){
		free_children(board, size);
		free(board);
		printf("invalid\n");
		exit(1);
	}	

	// Read the rest of the file line by line.
	// Tokenize each line wrt the delimiter character 
	// and store the values in your 2D array.
	char *line = NULL;
	size_t len = 0;
	char *token = NULL;
	for (int i = 0; i < size; i++) {

		if (getline(&line, &len, fp) == -1) {
			printf("Error while reading line %i of the file.\n", i+2);
			free_children(board, size);
			free(board);
			exit(1);
		}

		token = strtok(line, DELIM);
		for (int j = 0; j < size; j++) {
			// Complete the line of code below
			// to initialize your 2D array.
			*(*(board + i) + j) = atoi(token);
			token = strtok(NULL, DELIM);
		}
	}

	//  Call the function valid_board and print the appropriate
	//       output depending on the function's return value.
	if (valid_board(board, size)){
		printf("valid\n");
	}
	else {
		printf("invalid\n");
	}

	// Free all dynamically allocated memory.
	free_children(board, size);
	free(board);
		
	//Close the file.
	if (fclose(fp) != 0) {
		printf("Error while closing the file.\n");
		exit(1);
	} 

	return 0;       
}       

