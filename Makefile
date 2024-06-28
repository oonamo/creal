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
	@ clang -O1 -g -fsanitize=address -fno-omit-frame-pointer ./src/creal.c -o ./bin/creal.exe
	@ ./bin/creal $(test_file)
run_tests: build
	@ echo "<== Running Tests"
	@ echo $(test_file)
	@ ./bin/creal $(test_file)
	@ gcc ./tests/functions/remove_comment.c -o ./tests/bin/remove_comment
	@ ./tests/bin/remove_comment
	@ gcc ./tests/functions/parse_flag.c -o ./tests/bin/parse_flag
	@ ./tests/bin/parse_flag
test_release: release
	@ echo "<== Running Tests for release"
	@ echo ./creal $(test_file)
	@ ./bin/creal $(test_file)
install: release
	@ $(install_cmd)
gdb: debug
	@ gdb --args ./bin/creal.exe $(test_file)
run: build
	@ echo "<== Running"
	@ ./bin/creal $(test_file)
