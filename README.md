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