The IPC program that doesn't do IPC!

Usage: ./master -n x -s x -t x infile
-n - The total number of processes
-s - The total number of concurrent processes
-t - The amount of time the program has before force quitting
infile - The text file containing the strings to test for palindromes

This was a very difficult project where I feel like I figured a lot of it out considering I hadn't done 2750 in three years. I had to relearn a lot of it.

That and I mixed up my due dates and thought this was dues next week and a project due in another class was due on the 8th. 

This program can:
	Read a file and check if it contains palindromes!
	It reads the options and set the parameters!
	It uses child process like a boss!

What it cannot do:
	IPC to control the critical section
	React to CTRL+C or the overall time limit
	Read results into palin.out, nopalin.out, or log.out

Considering I only had 2 and a half days to work on it (yes I did start the day it was due), I think I figured out quite a bit of it. 
