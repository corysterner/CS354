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
// Main File:        csim.c
// This File:        csim.c
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

////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013,2019-2020
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission for Spring 2023
//
////////////////////////////////////////////////////////////////////////////////

/*
 * csim.c:  
 * A cache simulator that can replay traces (from Valgrind) and output
 * statistics for the number of hits, misses, and evictions.
 * The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 */  

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>


/******************************************************************************/
/* DO NOT MODIFY THESE VARIABLES **********************************************/

//Globals set by command line args.
int b = 0; //number of block (b) bits
int s = 0; //number of set (s) bits
int E = 0; //number of lines per set

//Globals derived from command line args.
int B; //block size in bytes: B = 2^b
int S; //number of sets: S = 2^s

//Global counters to track cache statistics in access_data().
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;

//Global to control trace output
int verbosity = 1; //print trace if set
/******************************************************************************/
  
  
//Type mem_addr_t: Use when dealing with addresses or address masks.
typedef unsigned long long int mem_addr_t;

//

//Type cache_line_t: Use when dealing with cache lines.
typedef struct cache_line {                    
    char valid;
    mem_addr_t tag;
    int next;
    int prev;
} cache_line_t;

//Initialize head and tail nodes
cache_line_t *head = NULL;
cache_line_t *tail = NULL;

//Type cache_set_t: Use when dealing with cache sets
//Note: Each set is a pointer to a heap array of one or more cache lines.
typedef cache_line_t* cache_set_t;

//Type cache_t: Use when dealing with the cache.
//Note: A cache is a pointer to a heap array of one or more sets.
typedef cache_set_t* cache_t;

// Create the cache we're simulating. 
//Note: A cache is a pointer to a heap array of one or more cache sets.
cache_t cache;  

mem_addr_t t_mask;
mem_addr_t s_mask;
mem_addr_t b_mask;
unsigned int t_size; 

void set_t_size(){
	t_size = 64 - b - s;
}

void create_t_mask(){
	t_mask = (1 << t_size) - 1;
}

void create_s_mask(){
	s_mask = ((1ull << (64 - t_size - b)) - 1) << b;
}

void create_b_mask(){
	b_mask = (1ull << (64 - b)) - 1;
}
/* 
 * init_cache:
 * Allocates the data structure for a cache with S sets and E lines per set.
 * Initializes all valid bits and tags with 0s.
 */                    
void init_cache() {        
    S = 1 << s;	
    set_t_size();
    create_t_mask();
    create_s_mask();
    create_b_mask();

    // Allocate memory for the cache data structure
    cache = malloc(sizeof(cache_set_t) * S);
    if (cache == NULL){
	free(cache);
	exit(1);
    }
    for (int set = 0; set < S; set++) {
        cache[set] = malloc(sizeof(cache_line_t) * E);
	if (cache[set] == NULL){
	    free(cache);
    	    exit(1);	    
	}
        for (int line = 0; line < E; line++) {
            // Initialize all valid bits and tags with 0s
            cache[set][line].valid = 0;
            cache[set][line].tag = 0;
	    cache[set][line].next = (line + 1) % E;
	    if (line == 0){
	        cache[set][line].prev = E - 1;
	    }
	    else{
	    	cache[set][line].prev = line - 1;
	    }
        }
    }
}
  

 /* free_cache:
 * Frees all heap allocated memory used by the cache.
 */                    
void free_cache() {             
    for (int set = 0; set < S; set++) {
        free(cache[set]);
    }
    free(cache);
}

mem_addr_t get_t_bit(mem_addr_t addr){
	return (addr & t_mask) >> (64 - t_size);
}

mem_addr_t get_s_bit(mem_addr_t addr){
	return ((addr & s_mask) >> b);
}

mem_addr_t get_b_bit(mem_addr_t addr){
	return (addr & b_mask);
}

typedef struct Header{
    int head;
    int tail;
	
}header;

header *headers;
void init_header(){
	headers = malloc(sizeof(header) * S);
	if (headers == NULL){
		free_cache();
		free(headers);
		exit(1);
	}
	for (int set = 0; set < S; set++){
		headers[set].head = 0;
		headers[set].tail = E-1;
	}
}


/* TODO - COMPLETE THIS FUNCTION 
 * access_data:
 * Simulates data access at given "addr" memory address in the cache.
 *
 * If already in cache, increment hit_cnt
 * If not in cache, cache it (set tag), increment miss_cnt
 * If a line is evicted, increment evict_cnt
 */                    
