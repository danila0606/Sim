#include <stdint.h>

#define WIDTH 1920
#define HEIGHT 1080

extern "C" void put_pixel(uint32_t x, uint32_t y, uint32_t r, uint32_t g, uint32_t b, uint32_t a);
extern "C" void flush();
extern "C" void init();
extern "C" int is_window_opened();
extern "C" int get_time_milliseconds();
extern "C" int gmul(int a, int b);
extern "C" int gdiv(int a, int b);