/*
 * Copyright (C) 2012 Samsung Electronics, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/platform_device.h>
#include <linux/i2c-gpio.h>
#include <linux/i2c.h>
#include <linux/regulator/machine.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/gpio.h>

#include <asm/io.h>

#include <plat/devs.h>
#include <plat/iic.h>
#include <plat/gpio-cfg.h>

#include <mach/regs-pmu.h>
#include <mach/irqs.h>

#include <linux/mfd/samsung/core.h>

#include "board-universal5420.h"

#include "common.h"

#if defined(CONFIG_S3C_ADC)
#include <plat/adc.h>
#endif

#if defined(CONFIG_BATTERY_SAMSUNG)
#include <linux/battery/sec_battery.h>
#include <linux/battery/sec_fuelgauge.h>
#include <linux/battery/sec_charger.h>
#include <linux/battery/charger/max77803_charger.h>

#define SEC_BATTERY_PMIC_NAME ""

static struct s3c_adc_client* adc_client;

#if 0
static unsigned int sec_bat_recovery_mode;
#endif

bool is_wpc_cable_attached = false;
bool is_ovlo_state = false;

/* For TEMP*/
/*
#define	GPIO_FUEL_ALERT			EXYNOS5420_GPX1(5)
#define	GPIO_FUEL_SCL_18V		EXYNOS5420_GPB0(4)
#define	GPIO_FUEL_SDA_18V		EXYNOS5420_GPB0(3)
#define	GPIO_TA_INT				EXYNOS5420_GPY7(3)
*/
unsigned int lpcharge;
EXPORT_SYMBOL(lpcharge);

#if defined(CONFIG_N1A)
static sec_charging_current_t charging_current_table[] = {
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1800,	2200,	400,	170},
	{475,	480,	400,	170},
	{1000,	1000,	400,	170},
	{1000,	1000,	400,	170},
	{475,	480,	400,	170},
	{1800,	2200,	400,	170},
	{0,	0,	0,	0},
	{650,	720,	400,	170},
	{1800,	2200,	400,	170},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1000,	1000,	400,	170},/* LAN hub */
	{460,	460,	400,	170},/*mhl usb*/
	{0, 0,	0,	0},/*power sharing*/
	{900,	1200,	400,	170}, /* SMART_OTG */
	{1500,	1500,	400,	170}, /* SMART_NOTG */
	{1400,	1400,	400,	170}, /* MDOCK_TA */
	{450,	450,	400,	170}  /* MDOCK_USB */
};
#elif defined(CONFIG_KLIMT)
static sec_charging_current_t charging_current_table[] = {
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1800,	2200,	250,	150},
	{475,	480,	250,	150},
	{1000,	1000,	250,	150},
	{1000,	1000,	250,	150},
	{475,	480,	250,	150},
	{1800,	2200,	250,	150},
	{0,	0,	0,	0},
	{650,	720,	250,	150},
	{1800,	2200,	250,	150},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1000,	1000,	250,	150},/* LAN hub */
	{460,	460,	250,	150},/*mhl usb*/
	{0, 0,	0,	0},/*power sharing*/
	{900,	1200,	250,	150}, /* SMART_OTG */
	{1500,	1500,	250,	150}, /* SMART_NOTG */
	{1400,	1400,	250,	150}, /* MDOCK_TA */
	{450,	450,	250,	150}  /* MDOCK_USB */
};
#elif defined(CONFIG_CHAGALL)
static sec_charging_current_t charging_current_table[] = {
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1800,	2200,	400,	150},
	{475,	480,	400,	150},
	{1000,	1000,	400,	150},
	{1000,	1000,	400,	150},
	{475,	480,	400,	150},
	{1800,	2200,	400,	150},
	{0,	0,	0,	0},
	{650,	720,	400,	150},
	{1800,	2200,	400,	150},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1000,	1000,	400,	150},/* LAN hub */
	{460,	460,	400,	150},/*mhl usb*/
	{0, 0,	0,	0},/*power sharing*/
	{900,	1200,	400,	150}, /* SMART_OTG */
	{1500,	1500,	400,	150}, /* SMART_NOTG */
	{1400,	1400,	400,	150}, /* MDOCK_TA */
	{450,	450,	400,	150}  /* MDOCK_USB */
};

#else
static sec_charging_current_t charging_current_table[] = {
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1800,	2200,	250,	40*60},
	{475,	480,	250,	40*60},
	{1000,	1000,	250,	40*60},
	{1000,	1000,	250,	40*60},
	{475,	480,	250,	40*60},
	{1800,	2200,	250,	40*60},
	{0,	0,	0,	0},
	{650,	720,	250,	40*60},
	{1800,	2200,	250,	40*60},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1000,	1000,	250,	40*60},/* LAN hub */
	{460,	460,	250,	40*60},/*mhl usb*/
	{0, 0,	0,	0},/*power sharing*/
	{900,	1200,	250,	40*60}, /* SMART_OTG */
	{1500,	1500,	250,	40*60}, /* SMART_NOTG */
	{1400,	1400,	250,	40*60}, /* MDOCK_TA */
	{450,	450,	250,	40*60}  /* MDOCK_USB */
};
#endif

