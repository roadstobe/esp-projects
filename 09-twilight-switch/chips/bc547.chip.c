#include "wokwi-api.h"
#include <stdlib.h>

typedef struct {
  pin_t c;
  pin_t b;
} chip_state_t;

static void update(chip_state_t *s) {
  pin_mode(s->c, pin_read(s->b) ? OUTPUT_LOW : INPUT);
}

static void on_base_change(void *user_data, pin_t pin, uint32_t value) {
  update((chip_state_t *)user_data);
}

void chip_init(void) {
  chip_state_t *s = malloc(sizeof(chip_state_t));
  s->c = pin_init("C", INPUT);
  s->b = pin_init("B", INPUT);
  pin_init("E", INPUT);

  const pin_watch_config_t cfg = {
    .edge = BOTH,
    .pin_change = on_base_change,
    .user_data = s,
  };
  pin_watch(s->b, &cfg);
  update(s);
}