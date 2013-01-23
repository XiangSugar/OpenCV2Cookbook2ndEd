/*------------------------------------------------------------------------------------------*\
   This file contains material supporting chapter 4 of the cookbook:  
   Computer Vision Programming using the OpenCV Library 
   Second Edition 
   by Robert Laganiere, Packt Publishing, 2013.

   This program is free software; permission is hereby granted to use, copy, modify, 
   and distribute this source code, or portions thereof, for any purpose, without fee, 
   subject to the restriction that the copyright notice may not be removed 
   or altered from any source or altered source distribution. 
   The software is released on an as-is basis and without any warranties of any kind. 
   In particular, the software is not guaranteed to be fault-tolerant or free from failure. 
   The author disclaims all warranties with regard to this software, any use, 
   and any consequent failure, is purely the responsibility of the user.
 
   Copyright (C) 2013 Robert Laganiere, www.laganiere.name
\*------------------------------------------------------------------------------------------*/

#if !defined OFINDER
#define OFINDER

#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class ContentFinder {

  private:

	// histogram parameters
	float hranges[2];
    const float* ranges[3];
    int channels[3];

	float threshold;           // decision threshold
	cv::Mat histogram;         // histogram can be sparse 
	cv::SparseMat shistogram;  // or not
	bool isSparse;

  public:

	ContentFinder() : threshold(0.1f), isSparse(false) {

		// all channels have the same range
		ranges[0]= hranges;  
		ranges[1]= hranges; 
		ranges[2]= hranges; 
	}
   
	// Sets the threshold on histogram values [0,1]
	void setThreshold(float t) {

		threshold= t;
	}

	// Gets the threshold
	float getThreshold() {

		return threshold;
	}

	// Sets the reference histogram
	void setHistogram(const cv::Mat& h) {

		isSparse= false;
		histogram= h;
		cv::normalize(histogram,histogram,1.0);
	}

	// Sets the reference histogram
	void setHistogram(const cv::SparseMat& h) {

		isSparse= true;
		shistogram= h;
		cv::normalize(shistogram,shistogram,1.0,cv::NORM_L2);
	}

	// All channels used, with range [0,255]
	cv::Mat find(const cv::Mat& image) {

		cv::Mat result;

		hranges[0]= 0.0;	// range [0,255]
		hranges[1]= 255.0;
		channels[0]= 0;		// the three channels 
		channels[1]= 1; 
		channels[2]= 2; 

		return find(image, 0.0, 255.0, channels);
	}

	// Finds the pixels belonging to the histogram
	cv::Mat find(const cv::Mat& image, float minValue, float maxValue, int *channels) {

		cv::Mat result;

		hranges[0]= minValue;
		hranges[1]= maxValue;

		if (isSparse) { // call the right function based on histogram type

		   for (int i=0; i<shistogram.dims(); i++)
			  this->channels[i]= channels[i];

		   cv::calcBackProject(&image,
                      1,            // we only use one image at a time
                      channels,     // vector specifying what histogram dimensions belong to what image channels
                      shistogram,   // the histogram we are using
                      result,       // the resulting back projection image
                      ranges,       // the range of values, for each dimension
                      255.0         // the scaling factor is chosen such that a histogram value of 1 maps to 255
		   );

		} else {

		   for (int i=0; i<histogram.dims; i++)
			  this->channels[i]= channels[i];

		   cv::calcBackProject(&image,
                      1,            // we only use one image at a time
                      channels,     // vector specifying what histogram dimensions belong to what image channels
                      histogram,    // the histogram we are using
                      result,       // the resulting back projection image
                      ranges,       // the range of values, for each dimension
                      255.0         // the scaling factor is chosen such that a histogram value of 1 maps to 255
		   );
		}

        // Threshold back projection to obtain a binary image
		if (threshold>0.0)
			cv::threshold(result, result, 255*threshold, 255, cv::THRESH_BINARY);

		return result;
	}

};


#endif