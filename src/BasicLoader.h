/*! \file BasicLoader.h
 * 	\brief Default web browser start and image acquire methods
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

#ifndef BASICLOADER_H_
#define BASICLOADER_H_

#ifdef WIN32
	#include <windows.h>
	#include <direct.h>
#endif


#include "misc.h"
#include "loader_traits.h"
#include "BasicComparator.h"
#include "BasicOptions.h"
#include <string>
#include <vector>
#include <opencv2/core/core.hpp> 
#include <stdio.h>

namespace wpc {
/*!
 * \brief Single browser descriptor for BasicLoader
 * \param path - path to the browser executable file [Used under Windows]
 * \param title - title of browser window [Used under Windows]
 */
struct BrowserData {
	std::string path;
	std::string title;
#ifdef WIN32
	PROCESS_INFORMATION processInfo;
#endif
};
typedef std::vector<BrowserData> BrowserDataVector;
/*!
 * \brief Multiple browser descriptor for BasicLoader
 * \param browser_data - vector of browsers to be started
 * \param file_name - file to be compared across browsers
 */
struct BasicLoaderAppData : public AppData {
	BrowserDataVector browser_data;
	std::string file_name;
	std::string working_directory;
};
/*!
 * \brief Layer descriptor. Descripts a highlighted layer
 *  which represents particular part of a web page in specified browser
 *
 * \param image - image of highlighted fragment [from non-modified web page]
 * \param browser - browser title
 *
 */
struct LayerData {
	cv::Mat image;
	std::string browser;
};
typedef std::vector<LayerData> LayerDataVector;
/*!
 *
 */
struct ResultType {
	LayerData def1;
	LayerData def2;
	cmp_trait<BasicComparator>::result_type cmpCoeff;
	int layerNumber;
};
/*!
 * \brief Loads and carryies out the comparison process.
 *
 * Class is being used for loading (starting browsers) and carrying out the process
 * of cross browser validation using specified methods (in default solution
 * provided by BasicComparison class).
 *
 * */
class BasicLoader {
public:
	typedef std::vector<ResultType> ComparisonResultType;
	BasicLoader() : m_layerNumber(0),
		m_counterInitial(0),m_layerChangeDelay(5000) { }
	/*!
	 * \brief Operator() is responsible for loading specified browsers
	 * and starting comparison process using provided BasicComparator object
	 *
	 * \param[in] appData - list specifing paths/titles of the browsers to be started.
	 * For further details see \link BasicLoaderAppData \endlink
	 */
	void operator()(AppData& appData);
	/*!
	 * \brief Sets comparator object
	 *
	 * \param st - slot based on boost::signals2 library providing call to operator()
	 * of BasicComparator
	 */
	void setComparatorObject(const cmp_trait<BasicComparator>::cmp_call_sig& st);

	const ComparisonResultType& getResults() const;
	~BasicLoader() {}
private:
	int m_layerNumber;
	int m_counterInitial;
	int m_layerChangeDelay;
	BasicOptions m_options;
	ComparisonResultType m_results;
	/*! \brief Describes name of JavaScript file, responsible for highlightning layers */
	static std::string m_jsScriptName;
	/*! \brief Temporary name of */
	static std::string m_TempName;
#ifdef WIN32
	/*!
	 * \brief [Windows] Converts Windows Bitmap from RGB format to cv::Mat RGB format.
	 *	Using specified handle, this function creates a screen shot of window and
	 *  converts it to cv::Mat object
	*/
	void toMat(HWND hWnd,cv::Mat& Screen) const;
#else
	/*!
	 * \brief [Linux] Not supported yet.
	*/
	void toMat(int,cv::Mat& Screen) const;
#endif
	/*!
	 * \brief Same as operator()
	 */
	void run(AppData&);
	/*!
	 * \brief Starts browsers.
	 *  Function launches specified web browser with additional parameter "filename".
	 *
	 * \param[in] application - browser descriptor see: \link BrowserData \endlink
	 * \param[in] fileName - name of file to start
	 *
	 */
	bool start(BrowserData& application,const std::string& fileName);
	/*!
	 * \brief Closes started browsers afer comparison.
	 * \param[in] bd - descriptor of a single web browser
	 *
	 */
	void closeBrowsers(const BrowserData& bd) const;
	/*!
	* \brief Compares pages using selected comparator.
	*
	* \param[in] pg - defines list of browsers which should be compared
	*
	*/
	void comparePages(const BasicLoaderAppData& pg);
	/*!
	 * \brief Acquires specified layer from web browser as a screen shot.
	 *
	 * \param[in] i - layer number
	 * \param[in] pg - list of browsers to compare
	 * \param[in] layers - descriptor (image,browser)
	 *
	 */
	void acquireLayers(int i,const BasicLoaderAppData& pg,
			 LayerDataVector& layers);
	/*!
	 * \brief Localizes ROI in provided image. ROI is being described using specified colour.
	 *
	 * \param[in] mask - image with highlighted ROI
	 * \param[out] P1 - coordinates of left upper corner of found ROI
	 * \param[out] P2 - coordinates of left upper corner of found ROI
	 */
	bool locROI(const cv::Mat& mask,cv::Point& P1,cv::Point& P2);
	/*!
	 * \brief Retrieves associated image from an array using browser description.
	 *
	 * \param[in] object - describes particular browser
	 * \param[in] array - array of objects
	 */
	cv::Mat* getImage(const LayerData& object,const LayerDataVector& array) const;


	cmp_trait<BasicComparator>::cmp_call_sig comparator;
};

inline const BasicLoader::ComparisonResultType& BasicLoader::getResults() const {
	return m_results;
}
/*!
 * \brief Sets the comparison object used for
 * comparing selected parts of web page.
 *
 * \param[in] st - comparison object
 */
inline void BasicLoader::setComparatorObject(const cmp_trait<BasicComparator>::cmp_call_sig& st)
{
	comparator = st;
}
} /* namespace wpc */

#endif /* BASICLOADER_H_ */
