#include <linux/module.h>
#include <linux/fs.h>
#include <linux/printk.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/mutex.h>

#define CLASS_NAME	"seesawadc"
#define NAME		"seesaw-adc"
#define SEESAW_REG      	  0x09
#define SEESAWNEO             0xe
#define NEOWRITE              0x4
#define SEESAWADC_PWMCTL			  0x08
#define ADC0                  0x07
#define ADC1                  0x08
#define ADC2                  0x09



static const unsigned short normal_i2c[] = { 0x49, I2C_CLIENT_END };



static struct i2c_device_id seesawadc_idtable[] = {
      { "seesawadc", 0 },
      { }
};
MODULE_DEVICE_TABLE(i2c, seesawadc_idtable);

struct seesawadc_data {
	struct i2c_client *client;
	struct mutex update_lock;
	u8 adc1;
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
//	u8 adc1_min;
//	u8 adc1_max;
	u8 adc0_input;
	u8 adc0_target;
    u8 adc0_label;
	u8 adc1_input;
	u8 adc1_target;
    u8 adc1_label;
	u8 adc2_input;
	u8 adc2_target;
    u8 adc2_label;
    u8 neonum;
    u8 neobufsize;
    u8 neostart1;
    u8 neostart2;
    u8 neogreen;
    u8 neored;
    u8 neoblue;
};

static int I2C_Write(struct i2c_client *client, unsigned char *buf, unsigned int len)
{
    int ret = i2c_master_send(client, buf, len);

    return ret;
}

/*
int seesawadc_write_value(struct i2c_client *client, u8 reg, u8 value)
{
//      printk(KERN_INFO "setting adc to %d\n", value);
      return i2c_smbus_write_byte_data(client, SEESAW_REG, value);
}
*/
static int seesawpwm1_write_value(struct i2c_client *client, u8 reg, u8 pwm1value)
{
	unsigned char buf[4] = {0};
	int ret;
	buf[0] = 0x8;
	buf[1] = 0x1;
	buf[2] = 0x1;
	buf[3] = pwm1value;
    ret = I2C_Write(client, buf, 4);
    return ret;
}

static int seesawpwm2_write_value(struct i2c_client *client, u8 reg, u8 pwm1value)
{
	unsigned char buf[4] = {0};
	int ret;
	buf[0] = 0x8;
	buf[1] = 0x1;
	buf[2] = 0x2;
	buf[3] = pwm1value;
    ret = I2C_Write(client, buf, 4);
    return ret;
}

static int seesawpwm3_write_value(struct i2c_client *client, u8 reg, u8 pwm1value)
{
	unsigned char buf[4] = {0};
	int ret;
	buf[0] = 0x8;
	buf[1] = 0x1;
	buf[2] = 0x3;
	buf[3] = pwm1value;
    ret = I2C_Write(client, buf, 4);
    return ret;
}

int seesawadc_read_value(struct i2c_client *client, u8 reg, u8 value)
{
	  i2c_smbus_write_byte_data(client, SEESAW_REG, value);
	  msleep(50);
      return i2c_smbus_read_word_data(client, SEESAW_REG);
}

static ssize_t adc1_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
    int ret2;
//    ret2 = data->adc1;

    mutex_lock(&data->update_lock);

    ret2 = seesawadc_read_value(client, SEESAW_REG, ADC1);

	mutex_unlock(&data->update_lock);

	return scnprintf(buf, PAGE_SIZE, "%d\n", ret2);
}

static ssize_t pwm1_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

	u8 pwm1value;
	u8 ret2;
	ret2 = kstrtou8(buf, 10, &pwm1value);

	mutex_lock(&data->update_lock);

    seesawpwm1_write_value(client, SEESAWADC_PWMCTL, pwm1value);

	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t pwm2_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

	u8 pwm2value;
	u8 ret2;
	ret2 = kstrtou8(buf, 10, &pwm2value);

	mutex_lock(&data->update_lock);

    seesawpwm2_write_value(client, SEESAWADC_PWMCTL, pwm2value);

	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t pwm3_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

	u8 pwm3value;
	u8 ret2;
	ret2 = kstrtou8(buf, 10, &pwm3value);

	mutex_lock(&data->update_lock);

    seesawpwm3_write_value(client, SEESAWADC_PWMCTL, pwm3value);

	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t adc0_input_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
    int ret2;
    mutex_lock(&data->update_lock);

    ret2 = seesawadc_read_value(client, SEESAW_REG, ADC0);

	mutex_unlock(&data->update_lock);
	printk(KERN_INFO "%d\n", ret2);
	return scnprintf(buf, PAGE_SIZE, "%d\n", ret2);
}

