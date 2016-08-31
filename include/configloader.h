#ifndef __YAMLREADER_H__
#define __YAMLREADER_H__

#include "yaml-cpp/yaml.h"
#include "pcre++.h"

namespace UAConfig {

using std::string;
using std::vector;

struct OSConfig {
	pcrepp::Pcre reg;
	string name, version;
};
struct BrowserConfig {
	pcrepp::Pcre reg;
	string name, version;
};
struct MobileConfig {
	pcrepp::Pcre reg;
	string brand, type;
	struct Model {
		pcrepp::Pcre reg;
		string type, model;
	};
	vector<Model> models;
};

class ConfigLoader {
public:
	ConfigLoader(const string& file_os, const string& file_browser, const string& file_mobile);
	pcrepp::Pcre android_reg;
	vector<OSConfig> oses;
	vector<BrowserConfig> browsers;
	vector<MobileConfig> mobiles;
private:
	bool ensure(vector<MobileConfig> &, const string &, const string &);
};

}

#endif