static bool sec_bat_adc_none_init(
		struct platform_device *pdev) {return true; }
static bool sec_bat_adc_none_exit(void) {return true; }
static int sec_bat_adc_none_read(unsigned int channel) {return 0; }

static bool sec_bat_adc_ap_init(struct platform_device *pdev)
{
	if (!adc_client) {
		adc_client = s3c_adc_register(pdev, NULL, NULL, 0);
		if (IS_ERR(adc_client))
			pr_err("ADC READ ERROR");
		else
			pr_info("%s: sec_bat_adc_ap_init succeed\n", __func__);
	}
	return true;
}
static bool sec_bat_adc_ap_exit(void)
{
	s3c_adc_release(adc_client);
	return true;
}
static int sec_bat_adc_ap_read(unsigned int channel)
{
	int data = -1;

	switch (channel) {
	case SEC_BAT_ADC_CHANNEL_TEMP:
		data = s3c_adc_read(adc_client, 4);
		break;
	case SEC_BAT_ADC_CHANNEL_TEMP_AMBIENT:
		data = 33000;
		break;
	case SEC_BAT_ADC_CHANNEL_INBAT_VOLTAGE:
		data = s3c_adc_read(adc_client, 2);
		break;
	}

	return data;
}

static bool sec_bat_adc_ic_init(
		struct platform_device *pdev) {return true; }
static bool sec_bat_adc_ic_exit(void) {return true; }
static int sec_bat_adc_ic_read(unsigned int channel) {return 0; }

static bool sec_bat_gpio_init(void)
{
	return true;
}

static bool sec_fg_gpio_init(void)
{
	s3c_gpio_cfgpin(GPIO_FUEL_ALERT, S3C_GPIO_INPUT);

	return true;
}

static bool sec_chg_gpio_init(void)
{
	/* For wireless charging */
	/*s3c_gpio_cfgpin(GPIO_WPC_INT, S3C_GPIO_INPUT);
	s3c_gpio_setpull(GPIO_WPC_INT, S3C_GPIO_PULL_NONE);*/
	return true;
}

static int sec_bat_is_lpm_check(char *str)
{
	if (strncmp(str, "charger", 7) == 0)
		lpcharge = 1;

	pr_info("%s: Low power charging mode: %d\n", __func__, lpcharge);

	return lpcharge;
}
__setup("androidboot.mode=", sec_bat_is_lpm_check);

#if defined(CONFIG_PREVENT_SOC_JUMP)
int fg_reset;
EXPORT_SYMBOL(fg_reset);
static int sec_bat_get_fg_reset(char *val)
{
	fg_reset = strncmp(val, "1", 1) ? 0 : 1;
	pr_info("%s, fg_reset:%d\n", __func__, fg_reset);
	return 1;
}
__setup("fg_reset=", sec_bat_get_fg_reset);
#endif

static bool sec_bat_is_lpm(void)
{
	return lpcharge;
}

int extended_cable_type;

static void sec_bat_initial_check(void)
{
	union power_supply_propval value;

	if (POWER_SUPPLY_TYPE_BATTERY < current_cable_type) {
		if (current_cable_type == POWER_SUPPLY_TYPE_POWER_SHARING) {
			value.intval = current_cable_type;
			psy_do_property("ps", set,
				POWER_SUPPLY_PROP_ONLINE, value);
		} else {
			value.intval = current_cable_type<<ONLINE_TYPE_MAIN_SHIFT;
			psy_do_property("battery", set,
				POWER_SUPPLY_PROP_ONLINE, value);
		}
	} else {
		psy_do_property("sec-charger", get,
				POWER_SUPPLY_PROP_ONLINE, value);
		if (value.intval == POWER_SUPPLY_TYPE_WPC) {
			value.intval =
				POWER_SUPPLY_TYPE_WPC<<ONLINE_TYPE_MAIN_SHIFT;
			psy_do_property("battery", set,
				POWER_SUPPLY_PROP_ONLINE, value);
		}
	}
}


static bool sec_bat_switch_to_check(void) {return true; }
static bool sec_bat_switch_to_normal(void) {return true; }

static int sec_bat_check_cable_callback(void)
{
	return current_cable_type;
}

static bool sec_bat_check_jig_status(void)
{
	if (gpio_get_value_cansleep(GPIO_IF_CON_SENSE))
		return true;
	else
		return false;
}

int mhl_class_usb = 300;
int mhl_class_500 = 400;
int mhl_class_900 = 700;
int mhl_class_1500 = 1300;

