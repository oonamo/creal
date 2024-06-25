ifeq ($(OS),Windows_NT)
	test_file := "./tests/testmocks/tests.creal"
	install_cmd := install.bat
else
	test_file := "./tests/testmocks/unix_tests.creal"
	install_cmd := install.sh
endif

build:
	@ echo "<== GCC Build"
	@ gcc ./src/creal.c -o ./bin/creal
release:
	@ echo "<== GCC Build Release"
	@ gcc ./src/creal.c -o creal -O3
debug:
	@ echo "<== GCC Debug"
	@ gcc -g -fno-inline -fno-omit-frame-pointer ./src/creal.c -o ./bin/creal
clang_debug:
	@ echo "<== Clang Debug"
	@ clang -O1 -g -fsanitize=address -fno-omit-frame-pointer ./src/creal.c -o ./bin/creal
	@ ./bin/creal $(test_file)
run_tests: build
	@ echo "<== Running Tests"
	@ echo $(test_file)
	@ ./bin/creal $(test_file)
test_release: release
	@ echo "<== Running Tests for release"
	@ echo ./creal $(test_file)
	@ ./bin/creal $(test_file)
install: release
	@ $(install_cmd)
gdb: debug
	@ gdb --args ./bin/creal.exe ./tests/mocks/variables.creal
run: build
	@ echo "<== Running"
	@ ./bin/creal $(test_file)
