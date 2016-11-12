#include "configloader.h"

#include "common.h"

namespace UAConfig {

using std::string;
using std::vector;

/* We must ensure the a is ahead of b when recognizing. */
bool ConfigLoader::ensure(vector<MobileConfig> &mobiles, const string &a, const string &b) {
	int lef = -1, rig = -1;
	for (size_t i = 0; i < mobiles.size() && (lef < 0 || rig < 0); ++i) {
		if (mobiles[i].brand == a)
			lef = i;
		if (mobiles[i].brand == b)
			rig = i;
	}
	if (lef < 0 || rig < 0) return false;
	if (lef > rig)
		std::swap(mobiles[lef], mobiles[rig]);
	return true;
}

ConfigLoader::ConfigLoader(const string& file_os, const string& file_browser, const string& file_mobile) {

	android_reg = pcrepp::Pcre("[Aa]ndroid ?\\d+(?:\\.\\d+)*[; ]*(.*)\\)", "g");

	std::ifstream fin;
	YAML::Parser parser;

	fin.open(file_os);
	parser.Load(fin);
	YAML::Node osesConfig;
	while (parser.GetNextDocument(osesConfig)) {
		for (size_t i = 0; i < osesConfig.size(); ++i) {
			string restr, name, version;
			osesConfig[i]["regex"] >> restr;
			osesConfig[i]["name"] >> name;
			osesConfig[i]["version"] >> version;
			OSConfig osConfig = (OSConfig) {
				pcrepp::Pcre(restr, "g"), name, version
			};
			oses.push_back(osConfig);
		}
	}
	fin.close();

	fin.open(file_browser);
	parser.Load(fin);
	YAML::Node browsersConfig;
	while (parser.GetNextDocument(browsersConfig)) {
		for (size_t i = 0; i < browsersConfig.size(); ++i) {
			string restr, name, version;
			browsersConfig[i]["regex"] >> restr;
			browsersConfig[i]["name"] >> name;
			browsersConfig[i]["version"] >> version;
			BrowserConfig browserConfig = (BrowserConfig) {
				pcrepp::Pcre(restr, "g"), name, version
			};
			browsers.push_back(browserConfig);
		}
	}
	fin.close();

	fin.open(file_mobile);
	parser.Load(fin);
	YAML::Node mobilesConfig;
	while (parser.GetNextDocument(mobilesConfig)) {
		for (YAML::Iterator it = mobilesConfig.begin(); it != mobilesConfig.end(); ++it) {
			string restr, brand;
			it.second()["regex"] >> restr;
			it.first() >> brand;
			
			MobileConfig mobileConfig;

			mobileConfig.reg   = pcrepp::Pcre(restr, "g");
			mobileConfig.brand = brand;
			if (const YAML::Node *type = it.second().FindValue("device")) {
				*type >> mobileConfig.type;
			}

			//trace(brand, restr, mobileConfig.type);
			for (size_t i = 0; i < it.second()["models"].size(); ++i) {
				MobileConfig::Model model;
				it.second()["models"][i]["regex"] >> restr;
				model.reg = pcrepp::Pcre(restr, "g");
				it.second()["models"][i]["model"] >> model.model;
				if (const YAML::Node *type = it.second()["models"][i].FindValue("device")) {
					*type >> model.type;
				}
				mobileConfig.models.push_back(model);
			}

			mobiles.push_back(mobileConfig);
		}
	}
/*
struct MobileConfig {
	pcrepp::Pcre reg;
	string brand, type;
	struct Model {
		pcrepp::Pcre reg;
		string type, model;
	};
	vector<Model> models;
};
*/


	assert(ensure(mobiles, "Zhuomi", "Xiaomi"));
	assert(ensure(mobiles, "Meimi", "Xiaomi"));
	assert(ensure(mobiles, "ZZMisc", "GiONEE"));
	assert(ensure(mobiles, "ZZMisc", "ASUS"));


	for (size_t i = 0; i < mobiles.size(); i++) {
		for  (int j = 0; j < mobiles[i].models.size(); j++) {
			printf("%s %s\n",mobiles[i].brand.c_str(),
					mobiles[i].models[j].model.c_str());
		}
	}
}

}
