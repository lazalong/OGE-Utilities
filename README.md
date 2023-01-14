# OGE Utilities

This is part of Open Game Engine (OGE) which is a working-in-progress game engine. 
This project illustrate the logger, memory allocator, leak detecor and a javascript memory alloction visualiser.

Happy to receive any suggestion, fix or improvement.


![oge.cpp icon](HeapLogVisualier.png.png)

## Features

 - [x] Basic Logger
 - [x] Load json log
 - [ ] Load compact text file
 - [ ] Load compact binary file
 - [x] Basic Memory Allocator & Leak Detector
 - [x] Javascript memory allocation visualiser. See the VisualCode project.

## TODO

 - In Memory.h change the casting from void * to unsigned long to avoid the warning
 - Only able to visualise json log files. Text and binary log are TODOs
 - Several other TODOs in the code. Especially in the Logger
 - The memory allocator will be rewritten in OGE

## Installation

 - OGE.lib : Open the Visual Studio solution to compile the static lib
 - Heap log visualier: Open the Visual Code solution to see the code. But you only need to open the file index.html and load a .json log file

