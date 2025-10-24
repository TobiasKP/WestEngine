# GDB Debugging Cheat Sheet

## Starting GDB

```bash
# Basic start
gdb ./program_name

# Start with core dump
gdb ./program_name core

# Attach to running process
gdb -p <pid>
```

## Essential Commands

### Running Programs
```gdb
run                    # Start program
run arg1 arg2         # Start with arguments
continue (c)          # Continue execution
step (s)              # Step into functions
next (n)              # Step over functions
finish                # Run until current function returns
kill                  # Kill running program
quit (q)              # Exit GDB
```

### Breakpoints
```gdb
break main            # Break at function
break file.cpp:123    # Break at line number
break ClassName::method # Break at method
break *0x401234       # Break at address
info breakpoints      # List all breakpoints
delete 1              # Delete breakpoint 1
disable 1             # Disable breakpoint 1
enable 1              # Enable breakpoint 1
clear                 # Delete all breakpoints
```

### Examining Code
```gdb
list                  # Show source around current line
list 50               # Show source around line 50
list function_name    # Show source of function
disassemble          # Show assembly of current function
info functions       # List all functions
info variables       # List all variables
```

### Stack and Frames
```gdb
backtrace (bt)        # Show call stack
bt full               # Show call stack with local variables
frame 3               # Switch to frame 3
up                    # Move up one frame
down                  # Move down one frame
info frame            # Show current frame info
info args             # Show function arguments
info locals           # Show local variables
```

### Examining Variables and Memory
```gdb
print variable        # Print variable value
print *pointer        # Dereference pointer
print array[5]        # Print array element
print sizeof(var)     # Print size of variable
x/10x address         # Examine 10 hex words at address
x/s pointer           # Examine string at pointer
x/10i $pc             # Examine 10 instructions at program counter
info registers        # Show all registers
print $rax            # Print specific register
```

## Memory Debugging Techniques

### Environment Variables (set before running GDB)
```bash
export MALLOC_CHECK_=2      # Abort on heap corruption
export MALLOC_PERTURB_=42   # Fill freed memory with pattern
export GLIBC_TUNABLES=glibc.malloc.check=2
```

### GDB Memory Commands
```gdb
set environment MALLOC_CHECK_=2  # Set inside GDB
watch *0x601234               # Watch memory address
rwatch variable               # Break when variable is read
awatch variable               # Break when variable is accessed
info watchpoints              # List watchpoints
```

### Debugging Double-Free Errors
```gdb
# 1. Set heap checking
set environment MALLOC_CHECK_=2

# 2. Set breakpoints at allocation/deallocation
break malloc
break free
break delete
break new

# 3. When program crashes
bt full                       # Get full backtrace
frame N                       # Go to interesting frame
print pointer                 # Check pointer value
info symbol 0xaddress         # Find what owns memory address
```

## Advanced Debugging

### Conditional Breakpoints
```gdb
break function if variable == 5
break file.cpp:123 if pointer != 0
condition 1 x > 10            # Add condition to existing breakpoint
```

### Threading
```gdb
info threads              # List all threads
thread 3                  # Switch to thread 3
thread apply all bt       # Backtrace all threads
set scheduler-locking on  # Only current thread runs
```

### Core Dumps
```bash
# Generate core dump
ulimit -c unlimited
# Run program, let it crash, then:
gdb ./program core
```

### Useful Settings
```gdb
set print pretty on           # Pretty print structures
set print array on            # Print arrays nicely
set pagination off            # Don't pause output
set confirm off               # Don't ask for confirmation
set logging on                # Log session to gdb.txt
```

## Debugging Your Double-Free Issue

### Step-by-Step Investigation
```gdb
# 1. Start with heap checking
set environment MALLOC_CHECK_=2
set environment MALLOC_PERTURB_=42

# 2. Set strategic breakpoints
break RenderManager::renderUserInterfaces
break ComponentDataPool::~ComponentDataPool
break ComponentDataPool::deleteRange

# 3. Run and analyze
run

# 4. When stopped at renderUserInterfaces
print renderData
print renderData.size()
print renderData[0]
print &renderData[0]         # Note the address

# 5. Continue to next breakpoint
continue

# 6. When program crashes
bt full
frame 6                      # Go to RenderManager frame
list                         # Show source
info locals                  # Check local variables
print cd                     # Check the pointer being deleted
print &cd                    # Compare with address from step 4
```

### Memory Ownership Analysis
```gdb
# When examining pointers
print pointer
info symbol 0x555555abc123   # Find what section owns this memory
x/10x pointer               # Examine memory content
whatis pointer              # Show pointer type
ptype *pointer              # Show pointed-to type
```

## Common GDB Shortcuts

- `Ctrl+C` - Interrupt running program
- `Enter` - Repeat last command
- `Tab` - Auto-complete commands/symbols
- `Ctrl+L` - Clear screen
- `help command` - Get help for specific command

## Useful Aliases
```gdb
alias ni = nexti
alias si = stepi
alias bt = backtrace
alias p = print
alias l = list
```

## Tips for C++ Debugging

```gdb
# Print STL containers
print vector
print vector.size()
print vector[0]

# Call methods on objects
print object.method()
call object.debug_print()

# Set breakpoints on overloaded functions
break 'ClassName::method(int)'
break 'ClassName::method(std::string)'

# Examine vtables
print *object                # Shows vtable
info vtbl object             # Virtual function table
```

## Quick Reference for Memory Errors

| Error Type | GDB Strategy |
|------------|--------------|
| Double Free | Set breakpoints on delete/free, track pointer lifecycle |
| Memory Leak | Use `info proc mappings`, track allocations |
| Buffer Overflow | Set watchpoints, examine memory around buffers |
| Use After Free | Set breakpoints after free, watch memory access |
| Null Pointer | Check pointer values, set conditional breakpoints |

## Example Session Log
```
$ gdb ./WestCore
(gdb) set environment MALLOC_CHECK_=2
(gdb) break RenderManager::renderUserInterfaces
(gdb) run
Breakpoint 1, RenderManager::renderUserInterfaces()
(gdb) print renderData.size()
$1 = 4
(gdb) print renderData[0]
$2 = (ComponentData *) 0x555555abc123
(gdb) continue
free(): double free detected in tcache 2
(gdb) bt
#0  0x7ffff749894c in raise () from /usr/lib/libc.so.6
#6  0x5555555b2309 in RenderManager::renderUserInterfaces()
(gdb) frame 6
(gdb) list
177      for (ComponentData *cd : renderData) {
178        delete cd;  // ← PROBLEM: Deleting pool-owned memory!
179      }
```