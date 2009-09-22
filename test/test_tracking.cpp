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
  CvTracks tracks;

  cvNamedWindow("test_tracking", CV_WINDOW_AUTOSIZE);

  for (unsigned int frame=1; frame<=484; frame++)
  {
    char filename[200];
    sprintf(filename, "/home/ccarnero/Proyectos/ComputerVision/videos/CAVIAR/JPEGS/EnterExitCrossingPaths2front_m%04d.png", frame);
    IplImage *img = cvLoadImage(filename, 1);

    cvSetImageROI(img, cvRect(0, 25, 383, 287));

    cvThreshold(img, img, 100, 200, CV_THRESH_BINARY);

    IplImage *chB=cvCreateImage(cvGetSize(img),8,1);
    cvSplit(img,chB,NULL,NULL,NULL);

    IplImage *labelImg = cvCreateImage(cvGetSize(img), IPL_DEPTH_LABEL, 1);

    CvBlobs blobs;
    unsigned int result = cvLabel(chB, labelImg, blobs);

    cvFilterByArea(blobs, 500, 1000);

    cvUpdateTracks(blobs, tracks, 5., 10);
    //cvUpdateTracks(blobs, tracks, 10., 5);

    cvRenderBlobs(labelImg, blobs, img, img, CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_BOUNDING_BOX);
    cvRenderTracks(tracks, img, img, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_TO_LOG);

    cvShowImage("test_tracking", img);
    //cvShowImage("test_tracking", chB);

    cvReleaseBlobs(blobs);

    cvReleaseImage(&chB);
    cvReleaseImage(&labelImg);
    cvReleaseImage(&img);

    if ((cvWaitKey(10)&0xff) == 27)
      break;
  }

  cvDestroyWindow("test_tracking");

  return 0;
}
