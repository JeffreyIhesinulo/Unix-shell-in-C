# mush — a simple Unix shell in C

A minimal command-line shell written in C, built to understand how shells work under the hood: process creation, `fork`/`exec`, built-in commands, and basic parsing.

## Features

- Runs any external command available on your system (`ls`, `grep`, `cat`, etc.) via `PATH` lookup
- Built-in commands:
  - `cd [directory]` — change directory (defaults to `$HOME` if no argument given)
  - `pwd` — print current working directory
  - `exit` — quit the shell
  - `help` — list available commands
- Handles empty input and whitespace-only input gracefully
- Reports errors for unknown commands and failed system calls

## Building

Requires `gcc` and a POSIX-compliant system (Linux/macOS).

```bash
make
```

Or manually:

```bash
gcc -Wall -Wextra -o mush shell.c
```

## Running

```bash
./mush
```

Example session:

```
mush> pwd
/home/jeffrey/projects/mush
mush> ls
mush  shell.c  Makefile  README.md
mush> cd ..
mush> pwd
/home/jeffrey/projects
mush> exit
Bye!
```
## How it works

- Input is read with `fgets` and split into arguments with `strtok`
- Built-in commands (`cd`, `pwd`, `exit`, `help`) are handled directly in the shell process
- All other commands are run by forking a child process and calling `execvp`, which searches `PATH` for the matching executable
- The parent process waits for the child to finish before showing the next prompt

## Known limitations

- No support for pipes (`|`) or redirection (`>`, `<`)
- No background processes (`&`)
- No command history
- Max line length: 1024 characters, max 63 arguments per command

## Possible future improvements

- Pipe support
- Background job execution
- Command history (arrow key navigation)