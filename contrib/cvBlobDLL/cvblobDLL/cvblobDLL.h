// Copyright (C) 2010 by Giandomenico De Sanctis
// gidesay@yahoo.it
// Use CvBlob C++ library: Copyright (C) 2007 by Cristóbal Carnero Liñán grendel.ccl@gmail.com

/// \file cvblobDLL.h
/// \brief OpenCV Blob dynamic library (DLL) header file.


#define C_EXTERN extern "C"

#ifdef _USRDLL
#define CVBLOBDLL_API __declspec(dllexport)
#else
#define CVBLOBDLL_API 
#endif

#include "cxtypes.h"

/// \brief C style struct that contain information about one blob.
typedef struct CvBlobObj
  {
    unsigned int label; ///< Label assigned to the blob.
    
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
    
	BOOLEAN centralMoments; ///< True if central moments are being calculated.
    double u11; ///< Central moment 11.
    double u20; ///< Central moment 20.
    double u02; ///< Central moment 02.

    double   angle; ///< Blob main axe angle with horizontal (radiants).
	double   eccentricity; ///< Blob measure of eccentricity.
    CvScalar meanColor; ///< Mean color in the blob area, R-G-B format
    CvSeq*   contour;   ///< Blob external contour.
    CvSeq*   internalContours;  ///< Blob internal holes contours.
  } CvBlobObj;

/// \brief Possible values for flags param in CvBlobDLabel.
#define CVBLOBDLL_GETCONTOUR	1
#define CVBLOBDLL_GETHOLESCONT	2
#define CVBLOBDLL_GETMEANCOLOR	4

/// \fn unsigned int cvBlobDLabel(IplImage const *img, IplImage *imgOut, CvMemStorage * memstor,CvSeq** blobs, int minArea, int flags, const IplImage* colorImg) 
/// \brief finds all blobs in the input binary image, and return them in a OpenCV CvSeq structure
/// \param img Input binary image (depth=IPL_DEPTH_8U and num. channels=1).
/// \param imgOut Output image (depth=32 and num. channels=1).
/// \param memstor OpenCV CvMemStorage struct for allocating sequences.
/// \param blobs Output CvSeq with blobs.
/// \param minArea Minimum area of extracted blobs.
/// \param flags OR-ed value of requested blob informations: contour, holes contours, mean color.
/// \param colorImg Optional color image (3 channels) to calculate mean color.
/// \return Number of pixels that has been labeled.
/// \see CvBlobsObj
C_EXTERN CVBLOBDLL_API CDECL unsigned int cvBlobDLabel 
         (IplImage const *img, IplImage *imgOut, CvMemStorage * memstor,  
	      CvSeq** blobs, int minArea, int flags, const IplImage* colorImg);

/// \fn void cvBlobDRelease()
/// \brief release all memory areas allocated by CvBlobDLabel
C_EXTERN CVBLOBDLL_API CDECL void cvBlobDRelease(); 

