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

#ifdef WIN32
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <cvblob.h>

int main()
{
  IplImage *img = cvLoadImage("test.png", 1);

  cvThreshold(img, img, 100, 200, CV_THRESH_BINARY);

  IplImage *chB=cvCreateImage(cvGetSize(img),8,1);

  cvSplit(img,chB,NULL,NULL,NULL);

  //std::clog << " --> " << img->depth << ", --> " << img->nChannels << std::endl;

  IplImage *labelImg = cvCreateImage(cvGetSize(img),IPL_DEPTH_LABEL,1);

  CvBlobs blobs;
  unsigned int result = cvLabel(chB, labelImg, blobs);

  for (CvBlobs::iterator it=blobs.begin();it!=blobs.end();++it)
  {
    CvBlob *blob=(*it).second;
    cvCentralMoments(blob,labelImg);

    cvRectangle(img,cvPoint(blob->minx,blob->miny),cvPoint(blob->maxx,blob->maxy),CV_RGB(255.,0.,0.));

    cvLine(img,cvPoint(int(blob->centroid.x)-3,int(blob->centroid.y)),cvPoint(int(blob->centroid.x)+3,int(blob->centroid.y)),CV_RGB(0.,0.,255.));
    cvLine(img,cvPoint(int(blob->centroid.x),int(blob->centroid.y)-3),cvPoint(int(blob->centroid.x),int(blob->centroid.y)+3),CV_RGB(0.,0.,255.));

    double angle = cvAngle(blob);

    double x1,y1,x2,y2;

    x1=blob->centroid.x-.005*blob->area*cos(angle);
    y1=blob->centroid.y-.005*blob->area*sin(angle);
    x2=blob->centroid.x+.005*blob->area*cos(angle);
    y2=blob->centroid.y+.005*blob->area*sin(angle);
    cvLine(img,cvPoint(int(x1),int(y1)),cvPoint(int(x2),int(y2)),CV_RGB(0.,255.,0.));
  }

  cvNamedWindow("test", 1);
  cvShowImage("test", img);
  cvWaitKey(0);
  cvDestroyWindow("test");

  cvReleaseImage(&chB);
  cvReleaseImage(&labelImg);
  cvReleaseImage(&img);

  return 0;
}
