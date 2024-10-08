# Easy-Assembler By: TFS
Assembling, linking and execution in just one script.\
Requirements:\
NASM Assembler\
GDB\
<br />
# Installation:
Download the executable from releases.
```sh
sudo cp easm /opt && echo "export PATH=$PATH:/opt" >> .bashrc && source ~/.bashrc
```
# Usage
```bash
 ___                  _                     _    _
| __|__ _ ____  _    /_\   ______ ___ _ __ | |__| |___ _ _
| _|/ _` (_-< || |  / _ \ (_-<_-</ -_) '  \| '_ \ / -_) '_|
|___\__,_/__/\_, | /_/ \_\/__/__/\___|_|_|_|_.__/_\___|_|
             |__/
                                    BY: M.H Aghaee - 0xTFS

Usage: asm <filename.s/.asm> [Switches]
----------------------------
        -nof | --no-output-file         .o and executable files will be deleted
                                        after running the code.
        -g   | --gdb-debug              run the assembly through gdb.
        -v   | --verbose                verbose mode.
----------------------------

Example:
    easm helloworld.asm -nof -v -g
    easm helloworld.asm --no-output-file --verbose --gdb-debug
```
<br />
Just got tired of assembling, linking and executing step by step, so i built this🤷‍♂️ <br />
Hope you find it useful!
