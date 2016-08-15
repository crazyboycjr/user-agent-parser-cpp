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
		trace(parser.getOS().toString(), parser.getBrowser().toString(),
				parser.getMobile().toString());
	}
}

int main() {
	doTest();
	return 0;
}
