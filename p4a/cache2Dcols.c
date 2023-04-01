int array2[3000][500];

int main(){
        for (int j = 0; j < 500; j++){
                for (int i = 0; i < 500; i++){
                        array2[i][j] = i + j;
                }
        }
	return 0;
}

