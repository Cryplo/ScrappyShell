# Run Everything in WSL!!!

cr: c r

c:
# cd "/mnt/c/Users/dylan/OneDrive/Documents/ScrappyShell"
	g++ main.cpp lexer.cpp parser.cpp builtins.cpp -o shell.exe

r:
	./shell.exe