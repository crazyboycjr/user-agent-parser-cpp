#include <cstdio>
#include <parser.h>
#include <string>

void doTest() {

	const int MAXN = 10000;
	char teststr[MAXN];

	std::string configPath = "/home/cjr/Developing/user-agent-parser-cpp/regexes/";
	UAConfig::ConfigLoader config(configPath + "oses.yml", configPath + "browsers.yml", configPath + "mobiles.yml");
	while (fgets(teststr, MAXN, stdin)) {
		teststr[strlen(teststr) - 1] = '\0';
		UAParser::Parser parser(&config, teststr);
		parser.getOS();
		parser.getBrowser();
		parser.getMobile();

		printf("1|||%s|||%s|||%s|||", parser.getMobile().brand.c_str(), parser.getMobile().type.c_str(), parser.getMobile().model.c_str());
		printf("%s|||%s|||", parser.getBrowser().version.c_str(), parser.getBrowser().name.c_str());
		printf("%s|||%s|||", parser.getOS().version.c_str(), parser.getOS().name.c_str());
		printf("%s\n", parser.getText().c_str());
//		printf("OSname:%s\t%s\tClient:%s\t%s\tDevice:%s\t%s\t%s\n", parser.getOS().name.c_str(), parser.getOS().version.c_str(),
//				parser.getBrowser().name.c_str(), parser.getBrowser().version.c_str(),
//				parser.getMobile().brand.c_str(), parser.getMobile().type.c_str(), parser.getMobile().model.c_str());
		//trace(parser.getOS().toString(), parser.getBrowser().toString(),
		//		parser.getMobile().toString());
	}
}

int main() {
	doTest();
	return 0;
}