static ssize_t adc1_input_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
    int ret2;
    mutex_lock(&data->update_lock);

    ret2 = seesawadc_read_value(client, SEESAW_REG, ADC1);

	mutex_unlock(&data->update_lock);
	printk(KERN_INFO "%d\n", ret2);
	return scnprintf(buf, PAGE_SIZE, "%d\n", ret2);
}

static ssize_t adc2_input_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
    int ret2;
    mutex_lock(&data->update_lock);

    ret2 = seesawadc_read_value(client, SEESAW_REG, ADC2);

	mutex_unlock(&data->update_lock);
	printk(KERN_INFO "%d\n", ret2);
	return scnprintf(buf, PAGE_SIZE, "%d\n", ret2);
}

static ssize_t pwm1_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);

    int ret2;
    ret2 = data->pwm1;

	return scnprintf(buf, PAGE_SIZE, "%d\n", ret2);
}

static ssize_t pwm2_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);

    int ret2;
    ret2 = data->pwm2;

	return scnprintf(buf, PAGE_SIZE, "%d\n", ret2);
}

static ssize_t pwm3_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);

    int ret2;
    ret2 = data->pwm3;

	return scnprintf(buf, PAGE_SIZE, "%d\n", ret2);
}


static ssize_t adc1_label_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{

	return scnprintf(buf, PAGE_SIZE, "seesaweadc\n");
}

static ssize_t neoint_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

    u8 neo2bufs;
	u8 ret;
	u8 ret2;
	u8 ret3;
	u8 ret4;
	u8 ret5;
	u8 ret6;
	u8 ret7;
	unsigned char buf4[2]  = {0};
	unsigned char buf3[3] = {0};
	unsigned char buf2[4] = {0};
	unsigned char buf1[3] = {0};
	ret2 = kstrtou8(buf, 10, &neo2bufs);
	 printk(KERN_INFO "to u8  %d\n", neo2bufs);
    data->neonum = neo2bufs;
    data->neobufsize = neo2bufs * 3;
    printk(KERN_INFO "buff size set to  %d\n", data->neobufsize);
	
	buf1[0] = 0xe;
	buf1[1] = 0x2; 
	buf1[2] = 0x1;
	mutex_lock(&data->update_lock);
    ret = I2C_Write(client, buf1, 3);
    mutex_unlock(&data->update_lock);
    if (ret) {
    ;
    }
    msleep(50);
    
    buf2[0] = 0xe;
	buf2[1] = 0x3; 
	buf2[2] = 0x0;
	buf2[3] = data->neobufsize;
	mutex_lock(&data->update_lock);
    ret3 = I2C_Write(client, buf2, 4);
    mutex_unlock(&data->update_lock);
        if (ret3) {
    ;
    }
    msleep(50);
    
    buf3[0] = 0xe;
	buf3[1] = 0x1; 
	buf3[2] = 0xa;
	mutex_lock(&data->update_lock);
    ret4 = I2C_Write(client, buf3, 3);
    mutex_unlock(&data->update_lock);
        if (ret4) {
    ;
    }
    msleep(50);
            
    buf4[0] = 0xe;
	buf4[1] = 0x5; 
	mutex_lock(&data->update_lock);
    ret5 = I2C_Write(client, buf4, 2);
    mutex_unlock(&data->update_lock);
        if (ret5) {
    ;
    }
    msleep(50);       
	
	uint8_t cmdWrite[] = { 0xe, 0x4, 0x0, 0x3, 0x6, 0x2, 0x6f };
	mutex_lock(&data->update_lock);
	ret6 = I2C_Write(client, cmdWrite, sizeof(cmdWrite));
	mutex_unlock(&data->update_lock);
	    if (ret6) {
    ;
    }
	msleep(50);
	
	uint8_t cmdWrite4[] = { 0xe, 0x5 };
	mutex_lock(&data->update_lock);
	ret7 = I2C_Write(client, cmdWrite4, sizeof(cmdWrite4));
	mutex_unlock(&data->update_lock);
	    if (ret7) {
    ;
    }
	return count;
}

