int array3[128][8];

int main(){
	for (int count = 0; count < 100; count++){
		for (int i = 0; i < 128; i += 64){
			for (int j = 0; j < 8; j++){
				array3[i][j] = count + i + j;
			}
		}
	}
	return 0;	
}
