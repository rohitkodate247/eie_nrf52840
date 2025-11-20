/*
 * Lesson 4 – Debug and Buttons
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

/* Simple polling period for the main loop */
#define SLEEP_MS 10

/* Use the board aliases from devicetree */
#define LED0_NODE DT_ALIAS(led0)
#define SW0_NODE DT_ALIAS(sw0)

/* Create GPIO descriptor structs for LED and button */
static const struct gpio_dt_spec led0 =
    GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec button0 =
    GPIO_DT_SPEC_GET(SW0_NODE, gpios);

int main(void)
{
  int ret;
  bool last_pressed = false;

  /* Sanity-check that both devices exist and are ready */
  if (!device_is_ready(led0.port))
  {
    printk("LED device not ready\n");
    return 0;
  }

  if (!device_is_ready(button0.port))
  {
    printk("Button device not ready\n");
    return 0;
  }

  /* Configure LED as output, initially OFF */
  ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
  if (ret < 0)
  {
    printk("Failed to configure LED (err %d)\n", ret);
    return 0;
  }

  /* Configure button as input (board’s pull-up is already handled
   * by devicetree on the nRF52840 DK)
   */
  ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
  if (ret < 0)
  {
    printk("Failed to configure button (err %d)\n", ret);
    return 0;
  }

  printk("Lesson 4: Debug + Button example started\n");

  while (1)
  {
    /* Read current button state */
    int value = gpio_pin_get_dt(&button0);
    bool pressed = (value > 0);

    /* Edge detection: react only when state changes */
    if (pressed != last_pressed)
    {
      last_pressed = pressed;

      if (pressed)
      {
        /* Turn LED on when pressed */
        gpio_pin_set_dt(&led0, 1);
        printk("Button PRESSED\n");
        LOG_INF("Button pressed, LED ON");
      }
      else
      {
        /* Turn LED off when released */
        gpio_pin_set_dt(&led0, 0);
        printk("Button RELEASED\n");
        LOG_INF("Button released, LED OFF");
      }
    }

    k_msleep(SLEEP_MS);
  }

  return 0;
}
