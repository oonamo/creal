ifeq ($(OS),Windows_NT)
	test_file := "./tests/testmocks/tests.creal"
else
	test_file := "./tests/testmocks/unix_tests.creal"
endif
build:
	@ echo "<== GCC Build"
	@ gcc ./creal.c -o creal
debug:
	@ echo "<== GCC Debug"
	@ gcc -g -fno-inline -fno-omit-frame-pointer ./creal.c -o creal
clang_debug:
	@ echo "<== Clang Debug"
	@ clang ./creal.c -g -fsanitize=address -o creal.exe
run_tests: build
	@ echo "<== Running Tests"
	@ ./creal $(test_file)
install: build
	@ install.bat
