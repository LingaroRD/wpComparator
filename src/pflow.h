/*! \file  pflow.h
 * 	\brief Miscellaneous common defines
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

#ifndef PFLOW_H_
#define PFLOW_H_


#include <boost/bind.hpp>
#include <exception>
#include <stdexcept>
#include <iostream>

namespace wpc {
/*!
 * \brief Describes work flow process.
 *
 * This class creates and manages Loader (\link BasicLoader \endlink) and Comparison (\link BasicComparator \endlink)
 * object with are used to load and carry out web browser validation.
 *
 * \param[in] L - type of loader object.
 * \param[in] T - type of comparator object.
 */
template<typename L,typename C>
class ProcessingFlow {
public:
	ProcessingFlow();
	ProcessingFlow(const ProcessingFlow& c) = delete;
	ProcessingFlow& operator=(const ProcessingFlow&) = delete;
	~ProcessingFlow();
	/*!
	 * \brief Start validation.
	 * \param[in] appData - web browser list to compare
	 * */
	typename L::ComparisonResultType operator()(AppData& appData);
private:
	//bool compare();
};
//--------------------------------------------------------------------
template<typename L,typename C>
ProcessingFlow<L,C>::ProcessingFlow() {}
//--------------------------------------------------------------------
template<typename L,typename C>
ProcessingFlow<L,C>::~ProcessingFlow() {}
//--------------------------------------------------------------------
/*!
 * \brief Implements validation process.
 *
 * \param[in] L - type of loader object.
 * \param[in] T - type of comparator object.
 * \param[in] appPaths - array of AppData objects describing WebBrowser.
 */
template<typename L,typename C>
typename L::ComparisonResultType ProcessingFlow<L,C>::operator()(AppData& appPaths) {
	typename L::ComparisonResultType results;
	try {
		L loader;
		C comparator;
		/* Set callback function for comparing images */
		loader.setComparatorObject(boost::bind(boost::ref(comparator),_1,_2));
		loader(appPaths);
		results = loader.getResults();
	}
	catch (std::runtime_error& le) {
		std::cout << le.what() << std::endl;
	}
	catch (std::exception& se) {

	}
	catch (...) {
		throw;
	}
	return results;
}
//--------------------------------------------------------------------
};
#endif /* PFLOW_H_ */
