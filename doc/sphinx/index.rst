.. CamlImages documentation master file, created by
   sphinx-quickstart on Tue Jan 25 22:34:37 2011.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

======================================
CamlImages v4.0.1
======================================
*Objective Caml image processing library*

.. toctree::
   :numbered:
   
What is CamlImages ?
======================================

CamlImages is an image processing library, which provides

* Basic functions for image processing and loading/saving various image file formats (hence providing a translation facility from format to format),
* Interfaces with the Caml graphics library and LablGtk2 allows to display images in GUI environments
* A freetype interface, integrated into the library: you can draw texts into images using any truetype fonts. 

In addition, the library can handle huge images that cannot be (or can hardly be) stored into the main memory (the library then automatically creates swap files and escapes them to reduce the memory usage).

Additional example programs are supplied in the examples directory of the library:

converter
    Conversion program, that converts image from formats to formats. 
crop
    `Cropping' program, that cuts an image to its edges.
edgedetect
    Program to detect edges into an image.
gifanim
    Animation program for gif encoded series of images. 
liv
    Image viewer using LablGtk.
monochrome
    Converts a color image to monochrome. 
normalize
    Normalizes the colormap of an image. This program analyzes the colormap information of the image to normalize its brightness over the full range available. 
resize
    Program to resize an image. 
tiffps
    Program to output a postscript version of a tiff image (for instance to print it). 
ttfimg
    True-type font dump program. It can be used to create fancy banners or buttons.
    Try for instance ``ttfimg ../../test/micap.ttf -message Bonjour -fg red -bg black``. Visualize with liv (of course). ttfimg also creates some sample images of true type fonts. Try ``ttfimg -o sample_micap.jpg ../../test/micap.ttf``.

Download and Installation
--------------------------

Get the source tar ball, decompress it, and read the file INSTALL.
You can also access the read-only repository which contains the latest developing (but not yet released) version.

Source download
    https://bitbucket.org/camlspotter/camlimages/get/v4.0.1.tar.gz

Repository
    hg clone https://bitbucket.org/camlspotter/camlimages

Using CamlImages
-------------------

Color models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CamlImages supports the following color models:

* Rgb24: 24bit depth full color image.
* Index8: 8bit depth indexed image with transparent information.
* Index16: 16bit depth indexed image with transparent.
* Rgba32: 32bit depth full color image, with the alpha channel.
* Cmyk32: 32bit Cyan Magenta Yellow and black image. (Unfortunately, beautiful conversion between RGB and CMYK is not supported.) 

For each color model, a corresponding module is provided. For example, use the module Rgb24 if you want to access 24bit depth full color images.

Load/Save image files and other fancy features
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CamlImages supports loading and saving of the following file formats:

* Bitmap, windows bitmap format (.bmp).
* Gif, graphics interchange format (.gif) (not recommended).
* Jpeg, joint photographic experts group (.jpeg or .jpg).
* Png, portable network graphics (.png).
* Tiff, tagged image file format (.tiff or .tif). (Color only.)
* X pixmaps (.xpm). (Loading only.)
* Encapsulated Postscript (using ghostscript)
* xv thumbnail files (see xvthumb directory). 

For each image format, we provide a separate module. For instance, use the Tiff module to load and save images stored in the tiff file format.
If you do not want to specify the file format, you can use Image.load: this function automatically analyzes the header of the image file at hand and loads the image into the memory, if the library supports this format.

CamlImages also provides an interface to the internal image format of O'Caml's Graphics library (this way you can draw your image files into the Graphics window).

You can also draw strings on images using the Freetype library, which is an external library to load and render TrueType fonts.

Class interface
^^^^^^^^^^^^^^^^^^^^^^^^^^

The modules with names beginning with the letter 'o' contains the object class interface for CamlImages.

Image swap
^^^^^^^^^^^^^^^^^^^^^^^^^^

When you create/load a huge image, the computer memory may not be sufficient to contain all the data. (For example, this may happen if you are working with a scanned image of A4, 720dpi, 24bit fullcolor, even if you have up to 128Mb of memory!) To work with such huge images, CamlImages provides image swaps, which can escape part of the images into files stored on the hard disk. A huge image is thus partitioned into several blocks and if there is not enough free memory, the blocks which have not been accessed recently are swapped to temporary files. If a program requests to access to such a swapped block, the library silently loads it back into memory.
By default, image swapping is disabled, because it slows down the programs. To activate this function, you have to modify Bitmap.maximum_live and Bitmap.maximum_block_size:

* Bitmap.maximum_live is the maximum heap live data size of the program (in words)
* Bitmap.maximum_block_size is the maximum size of swap blocks (in words). 


For example, if you do not want to use more than 10M words (that is 40Mb for a 32bit architecture or 80Mb for a 64bit architecture), set Bitmap.maximum_live to 10000000. You may (and you should) enable heap compaction, look at the GC interface file, gc.mli, in the standard library for more details (you should change the compaction configuration).
Bitmap.maximum_block_size affects the speed and frequency of image block swapping. If it is larger, each swapping becomes slower. If it is smaller, more swappings will occur. Too large and too small maximum_block_size, both may make the program slower. I suggest to have maximum_block_size set to !Bitmap.maximum_live / 10.
If you activated image swapping, cache files for unused swapped blocks will be removed automatically by Caml GC finalization, but you may free them explicitly by hand also. The functions and methods named "destroy" will free those blocks.
The swap files are usually created in the /tmp directory. If you set the environment variable "CAMLIMAGESTMPDIR", then its value replaces the default "/tmp" directory. The temporary files are erased when the program exits successfully. In other situations, for instance in case of spurious exception, you may need to erase temporary files manually.

Miscellaneous
-----------------

Examples of use of the library ?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can find some examples in the examples directory including image auto-cropping, resizing and an image viewer with the lablgtk library.

How do I organize my Makefile to use the library ?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

From v4.0.0, CamlImages use the `OMake build system <http://omake.metaprl.org/index.html>`_ and you need to write OMakefile instead of Makefile. You can see such an example of OMakefile at:

        examples/converter-external/OMakefile

Issue tracker
----------------------
http://modulogic.inria.fr/bugtracker/view_all_bug_page.php
