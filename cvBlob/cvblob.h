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

#ifndef CVBLOB_H
#define CVBLOB_H

#include <iostream>
#include <map>

#ifdef WIN32
#include <cv.h>
#else
#include <opencv/cv.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


  typedef unsigned int CvLabel;
  //typedef unsigned char CvLabel;

//#define IPL_DEPTH_LABEL IPL_DEPTH_32F
//#define IPL_DEPTH_LABEL IPL_DEPTH_8U
#define IPL_DEPTH_LABEL (sizeof(CvLabel)*8)
  
  ///
  /// Struct that contain information about one blob.
  struct CvBlob
  {
    CvLabel label; ///< Blob's label.
    
    union
    {
      unsigned int area; ///< Area.
      unsigned int m00; ///< Moment 00.
    };
    
    unsigned int minx; ///< X min.
    unsigned int maxx; ///< X max.
    unsigned int miny; ///< Y min.
    unsigned int maxy; ///< y max.
    
    CvPoint2D64f centroid; ///< Centroid.
    
    double m10; ///< Moment 10.
    double m01; ///< Moment 01.
    double m11; ///< Moment 11.
    double m20; ///< Moment 20.
    double m02; ///< Moment 02.
    
    bool centralMoments;
    double u11; ///< Central moment 11.
    double u20; ///< Central moment 20.
    double u02; ///< Central moment 02.
    
    /// Parent of the union-find data estructure.
    CvBlob *_parent;
    /// Rank of the union-find data estructure.
    unsigned int _rank;
  };
  
  typedef std::map<CvLabel,CvBlob *> CvBlobs;
  typedef std::pair<CvLabel,CvBlob *> CvLabelBlob;
  
  unsigned int cvLabel (IplImage *img, IplImage *imgOut,CvBlobs &blobs);
  //IplImage *cvFilterLabel(IplImage *imgIn, CvLabel label);
  void cvFilterLabels(IplImage *imgIn, IplImage *imgOut, CvBlobs blobs);

  CvLabel cvGreaterBlob(CvBlobs blobs);
  void cvFilterByArea(CvBlobs &blobs,unsigned int minArea,unsigned int maxArea);
  CvPoint2D64f cvCentroid(CvBlob *blob);
  void cvCentralMoments(CvBlob *blob, const IplImage *img);
  double cvAngle(CvBlob *blob);
  
#define CV_BLOB_RENDER_COLOR            0x0001 // Render each blog with a different color.
#define CV_BLOB_RENDER_CENTROID         0x0002 // Render bounding box.
#define CV_BLOB_RENDER_BOUNDING_BOX     0x0004 // Render bounding box.
#define CV_BLOB_RENDER_ANGLE            0x0008 // Render bounding box.
  void cvRenderBlobs(const IplImage *imgLabel, CvBlobs blobs, IplImage *imgSource, IplImage *imgDest, unsigned short mode=0x00ff, double alpha=1.);
  
#ifdef __cplusplus
}
#endif

#endif
