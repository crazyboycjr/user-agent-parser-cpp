/*
 * parser.h
 *
 *  Created on: Aug 5, 2016
 *      Author: root
 */

#ifndef UA_PARSER_H_
#define UA_PARSER_H_

#include "common.h"
#include "configloader.h"
#include "pcre++.h"

namespace UAParser {

using UAConfig::ConfigLoader;
using std::string;

enum ResultType {
	unknown = 0,
	PC,
	mobile,
	tablet,
	tv
};

struct OS_t {
	bool isNull;
	OS_t() { isNull = true; }
	string name, version;
	string toString() const {
		return name + " " + version;
	}
};
struct Browser_t {
	bool isNull;
	Browser_t() { isNull = true; }
	string name, version;
	string toString() const {
		return name + " " + version;
	}
};
struct Mobile_t {
	bool isNull;
	Mobile_t() { isNull = true; }
	Mobile_t(string _brand, string _type, string _model):
		brand(_brand), type(_type), model(_model) { isNull = true; }

	string brand, type, model;
	string toString() const {
		return brand + " " + model;
	}
};

class Parser {
public:
	Parser();
	Parser(ConfigLoader *loader);
	Parser(const string &text);
	Parser(ConfigLoader *loader, const string &text);

	void setConfig(ConfigLoader *loader);

	string getText() const;
	void setText(const string &_text);
	OS_t getOS();
	Browser_t getBrowser();
	Mobile_t getMobile();

private:
	string trim(const string &);
	void chkConfig();
	int expand(const pcrepp::Pcre *, const string &, string &);
	void expand(const pcrepp::Pcre *, const string &, string &, const string &);
	int replaceAll(string &, char, char);

private:
	bool configLoaded;
	ConfigLoader *config;
	string text;
	ResultType type;
	OS_t OS;
	Browser_t browser;
	Mobile_t mobile;
};

}

#endif /* CXX_UA_PARSER_H_ */
