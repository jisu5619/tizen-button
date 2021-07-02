/*
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

#include <tizen.h>
#include <service_app.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <Ecore.h>

#include "st_things.h"
#include "log.h"
#include "sensor-data.h"
#include "resource.h"

#define JSON_PATH "device_def.json"

#define SENSOR_URI_ILLUMINANCE "/capability/illuminanceMeasurement/main/0"
#define SENSOR_KEY_ILLUMINANCE "illuminance"
#define SENSOR_KEY_RANGE "range"

#define SENSOR_URI_DOOR "/capability/doorControl/main/0"
#define SENSOR_KEY_DOOR "doorState"
#define SENSOR_POWER_INITIALIZING BLIND_DOWN

#define I2C_BUS_NUMBER (1)
#define SENSOR_GATHER_INTERVAL (1.0f)
#define PAGE_SCR (0)

typedef struct app_data_s {
	Ecore_Timer *getter_sw;
	sensor_data *sw_data;

} app_data;

static app_data *g_ad = NULL;

static inline int __get_sw(void *data, unsigned int *sw_value)
{
	int ret = 0;
	app_data *ad = data;
	unsigned int delay_usec = 200000; // 20mS delay
	retv_if(!ad, -1);
	retv_if(!ad->sw_data, -1);

/*
#if 1
	ret = resource_read_illuminance_sensor(I2C_BUS_NUMBER, sw_value);
	retv_if(ret != 0, -1);
#endif

	sensor_data_set_uint(ad->sw_data, *sw_value);
	_D2("sw value : %u", *sw_value);
*/
#if 1
	ret = resource_read_sw_sensor(20, &sw_value);
	retv_if(ret != 0, -1);
#endif

//	if (ret != 0) _E("Cannot read sensor value");

	_D2("Detected sw value is: %d", sw_value);

	// Send value to LED light
	// resource_write_led(26, value);
	sw_value = 1;
	if (sw_value)
	{
	// change to LED light

	resource_write_led(5, 1);
	resource_write_led(26, 1);
	sw_value = 0;

	}
	else
	{
	// change to LED light
	resource_write_led(5, 0);
	usleep(delay_usec);
	resource_write_led(5, 1);
	usleep(delay_usec);

	resource_write_led(26, 0);
	usleep(delay_usec);
	resource_write_led(26, 1);
	usleep(delay_usec);

	}
	return 0;
}

static Eina_Bool __sw_to_value(void *data)
{
	int ret = 0;
	unsigned int sw_value = 0;
	uint32_t value = 1;
	app_data *ad = data;
	resource_write_led(26, value); //debug led on
	if (!ad) {
		_E("failed to get app_data");
		service_app_exit();
	}
# if 1
	if (!ad->sw_data) {
		_E("failed to get sw_data");
		service_app_exit();
	}
	ret = __get_sw(ad, &sw_value);
	retv_if(ret != 0, ECORE_CALLBACK_RENEW);

#endif 
	
	return ECORE_CALLBACK_RENEW;
}

void gathering_stop(void *data)
{
	app_data *ad = data;

	ret_if(!ad);

	if (ad->getter_sw)
		ecore_timer_del(ad->getter_sw);
}

void gathering_start(void *data)
{
	app_data *ad = data;

	ret_if(!ad);

	gathering_stop(ad);

	ad->getter_sw = ecore_timer_add(SENSOR_GATHER_INTERVAL, __sw_to_value, ad);
	if (!ad->getter_sw)
		_E("Failed to add getter_sw");
}


static bool service_app_create(void *user_data)
{
	app_data *ad = (app_data *)user_data;

	ad->sw_data = sensor_data_new(SENSOR_DATA_TYPE_UINT);
	if (!ad->sw_data)
		return false;
	
	resource_write_led(5, 1);
	usleep(delay_usec);
	resource_write_led(5, 0);

	return true;
}

static void service_app_control(app_control_h app_control, void *user_data)
{

	gathering_start(user_data);

}

static void service_app_terminate(void *user_data)
{
	app_data *ad = (app_data *)user_data;

	resource_write_led(5, 0);

	resource_close_all();

	gathering_stop(ad);


	sensor_data_free(ad->sw_data);
	free(ad);
}

int main(int argc, char *argv[])
{
	app_data *ad = NULL;
	service_app_lifecycle_callback_s event_callback;

	ad = calloc(1, sizeof(app_data));
	retv_if(!ad, -1);

	g_ad = ad;

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	return service_app_main(argc, argv, &event_callback, ad);
}

