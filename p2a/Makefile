
output: check_board.c
	gcc -Wall -m32 -std=gnu99 check_board.c -o check_board

clean:
	rm -f check_board

testValid:
	./check_board valid1.txt
	./check_board valid2.txt
	./check_board valid3.txt
	./check_board valid4.txt
	./check_board valid5.txt
	./check_board valid6.txt
	./check_board valid7.txt
	./check_board valid8.txt

testInvalid:
	./check_board invalid1.txt
	./check_board invalid2.txt
	./check_board invalid3.txt
	./check_board invalid4.txt
	./check_board invalid5.txt
	./check_board invalid6.txt
	./check_board invalid7.txt

