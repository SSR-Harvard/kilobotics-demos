# Kilobotics Demos

[Kilobotic Labs](https://www.kilobotics.com/labs)

[Kilobot Library](https://www.kilobotics.com/docs/index.html)

[Documentation](https://www.kilobotics.com/documentation)

[Hex Compiler](https://diode.group.shef.ac.uk/kilobots/index.php/Getting_Started:_How_to_work_with_kilobots)
---


## Compiling Hex Files

- First, download the avr libraries seen [here for mac](https://github.com/osx-cross/homebrew-avr). On mac, I had first run 'brew tap osx-cross/avr', and then 'brew install avr-gcc'
- Put your source file in the source directory, /kilobotics-demos/src/
- In the /kilobotics-demos/ directory, run "make BASE=your_file", where your_file.c is the aforementioned source file in /src/
- your_file.hex will be compiled in the /kilobotics-demos/bin/ folder

## Cleaning Hex Files

- Run "make clean BASE=your_file" to delete your_file.hex from /kilobotics-demos/bin/
