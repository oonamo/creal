# Creal
A fast static tester, based on files.

## Examples
```
#strict
name: example
command: echo "this is an example"
stdout: "this is an example"
returncode: 0
---
#verbose
name: cat log file
command: cat ./weather.log
stdout: |
starting weater station...
could not connect to weather api, aborting.
        |
returncode: 1
---
```
