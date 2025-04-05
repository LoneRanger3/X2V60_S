#ifndef ADC_H_
#define ADC_H_

typedef struct adc_info
{
	unsigned int id;
	unsigned int value;
} adc_info_s;

#define ADC_DRV "/dev/adc"
#define ADC_CMD_BASE	0
#define ADC_GET_VALUE	_IOWR(ADC_CMD_BASE, 1, adc_info_s)

int get_adc_val(int id);
void usage_exit(char* arg);
int  get_adc_value(int id);

#endif // !ADC_H_
