/*! \file BasicComparator.h
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

#ifndef BASICCOMPARATOR_H_
#define BASICCOMPARATOR_H_


#include "comparator_traits.h"
#include <boost/signals2.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>

namespace wpc {
/*!
 * \brief Type trait for BasicComparator data type.
 *
 */
class BasicComparator;
template<> struct cmp_trait<BasicComparator> {
	typedef double result_type;
	typedef boost::function<result_type (const cv::Mat&,const cv::Mat&)> cmp_call_sig;
};
/*!
 * \brief Defines basic class for comparison of two selected web page fragments.
 * */
class BasicComparator {
public:
	enum COLOR {
		RED, /*! Red channel */
		GREEN,/*! Green channel */
		BLUE /*! Blue channel */
	};
	/*! \brief Result type of operator() [required] */
	typedef cmp_trait<BasicComparator>::result_type result_type;
	/*!
	 * \brief Operator() is being invoked by BasicLoader class to compare two images which are
	 * 	being considered as the same part of different web pages.
	 */
	result_type operator()(const cv::Mat& larg,const cv::Mat& parg);
	/*! \return Comparison results  */
	~BasicComparator() { }
private:
	/*!
	 * \brief Calculates CoG of selected colour in respect for selected axis
	 *
	 * \param[in] src - source matrix
	 * \param[in] color - selected channel see \link COLOR \endlink
	 * \param[in] axis - selected image axis (X or Y) By default 0 means X.
	*/
	float calcMomentum(const cv::Mat& src,COLOR color,int axis=0);
};


} /* namespace wpc */

#endif /* BASICCOMPARATOR_H_ */
