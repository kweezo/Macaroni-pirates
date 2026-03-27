#pragma once

#define MILLIS std::chrono::system_clock::now().time_since_epoch().count() 
#define NS 1000000

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900