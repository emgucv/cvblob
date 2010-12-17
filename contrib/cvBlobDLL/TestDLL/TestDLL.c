// TestDLL.c : test of CvBlob DLL
//

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "cvblobDLL.h"


int main(int argc, char* argv[])
{
  IplImage *img = cvLoadImage("test.png", 1);

 // cvSetImageROI(img, cvRect(100, 100, 800, 500));

  cvSmooth(img, img, 3, 7);

  IplImage *grey = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
  cvCvtColor(img, grey, CV_BGR2GRAY);
  cvAdaptiveThreshold(grey, grey, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, 0, 19, 2.0); 

  IplImage *labelImg = cvCreateImage( (cvGetSize(grey)),32,1);

  CvSeq* lb;


  CvMemStorage * ms = cvCreateMemStorage();
  int res = cvBlobDLabel(grey, labelImg, ms, &lb, 400, (CVBLOBDLL_GETCONTOUR | CVBLOBDLL_GETHOLESCONT | CVBLOBDLL_GETMEANCOLOR), img);
  
    CvBlobObj*  elBlob;
    int i;

	for ( i = 0; i < lb->total; i++)
	{
		elBlob = (CvBlobObj*) cvGetSeqElem((CvSeq*) lb, i);
		// extract and draws blob contour
		if (elBlob->contour) {
			cvDrawContours(img, elBlob->contour, CV_RGB(255,0,0), CV_RGB(255,255,0), 1, 3);


			//CvSeq* hull = cvConvexHull2(elBlob->contour, ms, CV_CLOCKWISE, 1);
			//cvDrawContours(img, hull, CV_RGB(0,0,255), CV_RGB(0,255,255), 1, 3);
		};
		// extract and draws every blob internal hole contour
		if (elBlob->internalContours) {
			CvSeq* seqCont = elBlob->internalContours;
			while ( seqCont ) 
			{
			    cvDrawContours(img, seqCont, CV_RGB(0,255,0), CV_RGB(0,0,0), 1, 1);
				seqCont = seqCont->h_next;
			}
		};
		cvCircle(img, cvPoint((int)(elBlob->centroid.x),(int)(elBlob->centroid.y)), 2, CV_RGB(0,0,255), 2); 
	};

	cvBlobDRelease();
	cvReleaseMemStorage(&ms);

    cvResetImageROI(img);
	cvSaveImage("imgout.png", img);
	return 0;
}