static int sec_bat_get_cable_from_extended_cable_type(
	int input_extended_cable_type)
{
	int cable_main, cable_sub, cable_power;
	int cable_type = POWER_SUPPLY_TYPE_UNKNOWN;
	union power_supply_propval value;
	int charge_current_max = 0, charge_current = 0;

	cable_main = GET_MAIN_CABLE_TYPE(input_extended_cable_type);
	if (cable_main != POWER_SUPPLY_TYPE_UNKNOWN)
		extended_cable_type = (extended_cable_type &
			~(int)ONLINE_TYPE_MAIN_MASK) |
			(cable_main << ONLINE_TYPE_MAIN_SHIFT);
	cable_sub = GET_SUB_CABLE_TYPE(input_extended_cable_type);
	if (cable_sub != ONLINE_SUB_TYPE_UNKNOWN)
		extended_cable_type = (extended_cable_type &
			~(int)ONLINE_TYPE_SUB_MASK) |
			(cable_sub << ONLINE_TYPE_SUB_SHIFT);
	cable_power = GET_POWER_CABLE_TYPE(input_extended_cable_type);
	if (cable_power != ONLINE_POWER_TYPE_UNKNOWN)
		extended_cable_type = (extended_cable_type &
			~(int)ONLINE_TYPE_PWR_MASK) |
			(cable_power << ONLINE_TYPE_PWR_SHIFT);

	switch (cable_main) {
	case POWER_SUPPLY_TYPE_CARDOCK:
		switch (cable_power) {
		case ONLINE_POWER_TYPE_BATTERY:
			cable_type = POWER_SUPPLY_TYPE_BATTERY;
			break;
		case ONLINE_POWER_TYPE_TA:
			switch (cable_sub) {
			case ONLINE_SUB_TYPE_MHL:
				cable_type = POWER_SUPPLY_TYPE_USB;
				break;
			case ONLINE_SUB_TYPE_AUDIO:
			case ONLINE_SUB_TYPE_DESK:
			case ONLINE_SUB_TYPE_SMART_NOTG:
			case ONLINE_SUB_TYPE_KBD:
				cable_type = POWER_SUPPLY_TYPE_MAINS;
				break;
			case ONLINE_SUB_TYPE_SMART_OTG:
				cable_type = POWER_SUPPLY_TYPE_CARDOCK;
				break;
			}
			break;
		case ONLINE_POWER_TYPE_USB:
			cable_type = POWER_SUPPLY_TYPE_USB;
			break;
		default:
			cable_type = current_cable_type;
			break;
		}
		break;
	case POWER_SUPPLY_TYPE_MISC:
		switch (cable_sub) {
		case ONLINE_SUB_TYPE_MHL:
			switch (cable_power) {
			case ONLINE_POWER_TYPE_BATTERY:
				cable_type = POWER_SUPPLY_TYPE_BATTERY;
				break;
			case ONLINE_POWER_TYPE_MHL_500:
				cable_type = POWER_SUPPLY_TYPE_MISC;
				charge_current_max = mhl_class_500;
				charge_current = mhl_class_500;
				break;
			case ONLINE_POWER_TYPE_MHL_900:
				cable_type = POWER_SUPPLY_TYPE_MISC;
				charge_current_max = mhl_class_900;
				charge_current = mhl_class_900;
				break;
			case ONLINE_POWER_TYPE_MHL_1500:
				cable_type = POWER_SUPPLY_TYPE_MISC;
				charge_current_max = mhl_class_1500;
				charge_current = mhl_class_1500;
				break;
			case ONLINE_POWER_TYPE_USB:
				cable_type = POWER_SUPPLY_TYPE_USB;
				charge_current_max = mhl_class_usb;
				charge_current = mhl_class_usb;
				break;
			default:
				cable_type = cable_main;
			}
			break;
		case ONLINE_SUB_TYPE_SMART_OTG:
			cable_type = POWER_SUPPLY_TYPE_USB;
			charge_current_max = 1000;
			charge_current = 1000;
			break;
		case ONLINE_SUB_TYPE_SMART_NOTG:
			cable_type = POWER_SUPPLY_TYPE_MAINS;
			charge_current_max = 1900;
			charge_current = 1600;
			break;
		default:
			cable_type = cable_main;
			charge_current_max = 0;
			break;
		}
		break;
	default:
		cable_type = cable_main;
		break;
	}

#if 0
	if (cable_type == POWER_SUPPLY_TYPE_WPC)
		is_wpc_cable_attached = true;
	else
		is_wpc_cable_attached = false;
#endif

	if (charge_current_max == 0) {
		charge_current_max =
			charging_current_table[cable_type].input_current_limit;
		charge_current =
			charging_current_table[cable_type].
			fast_charging_current;
	}
	value.intval = charge_current_max;
	psy_do_property(sec_battery_pdata.charger_name, set,
			POWER_SUPPLY_PROP_CURRENT_MAX, value);
	value.intval = charge_current;
	psy_do_property(sec_battery_pdata.charger_name, set,
			POWER_SUPPLY_PROP_CURRENT_AVG, value);
	return cable_type;
}

static bool sec_bat_check_cable_result_callback(
				int cable_type)
{
	current_cable_type = cable_type;

	switch (cable_type) {
	case POWER_SUPPLY_TYPE_USB:
		pr_info("%s set vbus applied\n",
				__func__);
		break;
	case POWER_SUPPLY_TYPE_BATTERY:
		pr_info("%s set vbus cut\n",
			__func__);
		break;
	case POWER_SUPPLY_TYPE_MAINS:
		break;
	default:
		pr_err("%s cable type (%d)\n",
			__func__, cable_type);
		return false;
	}
	return true;
}