static ssize_t neostart1_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	u8 startone;
	u8 poo;
	(void)client;
    
    poo = kstrtou8(buf, 10, &startone);
    data->neostart1 = startone;
    printk(KERN_INFO "neo start 1 =  %d\n", data->neostart1);
    return count;
    
}

static ssize_t neostart2_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	u8 starttwo;
    u8 poo;
    (void)client;
    
    poo = kstrtou8(buf, 10, &starttwo);
    data->neostart2 = starttwo;
    printk(KERN_INFO "neo start 2 =  %d\n", data->neostart2);
    return count;
    
}

static ssize_t neoblue_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	u8 blue;
    u8 poo;
    (void)client;
    
    poo = kstrtou8(buf, 10, &blue);
    data->neoblue = blue;
    printk(KERN_INFO "neo blue =  %d\n", data->neoblue);
    return count;
    
}

static ssize_t neored_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	u8 red;
    u8 poo;
    (void)client;
    
    poo = kstrtou8(buf, 10, &red);
    data->neored = red;
    printk(KERN_INFO "neo red =  %d\n", data->neored);
    return count;
    
}

static ssize_t neogreen_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	u8 green;
    u8 poo;
    (void)client;
    
    poo = kstrtou8(buf, 10, &green);
    data->neogreen = green;
    printk(KERN_INFO "neo green =  %d\n", data->neogreen);
    return count;
    
}

static ssize_t neoshow_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	u8 neopixel[6] = {0};
	u8 poo;
	u8 buf2[7] = {0};
	u8 buf4[2] = {0};
	int ret;
	int ret5;

	poo = kstrtou8(buf, 10, neopixel);

    buf2[0] = 0xe;
    buf2[1] = 0x4;
	buf2[2] = data->neostart1;
	buf2[3] = data->neostart2;
	buf2[4] = data->neogreen;
	buf2[5] = data->neored;
	buf2[6] = data->neoblue;
	mutex_lock(&data->update_lock);
    ret = I2C_Write(client, buf2, 7);
    mutex_unlock(&data->update_lock);
    if (ret) {
    ;
    }
	printk(KERN_INFO "buf0  %d\n", buf2[0]);
    printk(KERN_INFO "buf1  %d\n", buf2[1]);
    printk(KERN_INFO "buf2  %d\n", buf2[2]);
    printk(KERN_INFO "buf3  %d\n", buf2[3]);
    printk(KERN_INFO "buf4  %d\n", buf2[4]);
    printk(KERN_INFO "buf5  %d\n", buf2[5]);
    printk(KERN_INFO "buf5  %d\n", buf2[6]);
    
    msleep(50);
            
    buf4[0] = 0xe;
	buf4[1] = 0x5; 
	mutex_lock(&data->update_lock);
    ret5 = I2C_Write(client, buf4, 2);
    mutex_unlock(&data->update_lock);
    if (ret5) {
    ;
    }
    return count;

}


static ssize_t neopixel_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct seesawadc_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	u8 neopixel[6] = {0};
	u8 poo;
	u8 buf2[6] = {0};
	int ret;

	poo = kstrtou8(buf, 10, neopixel);
