#include "imxrt.h"

namespace timer {

typedef void (*void_function_ptr)(void);

void setupPeriodic();
void setupPeriodicISR(uint8_t number, void_function_ptr function);

void startPeriodic();

} // namespace timer
