#ifndef _CR_VO_H_
#define _CR_VO_H_


struct lcd_vo_param {
	unsigned int ygain; //0 - 200
	unsigned int ugain; //0 - 200
	unsigned int vgain; //0 - 200
	unsigned int rgain; //0 - 200
	unsigned int ggain; //0 - 200
	unsigned int bgain; //0 - 200
	int yoffs;          //-256 - 255
	int uoffs;          //-512 - 511
	int voffs;          //-512 - 511
	int roffs;          //-128 - 127
	int goffs;          //-128 - 127
	int boffs;          //-128 - 127
	int rcoe[3];        //-1599 - 1599
	int gcoe[3];        //-1599 - 1599
	int bcoe[3];        //-1599 - 1599
	unsigned int gamma[65];
};
int LIBCR_VO_SetParam(struct lcd_vo_param *param);
int LIBCR_VO_GetParam(struct lcd_vo_param *param);


#endif
