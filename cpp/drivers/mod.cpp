#include "dht22.hpp"
#include "mq135.hpp"

void drivers_init(void) {
    dht22_init();
    mq135_init();
}
