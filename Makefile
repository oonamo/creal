build:
	echo "<== GCC Build"
	gcc ./creal.c -o creal
debug:
	echo "<== GCC Debug"
	gcc -g -fno-inline -fno-omit-frame-pointer ./creal.c -o creal
clang_debug:
	echo "<== Clang Debug"
	clang ./creal.c -g -fsanitize=address -o creal.exe
run_tests: build
	echo "<== Running Tests"
	./creal ./tests/testmocks/test_onerunner.creal
