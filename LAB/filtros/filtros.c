#include <stdio.h>
#include <stdlib.h>

#include "filtros.h"

// Inputs: A BMP image and a saturation factor
// Outputs: A new saturated BMP image
// Description: Saturates a BMP image by multiplying the RGB components of each pixel by a factor
BMPImage *saturate_bmp(BMPImage *image, float factor)
{
    // Allocate memory for the new image
    BMPImage *new_image = (BMPImage *)malloc(sizeof(BMPImage));
    if (!new_image)
    {
        fprintf(stderr, "Error: Could not allocate memory for the new saturated image.\n");
        return NULL;
    }
    // Copy the width and height of the original image
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (Pixel *)malloc(sizeof(Pixel) * image->width * image->height);
    if (!new_image->data)
    {
        fprintf(stderr, "Error: Could not allocate memory for the new saturated image data.\n");
        free(new_image);
        return NULL;
    }
    // Saturate the colors of the original image
    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            Pixel pixel = image->data[y * image->width + x];
            // Multiply the RGB components of each pixel by the factor and limit the value to 255
            // (if the result is greater than 255, assign 255)
            // (convert the result to unsigned char to ensure the value is in the range [0, 255])
            pixel.r = (pixel.r * factor > 255) ? 255 : (unsigned char)(pixel.r * factor);
            pixel.g = (pixel.g * factor > 255) ? 255 : (unsigned char)(pixel.g * factor);
            pixel.b = (pixel.b * factor > 255) ? 255 : (unsigned char)(pixel.b * factor);
            new_image->data[y * image->width + x] = pixel;
        }
    }

    return new_image;
}

// Inputs: A BMP image
// Outputs: A new grayscale BMP image
// Description: Converts a BMP image to grayscale
BMPImage *grayscale_bmp(BMPImage *image)
{
    // Allocate memory for the new image
    BMPImage *new_image = (BMPImage *)malloc(sizeof(BMPImage));
    if (!new_image)
    {
        fprintf(stderr, "Error: Could not allocate memory for the new grayscale image.\n");
        return NULL;
    }
    // Copy the width and height of the original image
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (Pixel *)malloc(sizeof(Pixel) * image->width * image->height);
    if (!new_image->data)
    {
        fprintf(stderr, "Error: Could not allocate memory for the new grayscale image data.\n");
        free(new_image);
        return NULL;
    }
    // Convert the colors of the original image to grayscale
    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            Pixel pixel = image->data[y * image->width + x];
            // Alter the value of each RGB component by applying a factor
            // Pixel = (R*0.3) + (G*0.59) + (B*0.11)
            // (convert the result to unsigned char to ensure the value is in the range [0, 255])
            unsigned char gray = (unsigned char)(pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11);
            pixel.r = gray;
            pixel.g = gray;
            pixel.b = gray;
            new_image->data[y * image->width + x] = pixel;
        }
    }
    return new_image;
}

// Inputs: A BMP image and a threshold
// Outputs: A new binarized BMP image
// Description: Binarizes a BMP image by converting it to black and white based on a threshold
BMPImage *binary_bmp(BMPImage *image, float threshold)
{
    // Allocate memory for the new image
    BMPImage *new_image = (BMPImage *)malloc(sizeof(BMPImage));
    if (!new_image)
    {
        fprintf(stderr, "Error: Could not allocate memory for the new binarized image.\n");
        return NULL;
    }
    // Copy the width and height of the original image
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (Pixel *)malloc(sizeof(Pixel) * image->width * image->height);
    if (!new_image->data)
    {
        fprintf(stderr, "Error: Could not allocate memory for the new binarized image data.\n");
        free(new_image);
        return NULL;
    }
    // Binarize the pixels of the image
    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            Pixel pixel = image->data[y * image->width + x];
            // Calculate the intensity of the pixel
            float intensity = pixel.r + pixel.g + pixel.b;
            float threshold_value = threshold * 255 * 3;

            // If the pixel intensity is greater than the threshold, set it to white (255), otherwise set it to black (0)
            if (intensity > threshold_value)
            {
                pixel.r = 255;
                pixel.g = 255;
                pixel.b = 255;
            }
            else
            {
                pixel.r = 0;
                pixel.g = 0;
                pixel.b = 0;
            }

            new_image->data[y * image->width + x] = pixel;
        }
    }
    return new_image;
}

// Inputs: A BMP image and a threshold
// Outputs: A boolean indicating whether the image is nearly black or not
// Description: Classifies a BMP image as nearly black or not by counting the black pixels and comparing them to a threshold
bool is_nearly_black(BMPImage *image, float threshold)
{
    int black_pixels = 0;                            // Counter for black pixels
    int total_pixels = image->width * image->height; // Counter for total pixels

    // Iterate over all the pixels in the image
    for (int y = 0; y < image->height; y++)
    { // For each pixel position
        for (int x = 0; x < image->width; x++)
        {
            Pixel pixel = image->data[y * image->width + x]; // Get the pixel
            // Check if the pixel is black
            if (pixel.r == 0 && pixel.g == 0 && pixel.b == 0)
            {
                black_pixels++; // If it is a black pixel, count it
            }
        }
    }

    // Calculate the percentage of black pixels
    float black_percentage = (float)black_pixels / total_pixels; // Calculate the percentage of black pixels relative to the total
    printf("The percentage is %f\n", black_percentage);

    // Compare the percentage to the threshold
    return black_percentage >= threshold;
}