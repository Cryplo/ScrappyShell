What I've learned from ScrappyShell so far

- Basic C++ Project Stuff
- Refreshed overall C++ knowledge
- How shell interacts with OS
- UNIX commands
- Setting up with MakeFiles
- Setting up WSL
- Language inferencing, i.e lexing and parsing
- Forking processes, and process IDs, etc. (remember, in child process fork returns 0, in parent process fork returns child ID)
- File I/O, i.e buffers
- Multithreading

I discovered MIT's xv6 shell implementation and am going to implement a lot of their ideas


To parse, parse the pipes, then parse the redirects, then parse commands, to build the tree