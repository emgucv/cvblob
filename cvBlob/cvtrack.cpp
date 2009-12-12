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

#include <cmath>
#include <iostream>
#include <sstream>
using namespace std;

#ifdef WIN32
#include <cv.h>
#else
#include <opencv/cv.h>
#endif

#include "cvblob.h"

#define D(Ax, Ay, Bx, By) (sqrt(((Ax)*(Ax) + (Bx)*(Bx)) + ((Ay)*(Ay) + (By)*(By))))

#define MIN8(a,b,c,d,e,f,g,h) MIN((a),MIN((b),MIN((c),MIN((d),MIN((e),MIN((f),MIN((g),(h))))))))

double distantBlobTrack(CvBlob const *b, CvTrack const *t)
{
  if (((b->centroid.x>=t->minx)&&(b->centroid.x<=(t->maxx))&&
	(b->centroid.y>=t->miny)&&(b->centroid.y<=(t->maxy)))||
      ((t->centroid.x>=b->minx)&&(t->centroid.x<=(b->maxx))&&
       (t->centroid.y>=b->miny)&&(t->centroid.y<=(b->maxy))))
    return 0.;
  else
  {
    return MIN8(
	D(b->centroid.x,b->centroid.y,t->minx,t->miny),
	D(b->centroid.x,b->centroid.y,t->maxx,t->miny),
	D(b->centroid.x,b->centroid.y,t->maxx,t->maxy),
	D(b->centroid.x,b->centroid.y,t->minx,t->maxy),
	D(t->centroid.x,t->centroid.y,b->minx,b->miny),
	D(t->centroid.x,t->centroid.y,b->maxx,b->miny),
	D(t->centroid.x,t->centroid.y,b->maxx,b->maxy),
	D(t->centroid.x,t->centroid.y,b->minx,b->maxy));
  }
}

void cvUpdateTracks(CvBlobs &b, CvTracks &t, const double thDistance, const unsigned int thInactive)
{
  CV_FUNCNAME("cvUpdateTracks");
  __BEGIN__;

  unsigned int nBlobs = b.size();
  unsigned int nTracks = t.size();

  // Proximity matrix:
  // Last row/column is for ID/label.
  // Last-1 "/" is for accumulation.
  CvID *close = new unsigned int[(nBlobs+2)*(nTracks+2)]; // XXX Must be same type than CvLabel.

  // Access to matrix
#define C(blob, track) close[((blob) + (track)*(nBlobs+2))]
  // Access to accumulators
#define AB(label) C((label), (nTracks))
#define AT(id) C((nBlobs), (id))
  // Access to identifications
#define IB(label) C((label), (nTracks)+1)
#define IT(id) C((nBlobs)+1, (id))
  // Access to registers
#define B(label) b[IB(label)]
#define T(id) t[IT(id)]

  try
  {
    // Inicialization:
    unsigned int i=0;
    for (CvBlobs::const_iterator it = b.begin(); it!=b.end(); ++it, i++)
    {
      AB(i) = 0;
      IB(i) = it->second->label;
    }

    CvID maxTrackID = 0;

    unsigned int j=0;
    for (CvTracks::const_iterator jt = t.begin(); jt!=t.end(); ++jt, j++)
    {
      AT(j) = 0;
      IT(j) = jt->second->id;
      if (jt->second->id > maxTrackID)
	maxTrackID = jt->second->id;
    }

    // Proximity matrix calculation:
    for (i=0; i<nBlobs; i++)
      for (j=0; j<nTracks; j++)
      {
	if (C(i, j) = (distantBlobTrack(B(i), T(j)) < thDistance))
	{
	  AB(i)++;
	  AT(j)++;
	}
      }

    // Run over tracks:
    for (j=0; j<nTracks; j++)
    {
      //unsigned int c = C(nBlobs, j);
      unsigned int c = AT(j);

      if (c == 1)
      {
	// Match track-blob
	
	// Search for the blob
	for (i=0; (i<nBlobs)&&(!C(i, j)); i++) {}

	// Update track
	CvBlob *blob = B(i);
	CvTrack *track = T(j);
	track->label = blob->label;
	track->centroid = blob->centroid;
	track->minx = blob->minx;
	track->miny = blob->miny;
	track->maxx = blob->maxx;
	track->maxy = blob->maxy;
	track->inactive = 0;
      }
      else if (c > 1)
      {
	// Track divides
	CvTrack *track = T(j);
	track->inactive++;
	track->label=0;

	// Create new tracks
	for (i=0; i<nBlobs; i++)
	{
	  if (C(i, j))
	  {
	    maxTrackID++;
	    CvBlob *blob = B(i);
	    CvTrack *track = new CvTrack;
	    track->id = maxTrackID;
	    track->label = blob->label;
	    track->minx = blob->minx;
	    track->miny = blob->miny;
	    track->maxx = blob->maxx;
	    track->maxy = blob->maxy;
	    track->centroid = blob->centroid;
	    track->inactive = 0;
	    t.insert(CvIDTrack(maxTrackID, track));
	  }
	}
      }
      else // if (c == 0)
      {
	// Inactive track
	CvTrack *track = T(j);
	track->inactive++;
	track->label = 0;
      }
    }

    // Run over blobs:
    for (i=0; i<nBlobs; i++)
    {
      //unsigned int c = C(i, nTracks);
      unsigned int c = AB(i);

      if (c == 0)
      {
	// New track
	maxTrackID++;
	CvBlob *blob = B(i);
	CvTrack *track = new CvTrack;
	track->id = maxTrackID;
	track->label = blob->label;
	track->minx = blob->minx;
	track->miny = blob->miny;
	track->maxx = blob->maxx;
	track->maxy = blob->maxy;
	track->centroid = blob->centroid;
	track->inactive = 0;
	t.insert(CvIDTrack(maxTrackID, track));
      }
      else if (c > 1)
      {
	// Tracks joins
	
	// New track
	maxTrackID++;
	CvBlob *blob = B(i);
	CvTrack *track = new CvTrack;
	track->id = maxTrackID;
	track->label = blob->label;
	track->minx = blob->minx;
	track->miny = blob->miny;
	track->maxx = blob->maxx;
	track->maxy = blob->maxy;
	track->centroid = blob->centroid;
	track->inactive = 0;
	t.insert(CvIDTrack(maxTrackID, track));
	
	// Others tracks inactives
	for (j=0; j<nTracks; j++)
	{
	  T(j)->inactive++;
	  T(j)->label = 0;
	}
      }
    }

    for (CvTracks::iterator jt=t.begin(); jt!=t.end();)
      if (jt->second->inactive>=thInactive)
      {
	delete jt->second;
	t.erase(jt++);
      }
      else
	++jt;
  }
  catch (...)
  {
    delete[] close;
    throw; // TODO: OpenCV style.
  }

  delete[] close;

  __END__;
}

