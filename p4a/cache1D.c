#define ARRAY_SIZE 100000

int array[ARRAY_SIZE];

int  main(){
	for (int i = 0; i < ARRAY_SIZE; i ++){
		array[i] = i;
	}
	return 0;
}
