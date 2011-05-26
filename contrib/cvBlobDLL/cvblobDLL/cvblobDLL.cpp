// cvblobDLL.cpp 
// A C style dynamic library (DLL) to encapsulate CvBlob functions
// Copyright (C) 2010 by Giandomenico De Sanctis
// gidesay@yahoo.it
// Use CvBlob C++ library: Copyright (C) 2007 by Cristóbal Carnero Liñán grendel.ccl@gmail.com

#include <vector>

#include "stdafx.h"
#include "cvblobDLL.h"
#include "cvblob.h"

using namespace cvb;

#define MIN_HOLE_POINTS 10

typedef std::vector<void*> memVect;
memVect pAllocatedMem;
int   nAllocatedMem=0;

void* cvBlobDAlloc(int bytes) 
{
	void* p = malloc(bytes);
	if (p) {
		pAllocatedMem.push_back(p);
		nAllocatedMem++;
	};
	return p;
};

CVBLOBDLL_API CDECL void cvBlobDRelease() 
{
    CV_FUNCNAME( "cvBlobDRelease" );

    __BEGIN__;


	memVect::const_iterator costIter = pAllocatedMem.begin();
	for(; costIter != pAllocatedMem.end(); costIter++)
	{
		void* P = (void*)(*costIter);
		free(P);
	};
	
	pAllocatedMem.clear();

	nAllocatedMem = 0;

 
	__END__;
};


BOOLEAN cvBlobDContour(CvSeq** seqCont, const CvContourChainCode blobListElem, CvMemStorage * memstor)
{
	BOOLEAN res = FALSE;


    CV_FUNCNAME( "cvBlobDContour" );

    __BEGIN__;

    if (( !seqCont ) || ( !(&blobListElem) )) 
        CV_ERROR( CV_StsNullPtr, "" );

	*seqCont = NULL;


	{ // init block	with iterators
	CvContourPolygon* pPolyg = cvConvertChainCodesToPolygon(&blobListElem);

	if (pPolyg->size() == 0) { delete pPolyg; return res; };

	// simplify a bit the contour
	CvContourPolygon* pContour = cvSimplifyPolygon(pPolyg, 5.0);
	delete pPolyg;

	CvSeq* seqpoints = NULL;
	seqpoints = cvCreateSeq(CV_SEQ_CONTOUR, sizeof(CvSeq), sizeof(CvPoint), memstor);

	CvPoint*  pNewPoint = (CvPoint*) cvBlobDAlloc(sizeof(CvPoint)*pContour->size());
    if (( !pNewPoint ) )
        CV_ERROR( CV_StsNullPtr, "Not enough memory for contour points" );
	CvContourPolygon::const_iterator pcontIter = pContour->begin();
	for(; pcontIter != pContour->end(); pcontIter++)
	{
		pNewPoint->x = (*pcontIter).x;
		pNewPoint->y = (*pcontIter).y;
        void* newElem = (void*)  cvSeqPush((CvSeq*) seqpoints, (void*) pNewPoint);
		pNewPoint++;
	};

	*seqCont = seqpoints;

	delete pContour;
	}; // end block with iterators


	__END__;


	return res;
};



CVBLOBDLL_API CDECL unsigned int cvBlobDLabel 
     (IplImage const *img, IplImage *imgOut, CvMemStorage * memstor, CvSeq** sblobs,
	 int minArea, int flags, const IplImage* colorImg)
{
	int res;
	CvBlobs lblobs;

	
    CV_FUNCNAME( "cvBlobDLabel" );

    __BEGIN__;

    if (( !memstor ) || ( !img ) || ( !imgOut ))
        CV_ERROR( CV_StsNullPtr, "" );
	if ( !CV_IS_STORAGE( memstor ))
		CV_ERROR( CV_StsBadArg, "1st param not a  CvMemStorage" );
	if ( !CV_IS_IMAGE( img ))
		CV_ERROR( CV_StsBadArg, "2st param not an IplImage" );
	if ( !CV_IS_IMAGE( imgOut ))
		CV_ERROR( CV_StsBadArg, "3st param not an IplImage" );
	if ( (colorImg) &&  (!CV_IS_IMAGE( colorImg )) )
		CV_ERROR( CV_StsBadArg, "colorImg param not an IplImage" );

	res = cvLabel(img, imgOut, lblobs);
	if (minArea<10) 
			minArea = 10;
	cvFilterByArea(lblobs, minArea, (img->width*img->height));

	if (lblobs.size() ==0)
	{
		sblobs = NULL;
        cvReleaseBlobs(lblobs);
		return 0;
	};
	CvSeq* seqblobs = NULL;
	seqblobs = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvSeq), sizeof(CvBlobObj), memstor);
	
	CvBlobObj* pNewBlobObj = (CvBlobObj*) cvBlobDAlloc(sizeof(CvBlobObj)*lblobs.size());
   if (( !pNewBlobObj ) )
        CV_ERROR( CV_StsNullPtr, "Not enough memory for blobs" );
   for (CvBlobs::const_iterator it=lblobs.begin(); it!=lblobs.end(); ++it)
   {
	   CvBlob curBlob = (CvBlob) *((*it).second);
	   // cvCentralMoments(&curBlob, imgOut);
			
	   memcpy(pNewBlobObj, &curBlob, sizeof(CvBlobObj));
		pNewBlobObj->angle = cvAngle(&curBlob);
		// eccentricity = ( (u20 - u02)^2 - 4 u11^2 ) / (u20 + u02)^2 );
		pNewBlobObj->eccentricity = 
			( (pNewBlobObj->u20 - pNewBlobObj->u02)*(pNewBlobObj->u20 - pNewBlobObj->u02) - 4*pNewBlobObj->u11*pNewBlobObj->u11 )
				/ ( (pNewBlobObj->u20 + pNewBlobObj->u02)*(pNewBlobObj->u20 + pNewBlobObj->u02) );
		pNewBlobObj->meanColor = cvScalarAll(0);
		pNewBlobObj->contour = NULL;
		pNewBlobObj->internalContours = NULL;

		if (flags & CVBLOBDLL_GETCONTOUR)
		{
			cvBlobDContour(&pNewBlobObj->contour, curBlob.contour, memstor);
		};

		if ((flags & CVBLOBDLL_GETHOLESCONT) && (curBlob.internalContours.size()>0))
		{
			CvSeq* precCont = NULL;
			CvSeq* curCont = NULL;
			CvContoursChainCode::const_iterator pcontIter = curBlob.internalContours.begin();
			for(; pcontIter != curBlob.internalContours.end(); pcontIter++)
			{
				// process only holes with at less MIN_HOLE_POINTS points
				if ( (*(*pcontIter)).chainCode.size()>=MIN_HOLE_POINTS) {
					cvBlobDContour(&curCont, *(*pcontIter) , memstor);
					if (precCont) {
						precCont->h_next = curCont;
					} else { 
						pNewBlobObj->internalContours = curCont;
					};
					curCont->h_prev = precCont;
					precCont = curCont;
				};
			};
		};
		if ((flags & CVBLOBDLL_GETMEANCOLOR) && (colorImg))  {
			pNewBlobObj->meanColor = cvBlobMeanColor(&curBlob, imgOut, colorImg);
		};
		
		void* newElem = (void*)  cvSeqPush((CvSeq*) seqblobs, (void*) pNewBlobObj);

		pNewBlobObj++;
  }



   *sblobs = seqblobs;
   cvReleaseBlobs(lblobs);
	
	__END__;


	return res;
};


