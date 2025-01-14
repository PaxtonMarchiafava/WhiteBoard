# Overview

In this project, and arduino pulls an image from an SD card and draws it on a white board.

## Hardware

The electronics consist of and arduino nano, an SD card reader, an SG90 servo, and 2 NEMA 17 stepper motors. The stepper motors move a gantry that holds a marker connected to a servo. When the stepper motors gets to their required positions the arduino board moves the servo to draw a dot. This is repeated for every pixel in the image.

## Software

The Arduino pulls image width and height from the header in the image file so it can move to a new line properly. it then starts looping through every pixel in the image and drawing. To determine if a pixel should be drawn, the arduino calculates the brightness and if the pixel is dark enough, the pixel is drawn.  

In order to move to the correct location on the white board, some simple trigonometry is done based on where the pixel is in the image and stepper motor step calculations are done accordingly.

## Outcome

![](/MEDIA/PAX05933.JPG)
![](/MEDIA/PAX05934.JPG)
