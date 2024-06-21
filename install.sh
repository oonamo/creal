#!usr/bin/bash

make release
file = "./creal"

add_to_bin() {
  mv file ~/bin
}
