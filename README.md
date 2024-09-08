Here’s a sample README for the assignment:

---

# smallsh - A Custom Shell Implementation

## Overview

A simple shell program in C, akin to the Bash shell, but with a reduced feature set. The goal was to develop a functional shell named `smallsh` that executes command line instructions, handles input/output redirection, and manages foreground and background processes.

## Features

- **Command Execution**: `smallsh` executes command line instructions and returns results.
- **Redirection**: Supports redirection of standard input and standard output.
- **Foreground and Background Processes**: Capable of handling both foreground and background processes.
- **Built-in Commands**:
  - `exit`: Terminates the shell session.
  - `cd`: Changes the current working directory.
  - `status`: Displays the exit status of the last executed command.
- **Comments**: Lines beginning with the `#` character are treated as comments and ignored.

## Getting Started

To get started with `smallsh`, follow these steps:

1. **Clone the Repository**: Clone this repository to your local machine.
   ```bash
   git clone https://github.com/yourusername/smallsh.git
   ```
2. **Navigate to the Project Directory**:
   ```bash
   cd smallsh
   ```
3. **Compile the Code**: Use `gcc` to compile the source code.
   ```bash
   gcc -o smallsh smallsh.c
   ```
4. **Run the Shell**: Execute the shell using the compiled binary.
   ```bash
   ./smallsh
   ```

## Development Notes

- **Development Environment**: Please perform all development work on the designated class server to avoid potential issues on other systems.
- **Handling Issues**: If you experience problems with runaway processes affecting your access, use the provided page to terminate unwanted processes.

## Example Usage

```bash
smallsh$ ls -l
smallsh$ cd /path/to/directory
smallsh$ ./myprogram > output.txt
smallsh$ ./myprogram &
smallsh$ exit
```
