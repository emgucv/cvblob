/***************************************************************************
 *   Copyright (C) 2007 by Cristóbal Carnero Liñán                         *
 *   grendel.ccl@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cmath>
#include <iostream>
using namespace std;

#ifdef WIN32
#include <cv.h>
#else
#include <opencv/cv.h>
#endif

#include "cvblob.h"

CvLabel cvGreaterBlob(CvBlobs blobs)
{
  CvLabel label=0;
  unsigned int maxArea=0;
  
  for (CvBlobs::iterator it=blobs.begin();it!=blobs.end();++it)
  {
    CvBlob *blob=(*it).second;
    //if ((!blob->_parent)&&(blob->area>maxArea))
    if (blob->area>maxArea)
    {
      label=blob->label;
      maxArea=blob->area;
    }
  }
  
  return label;
}

void cvFilterByArea(CvBlobs &blobs,unsigned int minArea,unsigned int maxArea)
{
  CvBlobs::iterator it=blobs.begin();
  while(it!=blobs.end())
  {
    CvBlob *blob=(*it).second;
    if ((blob->area<minArea)||(blob->area>maxArea))
    {
      delete blob;
      CvBlobs::iterator tmp=it;
      ++it;
      blobs.erase(tmp);
    }
    else
      ++it;
  }
}

CvPoint2D64f cvCentroid(CvBlob *blob)
{
  return blob->centroid=cvPoint2D64f(blob->m10/blob->area,blob->m01/blob->area);
}

void cvCentralMoments(CvBlob *blob,IplImage *img)
{
  if((img->depth!=IPL_DEPTH_LABEL)||(img->nChannels!=1))
  {
    cerr<<"Error: Image format."<<endl;
    return; /// TODO: Errores.
  }
  
  //cvCentroid(blob); // Here?
  
  blob->u11=blob->u20=blob->u02=0.;
  
  // Only in the bounding box
  CvLabel *imgData=(CvLabel *)img->imageData+img->width*blob->miny;
  for (int r=blob->miny;
       r<blob->maxy;
       r++,imgData+=img->width)
    for (int c=blob->minx;c<blob->maxx;c++)
      if (imgData[c]==blob->label)
      {
        double tx=(c-blob->centroid.x);
        double ty=(r-blob->centroid.y);
        blob->u11+=tx*ty;
        blob->u20+=tx*tx;
        blob->u02+=ty*ty;
      }
}

// Returns radians
double cvAngle(CvBlob *blob)
{
  return .5*atan2(2.*blob->u11,(blob->u20-blob->u02));
}
