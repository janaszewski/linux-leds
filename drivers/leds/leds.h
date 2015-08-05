/*
 * LED Core
 *
 * Copyright 2005 Openedhand Ltd.
 *
 * Author: Richard Purdie <rpurdie@openedhand.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef __LEDS_H_INCLUDED
#define __LEDS_H_INCLUDED

#include <linux/rwsem.h>
#include <linux/leds.h>

static inline void led_set_brightness_async(struct led_classdev *led_cdev,
					    enum led_brightness value)
{
	int ret;
	/*
	 * Drivers that implement brightness_set op in the old manner also
	 * don't set LED_BRIGHTNESS_BLOCKING flag. They use work queue
	 * internally in case they set brightness in a blocking way, thus we
	 * avoid scheduling another work queue task by the LED core.
	 */
	if (led_cdev->brightness_set &&
	   !(led_cdev->flags & LED_BRIGHTNESS_BLOCKING)) {
		ret = led_set_brightness_sync(led_cdev, value);
		if (ret < 0)
			dev_err(led_cdev->dev, "cannot set led brightness %d\n", ret);
		return;
	}

	led_cdev->delayed_set_value = value;
	schedule_work(&led_cdev->set_brightness_work);
}

static inline int led_get_brightness(struct led_classdev *led_cdev)
{
	return led_cdev->brightness;
}

void led_stop_software_blink(struct led_classdev *led_cdev);

extern struct rw_semaphore leds_list_lock;
extern struct list_head leds_list;

#endif	/* __LEDS_H_INCLUDED */
