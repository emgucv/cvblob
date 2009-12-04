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

#include <deque>
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

double cvContourPolygonArea(CvContourPolygon const *p)
{
  CV_FUNCNAME("cvContourPolygonArea");
  __BEGIN__;
  {
    CV_ASSERT(p!=NULL);
    CV_ASSERT(p->size()>2);

    CvContourPolygon::const_iterator it=p->begin();
    CvPoint lastPoint = p->back();

    double a = 0.;

    for (; it!=p->end(); ++it)
    {
      a += lastPoint.x*it->y - lastPoint.y*it->x;
      lastPoint = *it;
    }

    return a*0.5;
  }
  __END__;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// http://www.topcoder.com/tc?module=Static&d1=tutorials&d2=geometry1

double dot(CvPoint a, CvPoint b, CvPoint c)
{
  double abx = b.x-a.x;
  double aby = b.y-a.y;
  double bcx = c.x-b.x;
  double bcy = c.y-b.y;

  return abx*bcx + aby*bcy;
}

double cross(CvPoint a, CvPoint b, CvPoint c)
{
  double abx = b.x-a.x;
  double aby = b.y-a.y;
  double acx = c.x-a.x;
  double acy = c.y-a.y;

  return abx*acy - aby*acx;
}

double distancePointPoint(CvPoint a, CvPoint b)
{
  double abx = a.x-b.x;
  double aby = a.y-b.y;

  return sqrt(abx*abx + aby*aby);
}

double distanceLinePoint(CvPoint a, CvPoint b, CvPoint c, bool isSegment=true)
{
  if (isSegment)
  {
    double dot1 = dot(a, b, c);
    if (dot1>0) return distancePointPoint(b, c);

    double dot2 = dot(b, a, c);
    if(dot2>0) return distancePointPoint(a, c);
  }

  return abs(cross(a,b,c)/distancePointPoint(a,b));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void simplifyPolygonRecursive(CvContourPolygon const *p, int const i1, int const i2, bool *pnUseFlag, double const delta)
{
  CV_FUNCNAME("cvSimplifyPolygonRecursive");
  __BEGIN__;
  {
    int endIndex = (i2<0)?p->size():i2;

    if (abs(i1-endIndex)<=1)
      return;

    CvPoint firstPoint = (*p)[i1];
    CvPoint lastPoint = (i2<0)?p->front():(*p)[i2];

    double furtherDistance=0.;
    int furtherIndex=0;

    for (int i=i1+1; i<endIndex; i++)
    {
      double d = distanceLinePoint(firstPoint, lastPoint, (*p)[i]);

      if ((d>=delta)&&(d>furtherDistance))
      {
	furtherDistance=d;
	furtherIndex=i;
      }
    }

    if (furtherIndex)
    {
      pnUseFlag[furtherIndex]=true;

      simplifyPolygonRecursive(p, i1, furtherIndex, pnUseFlag, delta);
      simplifyPolygonRecursive(p, furtherIndex, i2, pnUseFlag, delta);
    }
  }
  __END__;
}

CvContourPolygon *cvSimplifyPolygon(CvContourPolygon const *p, double const delta)
{
  CV_FUNCNAME("cvSimplifyPolygon");
  __BEGIN__;
  {
    CV_ASSERT(p!=NULL);
    CV_ASSERT(p->size()>2);

    double furtherDistance=0.;
    unsigned int furtherIndex=0;

    CvContourPolygon::const_iterator it=p->begin();
    ++it;
    for (unsigned int i=1; it!=p->end(); ++it, i++)
    {
      double d = distancePointPoint(*it, p->front());

      if (d>furtherDistance)
      {
	furtherDistance = d;
	furtherIndex = i;
      }
    }

    if (furtherDistance<delta)
    {
      CvContourPolygon *result = new CvContourPolygon;
      result->push_back(p->front());
      return result;
    }

    bool *pnUseFlag = new bool[p->size()];
    for (int i=1; i<p->size(); i++) pnUseFlag[i] = false;

    pnUseFlag[0] = pnUseFlag[furtherIndex] = true;

    simplifyPolygonRecursive(p, 0, furtherIndex, pnUseFlag, delta);
    simplifyPolygonRecursive(p, furtherIndex, -1, pnUseFlag, delta);

    CvContourPolygon *result = new CvContourPolygon;

    for (int i=0; i<p->size(); i++)
      if (pnUseFlag[i])
	result->push_back((*p)[i]);

    delete[] pnUseFlag;

    return result;
  }
  __END__;
}

CvContourPolygon *cvPolygonContourConvexHull(CvContourPolygon const *p)
{
  CV_FUNCNAME("cvPolygonContourConvexHull");
  __BEGIN__;
  {
    CV_ASSERT(p!=NULL);
    CV_ASSERT(p->size()>2);

    deque<CvPoint> dq;

    if (cross((*p)[0], (*p)[1], (*p)[2])>0)
    {
      dq.push_back((*p)[0]);
      dq.push_back((*p)[1]);
    }
    else
    {
      dq.push_back((*p)[1]);
      dq.push_back((*p)[0]);
    }

    dq.push_back((*p)[2]);
    dq.push_front((*p)[2]);

    for (int i=3; i<p->size()-1; i++)
    {
      int s = dq.size();

      while (cross((*p)[i], dq.at(0), dq.at(1))>=0 && cross(dq.at(s-2), dq.at(s-1), (*p)[i])>=0)
      {
	i++;
	if (i>=p->size())
	  return new CvContourPolygon;
      }

      while (cross(dq.at(s-2), dq.at(s-1), (*p)[i])<0)
      {
	dq.pop_back();
	s = dq.size();
      }

      dq.push_back((*p)[i]);

      while (cross((*p)[i], dq.at(0), dq.at(1))<0)
	dq.pop_front();

      dq.push_front((*p)[i]);
    }

    return new CvContourPolygon;
  }
  __END__;
}