CvFont *defaultFont = NULL;

void cvRenderTracks(CvTracks const tracks, IplImage *imgSource, IplImage *imgDest, unsigned short mode, CvFont *font)
{
  CV_FUNCNAME("cvRenderTracks");
  __BEGIN__;

  CV_ASSERT(imgDest&&(imgDest->depth==IPL_DEPTH_8U)&&(imgDest->nChannels==3));

  if ((mode&CV_TRACK_RENDER_ID)&&(!font))
  {
    if (!defaultFont)
    {
      font = defaultFont = new CvFont;
      cvInitFont(font, CV_FONT_HERSHEY_DUPLEX, 0.5, 0.5, 0, 1);
      // Other fonts:
      //   CV_FONT_HERSHEY_SIMPLEX, CV_FONT_HERSHEY_PLAIN,
      //   CV_FONT_HERSHEY_DUPLEX, CV_FONT_HERSHEY_COMPLEX,
      //   CV_FONT_HERSHEY_TRIPLEX, CV_FONT_HERSHEY_COMPLEX_SMALL,
      //   CV_FONT_HERSHEY_SCRIPT_SIMPLEX, CV_FONT_HERSHEY_SCRIPT_COMPLEX
    }
    else
      font = defaultFont;
  }

  if (mode)
  {
    for (CvTracks::const_iterator it=tracks.begin(); it!=tracks.end(); ++it)
    {
      if (mode&CV_TRACK_RENDER_ID)
      {
	if (!it->second->inactive)
	{
	  stringstream buffer;
	  buffer << it->first;
	  cvPutText(imgDest, buffer.str().c_str(), cvPoint((int)it->second->centroid.x, (int)it->second->centroid.y), font, CV_RGB(0.,255.,0.));
	}
      }

      if (mode&CV_TRACK_RENDER_TO_LOG)
      {
	clog << "Track " << it->second->id << endl;
	if (it->second->label)
	  clog << " - Associated with blob " << it->second->label << endl;
	else
	  clog << " - Inactive for " << it->second->inactive << " frames" << endl;
	clog << " - Bounding box: (" << it->second->minx << ", " << it->second->miny << ") - (" << it->second->maxx << ", " << it->second->maxy << ")" << endl;
	clog << " - Centroid: (" << it->second->centroid.x << ", " << it->second->centroid.y << ")" << endl;
	clog << endl;
      }

      if (mode&CV_TRACK_RENDER_TO_STD)
      {
	cout << "Track " << it->second->id << endl;
	if (it->second->label)
	  cout << " - Associated with blob " << it->second->label << endl;
	else
	  cout << " - Inactive for " << it->second->inactive << " frames" << endl;
	cout << " - Bounding box: (" << it->second->minx << ", " << it->second->miny << ") - (" << it->second->maxx << ", " << it->second->maxy << ")" << endl;
	cout << " - Centroid: (" << it->second->centroid.x << ", " << it->second->centroid.y << ")" << endl;
	cout << endl;
      }
    }
  }

  __END__;
}
