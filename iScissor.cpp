/* iScissor.cpp */
/* Main file for implementing project 1.  See TO BE DONE statments below 
 * (see also correlation.cpp and iScissor.h for additional TODOs) */

// TODO:
//		- Test MinimumPath function
//		- Test LiveWireDP
//		- Remove auto-return code in LiveWireDP and MinimumPath
#include <assert.h>

#include "correlation.h"
#include "iScissor.h"
#include <math.h>

// two inlined routines that may help;

inline Node &NODE(Node *n, int i, int j, int width)
{ return *(n+j*width+i); }

inline unsigned char PIXEL(const unsigned char *p, int i, int j, int c, int width)
{ return *(p+3*(j*width+i)+c); }

/************************ TO BE DONE 1 ***************************
 *InitNodeBuf
 *	INPUT: 
 *		img:	a RGB image of size imgWidth by imgHeight;
 *		nodes:	a allocated buffer of Nodes of the same size, one node corresponds to a pixel in img;
 *  OUPUT:
 *      initializes the column, row, and linkCost fields of each node in the node buffer.  
 */

void InitNodeBuf(Node *nodes, const unsigned char *img, int imgWidth, int imgHeight)
{
	int nodeIndex = 0;
	for (int row = 0; row < imgHeight; row++)
	{
		for (int col = 0; col < imgWidth; col++){
			Node& curNode = nodes[nodeIndex++];

			curNode.column = col;
			curNode.row = row;
			curNode.prevNode = NULL;
		} 
	}

	//Calculate MaxD and d-links for each pixel for each kernel
	double **dlink = new double*[8];
	double maxDlink = 0;

	for (int i = 0; i < 8; i++)
	{
		double *rsltImg = new double[imgWidth * imgHeight * 3];
		image_filter(rsltImg, img, NULL, imgWidth, imgHeight, kernels[i], 3, 3, 1./2., 0.);

		// Matrix[i][j] where i = link and j = node
		dlink[i] = new double[imgWidth * imgHeight];
		for (int row = 0; row < imgHeight; row++)
		{
			for (int col = 0; col < imgWidth; col++)
			{
				double sum = pow(rsltImg[3*(row*imgWidth+col)+0],2) + pow(rsltImg[3*(row*imgWidth+col)+1],2) +
								pow(rsltImg[3*(row*imgWidth+col)+2],2);
				double magnitude = sqrt((sum/3.));

				dlink[i][row*imgWidth+col] = magnitude;
				maxDlink = (maxDlink > magnitude) ? maxDlink : magnitude;
			}
		}
		delete [] rsltImg;
	}

	for (nodeIndex = 0; nodeIndex < imgWidth * imgHeight; nodeIndex++)
	{
		Node& curNode = nodes[nodeIndex];
		
		for (int linkIndex = 0; linkIndex < 8; linkIndex++)
		{
			double deriv = dlink[linkIndex][nodeIndex];

			// Relies on odd-numbered links being diagonal
			double cost = (maxDlink - deriv) * ((linkIndex % 2 == 0) ? 1 : SQRT2);
			curNode.linkCost[linkIndex] = cost;
		}
	}

	for (int i = 0; i < 8; i++)
	{
		delete [] dlink[i];
	}
	delete [] dlink;
}
/************************ END OF TBD 1 ***************************/

/************************ TO BE DONE 4 ***************************
 *LiveWireDP:
 *	INPUT:
 *		seedX, seedY:	seed position in nodes
 *		nodes:			node buffer of size width by height;
 *      width, height:  dimensions of the node buffer;
 *		selection:		if selection != NULL, search path only in the subset of nodes[j*width+i] if selection[j*width+i] = 1;
 *						otherwise, search in the whole set of nodes. 
 *		numExpanded:		compute the only the first numExpanded number of nodes; (for debugging)
 *	OUTPUT:
 *		computes the minimum path tree from the seed node, by assigning 
 *		the prevNode field of each node to its predecessor along the minimum 
 *		cost path from the seed to that node.
 */

