# yac8-interpreter
Yet another CHIP-8 Interpreter

## Build
```bash
git submodule update --init --recursive     # Update all Submodules
mkdir build && cd build                     # Create Build Directory
cmake ..                                    # Generate Build
make                                        # Build the Project
```

## Running the Project
```bash
# Running in Debug Mode | yac8_interpreter [rom] --debug
yac8_interpreter ./path/to/rom --debug


# Running Regularly | yac8_interpreter [rom]
yac8_interpreter ./path/to/rom

# Disassembling a ROM | yac8_interpreter [rom] [outFile] -d
yac8_interpreter ./path/to/rom ./path/to/asm/output/file -d
```

## Keyboard Inputs
The CHIP8 uses a Hex Keyboard (0x0 - 0xF), which is mapped as shown below

```
SPACE   ->  0x0
Z       ->  0x1
UP      ->  0x2
X       ->  0x3
LEFT    ->  0x4
C       ->  0x5
RIGHT   ->  0x6
V       ->  0x7
DOWN    ->  0x8
LSHIFT  ->  0x9
A       ->  0xA
S       ->  0xB
D       ->  0xC
F       ->  0xD
Q       ->  0xE
W       ->  0xF
```

## YAC8 Screenshots

Breakout ROM\
![alt text][yac8-img1]

IBM ROM\
![alt text][yac8-img2]

Space Invaders ROM\
![alt text][yac8-img3]

Space Invaders ROM in Debug Mode\
![alt text][yac8-img4]

## License
Licensed under [MIT](LICENSE).


[yac8-img1]: res/images/yac8-breakout.png "Breakout"
[yac8-img2]: res/images/yac8-ibm.png "IBM"
[yac8-img3]: res/images/yac8-space-invaders.png "Space Invaders"
[yac8-img4]: res/images/yac8-debug.png "Space Invaders in Debug Mode"