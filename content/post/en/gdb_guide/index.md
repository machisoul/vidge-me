---
author: "Vidge Wong"
title: "GDB - Not an exhaustive guide"
date: "2022-01-14"
description: "A comprehensive guide to GDB (GNU Debugger) with essential commands and debugging techniques"
tags: ["shell", "c/cpp"]
categories: ["Tools"]
image: img/cover/neofetch.jpg
ShowToc: true
TocOpen: true
---

## Introduction

GDB (GNU Debugger) is a powerful debugging tool for programs written in C, C++, and other languages. This guide covers essential GDB commands and techniques for effective debugging.

## Compilation for Debugging

To use GDB, compile your program with debugging information:

```bash
# Add -g flag for debugging information
g++ -g program.cpp -o program

# Optional: disable optimization for better debugging
g++ -g -O0 program.cpp -o program
```

## Basic GDB Commands

### Starting GDB

```bash
# Start GDB with a program
gdb program

# Start with core dump
gdb program core

# Attach to running process
gdb -p <PID>
```

### Running and Stopping

```gdb
# Start program execution
(gdb) run [args]
(gdb) r [args]

# Continue execution
(gdb) continue
(gdb) c

# Execute next line (step over)
(gdb) next
(gdb) n

# Step into function
(gdb) step
(gdb) s

# Step out of current function
(gdb) finish

# Execute until line number/function
(gdb) until <line_num>
(gdb) until <function>
```

### Breakpoints

```gdb
# Set breakpoint at function
(gdb) break function_name
(gdb) b function_name

# Set breakpoint at line
(gdb) break file:line
(gdb) b 45

# Set conditional breakpoint
(gdb) break file:line if condition
(gdb) b 45 if i == 100

# List breakpoints
(gdb) info breakpoints
(gdb) i b

# Delete breakpoint
(gdb) delete <breakpoint_num>
(gdb) d 1

# Enable/disable breakpoint
(gdb) enable <breakpoint_num>
(gdb) disable <breakpoint_num>
```

### Examining State

```gdb
# Print variable value
(gdb) print variable
(gdb) p variable

# Print array elements
(gdb) p array[0]@length
(gdb) p *array@length

# Print in different format
(gdb) p/x variable  # hex
(gdb) p/t variable  # binary
(gdb) p/c variable  # character

# Display variable (auto-update)
(gdb) display variable
(gdb) undisplay <display_num>

# Examine memory
(gdb) x/nfu addr
# n: number of units
# f: format (x,d,u,o,t,a,c,s,i)
# u: unit size (b,h,w,g)
```

## Advanced Features

### Watchpoints

```gdb
# Watch variable for changes
(gdb) watch variable
(gdb) watch *pointer

# Watch expression
(gdb) watch expression

# Watch for reads
(gdb) rwatch variable

# Watch for reads/writes
(gdb) awatch variable
```

### Backtrace and Frame Navigation

```gdb
# Show call stack
(gdb) backtrace
(gdb) bt

# Show detailed frame info
(gdb) info frame
(gdb) i f

# Select frame
(gdb) frame <number>
(gdb) f <number>

# Move up/down stack
(gdb) up
(gdb) down
```

### Thread Debugging

```gdb
# List threads
(gdb) info threads

# Switch thread
(gdb) thread <thread_num>

# Apply command to all threads
(gdb) thread apply all bt

# Set break on all threads
(gdb) break file:line thread all
```

## Practical Examples

### Example 1: Debugging Segmentation Fault

```cpp
// buggy.cpp
int main() {
    int* ptr = nullptr;
    *ptr = 42;  // Segmentation fault
    return 0;
}
```

Debugging session:
```gdb
$ g++ -g buggy.cpp -o buggy
$ gdb buggy
(gdb) run
Program received signal SIGSEGV
(gdb) bt
#0 main () at buggy.cpp:3
(gdb) print ptr
$1 = 0x0
```

### Example 2: Conditional Breakpoint

```cpp
// loop.cpp
int main() {
    for(int i = 0; i < 1000; i++) {
        if(i * i > 5000) {
            // Break here when condition met
        }
    }
    return 0;
}
```

Debugging session:
```gdb
(gdb) break 4 if i * i > 5000
(gdb) run
Breakpoint 1, main () at loop.cpp:4
(gdb) print i
$1 = 71
```

## GDB TUI Mode

GDB's Text User Interface provides a more visual debugging experience:

```gdb
# Start GDB in TUI mode
gdb -tui program

# Toggle TUI mode
(gdb) tui enable
(gdb) tui disable

# Switch layouts
(gdb) layout src      # source only
(gdb) layout regs     # registers and source
(gdb) layout asm      # disassembly
```

## Best Practices

1. **Debugging Strategy**:
   - Start with a hypothesis
   - Use appropriate breakpoints
   - Check variable values systematically
   - Keep track of program flow

2. **Performance**:
   - Use conditional breakpoints judiciously
   - Remove unnecessary breakpoints
   - Consider watchpoints impact

3. **Core Dumps**:
   - Enable core dumps: `ulimit -c unlimited`
   - Analyze crashes post-mortem
   - Save debugging sessions for reference

## Common GDB Commands Cheat Sheet

| Command | Description | Example |
|---------|-------------|---------|
| `run` | Start program | `run arg1 arg2` |
| `break` | Set breakpoint | `break main` |
| `print` | Print value | `print variable` |
| `next` | Step over | `next` |
| `step` | Step into | `step` |
| `continue` | Continue execution | `continue` |
| `backtrace` | Show call stack | `bt` |
| `info` | Show information | `info breakpoints` |
| `watch` | Set watchpoint | `watch variable` |

## Conclusion

GDB is an essential tool for C/C++ developers. While it has a steep learning curve, mastering these basic commands and techniques will significantly improve your debugging efficiency. Remember to compile with debugging symbols and use appropriate commands based on your debugging needs. 