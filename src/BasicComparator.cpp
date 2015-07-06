/*! \file BasicComparator.cpp
 * 	\brief Default comparison method.
 */
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014, Lingaro sp. z o.o.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
//  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
//  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
/////////////////////////////////////////////////////////////////////////////


#include "BasicComparator.h"
#include <cmath>
#include <opencv2/core/core.hpp>

namespace wpc {
/*!
 * \brief Calculates specified color CoG with respect to appropriate axis
 *
 * \param[in] src - source matrix (3 channels)
 * \param[in] color - color (see \link BasicComparator::COLOR \endlink )
 * \param[in] axis - specified axis
 */
float BasicComparator::calcMomentum(const cv::Mat& src,COLOR color,int axis)
{
	const unsigned width = src.cols;
	const unsigned height = src.rows;
	float distance;
	float r,g,b,rs,gs,bs;
	r=g=b=gs=rs=bs=0;
	for (unsigned i=0;i<height;++i)
	{
		const uchar *p = src.ptr<uchar>(i);
		for (unsigned j=0;j<width;++j)
		{
			distance = (!axis ? j : i);
			bs+=(*p);
			b+=distance*(*p++);
			gs+=(*p);
			g+=distance*(*p++);
			rs+=(*p);
			r+=distance*(*p++);
		}
	}
	float retVal  = 0;
	switch (color)
	{
	case RED:
		retVal = r/rs;
		break;
	case GREEN:
		retVal = g/gs;
		break;
	case BLUE:
		retVal = b/bs;
	}
return retVal;
}
//---------------------------------------------------------------------------
/*!
 * \brief Compares given images calculating maximum distance between centers of
 * colours in 3D space.
 *
 * \param[in] larg - first image
 * \param[in] parg - second image
 * \return distance between centers of colours in 3D space.
 *
 */
BasicComparator::result_type BasicComparator::operator()(const cv::Mat& larg,const cv::Mat& parg) {
	static float r,g,b,r1,g1,b1;
	r = calcMomentum(larg,RED)-calcMomentum(parg,RED);
	g = calcMomentum(larg,GREEN)-calcMomentum(parg,GREEN);
	b = calcMomentum(larg,BLUE)-calcMomentum(parg,BLUE);
	r1 = calcMomentum(larg,RED,1)-calcMomentum(parg,RED,1);
	g1 = calcMomentum(larg,GREEN,1)-calcMomentum(parg,GREEN,1);
	b1 = calcMomentum(larg,BLUE,1)-calcMomentum(parg,BLUE,1);
	double ret_Val=std::max(std::sqrt(std::pow(r,2)+std::pow(r1,2)),
			std::max( std::sqrt(std::pow(g,2)+std::pow(g1,2)),
					std::sqrt(std::pow(b,2)+std::pow(b1,2))));
	std::cout << "Value: " << ret_Val << std::endl;
	return ret_Val;
}

} /* namespace wpc */
