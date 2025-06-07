# Working Time
June 2025
Will Baines

## Description
Working Time, or `work` is a cli app to easily and logically record a timesheet, and act as an interface for a timesheet database.

## Requirements
This app is targetted for c++23 - tested on windowsx64 with clang-17.

## Design
This app is split into the following modules:
* Command system
* Database handler
* Terminal User Interface
* Config/Settings handler
* Timing - use of std::chrono

The workflow simply goes from terminal input, to command generation, to command processing.

## Building
The intention is for this app to be a main file with header files, therefore reducing need to link.
As such using the compiler is all that's required.

for release, enable optimizations
```
clang++ -std=c++23 -O2 -o out/release-a/work src/main.cpp
```

for development (with debugging `-g`)
```
clang++ -std=c++23 -Wall -Wextra -Wpedantic -g -o out/dev/work src/main.cpp
```