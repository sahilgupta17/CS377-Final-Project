## CS 307 Final Project

## Description
In this project, I have tried to extend the project 2 shell in C++ and add more featueres to it. Shell is a program that emulates the functionality of a command-line shell or terminal. It allows users to interact with the operating system by executing commands, managing processes, and manipulating files and directories.

## New Features Added

1. Pipe - The pipe operator allows the output from the first command to be used as input for the second command. 
2. Alias - This command is used to set aliases. For example `alias sahil='ls -l'` will set sahil as an alias to `ls -l`. So by straightaway running `sahil` would run the command `ls -l`.
3. Change Directory (cd) - By running the `cd <filepath>`, will allow the user to change directories and go into desired path.
4. History - by running the `history` command would should the full history of all the commands used by the user during the projects execution.
5. Manual - provides description and usage examples for various commands. Use `man <command>` to run it.

## Installation Requirements
Download the code and have C++ on your machine. Prefereably run on edlab as it has been tested to run on the same. 

## Compiling the project

To compile the project run the following command:
```sh
make
```

## Run the program

To run the program run the following command
```sh
./tsh_app
```
or

```sh
make && ./tsh_app
```

## Code Structure

- `include`: This is where the C/C++ header files are present. 
- `lib`: Any libraries that might have been used are present over here.
- `obj`: Object files generated from the C compilation process or running the make commmand get stored over here. This folder is initially empty until the code is compiled.
- `src`: This is the source folder where all project related code is present.
- `test`: This folder contains test files if any are present. 
- `Makefile` - this is a "build" file. This file is used to compile the code.

## Video Link
https://youtu.be/QWDvy6KztJU