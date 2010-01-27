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

#include <stdexcept>
#include <iostream>
using namespace std;

#ifdef WIN32
#include <cv.h>
#else
#include <opencv/cv.h>
#endif

#include "cvblob.h"

#define _MIN_(a,b) ((a)<(b)?(a):(b))
#define _MAX_(a,b) ((a)>(b)?(a):(b))

//#define NULL 0L

///////////////////////////////////////////////////////////////////////////////////////////////////
// Based on http://en.wikipedia.org/wiki/Disjoint-set_data_structure
void makeSet(CvBlob *x)
{
  x->_parent=NULL;
  x->_rank=0;
}

CvBlob *find(CvBlob *x)
{
  if (!x->_parent) return x;
  else
  {
    x->_parent=find(x->_parent);
    return x->_parent;
  }
}

void merge(CvBlob *x, CvBlob *y)
{
  CvBlob *xRoot=find(x);
  CvBlob *yRoot=find(y);
  
  if (xRoot->_rank > yRoot->_rank)
    yRoot->_parent=xRoot;
  else if (xRoot->_rank < yRoot->_rank)
    xRoot->_parent=yRoot;
  else if (xRoot!=yRoot)
  {
    yRoot->_parent=xRoot;
    xRoot->_rank+=1;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int cvLabel (IplImage *img, IplImage *imgOut, CvBlobs &blobs)
{
  CV_FUNCNAME("cvLabel");
  __CV_BEGIN__;
  {
    CV_ASSERT(img&&(img->depth==IPL_DEPTH_8U)&&(img->nChannels==1));
    CV_ASSERT(imgOut&&(imgOut->depth==IPL_DEPTH_LABEL)&&(img->nChannels==1));

    int numPixels=0;

    //IplImage *imgOut=cvCreateImage (cvGetSize(img),IPL_DEPTH_LABEL,1);
    cvSetZero(imgOut);

    CvLabel label=0;
    cvReleaseBlobs(blobs);

    int stepIn = img->widthStep / (img->depth / 8);
    int stepOut = imgOut->widthStep / (imgOut->depth / 8);
    int imgIn_width = img->width;
    int imgIn_height = img->height;
    int imgIn_offset = 0;
    int imgOut_width = imgOut->width;
    int imgOut_height = imgOut->height;
    int imgOut_offset = 0;
    if(img->roi)
    {
      imgIn_width = img->roi->width;
      imgIn_height = img->roi->height;
      imgIn_offset = img->roi->xOffset + (img->roi->yOffset * stepIn);
    }
    if(imgOut->roi)
    {
      imgOut_width = imgOut->roi->width;
      imgOut_height = imgOut->roi->height;
      imgOut_offset = imgOut->roi->xOffset + (imgOut->roi->yOffset * stepOut);
    }

    char *imgDataIn = img->imageData + imgIn_offset;
    CvLabel *imgDataOut = (CvLabel *)imgOut->imageData + imgOut_offset;

    // Check first pixel (0, 0)
    if (imgDataIn[0])
    {
      label++;

      CvBlob *blob=new CvBlob;
      makeSet(blob);
      blob->label=label;
      blob->area=1;
      blob->minx=0; blob->maxx=0;
      blob->miny=0; blob->maxy=0;
      blob->m10=0; blob->m01=0;
      blob->m11=0*0;
      blob->m20=0*0; blob->m02=0*0;
      blob->centralMoments=false;
      blobs.insert(CvLabelBlob(label,blob));

      imgDataOut[0]=label;
    }

    // Check first row (c, 0)
    for (unsigned int c=1;c<(unsigned int)imgIn_width;c++)
    {
      if (imgDataIn[c])
      {
	numPixels++;
	if (imgDataOut[c-1])
	{
	  CvBlob *blob=blobs[imgDataOut[c-1]];
	  blob->area+=1;
	  blob->maxx=_MAX_(blob->maxx,c);
	  blob->m10+=c; blob->m01+=0;
	  blob->m11+=c*0;
	  blob->m20+=c*c; blob->m02+=0*0;

	  imgDataOut[c]=imgDataOut[c-1];
	}
	else
	{
	  label++;

	  CvBlob *blob=new CvBlob;
	  makeSet(blob);
	  blob->label=label;
	  blob->area=1;
	  blob->minx=c; blob->maxx=c;
	  blob->miny=0; blob->maxy=0;
	  blob->m10=c; blob->m01=0;
	  blob->m11=c*0;
	  blob->m20=c*c; blob->m02=0*0;
	  blob->centralMoments=false;
	  blobs.insert(CvLabelBlob(label,blob));

	  imgDataOut[c]=label;
	}
      }
    }

    CvLabel *lastRowOut=(CvLabel *)imgOut->imageData + imgOut_offset;

    imgDataIn+=stepIn;
    imgDataOut+=stepOut;

    for (unsigned int r=1;r<(unsigned int)imgIn_height;r++,
	lastRowOut+=stepOut,imgDataIn+=stepIn,imgDataOut+=stepOut)
    {
      if (imgDataIn[0])
      {
	numPixels++;
	if (lastRowOut[0])
	{
	  CvBlob *blob=blobs[lastRowOut[0]];
	  blob->area+=1;
	  blob->maxy=_MAX_(blob->maxy,r);
	  blob->m10+=0; blob->m01+=r;
	  blob->m11+=0*r;
	  blob->m20+=0*0; blob->m02+=r*r;

	  imgDataOut[0]=lastRowOut[0];
	}
	else
	{
	  label++;

	  CvBlob *blob=new CvBlob;
	  makeSet(blob);
	  blob->label=label;
	  blob->area=1;
	  blob->minx=0; blob->maxx=0;
	  blob->miny=r; blob->maxy=r;
	  blob->m10=0; blob->m01=r;
	  blob->m11=0*r;
	  blob->m20=0*0; blob->m02=r*r;
	  blob->centralMoments=false;
	  blobs.insert(CvLabelBlob(label,blob));

	  imgDataOut[0]=label;
	}
      }

      for (unsigned int c=1;c<(unsigned int)imgIn_width;c++)
      {
	if (imgDataIn[c])
	{
	  numPixels++;
	  if (lastRowOut[c])
	  {
	    CvBlob *blob=blobs[lastRowOut[c]];
	    blob->area+=1;
	    blob->maxy=_MAX_(blob->maxy,r);
	    blob->m10+=c; blob->m01+=r;
	    blob->m11+=c*r;
	    blob->m20+=c*c; blob->m02+=r*r;

	    imgDataOut[c]=lastRowOut[c];

	    if ((imgDataOut[c-1])&&(imgDataOut[c]!=imgDataOut[c-1]))
	    {
	      CvBlob *blob1=blobs[imgDataOut[c]];
	      CvBlob *blob2=blobs[imgDataOut[c-1]];

	      merge(blob1,blob2);
	    }
	  }
	  else if (imgDataOut[c-1])
	  {
	    CvBlob *blob=blobs[imgDataOut[c-1]];
	    blob->area+=1;
	    blob->maxx=_MAX_(blob->maxx,c);
	    blob->m10+=c; blob->m01+=r;
	    blob->m11+=c*r;
	    blob->m20+=c*c; blob->m02+=r*r;

	    imgDataOut[c]=imgDataOut[c-1];
	  }
	  else
	  {
	    label++;

	    CvBlob *blob=new CvBlob;
	    makeSet(blob);
	    blob->label=label;
	    blob->area=1;
	    blob->minx=c; blob->maxx=c;
	    blob->miny=r; blob->maxy=r;
	    blob->m10=c; blob->m01=r;
	    blob->m11=c*r;
	    blob->m20=c*c; blob->m02=r*r;
	    blob->centralMoments=false;
	    blobs.insert(CvLabelBlob(label,blob));

	    imgDataOut[c]=label;
	  }
	}
      }
    }

    unsigned int labelSize=blobs.size();
    CvLabel *luLabels=new CvLabel[labelSize+1];
    luLabels[0]=0;

    for (CvBlobs::iterator it=blobs.begin();it!=blobs.end();++it)
    {
      CvBlob *blob1=(*it).second;
      CvBlob *blob2=find(blob1);

      if (blob1!=blob2)
      {
	blob2->area+=blob1->area;
	blob2->minx=_MIN_(blob2->minx,blob1->minx); blob2->maxx=_MAX_(blob2->maxx,blob1->maxx);
	blob2->miny=_MIN_(blob2->miny,blob1->miny); blob2->maxy=_MAX_(blob2->maxy,blob1->maxy);
	blob2->m10+=blob1->m10; blob2->m01+=blob1->m01;
	blob2->m11+=blob1->m11;
	blob2->m20+=blob1->m20; blob2->m02+=blob1->m02;
      }

      luLabels[(*it).first]=blob2->label;
    }

    imgDataOut=(CvLabel *)imgOut->imageData + imgOut_offset;
    for (int r=0;r<imgOut_height;r++,imgDataOut+=stepOut)
      for (int c=0;c<imgOut_width;c++)
	imgDataOut[c]=luLabels[imgDataOut[c]];

    delete [] luLabels;

    // Eliminar los blobs hijos:
    CvBlobs::iterator it=blobs.begin();
    while (it!=blobs.end())
    {
      CvBlob *blob=(*it).second;
      if (blob->_parent)
      {
	delete blob;
	CvBlobs::iterator tmp=it;
	++it;
	blobs.erase(tmp);
      }
      else
      {
	cvCentroid((*it).second); // Here?
	++it;
      }
    }

    return numPixels;

  }
  __CV_END__;
}

unsigned int cvLabel8 (IplImage *img, IplImage *imgOut, CvBlobs &blobs)
{
  CV_FUNCNAME("cvLabel8");
  __CV_BEGIN__;
  {
    CV_ASSERT(img&&(img->depth==IPL_DEPTH_8U)&&(img->nChannels==1));
    CV_ASSERT(imgOut&&(imgOut->depth==IPL_DEPTH_LABEL)&&(img->nChannels==1));

    int numPixels=0;

    //IplImage *imgOut=cvCreateImage (cvGetSize(img),IPL_DEPTH_LABEL,1);
    cvSetZero(imgOut);

    CvLabel label=0;
    cvReleaseBlobs(blobs);

    int stepIn = img->widthStep / (img->depth / 8);
    int stepOut = imgOut->widthStep / (imgOut->depth / 8);
    int imgIn_width = img->width;
    int imgIn_height = img->height;
    int imgIn_offset = 0;
    int imgOut_width = imgOut->width;
    int imgOut_height = imgOut->height;
    int imgOut_offset = 0;
    if(img->roi)
    {
      imgIn_width = img->roi->width;
      imgIn_height = img->roi->height;
      imgIn_offset = img->roi->xOffset + (img->roi->yOffset * stepIn);
    }
    if(imgOut->roi)
    {
      imgOut_width = imgOut->roi->width;
      imgOut_height = imgOut->roi->height;
      imgOut_offset = imgOut->roi->xOffset + (imgOut->roi->yOffset * stepOut);
    }

    char *imgDataIn = img->imageData + imgIn_offset;
    CvLabel *imgDataOut = (CvLabel *)imgOut->imageData + imgOut_offset;

    // Check first pixel (0, 0)
    if (imgDataIn[0])
    {
      label++;

      CvBlob *blob=new CvBlob;
      makeSet(blob);
      blob->label=label;
      blob->area=1;
      blob->minx=0; blob->maxx=0;
      blob->miny=0; blob->maxy=0;
      blob->m10=0; blob->m01=0;
      blob->m11=0*0;
      blob->m20=0*0; blob->m02=0*0;
      blob->centralMoments=false;
      blobs.insert(CvLabelBlob(label,blob));

      imgDataOut[0]=label;
    }

    // Check first row (c, 0)
    for (unsigned int c=1;c<(unsigned int)imgIn_width;c++)
    {
      if (imgDataIn[c])
      {
	numPixels++;
	if (imgDataOut[c-1])
	{
	  CvBlob *blob=blobs[imgDataOut[c-1]];
	  blob->area+=1;
	  blob->maxx=_MAX_(blob->maxx,c);
	  blob->m10+=c; blob->m01+=0;
	  blob->m11+=c*0;
	  blob->m20+=c*c; blob->m02+=0*0;

	  imgDataOut[c]=imgDataOut[c-1];
	}
	else
	{
	  label++;

	  CvBlob *blob=new CvBlob;
	  makeSet(blob);
	  blob->label=label;
	  blob->area=1;
	  blob->minx=c; blob->maxx=c;
	  blob->miny=0; blob->maxy=0;
	  blob->m10=c; blob->m01=0;
	  blob->m11=c*0;
	  blob->m20=c*c; blob->m02=0*0;
	  blob->centralMoments=false;
	  blobs.insert(CvLabelBlob(label,blob));

	  imgDataOut[c]=label;
	}
      }
    }

    CvLabel *lastRowOut=(CvLabel *)imgOut->imageData + imgOut_offset;

    imgDataIn+=stepIn;
    imgDataOut+=stepOut;

    // Check rest of rows (r>0, c)
    for (unsigned int r=1;r<(unsigned int)imgIn_height;r++,
	lastRowOut+=stepOut,imgDataIn+=stepIn,imgDataOut+=stepOut)
    {
      // Check first pixel of the row
      if (imgDataIn[0])
      {
	numPixels++;
	// *.
	// 1.
	if (lastRowOut[0])
	{
	  CvBlob *blob=blobs[lastRowOut[0]];
	  blob->area+=1;
	  blob->maxy=_MAX_(blob->maxy,r);
	  blob->m10+=0; blob->m01+=r;
	  blob->m11+=0*r;
	  blob->m20+=0*0; blob->m02+=r*r;

	  imgDataOut[0]=lastRowOut[0];
	}
	// .*
	// 1.
	else if (lastRowOut[1])
	{
	  CvBlob *blob=blobs[lastRowOut[1]];
	  blob->area+=1;
	  blob->maxy=_MAX_(blob->maxy,r);
	  blob->m10+=0; blob->m01+=r;
	  blob->m11+=0*r;
	  blob->m20+=0*0; blob->m02+=r*r;

	  imgDataOut[0]=lastRowOut[1];
	}
	else
	{
	  label++;

	  CvBlob *blob=new CvBlob;
	  makeSet(blob);
	  blob->label=label;
	  blob->area=1;
	  blob->minx=0; blob->maxx=0;
	  blob->miny=r; blob->maxy=r;
	  blob->m10=0; blob->m01=r;
	  blob->m11=0*r;
	  blob->m20=0*0; blob->m02=r*r;
	  blob->centralMoments=false;
	  blobs.insert(CvLabelBlob(label,blob));

	  imgDataOut[0]=label;
	}
      }

      for (unsigned int c=1;c<(unsigned int)imgIn_width;c++)
      {
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	if (imgDataIn[c])
	{
	  numPixels++;
	  // .*.
	  // .1.
	  if (lastRowOut[c])
	  {
	    CvBlob *blob=blobs[lastRowOut[c]];
	    blob->area+=1;
	    blob->maxy=_MAX_(blob->maxy,r);
	    blob->m10+=c; blob->m01+=r;
	    blob->m11+=c*r;
	    blob->m20+=c*c; blob->m02+=r*r;

	    imgDataOut[c]=lastRowOut[c];

	    // .1.
	    // *1.
	    if ((imgDataOut[c-1])&&(imgDataOut[c]!=imgDataOut[c-1]))
	    {
	      CvBlob *blob1=blobs[imgDataOut[c]];
	      CvBlob *blob2=blobs[imgDataOut[c-1]];

	      merge(blob1,blob2);
	    }
	  }
	  else if (imgDataOut[c-1])
	  {
	    CvBlob *blob=blobs[imgDataOut[c-1]];
	    blob->area+=1;
	    blob->maxx=_MAX_(blob->maxx,c);
	    blob->m10+=c; blob->m01+=r;
	    blob->m11+=c*r;
	    blob->m20+=c*c; blob->m02+=r*r;

	    imgDataOut[c]=imgDataOut[c-1];
	  }
	  else
	  {
	    label++;

	    CvBlob *blob=new CvBlob;
	    makeSet(blob);
	    blob->label=label;
	    blob->area=1;
	    blob->minx=c; blob->maxx=c;
	    blob->miny=r; blob->maxy=r;
	    blob->m10=c; blob->m01=r;
	    blob->m11=c*r;
	    blob->m20=c*c; blob->m02=r*r;
	    blob->centralMoments=false;
	    blobs.insert(CvLabelBlob(label,blob));

	    imgDataOut[c]=label;
	  }
	}
      }
    }

    unsigned int labelSize=blobs.size();
    CvLabel *luLabels=new CvLabel[labelSize+1];
    luLabels[0]=0;

    for (CvBlobs::iterator it=blobs.begin();it!=blobs.end();++it)
    {
      CvBlob *blob1=(*it).second;
      CvBlob *blob2=find(blob1);

      if (blob1!=blob2)
      {
	blob2->area+=blob1->area;
	blob2->minx=_MIN_(blob2->minx,blob1->minx); blob2->maxx=_MAX_(blob2->maxx,blob1->maxx);
	blob2->miny=_MIN_(blob2->miny,blob1->miny); blob2->maxy=_MAX_(blob2->maxy,blob1->maxy);
	blob2->m10+=blob1->m10; blob2->m01+=blob1->m01;
	blob2->m11+=blob1->m11;
	blob2->m20+=blob1->m20; blob2->m02+=blob1->m02;
      }

      luLabels[(*it).first]=blob2->label;
    }

    imgDataOut=(CvLabel *)imgOut->imageData + imgOut_offset;
    for (int r=0;r<imgOut_height;r++,imgDataOut+=stepOut)
      for (int c=0;c<imgOut_width;c++)
	imgDataOut[c]=luLabels[imgDataOut[c]];

    delete [] luLabels;

    // Eliminar los blobs hijos:
    CvBlobs::iterator it=blobs.begin();
    while (it!=blobs.end())
    {
      CvBlob *blob=(*it).second;
      if (blob->_parent)
      {
	delete blob;
	CvBlobs::iterator tmp=it;
	++it;
	blobs.erase(tmp);
      }
      else
      {
	cvCentroid((*it).second); // Here?
	++it;
      }
    }

    return numPixels;

  }
  __CV_END__;
}

void cvFilterLabels(IplImage *imgIn, IplImage *imgOut, const CvBlobs &blobs)
{
  CV_FUNCNAME("cvFilterLabels");
  __CV_BEGIN__;
  {
    CV_ASSERT(imgIn&&(imgIn->depth==IPL_DEPTH_LABEL)&&(imgIn->nChannels==1));
    CV_ASSERT(imgOut&&(imgOut->depth==IPL_DEPTH_8U)&&(imgOut->nChannels==1));

    int stepIn = imgIn->widthStep / (imgIn->depth / 8);
    int stepOut = imgOut->widthStep / (imgOut->depth / 8);
    int imgIn_width = imgIn->width;
    int imgIn_height = imgIn->height;
    int imgIn_offset = 0;
    int imgOut_width = imgOut->width;
    int imgOut_height = imgOut->height;
    int imgOut_offset = 0;
    if(imgIn->roi)
    {
      imgIn_width = imgIn->roi->width;
      imgIn_height = imgIn->roi->height;
      imgIn_offset = imgIn->roi->xOffset + (imgIn->roi->yOffset * stepIn);
    }
    if(imgOut->roi)
    {
      imgOut_width = imgOut->roi->width;
      imgOut_height = imgOut->roi->height;
      imgOut_offset = imgOut->roi->xOffset + (imgOut->roi->yOffset * stepOut);
    }

    char *imgDataOut=imgOut->imageData + imgOut_offset;
    CvLabel *imgDataIn=(CvLabel *)imgIn->imageData + imgIn_offset;

    for (unsigned int r=0;r<(unsigned int)imgIn_height;r++,
	imgDataIn+=stepIn,imgDataOut+=stepOut)
    {
      for (unsigned int c=0;c<(unsigned int)imgIn_width;c++)
      {
	if (imgDataIn[c])
	{
	  if (blobs.find(imgDataIn[c])==blobs.end()) imgDataOut[c]=0x00;
	  else imgDataOut[c]=(char)0xff;
	}
      }
    }
  }
  __CV_END__;
}