//	printk(KERN_INFO "neopixels =   %d\n", neopixel);
	

	printk(KERN_INFO "buf0  %d\n", neopixel[0]);
    printk(KERN_INFO "buf1  %d\n", neopixel[1]);
    printk(KERN_INFO "buf2  %d\n", neopixel[2]);
    printk(KERN_INFO "buf3  %d\n", neopixel[3]);
    printk(KERN_INFO "buf4  %d\n", neopixel[4]);
    printk(KERN_INFO "buf5  %d\n", neopixel[5]);
    
	buf2[0] = 0x4;
	buf2[1] = neopixel[0];
	buf2[2] = neopixel[1];
	buf2[3] = neopixel[2];
	buf2[4] = neopixel[3];
	buf2[5] = neopixel[4];
    ret = I2C_Write(client, buf2, 6);
    
    printk(KERN_INFO "buf0  %d\n", buf2[0]);
    printk(KERN_INFO "buf1  %d\n", buf2[1]);
    printk(KERN_INFO "buf2  %d\n", buf2[2]);
    printk(KERN_INFO "buf3  %d\n", buf2[3]);
    printk(KERN_INFO "buf4  %d\n", buf2[4]);
    printk(KERN_INFO "buf5  %d\n", buf2[5]);
    return count;
}

static DEVICE_ATTR_RO(adc1);
static DEVICE_ATTR_RO(adc1_input);
static DEVICE_ATTR_RO(adc1_label);
static DEVICE_ATTR_RO(adc0_input);
static DEVICE_ATTR_RO(adc2_input);
static DEVICE_ATTR_WO(neopixel);
static DEVICE_ATTR_WO(neoint);
static DEVICE_ATTR_WO(neostart1);
static DEVICE_ATTR_WO(neostart2);
static DEVICE_ATTR_WO(neoblue);
static DEVICE_ATTR_WO(neored);
static DEVICE_ATTR_WO(neogreen);
static DEVICE_ATTR_WO(neoshow);
static DEVICE_ATTR_RW(pwm1);
static DEVICE_ATTR_RW(pwm2);
static DEVICE_ATTR_RW(pwm3);

static struct attribute *seesawadc_attrs[] = {
	&dev_attr_adc1.attr,
	&dev_attr_adc1_input.attr,
	&dev_attr_adc1_label.attr,
	&dev_attr_adc0_input.attr,
	&dev_attr_adc2_input.attr,
	&dev_attr_neopixel.attr,
	&dev_attr_neoint.attr,
	&dev_attr_neostart1.attr,
	&dev_attr_neostart2.attr,
	&dev_attr_neored.attr,
	&dev_attr_neogreen.attr,
	&dev_attr_neoblue.attr,
	&dev_attr_neoshow.attr,
	&dev_attr_pwm1.attr,
	&dev_attr_pwm2.attr,
	&dev_attr_pwm3.attr,
    NULL
};

ATTRIBUTE_GROUPS(seesawadc);

static int seesawadc_remove(struct i2c_client *client)
{
	return 0;
}



static int seesawadc_probe(struct i2c_client *client,
                     const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct seesawadc_data *data;
	struct device *hwmon_dev;
	data = devm_kzalloc(dev, sizeof(struct seesawadc_data), GFP_KERNEL);

	i2c_set_clientdata(client, data);
	data->client = client;

	hwmon_dev = devm_hwmon_device_register_with_groups(dev, client->name,
							   data,
							   seesawadc_groups);

	return PTR_ERR_OR_ZERO(hwmon_dev);
    return 0;
}

static const struct of_device_id seesawadc_of_match[] = {
	{ .compatible = "seesaw,seesawadc" },
	{ .compatible = "seesaw,seesaw" },
	{}
};
MODULE_DEVICE_TABLE(of, seesawadc_of_match);

static struct i2c_driver seesawadc_driver = {
	  .class		= I2C_CLASS_HWMON,
      .driver = {
              .name   = "seesawadc",
      },

      .id_table       = seesawadc_idtable,
      .address_list   = normal_i2c,
      .probe           = seesawadc_probe,

      .remove       = seesawadc_remove, /* optional */

};


module_i2c_driver(seesawadc_driver);


MODULE_AUTHOR("Ben Maddocks bm16ton@gmail.com");
MODULE_DESCRIPTION("Driver for Seesaw i2c mfd adafruit");
MODULE_LICENSE("GPL");
