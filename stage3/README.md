# Stage 3

This stage covers building and installing Alfred Arnold's 'AS' macro assembler.
This will be the tool that is used to assemble Z80 code, as GNU Binutils is
unable to do so.


## Building

AS is included as a Git submodule, so make sure that the submodule is correctly
checked-out with the `git submodule update --init asl-releases` command.

Once that is done, simply run the `as.sh` script to compile and install AS.
After this, the assembler will be ready for usage, and you can move onto the
next stage.
