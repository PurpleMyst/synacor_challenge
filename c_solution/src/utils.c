#include <stdint.h>

#include "utils.h"

uint16_t merge(uint16_t lower, uint16_t higher) {
    return (higher << 8) | lower;
}
