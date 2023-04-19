all: cache1D.c cache2Drows.c cache2Dcols.c cache2Dclash.c
	gcc cache1D.c -Wall -m32 -std=gnu99 -o cache1D
	gcc cache2Drows.c -Wall -m32 -std=gnu99 -o cache2Drows
	gcc cache2Dcols.c -Wall -m32 -std=gnu99 -o cache2Dcols
	gcc cache2Dclash.c -Wall -m32 -std=gnu99 -o cache2Dclash

test1:
	/s/pin-3.16/pin -injection child -t /s/pin-3.16/source/tools/Memory/obj-ia32/dcache.so -c 2 -a 1 -b 4 -o test1 -- cache1D

test2:
	/s/pin-3.16/pin -injection child -t /s/pin-3.16/source/tools/Memory/obj-ia32/dcache.so -c 2 -a 1 -b 32 -o test2 -- cache1D

test3:
	/s/pin-3.16/pin -injection child -t /s/pin-3.16/source/tools/Memory/obj-ia32/dcache.so -c 2 -a 1 -b 64 -o test3 -- cache1D

test6:
	/s/pin-3.16/pin -injection child -t /s/pin-3.16/source/tools/Memory/obj-ia32/dcache.so -c 2 -a 1 -b 64 -o test6 -- cache2Drows

test7:
	/s/pin-3.16/pin -injection child -t /s/pin-3.16/source/tools/Memory/obj-ia32/dcache.so -c 2 -a 1 -b 64 -o test7 -- cache2Dcols

test10:
	/s/pin-3.16/pin -injection child -t /s/pin-3.16/source/tools/Memory/obj-ia32/dcache.so -c 1 -a 1 -b 32 -o test10 -- cache2Dclash

test11:
	/s/pin-3.16/pin -injection child -t /s/pin-3.16/source/tools/Memory/obj-ia32/dcache.so -c 1 -a 2 -b 32 -o test11 -- cache2Dclash

cleantests:
	rm -f test*

