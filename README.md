# libcamera test program

This program has been developed to try and gain some understanding of the use of libcamera
on Raspberry Pi, and test the effect of the various controls.

It is very much a work in progress, but it sort of works sometimes.

Compilation requires:

* The libcamera libraries compiled as described in
https://www.raspberrypi.org/documentation/linux/software/libcamera/README.md
* sudo apt install libwxgtk3.0-gtk3-dev

The resulting ./CamTest application is an X-Window program. When run, it displays camera controls
on the left hand side, and captured images on the right hand side. If too large to display in full
the image can be scrolled to view sections of it.

Most of the controls should be obvious:

* "Image Scale" can be used to reduce the size of the captured image so that more is visible
in the right hand pane.
* The "Snap" button triggers capture of a single image.
* If the "Camera Run" box is checked, then repeated images will be captured. Click the "Snap"
button to start capture.
