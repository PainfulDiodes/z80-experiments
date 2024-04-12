This harness for the Z80 CPU has been updated to allow for actual ROM
The Arduino at this point needs to provide:
* IO
* RAM for a stack
* RAM for a buffer
The marvin ROM has been burned which makes use of the stack and buffer
The sketch needs to keep out of the way of the ROM
Keep the data lines high-z unless either a RAM read or an IO read is requested