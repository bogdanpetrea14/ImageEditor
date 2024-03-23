
Image Processing Program
The program reads a whole line from the command line, saving it into an integer variable. Later, depending on the first character of the string, it will enter a switch case structure to perform the following operations.

Load Image (L): If the first letter is "L", the program checks if there is a name after the command and attempts to load the respective image into memory. It opens the file with the corresponding name and saves values such as the magic number (P is eliminated, only the effective digit is saved, i.e., values 2, 3, 5, 6), the number of columns, rows, and the maximum value of the pixels in an array or matrix. For grayscale images (type 2 or 5), an array of rows * cols values is allocated to save all pixels. For color images, a matrix of rows * (cols * 3) is allocated, with 3 values for each pixel (values for red, green, and blue). Comments in the image are ignored. The subroutine returns a pointer to the matrix that stores the pixels.

Save Image (S): If the first letter is "S", it checks if the second letter is "A", in which case the image will be saved. The name with which the image is to be saved is extracted, and all the data is saved in the respective file. If the length of the command is greater than 11, then the Select operation will be performed, which restricts the effect of the following commands to the selected area. If the length of the command does not exceed 11 characters, then the command would be Select All, in which case the effect of the following commands will apply to the entire image.

Crop Image (C): If the first letter is "C", the selected image area is the one that remains in the end, everything outside being completely eliminated. After executing the command, it is assumed that the remaining area is automatically the one referred to when applying subsequent commands.

Histogram (H): If the first letter is "H", the histogram of the grayscale image is displayed. The frequency of pixels is calculated, and the histogram will be displayed accordingly.

Equalize Image (E): If the first letter is "E", it checks if the second letter is "Q" or "X". If the second letter is "Q", the image equalization will be performed. When the second letter is "X", the program will gracefully shut down, releasing all allocated resources and closing everything.

Apply Image Effect (A): If the first letter is "A", the apply command will be applied, which applies an image effect on the given selection.

Rotate Image (R): If the first letter is "R", the Rotate command will be applied, resulting in the rotation of the image.

This program is designed to perform various image processing operations based on user commands, offering functionalities such as image loading, saving, cropping, histogram generation, equalization, applying effects, and rotation.
