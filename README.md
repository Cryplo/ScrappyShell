## What This Is

A simple bash-like shell intended as a learning project

## What I've Accomplished

The shell receives a command-line input and lexes the input into tokens. Then, it converts tokens into three categories: commands (such as cd or ls), operators (such as | or <), and generic input (such as file names). Then, the shell parses the input following the same precendence as Bash and executes the input.

## What I've learned

What I've learned from ScrappyShell so far

- Basic C++ Project Stuff
- Refreshed overall C++ knowledge
- How shell interacts with OS
- UNIX commands
- Setting up MakeFiles
- Setting up WSL
- Language inferencing, i.e lexing and parsing
- Forking processes, and process IDs, etc. (remember, in child process fork returns 0, in parent process fork returns child ID)
- File I/O, i.e buffers
- Multithreading
- File Descriptors
- Recursive Descent Parsing

Thanks to MIT's xv6 shell implementation for inspiration