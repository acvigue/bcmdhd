/*
* Customer code to add GPIO control during WLAN start/stop
* $Copyright Open Broadcom Corporation$
*
* $Id: dhd_custom_gpio.c 493822 2014-07-29 13:20:26Z $
*/

#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>
#include <bcmutils.h>
#include <dngl_stats.h>
#include <dhd.h>
#include <dhd_linux.h>
#include <linux/gpio.h>

#include <wlioctl.h>
#include <wl_iw.h>

#define WL_ERROR(x) printf x
#define WL_TRACE(x)

#if defined(OOB_INTR_ONLY)

#if defined(BCMLXSDMMC)
extern int sdioh_mmc_irq(int irq);
#endif /* (BCMLXSDMMC)  */

#if defined(CUSTOMER_HW3) || defined(PLATFORM_MPS)
#include <mach/gpio.h>
#endif

/* Customer specific Host GPIO defintion  */
static int dhd_oob_gpio_num = -1;

module_param(dhd_oob_gpio_num, int, 0644);
MODULE_PARM_DESC(dhd_oob_gpio_num, "DHD oob gpio number");

/* This function will return:
 *  1) return :  Host gpio interrupt number per customer platform
 *  2) irq_flags_ptr : Type of Host interrupt as Level or Edge
 *
 *  NOTE :
 *  Customer should check his platform definitions
 *  and his Host Interrupt spec
 *  to figure out the proper setting for his platform.
 *  Broadcom provides just reference settings as example.
 *
 */
int dhd_customer_oob_irq_map(void* adapter, unsigned long* irq_flags_ptr)
{
	int  host_oob_irq = 0;

#if defined(CUSTOMER_HW2) && !defined(PLATFORM_MPS)
	host_oob_irq = wifi_platform_get_irq_number(adapter, irq_flags_ptr);

#else
#if defined(CUSTOM_OOB_GPIO_NUM)
	if (dhd_oob_gpio_num < 0) {
		dhd_oob_gpio_num = CUSTOM_OOB_GPIO_NUM;
	}
#endif /* CUSTOMER_OOB_GPIO_NUM */

	if (dhd_oob_gpio_num < 0) {
		WL_ERROR(("%s: ERROR customer specific Host GPIO is NOT defined \n",
			__FUNCTION__));
		return (dhd_oob_gpio_num);
	}

	WL_ERROR(("%s: customer specific Host GPIO number is (%d)\n",
		__FUNCTION__, dhd_oob_gpio_num));

	gpio_request(dhd_oob_gpio_num, "oob irq");
	host_oob_irq = gpio_to_irq(dhd_oob_gpio_num);
	gpio_direction_input(dhd_oob_gpio_num);
#endif 

	return (host_oob_irq);
}
#endif 

/* Customer function to control hw specific wlan gpios */
int dhd_customer_gpio_wlan_ctrl(void* adapter, int onoff)
{
	int err = 0;

	return err;
}

/* Customized Locale table : OPTIONAL feature */
const struct cntry_locales_custom translate_custom_table[] = {
	{"",   "XY", 4},  /* Universal if Country code is unknown or empty */
		{"US", "US", 69}, /* input ISO "US" to : US regrev 69 */
		{"CA", "US", 69}, /* input ISO "CA" to : US regrev 69 */
		{"EU", "EU", 5},  /* European union countries to : EU regrev 05 */
		{"AT", "EU", 5},
		{"BE", "EU", 5},
		{"BG", "EU", 5},
		{"CY", "EU", 5},
		{"CZ", "EU", 5},
		{"DK", "EU", 5},
		{"EE", "EU", 5},
		{"FI", "EU", 5},
		{"FR", "EU", 5},
		{"DE", "EU", 5},
		{"GR", "EU", 5},
		{"HU", "EU", 5},
		{"IE", "EU", 5},
		{"IT", "EU", 5},
		{"LV", "EU", 5},
		{"LI", "EU", 5},
		{"LT", "EU", 5},
		{"LU", "EU", 5},
		{"MT", "EU", 5},
		{"NL", "EU", 5},
		{"PL", "EU", 5},
		{"PT", "EU", 5},
		{"RO", "EU", 5},
		{"SK", "EU", 5},
		{"SI", "EU", 5},
		{"ES", "EU", 5},
		{"SE", "EU", 5},
		{"GB", "EU", 5},
		{"KR", "XY", 3},
		{"AU", "XY", 3},
		{"CN", "XY", 3}, /* input ISO "CN" to : XY regrev 03 */
		{"TW", "XY", 3},
		{"AR", "XY", 3},
		{"MX", "XY", 3},
		{"IL", "IL", 0},
		{"CH", "CH", 0},
		{"TR", "TR", 0},
		{"NO", "NO", 0}
};


/* Customized Locale convertor
*  input : ISO 3166-1 country abbreviation
*  output: customized cspec
*/
void get_customized_country_code(void* adapter, char* country_iso_code, wl_country_t* cspec)
{
	struct cntry_locales_custom* cloc_ptr;

	if (!cspec)
		return;

	cloc_ptr = wifi_platform_get_country_code(adapter, country_iso_code);
	if (cloc_ptr) {
		strlcpy(cspec->ccode, cloc_ptr->custom_locale, WLC_CNTRY_BUF_SZ);
		cspec->rev = cloc_ptr->custom_locale_rev;
	}
	return;
}
