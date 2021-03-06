#include "parser.h"

using UAConfig::ConfigLoader;
using std::string;
using namespace UAParser;

/*
 *     In order to use this class, we first need to initialize a ConfigLoader,
 * and pass it as the first argument to Parser.
 */


Parser::Parser() {}

Parser::Parser(ConfigLoader *loader) {
	setConfig(loader);
}

Parser::Parser(const string &text) {
	setText(text);
}

Parser::Parser(ConfigLoader *loader, const string &text) {
	setConfig(loader);
	setText(text);
}

void Parser::setConfig(ConfigLoader *loader) {
	config = loader;
	configLoaded = true;
}

string Parser::getText() const { return text; }

void Parser::setText(const string &_text) { text = trim(_text); }

void Parser::chkConfig() {
	Assert(configLoaded, "You must load config first.");
}

string Parser::trim(const string &str) {
	int start = 0, len = str.length();
	for (string::const_iterator it = str.begin(); it != str.end() && isspace(*it); ++it, ++start);
	for (string::const_reverse_iterator it = str.rbegin(); it != str.rend() && isspace(*it); ++it, --len);
	return str.substr(start, len - start);
}

int Parser::expand(const pcrepp::Pcre *reg, const string &pattern, string &res) {
	bool escaped = false;
	res.clear();
	int pos;
	for (string::const_iterator it = pattern.begin(); it != pattern.end(); ++it) {
		if (escaped && isdigit(*it)) {
			pos = *it - '0';
			//Assert(0 < pos && pos <= reg->matches(), pos, reg->matches());
			Try(0 < pos && pos <= reg->matches());
			res += reg->get_match(pos - 1);
			escaped = false;
			continue;
		}
		if (*it == '\\') escaped = true;
		else res.push_back(*it);
	}
	return 0;
}

/*
 *     This function is like the function re.expand in Python,
 * if failed to expand, the result will equal to failstr.
 *
 * reg: the matched regex,
 * pattern: the pattern of the goal str,
 * res: the result we get
 * fail: if we fail to expand, res = failstr
 */

void Parser::expand
(
	const pcrepp::Pcre *reg, const string &pattern,
	string &res, const string &failstr
)
{
	if (expand(reg, pattern, res) < 0)
		res = failstr;
}

/*
 * This function replace all of char s to char t
 */

int Parser::replaceAll(string &str, char s, char t) {
	int count = 0;
	for (string::iterator it = str.begin(); it != str.end(); ++it)
		if (*it == s) *it = t, ++count;
	return count;
}

OS_t Parser::getOS() {
	chkConfig();
	if (!OS.isNull) {
		return OS;
	}
	OS.name = text;
	OS.version = "";

	for (size_t i = 0; i < config->oses.size(); ++i) {
		pcrepp::Pcre reg = config->oses[i].reg;

		if (reg.search(text)) {
			expand(&reg, config->oses[i].name, OS.name, text);
			expand(&reg, config->oses[i].version, OS.version, "");
			break;
		}

	}

	replaceAll(OS.version, '_', '.');
	OS.isNull = false;
	return OS;
}

Browser_t Parser::getBrowser() {
	chkConfig();
	if (!browser.isNull) {
		return browser;
	}
	size_t pos = text.find(' ');
	if (pos == string::npos && text.find(';') != string::npos)
		pos = text.find(';');
	string prefix = text.substr(0, pos);
	browser.name = prefix;
	browser.version = "";

	for (size_t i = 0; i < config->browsers.size(); ++i) {
		pcrepp::Pcre reg = config->browsers[i].reg;

		if (reg.search(text)) {
			expand(&reg, config->browsers[i].name, browser.name, prefix);
			expand(&reg, config->browsers[i].version, browser.version, "");
			break;
		}
	}

	replaceAll(browser.version, '_', '.');
	browser.isNull = false;
	return browser;
}

