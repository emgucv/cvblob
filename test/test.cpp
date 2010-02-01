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
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <cvblob.h>
using namespace cvb;

int main()
{
  IplImage *img = cvLoadImage("test.png", 1);

  cvThreshold(img, img, 100, 200, CV_THRESH_BINARY);

  cvSetImageROI(img, cvRect(100, 100, 800, 500));

  IplImage *chB=cvCreateImage(cvGetSize(img),8,1);

  cvSplit(img,chB,NULL,NULL,NULL);

  IplImage *labelImg = cvCreateImage(cvGetSize(chB),IPL_DEPTH_LABEL,1);

  CvBlobs blobs;
  unsigned int result = cvLabel(chB, labelImg, blobs);

  cvRenderBlobs(labelImg, blobs, img, img);

  for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
  {
    CvContourChainCode *contour = cvGetContour(it->second, labelImg);
    cvRenderContourChainCode(contour, img);

    CvContourPolygon *polygon = cvConvertChainCodesToPolygon(contour);

    CvContourPolygon *sPolygon = cvSimplifyPolygon(polygon, 10.);
    CvContourPolygon *cPolygon = cvPolygonContourConvexHull(sPolygon);

    cvRenderContourPolygon(sPolygon, img, CV_RGB(0, 0, 255));
    cvRenderContourPolygon(cPolygon, img, CV_RGB(0, 255, 0));

    delete cPolygon;
    delete sPolygon;
    delete polygon;
    delete contour;
  }

  cvNamedWindow("test", 1);
  cvShowImage("test", img);
  cvWaitKey(0);
  cvDestroyWindow("test");

  cvReleaseImage(&chB);
  cvReleaseImage(&labelImg);
  cvReleaseImage(&img);

  cvReleaseBlobs(blobs);

  return 0;
}
