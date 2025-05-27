cr: c r

c:
	cd "c:\Users\dylan\OneDrive\Documents\ScrappyShell\"
	g++ main.cpp lexer.cpp parser.cpp builtins.cpp -o shell

r:
	.\shell