Mobile_t Parser::getMobile() {
	chkConfig();
	if (!mobile.isNull) {
		return mobile;
	}
	mobile = (Mobile_t){ text, "", "" };
	
	for (size_t i = 0; i < config->mobiles.size(); ++i) {
		pcrepp::Pcre reg = config->mobiles[i].reg;

		if (reg.search(text)) {
			mobile.brand = config->mobiles[i].brand;
			mobile.type = config->mobiles[i].type.length() > 1 ? config->mobiles[i].type : "";
			
			for (size_t j = 0; j < config->mobiles[i].models.size(); ++j) {
				reg = config->mobiles[i].models[j].reg;

				if (reg.search(text)) {
					mobile.type= config->mobiles[i].models[j].type.length() > 1
						? config->mobiles[i].models[j].type : mobile.type;
					expand(&reg, config->mobiles[i].models[j].model, mobile.model, text);
					goto done;
				}
			}
		}
	}

done:
	// 对杂牌的特殊处理
	if (mobile.brand == "ZZMisc") {
		mobile.brand = mobile.model;
		mobile.model = mobile.type;
		mobile.type = "smartphone";
		if (mobile.model == "smartphone")
			mobile.model = "";
	}

	/*
	 * Do some polish
	 */
	
	if (mobile.brand.substr(0, 7) == "Mozilla") {
		mobile = Mobile_t("", "PC", mobile.brand);
	} else if (mobile.brand.substr(0, 6) == "Dalvik") {
		mobile = Mobile_t("", "smartphone", mobile.brand);
		pcrepp::Pcre reg = config->android_reg;
		if (reg.search(mobile.model)) {
			int pos;
			string tmp = reg.get_match(0);
			if ((pos = tmp.find(' ')) >= 0) {
				mobile.brand = tmp.substr(0, pos);
				mobile.model = tmp.substr(pos + 1);
			}
		}
	}

	if (mobile.model.substr(0, 7) == "Mozilla") {
		mobile.type = "PC";
		mobile.model = "";
	} else if (mobile.model.substr(0, 7) == "Dalvik") {
		mobile.type = "smartphone";
		pcrepp::Pcre reg = config->android_reg;
		if (reg.search(mobile.model)) {
			int pos;
			string tmp = reg.get_match(0);
			if ((pos = tmp.find(' ')) >= 0)
				mobile.model = tmp.substr(pos + 1);
		}
	}

	/* 这里为大多数情况做一些强制性判断 */
	if (!OS.isNull && OS.name == "iOS" && mobile.brand != "Apple") {
		mobile.brand = "Apple";
		mobile.type = "smartphone";
		mobile.model = "iPhone";
	}

	/* DO more polish */
	polish(mobile);

	/*
	 * 如果brand, model都没有识别出来
	 * 并且OS识别为Android或者Windows Phone
	 * 就将type=PC的改成type=smartphone
	 */

	if (mobile.model.length() < 1 && mobile.brand.length() < 1) {
		if (this->OS.name == "Android" || this->OS.name == "Windows Phone") {
			mobile.type = "smartphone";
		}
	}

	replaceAll(mobile.model, '_', '-');
	mobile.isNull = false;
	return mobile;
}


/*
Coolpad Y70-c
Coolpad 
rules:
(1). trim空格和tab还有-_+
(2). 开头'+Coolpad%2B'去掉
(3). 把'Coolpad '头去掉
(4). 最后一个字符toupper
(5). 中间+,-,' '都替换成-

Huawei,Vivo:
(1). +HUAWEI%2B去掉
(2). 开头是Huawei或者HUAWEI或者Vivo就去掉然后trim一下

所有手机
结尾是Build就去掉并trim
*/

inline bool myisspace(char ch) {
	return ch == ' ' || ch == '\t' || ch == '+' || ch == '-' || ch == '_';
}

string mytrim(string &str) {
	int start = 0, len = str.length();
	for (string::const_iterator it = str.begin(); it != str.end() && myisspace(*it); ++it, ++start);
	for (string::const_reverse_iterator it = str.rbegin(); it != str.rend() && myisspace(*it); ++it, --len);
	return str.substr(start, len - start);
}

void cutPrefix(string &str, const string &prefix) {
	if (str.length() < prefix.length()) return;
	if (str.substr(0, prefix.length()) == prefix) {
		str = str.substr(prefix.length(), str.length() - prefix.length());
		str = mytrim(str);
	}
}

void cutSuffix(string &str, const string &suffix) {
	if (str.length() < suffix.length()) return;
	if (str.substr(str.length() - suffix.length(), suffix.length()) == suffix) {
		str = str.substr(0, str.length() - suffix.length());
		str = mytrim(str);
	}
}

void Parser::polish(Mobile_t &mobile) {
	if (mobile.model.length() < 2) return;
	if (mobile.brand == "Coolpad") {
		mobile.model = mytrim(mobile.model);
		cutPrefix(mobile.model, "+Coolpad%2B");
		cutPrefix(mobile.model, "Coolpad ");
		{
			auto ch = &mobile.model[mobile.model.length() - 1];
			*ch = toupper(*ch);
		}
		for (size_t i = 0; i < mobile.model.length(); i++) {
			auto ch = &mobile.model[i];
			if (*ch == '+' || *ch == ' ' || *ch == '_')
				*ch = '-';
		}
	}
	if (mobile.brand == "Huawei" || mobile.brand == "Vivo") {
		cutPrefix(mobile.model, "+HUAWEI%2B");
		cutPrefix(mobile.model, "Huawei");
		cutPrefix(mobile.model, "HUAWEI");
		cutPrefix(mobile.model, "Vivo");
		cutPrefix(mobile.model, "vivo");
		cutSuffix(mobile.model, ") AppleWebKit");
		replaceAll(mobile.model, '+', ' ');
		cutSuffix(mobile.model, "C02B120");
	}
	if (mobile.brand == "Sony") {
		if (mobile.model.substr(0, 8) == "Xperia Z") {
			mobile.model = "Xperia Z";
		}
	}
	cutSuffix(mobile.model, "Build");
	mobile.model = mytrim(mobile.model);

}
