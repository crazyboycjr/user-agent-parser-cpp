#include <cstdio>
#include <parser.h>

void doTest() {

	const int MAXN = 10000;
	char teststr[MAXN];

	int cnt = 0;
	UAConfig::ConfigLoader config("regexes/oses.yml", "regexes/browsers.yml", "regexes/mobiles.yml");
	while (fgets(teststr, MAXN, stdin)) {
		teststr[strlen(teststr) - 1] = '\0';
		UAParser::Parser parser(&config, teststr);

		printf("%d\n", ++cnt);
		printf("OSname:%s\t%s\t", parser.getOS().name.c_str(), parser.getOS().version.c_str());
		printf("Client:%s\t%s\t", parser.getBrowser().name.c_str(), parser.getBrowser().version.c_str());
		printf("Device:%s\t%s\t%s\n", parser.getMobile().brand.c_str(), parser.getMobile().type.c_str(), parser.getMobile().model.c_str());
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
