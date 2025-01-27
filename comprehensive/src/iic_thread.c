/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <logging/log.h>

#define LOG_MODULE_NAME i2c_thread
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

const struct device *i2c_dev;
const static struct device *gpio_dev;
//static struct k_delayed_work read_eeprom;
#ifdef CONFIG_EXAMPLE_EXT_INT
K_SEM_DEFINE(sem_iic_op, 0, 1);
#endif

#define INT0_NODE DT_NODELABEL(button3)
#define INT0	DT_GPIO_LABEL(INT0_NODE, gpios)
#define INT0_PIN	DT_GPIO_PIN(INT0_NODE, gpios)
#define INT0_FLAGS	DT_GPIO_FLAGS(INT0_NODE, gpios)

static struct gpio_callback ext_int_cb_data;

#define EEPROM_SIM_SIZE                   (320u) //!< Simulated EEPROM size.

#define EEPROM_SIM_ADDR                   0x50    //!< Simulated EEPROM TWI slave address.

/* Slave memory addressing byte length */
#define EEPROM_SIM_ADDRESS_LEN_BYTES    2

#define IN_LINE_PRINT_CNT   (16u)   //!< Number of data bytes printed in a single line.


static int eeprom_read(uint16_t addr, uint8_t * pdata, size_t size)
{
    int ret;
    if (size > (EEPROM_SIM_SIZE))
    {
        return EINVAL;
    }
    do
    {
       uint16_t addr16 = addr;
       ret = i2c_write(i2c_dev, (uint8_t *)&addr16, EEPROM_SIM_ADDRESS_LEN_BYTES, EEPROM_SIM_ADDR);
       if (0 != ret)
       {
           break;
       }
       ret = i2c_read(i2c_dev, pdata, size, EEPROM_SIM_ADDR);
    }while (0);
    return ret;
}

static void eeprom_cmd_read(void)
{    
    uint8_t buf[IN_LINE_PRINT_CNT+1];

    for (uint16_t addr = 0; addr < EEPROM_SIM_SIZE; addr += IN_LINE_PRINT_CNT)
    {
        int err_code;
        err_code = eeprom_read(addr, buf, IN_LINE_PRINT_CNT);
        buf[IN_LINE_PRINT_CNT] = '\0';
        if (0 != err_code)
        {
            LOG_ERR("EEPROM transmission error %d\n", err_code);
            return;
        }

        LOG_HEXDUMP_INF(buf, IN_LINE_PRINT_CNT, "EEPROM: ");        
      
    }
}

// static void eeprom_read_fn(struct k_work *work)
// {
//     printk("eeprom read in system workqueue\n");
// 	eeprom_cmd_read();
//     k_delayed_work_submit(&read_eeprom, K_SECONDS(2));
// }

#ifdef CONFIG_EXAMPLE_EXT_INT
void ext_int_isr(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	LOG_INF("external interrupt occurs at %x", pins);   
    k_sem_give(&sem_iic_op);    
}

void config_io_interrupt(void)
{
	int ret;

	gpio_dev = device_get_binding(INT0);
	if (!gpio_dev) {
		LOG_ERR("INT0 dev null");		
	}

	ret = gpio_pin_configure(gpio_dev, INT0_PIN, (GPIO_INPUT | GPIO_PULL_UP));
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure pin %d",
		       ret, INT0_PIN);		
	}

	ret = gpio_pin_interrupt_configure(gpio_dev,
					   INT0_PIN,
					   GPIO_INT_EDGE_FALLING);	                       
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure interrupt on pin %d",
			ret, INT0_PIN);		
	}

	gpio_init_callback(&ext_int_cb_data, ext_int_isr, BIT(INT0_PIN));
	gpio_add_callback(gpio_dev, &ext_int_cb_data);
	LOG_INF("External interrupt example at Pin:%d", INT0_PIN);
}
#endif //CONFIG_EXAMPLE_EXT_INT

void iic_thread(void)
{	
	
	LOG_INF("** I2C master example **");
	LOG_INF("This example is ported from nRF5_SDK\\examples\\peripheral\\twi_master_with_twis_slave");
	LOG_INF("The related twis example is from nRF5_SDK\\examples\\peripheral\\twi_master_with_twis_slave");	
	
	i2c_dev = device_get_binding(DT_LABEL(DT_NODELABEL(my_i2c)));
	if (!i2c_dev) {
		LOG_ERR("I2C Device driver not found");
		return;
	}
    uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER;
    if (i2c_configure(i2c_dev, i2c_cfg)) {
        LOG_ERR("I2C config failed");
        return;
    }
#ifdef CONFIG_EXAMPLE_EXT_INT
    config_io_interrupt();    
#endif
    
	// k_delayed_work_init(&read_eeprom, eeprom_read_fn);
    // k_delayed_work_submit(&read_eeprom, K_MSEC(50));	    

	while (1) {
#ifdef CONFIG_EXAMPLE_EXT_INT        
        k_sem_take(&sem_iic_op, K_FOREVER);
#endif                
        LOG_INF("i2c master thread");
        eeprom_cmd_read();
        k_sleep(K_SECONDS(2));
	}
}

K_THREAD_DEFINE(iic_thread_id, 1024, iic_thread, NULL, NULL,
		NULL, 7, 0, 0);