void access_data(mem_addr_t addr) {
	mem_addr_t set = get_s_bit(addr);
	mem_addr_t tag = get_t_bit(addr);

	header *set_header = &headers[set];
	cache_line_t *line = &cache[set][set_header->head];

	int curr_line = set_header->head;
	int allocated = 0;
	//Loop over all lines and look for matching value
	while (curr_line != (set_header->tail)){
		
		//Use first non-initialized value
		if (line->valid == 0){
			line->tag = get_t_bit(addr);
			line->valid = 1;
			miss_cnt++;
			if (verbosity){
				printf("%s","miss ");
			}
			allocated = 1;
			break;
		}
		
		//Move node to front if already present
		if (line->tag == tag){
			if (set_header->tail == curr_line){
				set_header->head = curr_line;
				set_header->tail = line->prev;
			}
			else if (set_header->head == curr_line){

			}
			else{
				//Move hit line to front of list
				//Remove the node from the list
				cache[set][line->next].prev = line->prev;
				cache[set][line->prev].next = line->next;

				//set the node in its new place
				line->prev = set_header->tail;
				line->next = set_header->head;
				
				//set loop for current tail and header
				cache[set][line->next].prev = curr_line;
				cache[set][line->prev].next = curr_line;

				//set the new header
				set_header->head = curr_line;
			}
			allocated = 1;
			hit_cnt++;
			if (verbosity){
				printf("%s", "hit ");
			}
			break;
		}
		
		curr_line = line->next;
		//Get next line
		line = &cache[set][line->next];
	}

	//Remove a declared line in favor of the new tag
	if (allocated == 0){

		//Can only happen if it is the only one in the list
		if ((line->tag) == tag && (line->valid) == 1){
			hit_cnt++;
			if (verbosity){
				printf("%s", "hit ");
			}
		}
		else{
			set_header->tail = cache[set][set_header->tail].prev;
			set_header->head = cache[set][set_header->head].prev;
			cache[set][set_header->head].tag = tag;
			miss_cnt++;
			if (verbosity){
				printf("%s", "miss ");
			}
			if (cache[set][set_header->head].valid == 1){
				evict_cnt++;
				if (verbosity){
					printf("%s", "eviction ");
				}		
			}
			cache[set][set_header->head].valid = 1;

		}
	}
		
}



/* TODO - FILL IN THE MISSING CODE
 * replay_trace:
 * Replays the given trace file against the cache.
 *
 * Reads the input trace file line by line.
 * Extracts the type of each memory access : L/S/M
 * TRANSLATE each "L" as a load i.e. 1 memory access
 * TRANSLATE each "S" as a store i.e. 1 memory access
 * TRANSLATE each "M" as a load followed by a store i.e. 2 memory accesses 
 */                    
void replay_trace(char* trace_fn) {           
    char buf[1000];  
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r"); 

    if (!trace_fp) { 
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);   
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            // TODO - MISSING CODE
            // GIVEN: 1. addr has the address to be accessed
            //        2. buf[1] has type of acccess(S/L/M)
            // call access_data function here depending on type of access
	    init_header();
	    access_data(addr);
	    if (buf[1] == 'M'){
	        access_data(addr);
	    }
            if (verbosity)
                printf("\n");
        }
    }
    free(headers);

    fclose(trace_fp);
}  
  
  
/*
 * print_usage:
 * Print information on how to use csim to standard output.
 */                    
void print_usage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of s bits for set index.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of b bits for block offsets.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}  
  
  
/*
 * print_summary:
 * Prints a summary of the cache simulation statistics to a file.
 */                    
void print_summary(int hits, int misses, int evictions) {                
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}  
  
  
/*
 * main:
 * Main parses command line args, makes the cache, replays the memory accesses
 * free the cache and print the summary statistics.  
 */                    
int main(int argc, char* argv[]) {                      
    char* trace_file = NULL;
    char c;
    
    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                print_usage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                print_usage(argv);
                exit(1);
        }
    }

    //Make sure that all required command line args were specified.
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    //Initialize cache.
    init_cache();

    //Replay the memory access trace.
    replay_trace(trace_file);

    //Free memory allocated for cache.
    free_cache();

    //Print the statistics to a file.
    //DO NOT REMOVE: This function must be called for test_csim to work.
    print_summary(hit_cnt, miss_cnt, evict_cnt);
    return 0;   
}  

// Spring 202301
