# User-agent-parser-cpp
[![License](https://poser.pugx.org/piwik/device-detector/license)](https://packagist.org/packages/piwik/device-detector)

## Description
This tool reads the user-agent field from stdin line-by-line and prints the OS, browser/App and device information to stdout.
Also, it provides c++ API for printing results with a more specific format.
这个小工具从标准输入读取UA字段，识别其中操作系统信息，浏览器/App信息，设备信息，并将结果打印到标准输出上。
程序参考了 piwik 的 [device-detector](https://github.com/piwik/device-detector) 中的正则表达式部分，并对其进行了一定的修改和更新。

## Dependencies
You should install these dependencies.
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [PCRE++](http://www.daemon.de/PCRE)

由于在一些集群上g\+\+版本过于落后，对c++11支持很差，所以程序使用了较旧版本的yaml-cpp。
Downloads：
- yaml-cpp-0.3.0: https://github.com/jbeder/yaml-cpp/releases/tag/release-0.3.0
- pcre++-0.9.5: http://www.daemon.de/idisk/Apps/pcre++/pcre++-0.9.5.tar.gz

Optionally you can put `libpcre++.a` and `libyaml-cpp.a` under `lib` directory

## Build
Navigate into the source directory, and type:
```bash
make
```
Then`parser` will appear in current directory and `libuaparser.a` will appear in `./lib`.

## Usage
### Data Format
One UA per line.
```
<UA>
<UA>
<UA>
...
Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36
Dalvik/2.1.0 (Linux; U; Android 6.0; HUAWEI NXT-TL00 Build/HUAWEINXT-TL00)
WeChat/6.3.16.17 CFNetwork/758.4.3 Darwin/15.5.0
```
You can use UNIX pipe or some other methods to send these queries to `./parser`.
Or use the API provided by libuaparser.a

### Getting Started
Demo
```cpp
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
```
You can customize your ouput.
Demo Output:
```
parser.getOS().toString() : Android 6.0 |  parser.getBrowser().toString() : Android Browser  |  parser.getMobile().toString() : Huawei NXT-TL00
parser.getOS().toString() : iOS 9.3.2 |  parser.getBrowser().toString() : WeChat/6.3.16.17  |  parser.getMobile().toString() : Apple 
```

In this demo, firstly we use
```cpp
	UAConfig::ConfigLoader config("regexes/oses.yml", "regexes/browsers.yml", "regexes/mobiles.yml");
```
to load regexes config. Secondly we use
```cpp
UAParser::Parser parser(&config, teststr);
```
to pass its reference to `UAParser::Parser parser` as well as the string to be parsed.
We can also use
```cpp
parser.setConfig(&config);
parser.setText(teststr);
```
to pass config and the string to `parser`.
We __must__ reconstruct the `UAParser::Parser` class every time we need to parse a new UA string.
After both `setText()` and `setConfig()` are called, we can use
```cpp
parser.getOS()
parser.getBrowser()
parser.getMobile()
```
to obtain one of `OS_t`, `Browser_t` or `Mobile_t`, which contains the result of this recognition.
Both `OS_t` and `Browswer_t` have string member `name` and `version`.
`Mobile_t` has string member `brand`, `type` and `model`.
All of these structs have a method `toString()`, which concats its name + version or brand + model with a space and return a `std::string`.

## LICENSE
[The MIT License (MIT)](https://opensource.org/licenses/MIT)

Copyright (c) 2016 Jingrong Chen

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
