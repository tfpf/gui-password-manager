# Password Manager
Stores your passwords under two layers of AES-256 encryption. Sensitive data is
cleared from RAM when the application exits.

Uses GTK+3.0 (to display GUI windows), OpenSSL (to hash and encrypt data) and
Sodium (to generate cryptographically secure random numbers). Compiling this
requires a POSIX-compliant system, because I have used functions not specified
in the C standard, but specified by POSIX.

This application can automatically type the username and password for you; this
is useful if a website has disabled copying and pasting. However, automatic
typing may not work with non-ASCII characters.

# Requirements
* basic computer knowledge
* 64-bit operating system (either Windows or a Linux distribution)

# Notes
* The default passphrase is `q`.
* `Queen.bin` contains the hashed passphrase.
* `King.bin` contains the encrypted passwords.

# Usage

### The Easy Way (run pre-built binaries)
See [releases](https://github.com/tfpf/gtk-windowing/releases) and download the
latest release. (It will be a ZIP file.) Once downloaded, extract the ZIP file.
Navigate to `bin` in the extracted folder.

#### If you are on a Linux distribution
Run `ED_Manager` or `ED_Manager.sh`. (Some file managers won't let you execute
binaries, while other won't let you execute shell scripts. If yours doesn't let
you do either, open a terminal there and enter the command `./ED_Manager`.)

#### If you are on Windows
Run `ED_Manager.exe`.

There is no release for macOS.

### The Hard Way (compile from source and run)
* Install the following dependencies.
  * GTK+3.0 (GUI widget toolkit)
  * OpenSSL (cryptography toolkit)
  * Sodium (cryptography toolkit)
* Clone this repository. Open the folder where you cloned it.
* Generate `resource.o` from `resource.rc`. (I forgot how.:sweat_smile:)
* Depending on your environment, enter the correct run command.
  * Linux terminal: `make lrun`
  * Windows MSYS2 (MinGW shell): `make wrun`
  * macOS terminal: `make mrun`

If you are using any other environment, you will have to find out how to run
the program on your own.
