#include "helpers.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for( int i = 0 ; i < height ; i++)
    {
        for (int j = 0 ; j < width ; j++)
        {
            float avg = ((float)image[i][j].rgbtBlue + (float)image[i][j].rgbtGreen + (float)image[i][j].rgbtRed ) / 3.0;
            image[i][j].rgbtBlue = round(avg);
            image[i][j].rgbtRed = round(avg);
            image[i][j].rgbtGreen = round(avg);
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    if (width % 2 == 0)
    {
        int middle = width/2;
        for (int j = 0 ; j<height ; j++)
        {
            for (int i = 0 ; i < middle ; i++)
            {
                RGBTRIPLE temp;
                temp  = image[j][width-(i+1)];
                image[j][width-(i+1)] = image[j][i] ;
                image[j][i] = temp;
            }
        }
    }
    else
    {
        int middle  = (width+1)/2;
        for (int j = 0 ; j<height ; j++)
        {
            for (int i = 0 ; i < middle ; i++)
            {
                RGBTRIPLE temp;
                temp  = image[j][width-(i+1)];
                image[j][width-(i+1)] = image[j][i] ;
                image[j][i] = temp;
            }
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE (*temp)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    for (int i = 0 ; i < height ; i++)
    {
        for(int j = 0 ; j < width ; j++)
        {
            temp[i][j] = image[i][j];
        }
    }
    for (int i = 0 ; i < height ; i++)
    {
        for(int j = 0 ; j < width ; j++)
        {
            int avgR = 0,avgB = 0 ,avgG = 0 ,surr = 0;
            for (int k = -1 ; k<=1 ; k++)
            {
                for (int l = -1 ; l<=1;l++)
                {
                    if ( i+k > height-1 || j+l > width-1 || i+k < 0 || j+l < 0)
                    {
                        continue;
                    }
                    else
                    {
                        avgR += temp[i+k][j+l].rgbtRed;
                        avgG += temp[i+k][j+l].rgbtGreen;
                        avgB += temp[i+k][j+l].rgbtBlue;
                        surr++;
                    }
                }
            }
            image[i][j].rgbtRed = round((float)avgR/(float)surr);
            image[i][j].rgbtGreen= round((float)avgG/(float)surr);
            image[i][j].rgbtBlue = round((float)avgB/(float)surr);
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    typedef struct
    {
        int Red;
        int Green;
        int Blue;
    }
    Sobel;
    RGBTRIPLE (*temp)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    for (int i = 0 ; i < height ; i++)
    {
        for(int j = 0 ; j < width ; j++)
        {
            temp[i][j] = image[i][j];
        }
    }
Sobel Gx, Gy;
    for (int i = 0 ; i < height ; i++)
    {
        for (int j = 0 ; j < width ; j++)
        {
            Gx.Red = 0, Gx.Blue = 0, Gx.Green = 0, Gy.Red = 0, Gy.Green = 0, Gy.Blue = 0;
            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    if (i+k > height-1 || j+l > width-1 || i+k < 0 || j+l < 0)
                    {
                        continue;
                    }
                    else
                    {
                        if (k!=0 && l!=0)
                        {
                            //update Gx value depending on surrounding corner pixels
                            Gx.Red += temp[i+k][j+l].rgbtRed*k;
                            Gx.Green += temp[i+k][j+l].rgbtGreen*k;
                            Gx.Blue += temp[i+k][j+l].rgbtBlue*k;
                            //update Gy values ..
                            Gy.Red += temp[i+k][j+l].rgbtRed * (l);
                            Gy.Green += temp[i+k][j+l].rgbtGreen * (l);
                            Gy.Blue += temp[i+k][j+l].rgbtBlue * (l);
                        }
                        else if (k == 0 && l!=0)
                        {
                            // middle lines
                            Gy.Red += temp[i+k][j+l].rgbtRed * (2*l);
                            Gy.Green += temp[i+k][j+l].rgbtGreen * (2*l);
                            Gy.Blue += temp[i+k][j+l].rgbtBlue * (2*l);
                        }
                        else if (l == 0 && k!=0)
                        {
                            //middle lines
                            Gx.Red += temp[i+k][j+l].rgbtRed*(2*k);
                            Gx.Green += temp[i+k][j+l].rgbtGreen*(2*k);
                            Gx.Blue += temp[i+k][j+l].rgbtBlue*(2*k);
                        }
                    }
                }
            // update pixels on image
            if (round(sqrt(pow(Gx.Red,2)+pow(Gy.Red,2))) > 255)
            {
                image[i][j].rgbtRed = 255;
            }
            else
            {
            image[i][j].rgbtRed = round(sqrt(pow(Gx.Red,2)+pow(Gy.Red,2)));
            }
            if (round(sqrt(pow(Gx.Green,2)+pow(Gy.Green,2))) > 255)
            {
                image[i][j].rgbtGreen = 255;
            }
            else
            {
            image[i][j].rgbtGreen = round(sqrt(pow(Gx.Green,2)+pow(Gy.Green,2)));
            }
            if (round(sqrt(pow(Gx.Blue,2)+pow(Gy.Blue,2))) > 255)
            {
                image[i][j].rgbtBlue = 255;
            }
            else
            {
            image[i][j].rgbtBlue = round(sqrt(pow(Gx.Blue,2)+pow(Gy.Blue,2)));
            }
        }
    }
}
}