# PTX Texture Converter

## Overview

PTX Texture Converter is a command line tool to convert images stored in different file formats (JPG/PNG/DDS/etc.) to PTX texture format. The format has been designed so that the files can be loaded to memory as binary blob without need for post-load processing, which enables storing the textures to flash memory on microcontrollers. Currently the tool supports various RGBA formats, but only 2D textures and no mipmaps. You can find the Windows executable of the tool in [`bin`](bin) directory and by running `ptx_conv.exe -h`. you'll see the different options. For example running
```
ptx_conv.exe -i smiley.png -o ptx_smiley.h -hexd -f r5g6b5
```
Converts the [`smiley.png`](bin/smiley.png) image in the dir to `ptx_smiley.h` file with pixel format `r5g6b5`, which can be #included in Arduino programs to be embedded in the executable and stored in the flash memory.

## Compilation Instructions
1) PTX Texture Converter depends on [mini_sxp](https://github.com/JarkkoPFC/mini_sxp) core library, so first you need to clone the library (or copy the zip) to some directory.
2) Run [install.bat](install.bat) and type in the directory where you cloned the library (i.e. where mini_sxp README.md file resides). This creates the appropriate symlink to the directory.

Now you should be able to open `ptx_conv.sln` in Visual Studio and compile the tool. If the created symlink doesn't work, Visual Studio will fail to load mini_sxp library.

## License

PTX Texture Converter is licensed under MIT license. See [LICENSE](LICENSE) for details.
