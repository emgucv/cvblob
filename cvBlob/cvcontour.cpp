// Copyright (C) 2007 by Cristóbal Carnero Liñán
// grendel.ccl@gmail.com
//
// This file is part of cvBlob.
//
// cvBlob is free software: you can redistribute it and/or modify
// it under the terms of the Lesser GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cvBlob is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Lesser GNU General Public License for more details.
//
// You should have received a copy of the Lesser GNU General Public License
// along with cvBlob.  If not, see <http://www.gnu.org/licenses/>.
//

#include <iostream>
using namespace std;

#ifdef WIN32
#include <cv.h>
#else
#include <opencv/cv.h>
#endif

#include "cvblob.h"

const char moves[4][3][4] = { { {-1, -1, 3, CV_CHAINCODE_UP_LEFT   }, { 0, -1, 0, CV_CHAINCODE_UP   }, { 1, -1, 0, CV_CHAINCODE_UP_RIGHT  } },
			      { { 1, -1, 0, CV_CHAINCODE_UP_RIGHT  }, { 1,  0, 1, CV_CHAINCODE_RIGHT}, { 1,  1, 1, CV_CHAINCODE_DOWN_RIGHT} },
			      { { 1,  1, 1, CV_CHAINCODE_DOWN_RIGHT}, { 0,  1, 2, CV_CHAINCODE_DOWN }, {-1,  1, 2, CV_CHAINCODE_DOWN_LEFT } },
			      { {-1,  1, 2, CV_CHAINCODE_DOWN_LEFT }, {-1,  0, 3, CV_CHAINCODE_LEFT }, {-1, -1, 3, CV_CHAINCODE_UP_LEFT   } }
		            };

CvContourChainCode *cvGetContour(CvBlob const *blob, IplImage const *img)
{
  CV_FUNCNAME("cvGetContour");
  __BEGIN__;
  {
    CV_ASSERT(img&&(img->depth==IPL_DEPTH_LABEL)&&(img->nChannels==1));

    CvContourChainCode *contour = new CvContourChainCode;

    // Only in the bounding box
    int stepIn = img->widthStep / (img->depth / 8);
    int img_width = img->width;
    int img_height = img->height;
    int img_offset = 0;
    if(img->roi)
    {
      img_width = img->roi->width;
      img_height = img->roi->height;
      img_offset = img->roi->xOffset + (img->roi->yOffset * stepIn);
    }

    CvLabel *imgData=(CvLabel *)img->imageData + (blob->miny * stepIn) + img_offset;

    // The starting point will be the first pixel of the first row
    unsigned int c = blob->minx;
    while (imgData[c]!=blob->label)
      c++;
    contour->startingPoint = cvPoint(c, blob->miny);

    unsigned char direction=1;
    unsigned int x = contour->startingPoint.x;
    unsigned int y = contour->startingPoint.y;

    imgData=(CvLabel *)img->imageData + img_offset;

    do
    {
      for (unsigned int numAttempts=0; numAttempts<3; numAttempts++)
      {
	bool found = false;

	for (unsigned char i=0; i<3; i++)
	{
	  int nx = x+moves[direction][i][0];
	  int ny = y+moves[direction][i][1];
	  if (((nx>=blob->minx)&&(nx<=blob->maxx)&&(ny>=blob->miny)&&(ny<=blob->maxy))&&
	      (imgData[nx+ny*stepIn]==blob->label))
	  {
	    found = true;

	    contour->chainCode.push_back(moves[direction][i][3]);

	    x=nx;
	    y=ny;

	    direction=moves[direction][i][2];
	    break;
	  }
	}

	if (!found)
	  direction=(direction+1)%4;
	else
	  break;
      }
    }
    while (!(x==contour->startingPoint.x && y==contour->startingPoint.y));

    return contour;

  }
  __END__;
}

void cvRenderContourChainCode(CvContourChainCode const *contour, IplImage const *img, CvScalar const &color)
{
  CV_FUNCNAME("cvRenderContourChainCode");
  __BEGIN__;
  {
    CV_ASSERT(img&&(img->depth==IPL_DEPTH_8U)&&(img->nChannels==3));

    int stepDst = img->widthStep/(img->depth/8);
    int img_width = img->width;
    int img_height = img->height;
    int img_offset = 0;

    if(img->roi)
    {
      img_width = img->roi->width;
      img_height = img->roi->height;
      img_offset = (img->nChannels * img->roi->xOffset) + (img->roi->yOffset * stepDst);
    }

    unsigned char *imgData = (unsigned char *)img->imageData + img_offset;

    unsigned int x = contour->startingPoint.x;
    unsigned int y = contour->startingPoint.y;

    for (CvChainCodes::const_iterator it=contour->chainCode.begin(); it!=contour->chainCode.end(); ++it)
    {
      imgData[img->nChannels*x+img->widthStep*y+0] = (unsigned char)(color.val[0]); // Blue
      imgData[img->nChannels*x+img->widthStep*y+1] = (unsigned char)(color.val[1]); // Green
      imgData[img->nChannels*x+img->widthStep*y+2] = (unsigned char)(color.val[2]); // Red

      x += cvChainCodeMoves[*it][0];
      y += cvChainCodeMoves[*it][1];
    }
  }
  __END__;
}

CvContourPolygon *cvConvertChainCodesToPolygon(CvContourChainCode const *cc)
{
  CV_FUNCNAME("cvConvertChainCodesToPolygon");
  __BEGIN__;
  {
    CV_ASSERT(cc);

    CvContourPolygon *contour = new CvContourPolygon;

    unsigned int x = cc->startingPoint.x;
    unsigned int y = cc->startingPoint.y;
    contour->push_back(cvPoint(x, y));

    CvChainCodes::const_iterator it=cc->chainCode.begin();
    CvChainCode lastCode = *it;

    x += cvChainCodeMoves[*it][0];
    y += cvChainCodeMoves[*it][1];

    ++it;

    for (; it!=cc->chainCode.end(); ++it)
    {
      if (lastCode!=*it)
      {
	contour->push_back(cvPoint(x, y));
	lastCode=*it;
      }

      x += cvChainCodeMoves[*it][0];
      y += cvChainCodeMoves[*it][1];
    }

    return contour;
  }
  __END__;
}

void cvRenderContourPolygon(CvContourPolygon const *contour, IplImage *img, CvScalar const &color)
{
  CV_FUNCNAME("cvRenderContourPolygon");
  __BEGIN__;
  {
    CV_ASSERT(img&&(img->depth==IPL_DEPTH_8U)&&(img->nChannels==3));

    CvContourPolygon::const_iterator it=contour->begin();

    if (it!=contour->end())
    {
      unsigned int fx, x, fy, y;
      fx = x = it->x;
      fy = y = it->y;

      for (; it!=contour->end(); ++it)
      {
	cvLine(img, cvPoint(x, y), cvPoint(it->x, it->y), color, 1);
	x = it->x;
	y = it->y;
      }

      cvLine(img, cvPoint(x, y), cvPoint(fx, fy), color, 1);
    }
  }
  __END__;
}
