output: myMagicSquare.c
	gcc myMagicSquare.c -Wall -m32 -std=gnu99 -o myMagicSquare
	gcc myMagicSquare.c -g -Wall -m32 -std=gnu99 -o debugMagicSquare
clean: 
	rm -f myMagicSquare

cleantest:
	rm -f *.out

testleaks:
	valgrind -s \
		--leak-check=full \
		--error-exitcode=1 \
		--show-leak-kinds=all \
		--track-origins=yes \
		./myMagicSquare leaks.out \
		< test3.in
test:
	./myMagicSquare test.out < test1.in
	./myMagicSquare test9.out < test9.in