void LiveWireDP(int seedX, int seedY, Node *nodes, int width, int height, const unsigned char *selection, int numExpanded)
{
	CTypedPtrHeap<Node> priorityQueue;
	Node *curNode = nodes + seedY*width + seedX;
	int expandedCount = 0;

	//Initialize all nodes
	for (int i = 0; i < width * height; i++)
	{
		nodes[i].state = INITIAL;
	}

	curNode->totalCost = 0.;
	curNode->prevNode = NULL;

	priorityQueue.Insert(curNode);
	while (!priorityQueue.IsEmpty())
	{
		// remove selected node from the running
		// and expand it
		curNode = priorityQueue.ExtractMin();
		curNode->state = EXPANDED;

		if (++expandedCount == numExpanded)
		{
			return;
		}

		//iterate across neighbors
		for (int linkIndex = 0; linkIndex < 8; linkIndex++)
		{
			int offsetX, offsetY;
			curNode->nbrNodeOffset(offsetX, offsetY, linkIndex);
			int xPosition = curNode->column + offsetX;
			int yPosition = curNode->row + offsetY;

			if (xPosition < 0 || xPosition >= width || yPosition < 0 || yPosition >= height)
			{
				continue;
			}
			if (selection == NULL || selection[yPosition*width+xPosition] == 1)
			{
				Node *tempNode = &nodes[yPosition*width+xPosition];
				if (tempNode->state == INITIAL)
				{
					tempNode->totalCost = curNode->totalCost + curNode->linkCost[linkIndex];
					tempNode->state = ACTIVE;
					tempNode->prevNode = curNode;
					priorityQueue.Insert(tempNode);
				}
				else if(tempNode->state == ACTIVE)
				{
					if (curNode->totalCost + curNode->linkCost[linkIndex] < tempNode->totalCost)
					{
						tempNode->prevNode = curNode;
						priorityQueue.Update(tempNode);
					}
				}
				else
				{
					continue;
				}
			}
		}
	}
}
/************************ END OF TBD 4 ***************************/

/************************ TO BE DONE 5 ***************************
 *MinimumPath:
 *	INPUT:
 *		nodes:				a node buffer of size width by height;
 *		width, height:		dimensions of the node buffer;
 *		freePtX, freePtY:	an input node position;
 *	OUTPUT:
 *		insert a list of nodes along the minimum cost path from the seed node to the input node.  
 *		Notice that the seed node in the buffer has a NULL predecessor.
 *		And you want to insert a *pointer* to the Node in the path, e.g., 
 *		insert nodes+j*width+i (or &(nodes[j*width+i])) if you want to insert node at (i,j), instead of nodes[nodes+j*width+i]!!!
 *		after the procedure, the seed should be the head of path and the input code should be the tail
 */

void MinimumPath(CTypedPtrDblList <Node> *path, int freePtX, int freePtY, Node *nodes, int width, int height)
{
	Node *curNode = nodes + freePtY*width + freePtX;

	if (curNode == NULL)
	{
		return;
	}
	
	CTypedPtrDblElement<Node> *prevDblElement = (path->AddHead(curNode));
	
	while (curNode->prevNode != NULL)
	{
		curNode = curNode->prevNode;
		prevDblElement = path->AddNext(prevDblElement, curNode);
	}

}
/************************ END OF TBD 5 ***************************/

/************************ An Extra Credit Item *******************
 *SeeSnap:
 *	INPUT:
 *		img:				a RGB image buffer of size width by height;
 *		width, height:		dimensions of the image buffer;
 *		x,y:				an input seed position;
 *	OUTPUT:
 *		update the value of x,y to the closest edge based on local image information.
 */

void SeedSnap(int &x, int &y, unsigned char *img, int width, int height)
{
	printf("SeedSnap in iScissor.cpp: to be implemented for extra credit!\n");
}

//generate a cost graph from original image and node buffer with all the link costs;
void MakeCostGraph(unsigned char *costGraph, const Node *nodes, const unsigned char *img, int imgWidth, int imgHeight)
{
	int graphWidth = imgWidth * 3;
	int graphHeight = imgHeight * 3;
	int dgX = 3;
	int dgY = 3*graphWidth;

	int i,j;
	for (j=0;j<imgHeight;j++)
	{
		for (i=0;i<imgWidth;i++)
		{
			int nodeIndex = j*imgWidth+i;
			int imgIndex = 3*nodeIndex;
			int costIndex = 3*((3*j+1)*graphWidth+(3*i+1));
			
			const Node *node = nodes+nodeIndex;
			const unsigned char *pxl = img + imgIndex;
			unsigned char *cst = costGraph + costIndex;

			cst[0] = pxl[0];
			cst[1] = pxl[1];
			cst[2] = pxl[2];
			
			//r,g,b channels are grad info in seperate channels;				
			DigitizeCost(cst	   + dgX, node->linkCost[0]);
			DigitizeCost(cst - dgY + dgX, node->linkCost[1]);
			DigitizeCost(cst - dgY      , node->linkCost[2]);
			DigitizeCost(cst - dgY - dgX, node->linkCost[3]);
			DigitizeCost(cst	   - dgX, node->linkCost[4]);
			DigitizeCost(cst + dgY - dgX, node->linkCost[5]);
			DigitizeCost(cst + dgY	   ,  node->linkCost[6]);
			DigitizeCost(cst + dgY + dgX, node->linkCost[7]);
		}
	}
}