/* callback for battery check
 * return : bool
 * true - battery detected, false battery NOT detected
 */
static bool sec_bat_check_callback(void)
{
#if 0/*No need for Tablet*/
	struct power_supply *psy;
	union power_supply_propval value;

	psy = get_power_supply_by_name(("sec-charger"));
	if (!psy) {
		pr_err("%s: Fail to get psy (%s)\n",
			__func__, "sec-charger");
		value.intval = 1;
	} else {
		int ret;
		ret = psy->get_property(psy, POWER_SUPPLY_PROP_PRESENT, &(value));
		if (ret < 0) {
			pr_err("%s: Fail to sec-charger get_property (%d=>%d)\n",
				__func__, POWER_SUPPLY_PROP_PRESENT, ret);
			value.intval = 1;
		}
	}

	return value.intval;
#endif
	return true;
}
static bool sec_bat_check_result_callback(void) {return true; }

/* callback for OVP/UVLO check
 * return : int
 * battery health
 */
static int sec_bat_ovp_uvlo_callback(void)
{
	int health;
	health = POWER_SUPPLY_HEALTH_GOOD;

	return health;
}

static bool sec_bat_ovp_uvlo_result_callback(int health) {return true; }

/*
 * val.intval : temperature
 */
static bool sec_bat_get_temperature_callback(
		enum power_supply_property psp,
		union power_supply_propval *val) {return true; }

static bool sec_fg_fuelalert_process(bool is_fuel_alerted) {return true; }

#if defined(CONFIG_CHAGALL)
static const sec_bat_adc_table_data_t temp_table[] = {
	{  1100,	 1100 },
	{  705,	 950 },
	{  690,	 900 },
	{  673,	 850 },
	{  655,	 800 },
	{  635,	 750 },
	{  612,	 700 },
	{  584,	 650 },
	{  552,	 600 },
	{  529,	 550 },
	{  500,	 500 },
	{  400,	 400 },
	{  300,	 300 },
	{  200,	 200 },
	{  100,	 100 },
	{  0,	 0 },
	{ -100,  -100 },
	{ -200,  -200 },
	{ -300,  -300 },
};
#elif defined(CONFIG_KLIMT)
static const sec_bat_adc_table_data_t temp_table[] = {
	{  1100,	 1100 },
	{  750,  950 },
	{  725,  900 },
	{  700,  850 },
	{  680,  800 },
	{  660,  750 },
	{  635,  700 },
	{  605,  650 },
	{  570,  600 },
	{  550,	 557 },
	{  540,	 540 },
	{  530,	 530 },
	{  520,	 520 },
	{  400,	 400 },
	{  300,	 300 },
	{  200,	 200 },
	{  100,	 100 },
	{  0,	 0 },
	{ -100,  -100 },
	{ -200,  -200 },
	{ -300,  -300 },
};
#else/* N2A */
static const sec_bat_adc_table_data_t temp_table[] = {
	{  1100,	 1140 },
	{  1000,	 1040 },
	{  900,	 940 },
	{  800,	 840 },
	{  700,	 740 },
	{  600,	 640 },
	{  561,	 601 },
	{  560,	 590 },
	{  536,	 566 },
	{  535,	 555 },
	{  521,	 541 },
	{  520,	 520 },
	{  400,	 400 },
	{  300,	 300 },
	{  200,	 200 },
	{  100,	 100 },
	{  0,	 0 },
	{ -100,  -100 },
	{ -200,  -200 },
	{ -300,  -300 },
};
#endif

/* Data for Chagall */
static const sec_bat_adc_table_data_t inbat_adc_table[] = {
	{	3165,	435 },
	{	3150,	433 },
	{	3136,	431 },
	{	3121,	429 },
	{	3107,	427 },
	{	3092,	425 },
	{	3078,	423 },
	{	3063,	421 },
	{	3048,	419 },
	{	3034,	417 },
	{	3019,	415 },
	{	3005,	413 },
	{	2990,	411 },
	{	2976,	409 },
	{	2961,	407 },
	{	2947,	405 },
	{	2932,	403 },
	{	2918,	401 },
	{	2903,	399 },
	{	2888,	397 },
	{	2874,	395 },
	{	2859,	393 },
	{	2845,	391 },
	{	2830,	389 },
	{	2816,	387 },
	{	2801,	385 },
	{	2787,	383 },
	{	2772,	381 },
	{	2757,	379 },
	{	2743,	377 },
	{	2728,	375 },
	{	2714,	373 },
	{	2699,	371 },
	{	2685,	369 },
	{	2670,	367 },
	{	2656,	365 },
	{	2641,	363 },
	{	2626,	361 },
	{	2612,	359 },
	{	2597,	357 },
	{	2583,	355 },
	{	2568,	353 },
	{	2554,	351 },
	{	2539,	349 },
	{	2525,	347 },
	{	2510,	345 },
	{	2496,	343 },
	{	2481,	341 },
	{	2466,	339 },
	{	2452,	337 },
	{	2437,	335 },
	{	2423,	333 },
	{	2408,	331 },
	{	2394,	329 },
	{	2379,	327 },
	{	2365,	325 },
};

