/*! \file BasicOptions.cpp
 * 	\brief Basic options loader
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
#include "BasicOptions.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace wpc {
//------------------------------------------------------------------------------------------------
void BasicOptions::load(const std::string& optPath) {
	clear();
	boost::property_tree::ptree pt;
	read_xml(optPath,pt);
	for (boost::property_tree::ptree::iterator it = pt.begin()->second.begin(),
			end = pt.begin()->second.end();it!=end;++it)
		m_optionsContainer[it->first]=it->second.get_value<std::string>();
}
//------------------------------------------------------------------------------------------------
void BasicOptions::clear() {
	m_optionsContainer.clear();
}
//------------------------------------------------------------------------------------------------
std::string BasicOptions::getProperty(const std::string& prop) const {
	std::map<std::string,std::string>::const_iterator pos = m_optionsContainer.find(prop);
	if (pos!=m_optionsContainer.end())
		return pos->second;
	return std::string("Property not found.");
}

}



