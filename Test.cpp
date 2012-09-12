#include "Test.h"
#include <assert.h>

void testSuite()
{
	testPixelFilter();
	testEdgePixelFilter();
	testImageFilter();
	testInitNodeBuf();
	//testMinimumPath();
}

void testPixelFilter()
{
	/*pixel_filter(double rsltPixel[3], int x, int y, const unsigned char *origImg, int imgWidth, int imgHeight, 
			  const double *kernel, int knlWidth, int knlHeight,
			  double scale, double offset)*/
	double rsltPixel[3] = {0., 0., 0.};
	const unsigned char origImg[27] = 
		{
			1., 1., 1., 2., 2., 2., 3., 3., 3.,
			4., 4., 4., 5., 5., 5., 6., 6., 6.,
			7., 7., 7., 8., 8., 8., 9., 9., 9.
		};
	double kernel[9] =
		{
			1., 1., 1.,
			1., 1., 1.,
			1., 1., 1.
		};
	pixel_filter(rsltPixel, 1, 1, origImg, 3, 3, kernel, 3, 3, 1./9., 0.);
	
	for (int i = 0; i < 3; i++)
	{
		assert(rsltPixel[i] == 5.);
	}
}

void testEdgePixelFilter()
{
	/*pixel_filter(double rsltPixel[3], int x, int y, const unsigned char *origImg, int imgWidth, int imgHeight, 
			  const double *kernel, int knlWidth, int knlHeight,
			  double scale, double offset)*/
	double rsltPixel[3] = {0., 0., 0.};
	const unsigned char origImg[27] = 
		{
			1., 1., 1., 2., 2., 2., 3., 3., 3.,
			3., 3., 3., 3., 3., 3., 3., 6., 6.,
			7., 7., 7., 8., 8., 8., 9., 9., 9.
		};
	double kernel[9] =
		{
			1., 1., 1.,
			1., 1., 1.,
			1., 1., 1.
		};
	pixel_filter(rsltPixel, 0, 0, origImg, 3, 3, kernel, 3, 3, 1./9., 0.);
	
	for (int i = 0; i < 3; i++)
	{
		assert(rsltPixel[i] == 1.);
	}
}

void testImageFilter()
{
	/*pixel_filter(double rsltPixel[3], int x, int y, const unsigned char *origImg, int imgWidth, int imgHeight, 
			  const double *kernel, int knlWidth, int knlHeight,
			  double scale, double offset)*/
	double rsltPixel[3] = {0., 0., 0.};
	const unsigned char origImg[27] = 
		{
			1., 1., 1., 2., 2., 2., 5., 5., 5.,
			4., 4., 4., 5., 5., 5., 6., 6., 6.,
			7., 7., 7., 8., 8., 8., 9., 9., 9.
		};
	double kernel[9] =
		{
			1., 1., 1.,
			1., 1., 1.,
			1., 1., 1.
		};
	const unsigned char selection[9] = 
		{
			0., 0., 1.,
			0., 0., 0.,
			0., 0., 0.
		};

	const unsigned char expected[27] = 
		{
			1., 1., 1., 2., 2., 2., 2., 2., 2.,
			4., 4., 4., 5., 5., 5., 6., 6., 6.,
			7., 7., 7., 8., 8., 8., 9., 9., 9.
		};

	double rsltImg[27] = {0};

	image_filter(rsltImg, origImg, selection, 3, 3, kernel, 3, 3, 1./9., 0.);
	
	for (int i = 0; i < 27; i++)
	{
		assert(rsltImg[i] == (double)origImg[i]);
	}
}

void testInitNodeBuf()
{
	const unsigned char img[27] =
	{
		9, 8, 1,	4, 7, 7,	3, 2, 5,

		8, 2, 9,	9, 6, 4,	8, 4, 4, 
		
		2, 1, 6,	2, 4, 0,	6, 5, 0
	};

	//Why does this work vs on the same line?
	Node *nodes;
	
	nodes = new Node[9]();
	InitNodeBuf(nodes, img, 3, 3);

	Node centerNode = nodes[4];

	for (int i = 0; i < 8; i++)
	{
		printf("Link %d has cost %f\n", i, centerNode.linkCost[i]);
	}
}

void testMinimumPath()
{
	//Implement
}