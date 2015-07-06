/*! \file BasicLoader.cpp
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

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include "misc.h"
#include "BasicLoader.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/lexical_cast.hpp>

namespace wpc {
	//----------------------------------------------------------------------
	std::string BasicLoader::m_jsScriptName = "data/default/hscript.js";
	std::string BasicLoader::m_TempName = "temporary.html";
	//----------------------------------------------------------------------
	bool BasicLoader::locROI(const cv::Mat& mask, cv::Point& P1, cv::Point& P2)
	{
		int rows = mask.rows;
		int cols = mask.cols;
		cv::Mat_<cv::Vec3f> ImPoint = mask;
		cv::Vec3f maskColor(0, 128, 0);
		cv::Point pMax(0, 0);
		cv::Point pMin(cols, rows);
		bool hasColor = false;
		for (int i = 0; i < rows; ++i)
			for (int j = 0; j < cols; ++j)
				if (ImPoint(i, j) == maskColor)
				{
					hasColor = true;
					if (pMax.x < j)
						pMax.x = j;
					if (pMax.y < i)
						pMax.y = i;
					if (pMin.x > j)
						pMin.x = j;
					if (pMin.y > i)
						pMin.y = i;
				}
		P1 = pMin;
		P2 = pMax;
		return hasColor;
	}
	//----------------------------------------------------------------------------------------
#ifdef WIN32
	void BasicLoader::toMat(HWND hWnd, cv::Mat& Screen) const {
		if (!hWnd)
			return;
		SendMessage(hWnd, WM_SIZE, SIZE_MAXIMIZED, 0);
		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		HDC hDesktop = GetDC(hWnd);
		HDC hDC = CreateCompatibleDC(hDesktop);
		HBITMAP hBMP = CreateCompatibleBitmap(hDesktop, nScreenWidth, nScreenHeight);
		SelectObject(hDC, hBMP);
		BitBlt(hDC, 0, 0, nScreenWidth, nScreenHeight, hDesktop, 0, 0, SRCCOPY);
		Screen.create(nScreenHeight, nScreenWidth, CV_8UC3);
		for (int i = 0; i < nScreenHeight; ++i)
		{
			uchar *p = Screen.ptr<uchar>(i);
			for (int j = 0; j < nScreenWidth; ++j)
			{
				int Color = GetPixel(hDC, j, i);
				*p++ = GetBValue(Color);
				*p++ = GetGValue(Color);
				*p++ = GetRValue(Color);
			}

		}
		ReleaseDC(hWnd, hDesktop);
		DeleteObject(hBMP);
		DeleteDC(hDC);
	}
#else
	void BasicLoader::toMat(int,cv::Mat& Screen) const {
		std::cout << "" << std::endl;
	}
#endif
	//--------------------------------------------------------------------------------------------------
	void BasicLoader::closeBrowsers(const BrowserData& bd) const {
#ifdef WIN32
		unsigned int exitCode = 0;
		TerminateProcess(bd.processInfo.hProcess, exitCode);
#endif
	}
	//--------------------------------------------------------------------------------------------------
	cv::Mat* BasicLoader::getImage(const LayerData& object, const LayerDataVector& array) const {
		LayerDataVector::const_iterator element = find_if(array.begin(), array.end(),
			[&](const LayerData& ld) { return ld.browser == object.browser; });
		if (element != array.end())
			return const_cast<cv::Mat*>(&(*element).image);
		return NULL;
	}
	//--------------------------------------------------------------------------------------------------
	void BasicLoader::comparePages(const BasicLoaderAppData& pg)  {
		LayerDataVector originalWebPage, images;
		cv::Point fP11, fP12, fP21, fP22;
		cv::Mat tmp1, tmp2, *scannedWeb, *originalWeb;
		/* layer 0 == not modified page */
		acquireLayers(0, pg, originalWebPage);
		/* get next layers */
		for (int i = 1; i <= m_layerNumber; ++i) {
			std::cout << "Layer number: " << i << std::endl;
			/* Make screenshots for current script configuration */
			acquireLayers(i, pg, images);
			for (unsigned n = 0; n < images.size(); ++n) {
				/* Retrieve an unmodified image of web page for current mask */
				if (!(originalWeb = getImage(images[n], originalWebPage)))
					continue;
				if (!locROI(images[n].image, fP11, fP12))
					break;
				tmp1 = (*originalWeb)(cv::Rect(fP11.x, fP11.y, fP12.x - fP11.x, fP12.y - fP11.y));
				for (unsigned m = n + 1; m < images.size(); ++m) {
					if (!(scannedWeb = getImage(images[m], originalWebPage)))
						continue;
					if (!locROI(images[m].image, fP21, fP22))
						continue;
					tmp2 = (*scannedWeb)(cv::Rect(fP21.x, fP21.y, fP22.x - fP21.x, fP22.y - fP21.y));
					ResultType rt;
					/* Compare selected fragments */
					rt.cmpCoeff = comparator(tmp1, tmp2);
					rt.def1.image = tmp1.clone();
					rt.def1.browser = images[n].browser;
					rt.def2.image = tmp2.clone();
					rt.def2.browser = images[m].browser;
					rt.layerNumber = i;
					m_results.push_back(rt);
				}
			}
		}
	}
	//---------------------------------------------------------------------------
	void BasicLoader::acquireLayers(int i, const BasicLoaderAppData& pg,
		LayerDataVector& layers) {
#ifdef WIN32
		layers.clear();
		layers.resize(pg.browser_data.size());
		std::stringstream windowName;
		HWND hHandle = NULL;
		BrowserDataVector::const_iterator it = pg.browser_data.begin();
		int m = 0;
		for (; it != pg.browser_data.end(); ++it, ++m) {
			hHandle = NULL;
			windowName.str(std::string());
			windowName << "WebPageComparator-Layer:" << i << (*it).title;
			do {
				hHandle = FindWindow(NULL, windowName.str().c_str());
				Sleep(10);
				std::cout << "Finding window nr " << i << "   " << windowName.str().c_str()<< std::endl;
			} while (!hHandle);
			toMat(hHandle, layers[m].image);
			/* Information about browser */
			layers[m].browser = (*it).title;
		}
#else
		/* Linux version */
#endif
	}
	//---------------------------------------------------------------------------
	bool BasicLoader::start(BrowserData& application, const std::string& fileName)  {
#ifdef WIN32
		char cCurrentPath[FILENAME_MAX];
		if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
		{
			return errno;
		}
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
		std::stringstream Path;
		STARTUPINFO info;
		//PROCESS_INFORMATION processInfo;
		info.cb = sizeof(STARTUPINFO);
		Path.str(std::string());
		Path.clear();
		Path << application.path << " \"" << cCurrentPath << "\\" << fileName << "\"";
		memset(&info, 0, sizeof(info));
		memset(&application.processInfo, 0, sizeof(application.processInfo));
		if (!CreateProcessA(NULL, const_cast<char*>(Path.str().c_str()),
			NULL, NULL, true, 0, NULL, NULL,
			&info, &application.processInfo))
			return false;
#else
		std::cout << "Starting: \"" << application.path << " " << fileName << "\" Title: \""
			<< application.title << "\""<< std::endl;
#endif
		return true;
	}
	//---------------------------------------------------------------------------
	void BasicLoader::run(AppData& appPaths) {
		/* May throw bad_cast */
		BasicLoaderAppData& appData = dynamic_cast<BasicLoaderAppData&>(appPaths);
		/* Load default settings */
		m_layerNumber = boost::lexical_cast<int>(m_options.getProperty("LayerNumber"));
		m_counterInitial = boost::lexical_cast<int>(m_options.getProperty("CounterInitial"));
		m_layerChangeDelay = boost::lexical_cast<int>(m_options.getProperty("LayerChangeDelay"));
		/* Load script file */
		const int MAX_SIZE = 300000;
		char dataBuffer[MAX_SIZE] = { 0 };
		std::ifstream JS(m_jsScriptName);
		if (!JS.is_open())
			throw  std::runtime_error(std::string("Unable to open: ") + m_jsScriptName + INSERT_POS_MARK());
		JS.read(dataBuffer, sizeof(dataBuffer));
		JS.close();
		std::string currentJS(dataBuffer);
		/* Replace all loaded parameters */
		if (!replaceString(currentJS, "{LayerNumber}", m_options.getProperty("LayerNumber")))
			throw std::runtime_error(std::string("Unable to substitute parameter: LayerNumber  ") + INSERT_POS_MARK());
		if (!replaceString(currentJS, "{CounterInitial}", m_options.getProperty("CounterInitial")))
			throw std::runtime_error(std::string("Unable to substitute parameter: CounterInitial  ") + INSERT_POS_MARK());
		if (!replaceString(currentJS, "{LayerChangeDelay}", m_options.getProperty("LayerChangeDelay")))
			throw std::runtime_error(std::string("Unable to substitute parameter: LayerChangeDelay  ") + INSERT_POS_MARK());
		/* Load tested HTML */
		std::string fullPath = appData.working_directory + appData.file_name;
		std::ifstream HTML(fullPath);
		if (!HTML.is_open())
			throw  std::runtime_error(std::string("Unable to load: ") + appData.file_name + INSERT_POS_MARK());
		HTML.read(dataBuffer, sizeof(dataBuffer));
		HTML.close();
		std::string currentHTML(dataBuffer);
		/* Save mixed html and js*/
		currentHTML += currentJS;
		std::ofstream outputFile(m_TempName, std::ios_base::out);
		if (!outputFile.is_open())
			throw  std::runtime_error(std::string("Unable to write: ") + m_TempName + INSERT_POS_MARK());
		outputFile << currentHTML;
		outputFile.close();
		/* Start selected browsers */
		BrowserDataVector::iterator it = appData.browser_data.begin();
		fullPath = appData.working_directory + m_TempName;
		for (; it != appData.browser_data.end(); ++it)
			if (!start((*it), fullPath))
				throw std::runtime_error(std::string("Unable to start: ") + (*it).path + INSERT_POS_MARK());
		/* Start comparison  */
		comparePages(appData);
		/* Close browsers */
		it = appData.browser_data.begin();
		for (; it != appData.browser_data.end(); ++it)
			closeBrowsers(*it);
	}
	//---------------------------------------------------------------------------
	void BasicLoader::operator()(AppData& appPaths) {
		/* Load options */
		m_options.load("data/default/config.xml");
		run(appPaths);
	}

	} /* namespace wpc */
