/*
 * Copyright (c) 2019 G.camp,
 *
 * Contact: Jin Seog Bang <seog814@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __POSITION_FINDER_RESOURCE_INTERNAL_H__
#define __POSITION_FINDER_RESOURCE_INTERNAL_H__

#include <peripheral_io.h>


#define PIN_MAX 40

struct _resource_s {
	int opened;
	peripheral_gpio_h sensor_h;
	void (*close) (int);
};
typedef struct _resource_s resource_s;

typedef void (*resource_read_cb)(double value, void *data);

struct _resource_read_cb_s {
	resource_read_cb cb;
	void *data;
	int pin_num;
};
typedef struct _resource_read_cb_s resource_read_s;

extern resource_s *resource_get_info(int pin_num);
extern void resource_close_all(void);

#endif /* __POSITION_FINDER_RESOURCE_INTERNAL_H__ */
