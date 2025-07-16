# FOS (Fictional Operating System)
## Overview
<b>FOS (Fictional Operating System):</b> is a custom operating system developed as a learning project to explore low-level systems programming, kernel development, and OS concepts. Built primarily in C, the project uses the Bochs emulator for testing and debugging. This repository contains the core kernel, boot loader, user-space utilities, and configuration files necessary to build and run the OS.

## Features
<ul>
  <li><b>Custom Kernel:</b> Implements core OS functionalities, including process scheduling and memory management.</li>
  <li><b>Boot Loader:</b> Initializes the system and loads the kernel into memory.</li>
  <li><b>User Space:</b> Basic user-space programs and utilities for testing kernel features.</li>
</ul>

## Milestones
<b>First Milestone:</b> Initial setup of the project, including boot loader (boot), and kernel (kern).<br/><br/>
<b>Second Milestone:</b> Advanced memory management for kernel, user and shared memory spaces with protection for shared operations.<br/><br/>
<b>Third Milestone:</b> Processes scheduleing on cpu and finalizing the project.<br/><br/>

## Directory Structure
<pre>
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
</pre>

## Installation
- Clone the repository: ```git clone https://github.com/devfadyhany/FOS_Project_2024```
- Navigate to the project directory and run FOS_Developer_console.bat


## Team Members
- Amr Mohammed
- Bishoy Ayman
- Mennah Mustafa
- Fady Adel
- Kerolous Roumany
