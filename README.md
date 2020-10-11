# Password Manager
Stores your passwords under two layers of AES256 encryption. Sensitive data is
cleared from RAM when the application exits.

Uses GTK+3.0 (to display GUI windows) and OpenSSL (to hash and encrypt data).
Requires a POSIX-compliant system, because I have used functions not specified
in the C standard, but specified by POSIX.

# Usage (Linux)
* Download or clone the repository to a convenient location on your computer.
* Open a terminal window to that location.
* Enter the command `make exec`. (This will run the Linux executable `main`.)

The Linux executable was compiled for a 64-bit CPU, so it may not run on your
system if you have a 32-bit CPU. In which case, you could compile the program
yourself and run it. (This is tested only on Debian; it is not guaranteed to
work on all systems. And I have ceased active development on this project, and
may never get around to test it further.)
* Install the required libraries.
  * GTK+3.0: `sudo apt install libgtk-3-dev`
  * OpenSSL: `sudo apt install libssl-dev`
* Enter the command `make run`.

It is possible to run this on Windows, too. However, most of the features will
be disabled. (You can only view passwords.) To get the Windows executable, look
for [releases](https://github.com/tfpf/gtk-windowing/releases).

The current passphrase is `q`, and you can change it. Some sample passwords are
already saved. You can view them once you have entered the passphrase.

