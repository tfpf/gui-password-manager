# Password Manager
Stores your passwords under two layers of AES256 encryption. Sensitive data is
cleared from RAM when the application exits.

Uses GTK+3.0 (to display GUI windows) and OpenSSL (to hash and encrypt data).
Compiling this requires a POSIX-compliant system, because I have used functions
not specified in the C standard, but specified by POSIX.

# Requirements
* basic computer knowledge
* 64-bit operating system (either Windows or a Linux distribution)

# Notes
* The default passphrase is `q`.
* `Master.bin` contains the hashed passphrase.
* `Slave.bin` contains the encrypted passwords.

# Usage

## The Easy Way
See [releases](https://github.com/tfpf/gtk-windowing/releases) and follow the
instructions there.

## The Hard Way
* Install the following dependencies.
  * GTK+3.0 (GUI widget toolkit)
  * OpenSSL (cryptography toolkit)
  * Sodium (cryptography toolkit)
* Clone this repository. Open the folder where you cloned it.
* Depending on your environment, enter the correct run command.
  * Linux terminal: `make lrun`
  * Windows MSYS2 (MinGW shell): `make wrun`
If you are using any other environment, you will have to find out how to run
the program on your own.

