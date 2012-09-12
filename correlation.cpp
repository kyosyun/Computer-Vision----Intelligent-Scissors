#include "correlation.h"
#include <assert.h>
#include <iostream>

using namespace std;

// TODO:
//		- Double-check that selection works

void printMatrix(double *matrix, int mWidth, int mHeight)
{
	cout << "Begin matrix \n";
	for (int i = 0; i < mWidth * mHeight; i++)
	{
		cout << matrix[i] << "\n";
	}
	cout << "End matrix \n\n";
}
/************************ TO BE DONE 2 **************************/
/*
 *	INPUT: 
 *		origImg:		the original image, 
 *		imgWidth:		the width of the image
 *		imgHeight:		the height of the image
 *						the image is arranged such that 
 *						origImg[3*(row*imgWidth+column)+0], 
 *						origImg[3*(row*imgWidth+column)+1], 
 *						origImg[3*(row*imgWidth+column)+2]
 *						are R, G, B values for pixel at (column, row).
 *
 *      kernel:			the 2D filter kernel,
 *		knlWidth:		the width of the kernel
 *		knlHeight:		the height of the kernel
 *		
 *		scale, offset:  after correlating the kernel with the origImg, 
 *						each pixel should be divided by scale and then added by offset
 *      
 *		selection:      a byte array of the same size as the image, 
 *						indicating where in the original image should be filtered, e.g., 
 *						selection[k] == 1 ==> pixel k should be filtered
 *                      selection[k] == 0 ==> pixel k should NOT be filtered
 *                      a special case is selection is a NULL pointer, which means all the pixels should be filtered. 
 *
 *  OUTPUT:
 *		rsltImg:		the filtered image of the same size as original image.
 *						it is a valid pointer ( allocated already ).
 */

void image_filter(double *rsltImg, const unsigned char *origImg, const unsigned char *selection, 
			int imgWidth, int imgHeight, 
			const double *kernel, int knlWidth, int knlHeight,
			double scale, double offset)
{
	// TODO:
	//		- Shouldn't selection be a char?
		for (int row = 0; row < imgHeight; row++)
		{
			for (int col = 0; col < imgWidth; col++)
			{
				if (selection == NULL || selection[3*(row*imgWidth+col)] == 1)
				{
					double rsltPixel[3] = {0.,0.,0.};
					pixel_filter(rsltPixel, col, row, origImg, imgWidth, imgHeight, kernel, knlWidth, knlHeight, scale, offset);
					
					for (int i = 0; i < 3; i++)
					{
						rsltImg[3*(row*imgWidth+col)+i] = rsltPixel[i];
					}
				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						rsltImg[3*(row*imgWidth+col)+i] = origImg[3*(row*imgWidth+col)+i];
					}
				}
			}
		}
}

/************************ END OF TBD 2 **************************/


/************************ TO BE DONE 3 **************************/
/*
 *	INPUT: 
 *      x:				a column index,
 *      y:				a row index,
 *		origImg:		the original image, 
 *		imgWidth:		the width of the image
 *		imgHeight:		the height of the image
 *						the image is arranged such that 
 *						origImg[3*(row*imgWidth+column)+0], 
 *						origImg[3*(row*imgWidth+column)+1], 
 *						origImg[3*(row*imgWidth+column)+2]
 *						are R, G, B values for pixel at (column, row).
 *
 *      kernel:			the 2D filter kernel,
 *		knlWidth:		the width of the kernel
 *		knlHeight:		the height of the kernel
 *
 *		scale, offset:  after correlating the kernel with the origImg, 
 *						the result pixel should be divided by scale and then added by offset
 *
 *  OUTPUT:
 *		rsltPixel[0], rsltPixel[1], rsltPixel[2]:		
 *						the filtered pixel R, G, B values at row y , column x;
 */

void pixel_filter(double rsltPixel[3], int x, int y, const unsigned char *origImg, int imgWidth, int imgHeight, 
			  const double *kernel, int knlWidth, int knlHeight,
			  double scale, double offset)
{
	//ensure that the kernel is of odd dimensions
	assert(knlWidth % 2 == 1);
	assert(knlHeight % 2 ==1);

	for (int row = - knlHeight/2; row <= knlHeight/2; row++)
	{
		for (int col = - knlWidth/2; col <= knlWidth/2; col++)
		{
			int offsetY = y + row;
			int offsetX = x + col;
			
			for (int i = 0; i < 3; i++)
			{
				double valueAtPixel;

				if (offsetY < 0 || offsetX < 0 || offsetY >= imgHeight || offsetX >= imgWidth)
				{
					valueAtPixel = 0;
				}
				else
				{
					valueAtPixel =  origImg[3*(offsetY*imgWidth+offsetX)+i];
				}

				double valueInKernel = kernel[(row + knlHeight/2)*knlWidth + (col + knlWidth/2)];
				rsltPixel[i] += valueInKernel * valueAtPixel; 
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		rsltPixel[i] = rsltPixel[i] * scale + offset;
	}
	

}

/************************ END OF TBD 3 **************************/