/* ADC region should be exclusive */
static sec_bat_adc_region_t cable_adc_value_table[] = {
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
};

static int polling_time_table[] = {
	10,	/* BASIC */
	30,	/* CHARGING */
	30,	/* DISCHARGING */
	30,	/* NOT_CHARGING */
	3600,	/* SLEEP */
};

/* for MAX17050 */
static struct battery_data_t adonis_battery_data[] = {
	/* SDI battery data */
	{
		.Capacity = 0x3F76,
		.low_battery_comp_voltage = 3600,
		.low_battery_table = {
			/* range, slope, offset */
			{-5000,	0,	0},	/* dummy for top limit */
			{-1250, 0,	3320},
			{-750, 97,	3451},
			{-100, 96,	3461},
			{0, 0,	3456},
		},
		.temp_adjust_table = {
			/* range, slope, offset */
			{47000, 122,	8950},
			{60000, 200,	51000},
			{100000, 0,	0},	/* dummy for top limit */
		},
		.type_str = "SDI",
	}
};

static void sec_bat_check_batt_id(void)
{
	int ret = 0;

	ret = s3c_adc_read(adc_client, 4);

#if !defined(CONFIG_CHAGALL) && !defined(CONFIG_KLIMT)
	/* SDI: +/-700, BYD: +/-1300, ATL: +2000 */
	if (ret > 1700) {
		sec_battery_pdata.vendor = "ATL ATL";
		adonis_battery_data[0].Capacity = 0x4074;
		adonis_battery_data[0].type_str = "ATL";
	} else if (ret > 1000) {
		sec_battery_pdata.vendor = "BYD BYD";
		adonis_battery_data[0].Capacity = 0x4010;
		adonis_battery_data[0].type_str = "BYD";
	}
#endif

	pr_info("%s: batt_type(%s), batt_id(%d), cap(0x%x), type(%s)\n",
		__func__, sec_battery_pdata.vendor, ret,
		adonis_battery_data[0].Capacity, adonis_battery_data[0].type_str);
}

