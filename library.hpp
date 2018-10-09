#pragma once

#ifdef DEBUG 

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define logger_info(...) \
  { \
    int time = now(); \
    int hour = (time/3600)%24; \
    time = time%3600; \
    int min = time/60; \
    time = time%60; \
    int sec = time; \
    print(hour, ":", min, ":", sec); \
    print(" ", __FILENAME__, ":", __LINE__, " (Func:", __FUNCTION__, ") "); \
    print(__VA_ARGS__, " ||| "); \
  }
#else

#define logger_info(...){}

#endif



bool isdecnum(const std::string data)
{
	for(size_t i=0; i<data.length(); i++)
	{
		if (isdigit(data[i]) == 0) {
			return false;
		}
	}

	return true;
}


bool isdecnum(const char* data)
{
	if (data == nullptr) {
		return false;
	}

	std::string str(data);

	return isdecnum(str);
}

