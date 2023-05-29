PRESENT_16: PRESENT_16.c
	gcc -o PRESENT_16 PRESENT_16.c
make_dataset: PRESENT_16.c
	gcc -shared -o PRESENT_16.so PRESENT_16.c 