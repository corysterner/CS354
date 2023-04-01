int array2[3000][500];

int main(){
	for (int i = 0; i < 3000; i++){
		for (int j = 0; j < 500; j++){
			array2[i][j] = i + j;
		}
	}
	return 0;
}
