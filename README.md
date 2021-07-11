# libcamera test program

This program has been developed to try and gain some understanding of the use of libcamera
on Raspberry Pi, and test the effect of the various controls.

Since my interest is in capturing multiple still images, this program has only a single
"Still" stream, no seperate "Viewfinder" stream.

It is very much a work in progress, but it sort of works sometimes.

Compilation requires:

* The libcamera libraries compiled as described in
https://www.raspberrypi.org/documentation/linux/software/libcamera/README.md
* sudo apt install libwxgtk3.0-gtk3-dev

There is now one option in CTFrame.cpp that can be configured before compilation:

* If QUEUE_MULTIPLE is zero then repeated frame capture will not queue a new request
until the previous request has returned.
* If QUEUE_MULTIPLE is non-zero then repeated frame capture will queue all request
buffers as soon as they are available.

The resulting ./CamTest application is an X-Window program. When run, it displays camera controls
on the left hand side, and captured images on the right hand side. If too large to display in full
the image can be scrolled to view sections of it.

Most of the controls should be obvious:

* "Image Scale" can be used to reduce the size of the captured image so that more is visible
in the right hand pane.
* The "Snap" button triggers capture of a single image.
* If the "Camera Run" box is checked, then repeated images will be captured. Click the "Snap"
button to start capture.
