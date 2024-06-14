build:
	gcc ./creal.c -o creal
debug:
	gcc -g -fno-inline -fno-omit-frame-pointer ./creal.c -o creal
clang_debug:
	clang ./creal.c -g -fsanitize=address -o creal.exe
