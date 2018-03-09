# VMA Dump Vis

Vulkan Memory Allocator Dump Visualization. It is an auxiliary tool that can visualize internal state of [Vulkan Memory Allocator](../README.md) library on a picture. It is a Python script that must be launched from command line with appropriate parameters.

## Requirements

- Python 3 installed
- [Pillow](http://python-pillow.org/) - Python Imaging Library (Fork) installed

## Usage

```
python VmaDumpVis.py -o OUTPUT_FILE INPUT_FILE
```

* `INPUT_FILE` - path to source file to be read, containing dump of internal state of the VMA library in JSON format (encoding: UTF-8), generated using `vmaBuildStatsString()` function.
* `OUTPUT_FILE` - path to destination file to be written that will contain generated image. Image format is automatically recognized based on file extension. List of supported formats can be found [here](http://pillow.readthedocs.io/en/latest/handbook/image-file-formats.html) and includes: BMP, GIF, JPEG, PNG, TGA.

You can also use typical options:

* `-h` - to see help on command line syntax
* `-v` - to see program version number

## Example output

![Example output](README_files/ExampleOutput.png "Example output")

## Legend

* ![Free space](README_files/Legend_Bkg.png "Free space") Light gray without border - a space in Vulkan device memory block unused by any allocation.
* ![Buffer](README_files/Legend_Buffer.png "Buffer") Yellow rectangle - buffer.
* ![Image Optimal](README_files/Legend_ImageOptimal.png "Image Optimal") Aqua rectangle - image with TILING_OPTIMAL.
* ![Image Linear](README_files/Legend_ImageLinear.png "Image Linear") Green rectangle - image with TILING_LINEAR.
* ![Details](README_files/Legend_Details.png "Details") Black bar or rectangle - one or more allocations of any kind too small to be visualized as filled rectangles.
