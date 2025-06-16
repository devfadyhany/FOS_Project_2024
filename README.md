FOS (Fictional Operating System)
Overview
FOS (Fictional Operating System) is a custom operating system developed as a learning project to explore low-level systems programming, kernel development, and OS concepts. Built primarily in C/C++, the project uses the Bochs emulator for testing and debugging. This repository contains the core kernel, boot loader, user-space utilities, and configuration files necessary to build and run the OS.
Features

Custom Kernel: Implements core OS functionalities, including process scheduling and memory management.
Boot Loader: Initializes the system and loads the kernel into memory.
User Space: Basic user-space programs and utilities for testing kernel features.
Bochs Emulator Support: Configured for testing and debugging with Bochs (.bochsrc, .bochsrc-debug).
Build Automation: Uses GNUmakefile for building the OS and related scripts for testing and grading.
Portability: Efforts to port the project to modern GCC compilers (see Readme_Project_Port_To_ModernGCC.txt).

Milestones

First Milestone (8 months ago): Initial setup of the project, including boot loader (boot), kernel (kern), configuration files (conf), and Bochs setup (BIOS-bochs-latest, VGABIOS-lgpl-latest).
Scheduler Bug Fix (6 months ago): Fixed a bug in the scheduler promotion logic (inc).
Protection Test Fixes (6 months ago): Addressed issues in protection tests for both kernel (kern) and user-space (user).
Testing Framework (7 months ago): Added initial testing setup (test) and updated .gitignore.
Final Commit (6 months ago): Completed core kernel development and stabilized the project.

Directory Structure
fos/
├── boot/                          # Boot loader source code
├── conf/                          # Configuration files
├── inc/                           # Header files
├── kern/                          # Kernel source code
├── lib/                           # Library functions
├── user/                          # User-space programs
├── test/                          # Test cases and scripts
├── .bochsrc                       # Bochs emulator configuration
├── .bochsrc-debug                 # Bochs debug configuration
├── .cproject                      # Eclipse CDT project file
├── .gdbinit                       # GDB debugger configuration
├── .gitignore                     # Git ignore file
├── .project                       # Eclipse project file
├── BIOS-bochs-latest              # Bochs BIOS file
├── BIOS-bochs-latest_2-3          # Bochs BIOS (version 2.3)
├── FOS_Developer_Console.bat      # Windows script for developer console
├── GNUmakefile                    # Makefile for building the OS
├── Readme_Project_Port_To_ModernGCC.txt  # Notes on porting to modern GCC
├── UpgradeLog.XML                 # Upgrade log for project migrations
├── VGABIOS-lgpl-latest            # VGA BIOS for Bochs
├── bochscon.bat                   # Windows script for Bochs console
├── coding/                        # Coding guidelines or scripts
├── grade.sh                       # Script for grading or testing
├── mergedep.pl                    # Perl script for merging dependencies
├── stdout                         # Output logs or test results
└── README.md                      # This file

Prerequisites

Compiler: GCC (modern version, see Readme_Project_Port_To_ModernGCC.txt for details).
Bochs Emulator: Version compatible with BIOS-bochs-latest and VGABIOS-lgpl-latest.
Make: For building with GNUmakefile.
Perl: For running mergedep.pl (dependency merging script).
Operating System: Linux, macOS, or Windows (with WSL or MinGW for Unix-like environments).

Installation

Clone the repository:git clone <repository-url>


Navigate to the project directory:cd fos


Install Bochs emulator:
Linux: sudo apt-get install bochs
macOS: brew install bochs
Windows: Download and install Bochs from bochs.sourceforge.net


Build the project:make -f GNUmakefile


Run the OS in Bochs:bochs -f .bochsrc

For debugging, use:bochs -f .bochsrc-debug



Usage

Ensure the Bochs configuration files (.bochsrc, .bochsrc-debug) are correctly set up.
Build the OS using make -f GNUmakefile.
Run the OS in Bochs to test kernel and user-space functionalities.
Use grade.sh to run automated tests and verify functionality.
Debug with GDB by loading the .gdbinit configuration if needed.
On Windows, use FOS_Developer_Console.bat or bochscon.bat for development tasks.

Testing

Run the test suite:./grade.sh


Check the test/ directory for specific test cases and expected outputs.
Review stdout for test logs and results.

Contributing

Fork the repository.
Create a new branch:git checkout -b feature-name


Commit your changes:git commit -m "Add feature or fix description"


Push to your branch:git push origin feature-name


Open a pull request with a detailed description of your changes.

License
This project is licensed under the MIT License. See the LICENSE file for details.
Contact
For questions or feedback, open an issue on the GitHub repository or contact your-email@example.com.
Acknowledgments

Built as a personal project to explore operating system development.
Uses Bochs emulator for testing and debugging.
Inspired by educational OS projects like xv6 and Pintos.