sec_battery_platform_data_t sec_battery_pdata = {
	/* NO NEED TO BE CHANGED */
	.initial_check = sec_bat_initial_check,
	.bat_gpio_init = sec_bat_gpio_init,
	.fg_gpio_init = sec_fg_gpio_init,
	.chg_gpio_init = sec_chg_gpio_init,

	.is_lpm = sec_bat_is_lpm,
	.check_jig_status = sec_bat_check_jig_status,
	.check_cable_callback =
		sec_bat_check_cable_callback,
	.get_cable_from_extended_cable_type =
		sec_bat_get_cable_from_extended_cable_type,
	.cable_switch_check = sec_bat_switch_to_check,
	.cable_switch_normal = sec_bat_switch_to_normal,
	.check_cable_result_callback =
		sec_bat_check_cable_result_callback,
	.check_battery_callback =
		sec_bat_check_callback,
	.check_battery_result_callback =
		sec_bat_check_result_callback,
	.ovp_uvlo_callback = sec_bat_ovp_uvlo_callback,
	.ovp_uvlo_result_callback =
		sec_bat_ovp_uvlo_result_callback,
	.fuelalert_process = sec_fg_fuelalert_process,
	.get_temperature_callback =
		sec_bat_get_temperature_callback,

	.adc_api[SEC_BATTERY_ADC_TYPE_NONE] = {
		.init = sec_bat_adc_none_init,
		.exit = sec_bat_adc_none_exit,
		.read = sec_bat_adc_none_read
		},
	.adc_api[SEC_BATTERY_ADC_TYPE_AP] = {
		.init = sec_bat_adc_ap_init,
		.exit = sec_bat_adc_ap_exit,
		.read = sec_bat_adc_ap_read
		},
	.adc_api[SEC_BATTERY_ADC_TYPE_IC] = {
		.init = sec_bat_adc_ic_init,
		.exit = sec_bat_adc_ic_exit,
		.read = sec_bat_adc_ic_read
		},
	.cable_adc_value = cable_adc_value_table,
	.charging_current = charging_current_table,
	.polling_time = polling_time_table,
	.check_batt_id = sec_bat_check_batt_id,
	/* NO NEED TO BE CHANGED */

	.pmic_name = SEC_BATTERY_PMIC_NAME,

	.adc_check_count = 6,
	.adc_type = {
		SEC_BATTERY_ADC_TYPE_NONE,	/* CABLE_CHECK */
		SEC_BATTERY_ADC_TYPE_NONE,	/* BAT_CHECK */
		SEC_BATTERY_ADC_TYPE_AP,	/* TEMP */
		SEC_BATTERY_ADC_TYPE_NONE,	/* TEMP_AMB */
		SEC_BATTERY_ADC_TYPE_AP,	/* FULL_CHECK */
		SEC_BATTERY_ADC_TYPE_NONE,	/* VOLTAGE_NOW */
		SEC_BATTERY_ADC_TYPE_AP,	/* INBAT_VOLTAGE */
	},

	/* Battery */
	.vendor = "SDI SDI",
	.technology = POWER_SUPPLY_TECHNOLOGY_LION,
	.battery_data = (void *)adonis_battery_data,
	.bat_gpio_ta_nconnected = 0,
	.bat_polarity_ta_nconnected = 0,
	.bat_irq = 0,
	/*.bat_irq = IRQ_BOARD_IFIC_START + MAX77803_CHG_IRQ_BATP_I,*/
	.bat_irq_attr = IRQF_TRIGGER_FALLING | IRQF_EARLY_RESUME,
	.cable_check_type =
		SEC_BATTERY_CABLE_CHECK_NOUSBCHARGE |
		SEC_BATTERY_CABLE_CHECK_PSY,
	.cable_source_type =
		SEC_BATTERY_CABLE_SOURCE_EXTERNAL |
		SEC_BATTERY_CABLE_SOURCE_EXTENDED,
#if defined(CONFIG_N1A) || defined(CONFIG_KLIMT)
	.event_check = false,
#else
	.event_check = true,
#endif
	.event_waiting_time = 600,

	/* Monitor setting */
	.polling_type = SEC_BATTERY_MONITOR_ALARM,
	.monitor_initial_count = 3,

	/* Battery check */
	.battery_check_type = SEC_BATTERY_CHECK_NONE,
	.check_count = 0,
	/* Battery check by ADC */
	.check_adc_max = 0,
	.check_adc_min = 0,

	/* OVP/UVLO check */
	.ovp_uvlo_check_type = SEC_BATTERY_OVP_UVLO_CHGPOLLING,

	/* Temperature check */
	.thermal_source = SEC_BATTERY_THERMAL_SOURCE_FG,

	.temp_adc_table = temp_table,
	.temp_adc_table_size =
		sizeof(temp_table)/sizeof(sec_bat_adc_table_data_t),

	.temp_check_type = SEC_BATTERY_TEMP_CHECK_TEMP,
	.temp_check_count = 1,

#if defined(CONFIG_KLIMT)
	.QRTable00 = 0x7B00,
	.QRTable10 = 0x3700,
	.QRTable20 = 0x1402,
	.QRTable30 = 0x0F84,
#elif defined(CONFIG_CHAGALL)
	.QRTable00 = 0x8000,
	.QRTable10 = 0x3700,
	.QRTable20 = 0x0E0A,
	.QRTable30 = 0x098C,
#elif defined(CONFIG_N1A)
	.QRTable00 = 0x7000,
	.QRTable10 = 0x2F80,
	.QRTable20 = 0x1000,
	.QRTable30 = 0x0E00,
#endif

#if defined(CONFIG_N2A)
#if defined(CONFIG_TARGET_LOCALE_USA)
	.temp_high_threshold_event = 510,
	.temp_high_recovery_event = 460,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 510,
	.temp_high_recovery_normal = 460,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 510,
	.temp_high_recovery_lpm = 460,
	.temp_low_threshold_lpm = -50,
	.temp_low_recovery_lpm = 0,
#else
	.temp_high_threshold_event = 600,
	.temp_high_recovery_event = 460,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 600,
	.temp_high_recovery_normal = 460,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 600,
	.temp_high_recovery_lpm = 480,
	.temp_low_threshold_lpm = -50,
	.temp_low_recovery_lpm = 0,
#endif
#elif defined(CONFIG_CHAGALL)/*USA, Canna use csc files for setting*/
#if defined(CONFIG_TARGET_LOCALE_USA)
#if defined(CONFIG_CHAGALL_LTE)
#if defined(CONFIG_BATT_TEMP_TMO)
	.temp_high_threshold_event = 550,
	.temp_high_recovery_event = 500,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 550,
	.temp_high_recovery_normal = 500,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 550,
	.temp_high_recovery_lpm = 500,
	.temp_low_threshold_lpm = -50,
	.temp_low_recovery_lpm = 0,
#else
	.temp_high_threshold_event = 550,
	.temp_high_recovery_event = 500,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 550,
	.temp_high_recovery_normal = 500,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 550,
	.temp_high_recovery_lpm = 500,
	.temp_low_threshold_lpm = -50,
	.temp_low_recovery_lpm = 0,
#endif
#else
	.temp_high_threshold_event = 600,
	.temp_high_recovery_event = 460,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 530,
	.temp_high_recovery_normal = 460,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 530,
	.temp_high_recovery_lpm = 480,
	.temp_low_threshold_lpm = 0,
	.temp_low_recovery_lpm = 30,
#endif
#elif defined(CONFIG_TARGET_LOCALE_BMC)
	.temp_high_threshold_event = 550,
	.temp_high_recovery_event = 500,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 550,
	.temp_high_recovery_normal = 500,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 550,
	.temp_high_recovery_lpm = 500,
	.temp_low_threshold_lpm = -50,
	.temp_low_recovery_lpm = 0,
#else
	.temp_high_threshold_event = 550,
	.temp_high_recovery_event = 500,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 550,
	.temp_high_recovery_normal = 500,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 550,
	.temp_high_recovery_lpm = 500,
	.temp_low_threshold_lpm = -50,
	.temp_low_recovery_lpm = 0,
#endif
#elif defined(CONFIG_KLIMT)
#if defined(CONFIG_TARGET_LOCALE_USA)
	.temp_high_threshold_event = 530,
	.temp_high_recovery_event = 460,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 530,
	.temp_high_recovery_normal = 460,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 510,
	.temp_high_recovery_lpm = 460,
	.temp_low_threshold_lpm = -30,
	.temp_low_recovery_lpm = 0,
#else
	.temp_high_threshold_event = 550,
	.temp_high_recovery_event = 500,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 550,
	.temp_high_recovery_normal = 500,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 530,
	.temp_high_recovery_lpm = 480,
	.temp_low_threshold_lpm = -50,
	.temp_low_recovery_lpm = 0,
#endif
#else/*N1, */
#if defined(CONFIG_TARGET_LOCALE_USA)
	.temp_high_threshold_event = 500,
	.temp_high_recovery_event = 480,
	.temp_low_threshold_event = -20,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 500,
	.temp_high_recovery_normal = 480,
	.temp_low_threshold_normal = -20,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 500,
	.temp_high_recovery_lpm = 480,
	.temp_low_threshold_lpm = -20,
	.temp_low_recovery_lpm = 0,
#else
	.temp_high_threshold_event = 600,
	.temp_high_recovery_event = 430,
	.temp_low_threshold_event = -50,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 600,
	.temp_high_recovery_normal = 430,
	.temp_low_threshold_normal = -50,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 600,
	.temp_high_recovery_lpm = 400,
	.temp_low_threshold_lpm = -50,
	.temp_low_recovery_lpm = 0,
#endif
#endif

#if defined(CONFIG_CHAGALL) || defined(CONFIG_KLIMT)
	.inbat_adc_table = inbat_adc_table,
	.inbat_adc_table_size =
		sizeof(inbat_adc_table)/sizeof(sec_bat_adc_table_data_t),
#endif

#if defined(CONFIG_N1A)
	.full_check_type = SEC_BATTERY_FULLCHARGED_FG_CURRENT,
	.full_check_type_2nd = SEC_BATTERY_FULLCHARGED_NONE,
#elif defined(CONFIG_KLIMT)
	.full_check_type = SEC_BATTERY_FULLCHARGED_CHGPSY,
	.full_check_type_2nd = SEC_BATTERY_FULLCHARGED_NONE,
#elif defined(CONFIG_CHAGALL)
	.full_check_type = SEC_BATTERY_FULLCHARGED_CHGPSY,
	.full_check_type_2nd = SEC_BATTERY_FULLCHARGED_NONE,
#else
	.full_check_type = SEC_BATTERY_FULLCHARGED_CHGPSY,
	.full_check_type_2nd = SEC_BATTERY_FULLCHARGED_TIME,
#endif
	.full_check_count = 1,
	.full_check_adc_1st = 265,/*200*/
	.full_check_adc_2nd = 265,/*200*/
	.chg_gpio_full_check = 0,
	.chg_polarity_full_check = 1,
	.full_condition_type = SEC_BATTERY_FULL_CONDITION_SOC |
		SEC_BATTERY_FULL_CONDITION_NOTIMEFULL |
		SEC_BATTERY_FULL_CONDITION_VCELL,
#if defined(CONFIG_CHAGALL) || defined(CONFIG_KLIMT)
#if defined(CONFIG_CHAGALL)
	.full_condition_soc = 93,
#else
	.full_condition_soc = 95,
#endif
	.full_condition_vcell = 4250,
#else
	.full_condition_soc = 97,
	.full_condition_vcell = 4250,
#endif
	.recharge_check_count = 2,
	.recharge_condition_type =
		SEC_BATTERY_RECHARGE_CONDITION_VCELL |
		SEC_BATTERY_RECHARGE_CONDITION_AVGVCELL,
	.recharge_condition_soc = 98,
	.recharge_condition_avgvcell = 4150,
	.recharge_condition_vcell = 4280,
#if defined(CONFIG_BATTERY_SWELLING)
#if defined(CONFIG_CHAGALL) || defined(CONFIG_KLIMT)
	.swelling_high_temp_block = 450,
	.swelling_high_temp_recov = 400,
	.swelling_low_temp_block = 100,
	.swelling_low_temp_recov = 150,
	.swelling_normal_float_voltage = 4350,
	.swelling_drop_float_voltage = 4200,
	.swelling_high_rechg_voltage = 4150,
	.swelling_low_rechg_voltage = 4050,
#if defined(CONFIG_SW_SELF_DISCHARGING)
	.self_discharging_temp_block = 600,
	.self_discharging_volt_block = 4250,
	.self_discharging_temp_recov = 550,
	.self_discharging_temp_pollingtime = 400,
#endif
#else
	.swelling_high_temp_block = 450,
	.swelling_high_temp_recov = 400,
	.swelling_low_temp_block = 100,
	.swelling_low_temp_recov = 150,
	.swelling_drop_float_voltage = 4200,
	.swelling_normal_float_voltage = 4350,
	.swelling_high_rechg_voltage = 4150,
	.swelling_low_rechg_voltage = 4050,
	.swelling_block_time = 600,
#endif
#endif
	.charging_total_time = 10 * 60 * 60,
	.recharging_total_time = 90 * 60,
	.charging_reset_time = 0,

	/* Fuel Gauge */
	.fg_irq = GPIO_FUEL_ALERT,
	.fg_irq_attr =
		IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	.fuel_alert_soc = 1,
	.repeated_fuelalert = false,
	.capacity_calculation_type =
        SEC_FUELGAUGE_CAPACITY_TYPE_RAW |
        SEC_FUELGAUGE_CAPACITY_TYPE_SCALE |
#if defined(CONFIG_PREVENT_SOC_JUMP)
        SEC_FUELGAUGE_CAPACITY_TYPE_DYNAMIC_SCALE |
        SEC_FUELGAUGE_CAPACITY_TYPE_ATOMIC |
        SEC_FUELGAUGE_CAPACITY_TYPE_SKIP_ABNORMAL,
#else
        SEC_FUELGAUGE_CAPACITY_TYPE_DYNAMIC_SCALE,
        /* SEC_FUELGAUGE_CAPACITY_TYPE_ATOMIC, */
#endif
#if defined(CONFIG_KLIMT) || defined(CONFIG_CHAGALL)
	.capacity_max = 990,
#else
	.capacity_max = 1000,
#endif
	.capacity_max_margin = 50,
	.capacity_min = 0,

	/* Charger */
	.charger_name = "sec-charger",
	.chg_gpio_en = 0,
	.chg_polarity_en = 0,
	.chg_gpio_status = 0,
	.chg_polarity_status = 0,
	.chg_irq = 0,
	.chg_irq_attr = IRQF_TRIGGER_FALLING,
	.chg_float_voltage = 4350,
};

