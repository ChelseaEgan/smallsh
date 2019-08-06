smallsh:
	gcc -o smallsh main.c -g foregroundProcesses.c signals.c backgroundProcesses.c userInput.c builtInCommands.c processes.c

clean:
	rm -f *.o smallsh.exe smallsh