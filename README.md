# Kilobotics Demos

[Kilobotic Labs](https://www.kilobotics.com/labs)

[Kilobot Library](https://www.kilobotics.com/docs/index.html)

[Documentation](https://www.kilobotics.com/documentation)

[Hex Compiler](https://diode.group.shef.ac.uk/kilobots/index.php/Getting_Started:_How_to_work_with_kilobots)
---

## Included Demos

### Sync
Sync starts with the bots randomly flashing and vibrating. Bots average their neighbors' position in the flash cycle and correct themselves to reflect the average of their neighbors' position. Eventually, the swarm synchronizes their flash.  

### Adaptive Gradient
Bots with a kilo\_uid set to 42 are a seed. Bots transmit their "gradient number" to neighboring bots. The seed has a gradient number of 0. Non-seed bots set their gradient number to the minimum received gradient number + 1 (e.g. a bot receiving messages from the seed will have a gradient number of 0 + 1 = 1). Every 2 seconds or so, a bot will reset its gradient number. 

### Disperse 
If a bot is within communication range of at least one other bot (i.e. the bot is receiving messages), then the bot will do a random walk to get away from that bot. When a bot isn't in communication range of any other bots, it will stop moving.

### Phototaxis
Photo = light and taxis = motion. 
Bots spin until they see a dip in measured light intensity (i.e. when the light source is blocked by the charging tab so the light measurement decreases). Then, the bot turns until the light measurement rises, at which point, the bot turns the other direction until the light measurement rises once again. Effectively, this makes the bots "wiggle" back and forth towards the light source.

### Synchronized Phototaxis
Same as phototaxis except the bots turn at the same time.

### Morph
Implementation of Turing Morphogenesis patterns on a Kilobot swarm. Code from the paper ["Morphogenesis in Robot Swarms"](https://robotics.sciencemag.org/content/3/25/eaau9178). The public github repo is found [here](https://github.com/Danixk/Turing_morphogenesis).
NOTE: Only the hex file is included in this repo. See the aforementioned github repo for the source code.

### Herd
Herd simply spins up the kilobot motors to assist in herding them. 
The bots spin up for a few seconds periodically. They then take a few seconds of rest to keep them from overheating. Apply force to the bots when they are spun up. 

## Compiling Hex Files

- First, download the avr libraries seen [here for mac](https://github.com/osx-cross/homebrew-avr). On mac, I had first run 'brew tap osx-cross/avr', and then 'brew install avr-gcc'
- Put your source file in the source directory, /kilobotics-demos/src/
- In the /kilobotics-demos/ directory, run "make BASE=your\_file", where your\_file.c is the aforementioned source file in /src/
- your\_file.hex will be compiled in the /kilobotics-demos/bin/ folder

## Cleaning Hex Files

- Run "make clean BASE=your\_file" to delete your\_file.hex from /kilobotics-demos/bin/