#define SEC_FG_I2C_ID	18

static struct platform_device sec_device_battery = {
	.name = "sec-battery",
	.id = -1,
	.dev.platform_data = &sec_battery_pdata,
};

static struct i2c_gpio_platform_data gpio_i2c_data_fg = {
	.sda_pin = GPIO_FUEL_SDA_18V,
	.scl_pin = GPIO_FUEL_SCL_18V,
};

struct platform_device sec_device_fg = {
	.name = "i2c-gpio",
	.id = SEC_FG_I2C_ID,
	.dev.platform_data = &gpio_i2c_data_fg,
};

static struct i2c_board_info sec_brdinfo_fg[] __initdata = {
	{
		I2C_BOARD_INFO("sec-fuelgauge",
			SEC_FUELGAUGE_I2C_SLAVEADDR),
		.platform_data	= &sec_battery_pdata,
	},
};

static struct platform_device *universal5420_battery_devices[] __initdata = {
	&sec_device_fg,
	&sec_device_battery,
};

static void charger_gpio_init(void)
{
	s3c_gpio_cfgpin(GPIO_TA_INT, S3C_GPIO_INPUT);
	s3c_gpio_setpull(GPIO_TA_INT, S3C_GPIO_PULL_NONE);

	s3c_gpio_cfgpin(GPIO_IF_CON_SENSE, S3C_GPIO_INPUT);
	s3c_gpio_setpull(GPIO_IF_CON_SENSE, S3C_GPIO_PULL_DOWN);
	/*sec_battery_pdata.bat_irq = gpio_to_irq(GPIO_TA_INT);*/
#if 0
	sec_battery_pdata.chg_irq = gpio_to_irq(GPIO_CHG_INT);
#endif
}

#if 0
static int __init sec_bat_current_boot_mode(char *mode)
{
	/*
	*	1 is recovery booting
	*	0 is normal booting
	*/

	if (strncmp(mode, "1", 1) == 0)
		sec_bat_recovery_mode = 1;
	else
		sec_bat_recovery_mode = 0;

	pr_info("%s : %s", __func__, sec_bat_recovery_mode == 1 ?
				"recovery" : "normal");

	return 1;
}
__setup("androidboot.batt_check_recovery=", sec_bat_current_boot_mode);
#endif

void __init exynos5_universal5420_battery_init(void)
{
	/* board dependent changes in booting */
	charger_gpio_init();
#if defined(CONFIG_CHAGALL)
	adonis_battery_data[0].Capacity = 0x3A98;
#elif defined(CONFIG_KLIMT)
	adonis_battery_data[0].Capacity = 0x2710;
#endif
	platform_add_devices(
		universal5420_battery_devices,
		ARRAY_SIZE(universal5420_battery_devices));

	i2c_register_board_info(
		SEC_FG_I2C_ID,
		sec_brdinfo_fg,
		ARRAY_SIZE(sec_brdinfo_fg));
}

#endif