#pragma once

#include <iostream>
#include <string>


#define LOG(str) \
    std::cout << __FILE__ << " : " << __LINE__ << " " << \
    __TIMESTAMP__ << " : " << str << std::endl;
    