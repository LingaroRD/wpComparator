
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include "src/wpc.h"
#include "src/BasicComparator.h"
#include "src/BasicLoader.h"

int main(){
	using namespace wpc;

	BasicLoaderAppData basic;
	BrowserData bd;
	bd.path="C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";
	bd.title=" - Google Chrome";
	basic.browser_data.push_back(bd);
	bd.path="C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe";
	bd.title=" - Mozilla FireFox";
	basic.browser_data.push_back(bd);
	basic.working_directory = "";
	basic.file_name="_ztm.html";
	ProcessingFlow<BasicLoader,BasicComparator> wpcWebForge;
	BasicLoader::ComparisonResultType results = wpcWebForge(basic);
	BasicLoader::ComparisonResultType::iterator it = results.begin();
	
	for (;it!=results.end();++it)
	{
		cv::imshow(it->def1.browser,it->def1.image);
		cv::imshow(it->def2.browser,it->def2.image);
		std::cout << "Diff coeff: " << it->cmpCoeff << std::endl;
		cv::waitKey(0);
	}
	
}
