ifeq ($(OS),Windows_NT)
	test_file := "./tests/testmocks/tests.creal"
	install_cmd := install.bat
else
	test_file := "./tests/testmocks/unix_tests.creal"
	install_cmd := install.sh
endif

build:
	@ echo "<== GCC Build"
	@ gcc ./creal.c -o creal
release:
	@ echo "<== GCC Build Release"
	@ gcc ./creal.c -o creal -O3
debug:
	@ echo "<== GCC Debug"
	@ gcc -g -fno-inline -fno-omit-frame-pointer -Og ./creal.c -o creal
clang_debug:
	@ echo "<== Clang Debug"
	@ clang ./creal.c -g -fsanitize=address -o creal.exe
run_tests: build
	@ echo "<== Running Tests"
	@ echo $(test_file)
	@ ./creal $(test_file)
test_release: release
	@ echo "<== Running Tests for release"
	@ echo ./creal $(test_file)
	@ ./creal $(test_file)
install: release
	@ $(install_cmd)
