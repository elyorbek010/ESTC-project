#include "modules/logs.h"
#include "modules/button.h"
#include "modules/hsv_rgb.h"
#include "modules/pwm.h"
#include "modules/nvmc.h"
#include "modules/my_usb.h"

int main(void)
{
  logs_init();

  usb_main();
}