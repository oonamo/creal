<div align="center">
    <h1>Creal</h1>
    <p>Testing things that never change</p>
    <img src="https://img.shields.io/github/actions/workflow/status/oonamo/creal/c.yml?branch=main&style=flat-square&logo=github" alt="CI status">
</div>

<!-- Add Demo here -->

## Features
- Cross-platform
- Fast
    - No dependencies
- No configuration needed
    - Handled through flags
- Capture shell output for easy testing.

## Support
Tested on Windows 11 and Ubuntu.

## Installation
Soon.

### Building
> [!NOTE]
> Requires gcc
```
git clone https://github.com/oonamo/creal.git
cd creal
make build
```


## Examples
```
#strict
name: example
command: echo "this is an example"
output: "this is an example"
returncode: 0
---
#verbose
name: cat log file
command: cat ./weather.log
output: |
starting weather station...
could not connect to weather api, aborting.
        |
returncode: 1
---
```
