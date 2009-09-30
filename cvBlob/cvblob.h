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

/// \file cvblob.h
/// \brief OpenCV Blob header file.

#ifndef CVBLOB_H
#define CVBLOB_H

#include <iostream>
#include <map>
#include <list>

#ifdef WIN32
#include <cv.h>
#else
#include <opencv/cv.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


  /// \brief Type of label.
  /// \see IPL_DEPTH_LABEL
  typedef unsigned int CvLabel;
  //typedef unsigned char CvLabel;

  /// \def IPL_DEPTH_LABEL
  /// \brief Size of a label in bits.
  /// \see CvLabel
#define IPL_DEPTH_LABEL (sizeof(CvLabel)*8)
  
  /// \brief Type of identification numbers.
  typedef unsigned int CvID;

  /// \brief Struct that contain information about one blob.
  struct CvBlob
  {
    CvLabel label; ///< Label assigned to the blob.
    
    union
    {
      unsigned int area; ///< Area (moment 00).
      unsigned int m00; ///< Moment 00 (area).
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
    
    bool centralMoments; ///< True if central moments are being calculated.
    double u11; ///< Central moment 11.
    double u20; ///< Central moment 20.
    double u02; ///< Central moment 02.
    
    /// Parent of the union-find data estructure.
    CvBlob *_parent;
    /// Rank of the union-find data estructure.
    unsigned int _rank;
  };
  
  /// \var typedef std::map<CvLabel,CvBlob *> CvBlobs
  /// \brief List of blobs.
  /// A map is used to access each blob from its label number.
  /// \see CvLabel
  /// \see CvBlob
  typedef std::map<CvLabel,CvBlob *> CvBlobs;

  /// \var typedef std::pair<CvLabel,CvBlob *> CvLabelBlob
  /// \brief Pair (label, blob).
  /// \see CvLabel
  /// \see CvBlob
  typedef std::pair<CvLabel,CvBlob *> CvLabelBlob;
  
  /// \fn unsigned int cvLabel (IplImage *img, IplImage *imgOut, CvBlobs &blobs);
  /// \brief Label the connected parts of a binary image.
  /// \param img Input binary image (depth=IPL_DEPTH_8U and num. channels=1).
  /// \param imgOut Output image (depth=IPL_DEPTH_LABEL and num. channels=1).
  /// \param blobs List of blobs.
  /// \return Number of pixels that has been labeled.
  unsigned int cvLabel (IplImage *img, IplImage *imgOut, CvBlobs &blobs);

  //IplImage *cvFilterLabel(IplImage *imgIn, CvLabel label);

  /// \fn void cvFilterLabels(IplImage *imgIn, IplImage *imgOut, const CvBlobs &blobs)
  /// \brief Draw a binary image with the blobs that have been given.
  /// \param imgIn Input image (depth=IPL_DEPTH_LABEL and num. channels=1).
  /// \param imgOut Output binary image (depth=IPL_DEPTH_8U and num. channels=1).
  /// \param blobs List of blobs to be drawn.
  /// \see cvLabel
  void cvFilterLabels(IplImage *imgIn, IplImage *imgOut, const CvBlobs &blobs);

  /// \fn inline void cvReleaseBlobs(CvBlobs &blobs)
  /// \brief Clear blobs structure.
  /// \param blobs List of blobs.
  /// \see CvBlobs
  inline void cvReleaseBlobs(CvBlobs &blobs)
  {
    for (CvBlobs::iterator it=blobs.begin(); it!=blobs.end(); it++)
      delete (*it).second;

    blobs.clear();
  }

  /// \fn CvLabel cvGreaterBlob(const CvBlobs &blobs)
  /// \brief Find greater blob.
  /// \param blobs List of blobs.
  /// \return Label of greater blob.
  /// \see cvLabel
  CvLabel cvGreaterBlob(const CvBlobs &blobs);

  /// \fn void cvFilterByArea(CvBlobs &blobs, unsigned int minArea, unsigned int maxArea)
  /// \brief Filter blobs by area.
  /// Those blobs whose areas are not in range will be erased from the input list of blobs.
  /// \param blobs List of blobs.
  /// \param minArea Minimun area.
  /// \param maxArea Maximun area.
  void cvFilterByArea(CvBlobs &blobs, unsigned int minArea, unsigned int maxArea);

  /// \fn inline CvPoint2D64f cvCentroid(CvBlob *blob)
  /// \brief Calculates centroid.
  /// Centroid will be returned and stored in the blob structure.
  /// \param blob Blob whose centroid will be calculated.
  /// \return Centroid.
  /// \see CvBlob
  inline CvPoint2D64f cvCentroid(CvBlob *blob)
  {
    return blob->centroid=cvPoint2D64f(blob->m10/blob->area, blob->m01/blob->area);
  }

  /// \fn void cvCentralMoments(CvBlob *blob, const IplImage *img)
  /// \brief Calculates central moment for a blob.
  /// Central moments will be stored in blob structure.
  /// \param blob Blob.
  /// \param img Label image (depth=IPL_DEPTH_LABEL and num. channels=1).
  /// \see CvBlob
  /// \see cvLabel
  void cvCentralMoments(CvBlob *blob, const IplImage *img);

  /// \fn double cvAngle(CvBlob *blob)
  /// \brief Calculates angle orientation of a blob.
  /// This function uses central moments so cvCentralMoments should have been called before for this blob.
  /// \param blob Blob.
  /// \return Angle orientation in radians.
  /// \see cvCentralMoments
  /// \see CvBlob
  double cvAngle(CvBlob *blob);
  
#define CV_BLOB_RENDER_COLOR            0x0001 ///< Render each blog with a different color. \see cvRenderBlobs
#define CV_BLOB_RENDER_CENTROID         0x0002 ///< Render centroid. \see cvRenderBlobs
#define CV_BLOB_RENDER_BOUNDING_BOX     0x0004 ///< Render bounding box. \see cvRenderBlobs
#define CV_BLOB_RENDER_ANGLE            0x0008 ///< Render angle. \see cvRenderBlobs
#define CV_BLOB_RENDER_TO_LOG           0x0010 ///< Print blob data to log out. \see cvRenderBlobs
#define CV_BLOB_RENDER_TO_STD           0x0020 ///< Print blob data to std out. \see cvRenderBlobs

  /// \fn void cvRenderBlobs(const IplImage *imgLabel, const CvBlobs &blobs, IplImage *imgSource, IplImage *imgDest, unsigned short mode=0x000f, double alpha=1.)
  /// \brief Draws or prints information about blobs.
  /// \param imgLabel Label image (depth=IPL_DEPTH_LABEL and num. channels=1).
  /// \param blobs List of blobs.
  /// \param imgSource Input image (depth=IPL_DEPTH_8U and num. channels=3).
  /// \param imgDest Output image (depth=IPL_DEPTH_8U and num. channels=3).
  /// \param mode Render mode. By default is CV_BLOB_RENDER_COLOR|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_ANGLE.
  /// \param alpha If mode CV_BLOB_RENDER_COLOR is used. 1.0 indicates opaque and 0.0 translucent (1.0 by default).
  /// \see CV_BLOB_RENDER_COLOR
  /// \see CV_BLOB_RENDER_CENTROID
  /// \see CV_BLOB_RENDER_BOUNDING_BOX
  /// \see CV_BLOB_RENDER_ANGLE
  /// \see CV_BLOB_RENDER_TO_LOG
  /// \see CV_BLOB_RENDER_TO_STD
  void cvRenderBlobs(const IplImage *imgLabel, const CvBlobs &blobs, IplImage *imgSource, IplImage *imgDest, unsigned short mode=0x000f, double alpha=1.);

  /// \fn void cvSetImageROItoBlob(IplImage *img, CvBlob const *blob)
  /// \brief Set the ROI of an image to the bounding box of a blob.
  /// \param img Image.
  /// \param blob Blob.
  /// \see CvBlob
  inline void cvSetImageROItoBlob(IplImage *img, CvBlob const *blob)
  {
    cvSetImageROI(img, cvRect(blob->minx, blob->miny, blob->maxx-blob->minx, blob->maxy-blob->miny));
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Contours
  
  // Chain code:
  //        7 0 1
  //        6   2
  //        5 4 3
#define CV_CHAINCODE_UP		0 ///< Up.
#define CV_CHAINCODE_UP_RIGHT	1 ///< Up and right.
#define CV_CHAINCODE_RIGHT	2 ///< Right.
#define CV_CHAINCODE_DOWN_RIGHT	3 ///< Down and right.
#define CV_CHAINCODE_DOWN	4 ///< Down.
#define CV_CHAINCODE_DOWN_LEFT	5 ///< Down and left.
#define CV_CHAINCODE_LEFT	6 ///< Left.
#define CV_CHAINCODE_UP_LEFT	7 ///< Up and left.

  /// /brief Move vectors of chain codes.
  /// /see CV_CHAINCODE_UP
  /// /see CV_CHAINCODE_UP_LEFT
  /// /see CV_CHAINCODE_LEFT
  /// /see CV_CHAINCODE_DOWN_LEFT
  /// /see CV_CHAINCODE_DOWN
  /// /see CV_CHAINCODE_DOWN_RIGHT
  /// /see CV_CHAINCODE_RIGHT
  /// /see CV_CHAINCODE_UP_RIGHT
  const char cvChainCodeMoves[8][2] = { { 0, -1},
                                        { 1, -1},
					{ 1,  0},
					{ 1,  1},
					{ 0,  1},
					{-1,  1},
					{-1,  0},
					{-1, -1}
                                      };

  /// /brief Direction.
  /// /see CV_CHAINCODE_UP
  /// /see CV_CHAINCODE_UP_LEFT
  /// /see CV_CHAINCODE_LEFT
  /// /see CV_CHAINCODE_DOWN_LEFT
  /// /see CV_CHAINCODE_DOWN
  /// /see CV_CHAINCODE_DOWN_RIGHT
  /// /see CV_CHAINCODE_RIGHT
  /// /see CV_CHAINCODE_UP_RIGHT
  typedef unsigned char CvChainCode;

  /// /brief Chain code.
  /// /see CvChainCode
  typedef std::list<CvChainCode> CvChainCodes;

  /// /brief Chain code contour.
  /// /see CvChainCodes
  struct CvContourChainCode
  {
    CvPoint startingPoint;
    CvChainCodes chainCode;
  };

  /// /brief Polygon based contour.
  typedef std::list<CvPoint> CvContourPolygon;

  /// /var CvContourChainCode *cvGetContour(CvBlob const *blob, IplImage const *img)
  /// /brief Get the contour of a blob.
  /// Uses Theo Pavlidis' algorithm (see http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/theo.html ).
  /// /param blob Blob.
  /// /param img Label image.
  /// /return Chain code contour.
  /// /see CvContourChainCode
  /// /see CvBlob
  CvContourChainCode *cvGetContour(CvBlob const *blob, IplImage const *img);

  /// /var void cvRenderContourChainCode(CvContourChainCode const *contour, IplImage const *img, CvScalar const &color=CV_RGB(255, 255, 255))
  /// /brief Draw a contour.
  /// /param contour Chain code contour.
  /// /param img Image to draw on.
  /// /param color Color to draw (default, white).
  /// /see CvContourChainCode
  void cvRenderContourChainCode(CvContourChainCode const *contour, IplImage const *img, CvScalar const &color=CV_RGB(255, 255, 255));
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Tracking

  /// \brief Struct that contain information about one track.
  /// \see CvID
  /// \see CvLabel
  struct CvTrack
  {
    CvID id; ///< Track identification number.

    CvLabel label; ///< Label assigned to the blob related to this track.
    
    unsigned int minx; ///< X min.
    unsigned int maxx; ///< X max.
    unsigned int miny; ///< Y min.
    unsigned int maxy; ///< y max.
    
    CvPoint2D64f centroid; ///< Centroid.

    unsigned int inactive; ///< Indicates number of frames that has been missing.
  };

  /// \var typedef std:list<CvTrack *> CvTracks
  /// \brief List of tracks.
  /// \see CvID
  /// \see CvTrack
  typedef std::map<CvID, CvTrack *> CvTracks;

  /// \var typedef std::pair<CvID, CvTrack *> CvIDTracks
  /// \brief Pair (identification number, track).
  /// \see CvID
  /// \see CvTrack
  typedef std::pair<CvID, CvTrack *> CvIDTracks;

  /// \fn cvUpdateTracks(CvBlobs &b, CvTracks &t, const double thDistance, const unsigned int thInactive)
  /// \brief Updates list of tracks based on current blobs.
  /// Tracking based on:
  /// A. Senior, A. Hampapur, Y-L Tian, L. Brown, S. Pankanti, R. Bolle. Appearance Models for
  /// Occlusion Handling. Second International workshop on Performance Evaluation of Tracking and
  /// Surveillance Systems & CVPR'01. December, 2001.
  /// (http://www.research.ibm.com/peoplevision/PETS2001.pdf)
  /// \param b List of blobs.
  /// \param t List of tracks.
  /// \param thDistance Max distance to determine when a track and a blob match.
  /// \param thInactive Max number of frames a track can be inactive.
  /// \see CvBlobs
  /// \see Tracks
  void cvUpdateTracks(CvBlobs &b, CvTracks &t, const double thDistance, const unsigned int thInactive);

#define CV_TRACK_RENDER_ID            0x0001 ///< Print the ID of each track in the image. \see cvRenderTracks
#define CV_TRACK_RENDER_TO_LOG        0x0010 ///< Print track info to log out. \see cvRenderTracks
#define CV_TRACK_RENDER_TO_STD        0x0020 ///< Print track info to log out. \see cvRenderTracks

  /// \fn void cvRenderTracks(CvTracks const tracks, IplImage *imgSource, IplImage *imgDest, unsigned short mode=0x00ff, CvFont *font=NULL)
  /// \brief Prints tracks information.
  /// \param tracks List of tracks.
  /// \param imgSource Input image (depth=IPL_DEPTH_8U and num. channels=3).
  /// \param imgDest Output image (depth=IPL_DEPTH_8U and num. channels=3).
  /// \param mode Render mode. By default is CV_TRACK_RENDER_ID.
  /// \param font OpenCV font for print on the image.
  /// \see CV_TRACK_RENDER_ID
  /// \see CV_TRACK_RENDER_TO_LOG
  /// \see CV_TRACK_RENDER_TO_STD
  void cvRenderTracks(CvTracks const tracks, IplImage *imgSource, IplImage *imgDest, unsigned short mode=0x000f, CvFont *font=NULL);
#ifdef __cplusplus
}
#endif

#endif
