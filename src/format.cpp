#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) { 
    std::ostringstream elapsedTime;
    long hours = seconds / 3600;
    long rest = seconds % 3600;
    long minutes = rest / 60;
    rest = rest % 60;
    elapsedTime << std::setfill('0') << std::setw(2) << hours  << ":"  << std::setfill('0') << std::setw(2) << 		minutes << ":" << std::setfill('0') << std::setw(2) << rest;
    return elapsedTime.str();
     }