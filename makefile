smallsh:
	gcc -o smallsh main.c -g foregroundProcesses.c signals.c backgroundProcesses.c -std=c99

clean:
	rm -f *.o smallsh.exe smallsh