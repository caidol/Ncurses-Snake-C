# Ncurses-Snake-C

## What is it?
A simple snake game written in C primarily using the Ncurses library and a few standard libraries.

Ncurses is a C library which helps to create a wrapper over terminal capabilities, allowing you to influence how data is presented and create a nice UI / terminal graphics representation

## What does the game look like?

Here is an image below showing what the game will look like when played on the terminal:

![Alt text](https://github.com/caidol/Ncurses-Snake-C/blob/main/game_image.png)


## Pre-installation requirements:
C compiler (e.g gnu)

Unix-based OS (Ncurses provides capability for unix based terminals)

## Installation:

Clone the repository from Github

```bash
caidol@fedora ~ sudo git clone "https://github.com/caidol/Ncurses-Snake-C"
``` 

Change directory into repo folder location

```bash
caidol@fedora ~ cd Ncurses-Snake-C
``` 

Compile the C code, ensuring that the -lncurses linker flag is appended to the usual compilation command

```bash
caidol@fedora ~ gcc -o snake snake.c -lncurses # Make sure that you add the linker flag for ncurses
```

Then you will be able to run the executable and play the game with the wasd keys 

```bash
caidol@fedora ~ ./snake
```
