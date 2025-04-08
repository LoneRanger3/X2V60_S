#include <string>
#include "osd_user.h"
#include "xm_middleware_api.h"
#include "xm_middleware_network.h"
#include "xm_ia_td_interface.h"
#include "Log.h"
#include "lvgl.h"
#include "global_data.h"
#include "mpp/MppMdl.h"
#include "DemoDef.h"
#define OSDWidth_LOGO	172//水印LOGO图片的宽度
#define OSDHeight_LOGO	48//水印LOGO图片的高度
#define OSDWidth	32//GPS水印图片的宽度
#define OSDHeight	64//GPS水印图片的高度
#define R_Width     1920
#define R_Heigth    1080


#if GPS_EN

#define OSDNum 		32//32//变化水印的总字符长度
#define OSDToalNim	72//变化水印的图片总数量
static char osd_num_data[OSDToalNim][OSDWidth*OSDHeight*2]={0};
static XM_MW_OSD_UPDATE_INFO osd_change_info[3];
static char osd_str_data[OSDNum+1]={0};
static char osd_str_flag=0;
static const char osd_num_str[OSDToalNim] = {
	'0','1','2','3','4','5','6','7','8','9',
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'!','/',':','-',' ','%','$','#','"','.'
};

static const char* osd_num_path[OSDToalNim+1] = {
	"/mnt/custom/Pic/osd_str0.rgb",  //'0'
	"/mnt/custom/Pic/osd_str1.rgb",  //'1'
	"/mnt/custom/Pic/osd_str2.rgb",  //'2'
	"/mnt/custom/Pic/osd_str3.rgb",  //'3'
	"/mnt/custom/Pic/osd_str4.rgb",  //'4'
	"/mnt/custom/Pic/osd_str5.rgb",  //'5'
	"/mnt/custom/Pic/osd_str6.rgb",  //'6'
	"/mnt/custom/Pic/osd_str7.rgb",  //'7'
	"/mnt/custom/Pic/osd_str8.rgb",  //'8'
	"/mnt/custom/Pic/osd_str9.rgb",  //'9'

	"/mnt/custom/Pic/osd_strA.rgb",  //'A'
	"/mnt/custom/Pic/osd_strB.rgb",  //'B'
	"/mnt/custom/Pic/osd_strC.rgb",  //'C'
	"/mnt/custom/Pic/osd_strD.rgb",  //'D'
	"/mnt/custom/Pic/osd_strE.rgb",  //'E'
	"/mnt/custom/Pic/osd_strF.rgb",  //'F'
	"/mnt/custom/Pic/osd_strG.rgb",  //'G'
	"/mnt/custom/Pic/osd_strH.rgb",  //'H'
	"/mnt/custom/Pic/osd_strI.rgb",  //'I'
	"/mnt/custom/Pic/osd_strJ.rgb",  //'J'
	"/mnt/custom/Pic/osd_strK.rgb",  //'K'
	"/mnt/custom/Pic/osd_strL.rgb",  //'L'
	"/mnt/custom/Pic/osd_strM.rgb",  //'M'
	"/mnt/custom/Pic/osd_strN.rgb",  //'N'
	"/mnt/custom/Pic/osd_strO.rgb",  //'O'
	"/mnt/custom/Pic/osd_strP.rgb",  //'P'
	"/mnt/custom/Pic/osd_strQ.rgb",  //'Q'
	"/mnt/custom/Pic/osd_strR.rgb",  //'R'
	"/mnt/custom/Pic/osd_strS.rgb",  //'S'
	"/mnt/custom/Pic/osd_strT.rgb",  //'T'
	"/mnt/custom/Pic/osd_strU.rgb",  //'U'
	"/mnt/custom/Pic/osd_strV.rgb",  //'V'
	"/mnt/custom/Pic/osd_strW.rgb",  //'W'
	"/mnt/custom/Pic/osd_strX.rgb",  //'X'
	"/mnt/custom/Pic/osd_strY.rgb",  //'Y'
	"/mnt/custom/Pic/osd_strZ.rgb",  //'Z'

	"/mnt/custom/Pic/osd_str_a.rgb",  //'a'
	"/mnt/custom/Pic/osd_str_b.rgb",  //'b'
	"/mnt/custom/Pic/osd_str_c.rgb",  //'c'
	"/mnt/custom/Pic/osd_str_d.rgb",  //'d'
	"/mnt/custom/Pic/osd_str_e.rgb",  //'e'
	"/mnt/custom/Pic/osd_str_f.rgb",  //'f'
	"/mnt/custom/Pic/osd_str_g.rgb",  //'g'
	"/mnt/custom/Pic/osd_str_h.rgb",  //'h'
	"/mnt/custom/Pic/osd_str_i.rgb",  //'i'
	"/mnt/custom/Pic/osd_str_j.rgb",  //'j'
	"/mnt/custom/Pic/osd_str_k.rgb",  //'k'
	"/mnt/custom/Pic/osd_str_l.rgb",  //'l'
	"/mnt/custom/Pic/osd_str_m.rgb",  //'m'
	"/mnt/custom/Pic/osd_str_n.rgb",  //'n'
	"/mnt/custom/Pic/osd_str_o.rgb",  //'o'
	"/mnt/custom/Pic/osd_str_p.rgb",  //'p'
	"/mnt/custom/Pic/osd_str_q.rgb",  //'q'
	"/mnt/custom/Pic/osd_str_r.rgb",  //'r'
	"/mnt/custom/Pic/osd_str_s.rgb",  //'s'
	"/mnt/custom/Pic/osd_str_t.rgb",  //'t'
	"/mnt/custom/Pic/osd_str_u.rgb",  //'u'
	"/mnt/custom/Pic/osd_str_v.rgb",  //'v'
	"/mnt/custom/Pic/osd_str_w.rgb",  //'w'
	"/mnt/custom/Pic/osd_str_x.rgb",  //'x'
	"/mnt/custom/Pic/osd_str_y.rgb",  //'y'
	"/mnt/custom/Pic/osd_str_z.rgb",  //'z'

	"/mnt/custom/Pic/osdgth.rgb",  //'!'	
	"/mnt/custom/Pic/osdxg.rgb",  //'/'
	"/mnt/custom/Pic/osdfh.rgb",  //':'
	"/mnt/custom/Pic/osdhg.rgb",  //'-'
	"/mnt/custom/Pic/osdkg.rgb",  //' '
	"/mnt/custom/Pic/osdbfh.rgb",  //'%'	
	"/mnt/custom/Pic/osddzf.rgb",  //'$'
	"/mnt/custom/Pic/osdjh.rgb",  //'#'
	"/mnt/custom/Pic/osdsyh.rgb",  //'"'
	"/mnt/custom/Pic/osdxsd.rgb",  //'.'
};
static unsigned int getcharindex(char str)
{
	if(osd_num_str){
		for(int i=0;i<OSDToalNim;i++){
			if(str == osd_num_str[i]){
				return i;
			}
		}
	}
	putchar('V');
	return 0;
}

int osdsetchange(char* buf, int num, int pos, int width, int sub)
{
	int osd_width = sub ? (OSDWidth/2) : OSDWidth;
	char* src = osd_num_data[num];
	char* dst = buf + pos*osd_width*2;
	if (sub) {
		for (int i = 0; i < OSDHeight/2; i++) {
			for (int j = 0; j < OSDWidth/2; j++) {
				dst[2*j] = src[4*j];
				dst[2*j+1] = src[4*j+1];
			}
			src += (OSDWidth*2*2);
			dst += (width*OSDWidth);
		}
	}
	else {
		for(int i=0;i<OSDHeight;i++){
			memcpy(dst,src,OSDWidth*2);
			src += OSDWidth*2;
			dst += (width*OSDWidth*2);
		}
	}
	return 0;
}
int updata_osd_flag= 0;
int osd_upgrade_data(int channel,char* str)
{
		if(!osd_str_flag){
			return -1;
		}else{
			if(str){
				int pos = 0;
				int size = strlen(str);
					//XMLogW("\nsize========%d\n",size);
				 if(size < OSDNum){
					memset(str+size,' ',OSDNum-size);
					//XMLogW("\nsizex========%d,(%s)\n",strlen(str),str);
				 }
                size = OSDNum;

				for(int k=0;k<size;k++){
					 if(*(str+k) != *(osd_str_data+k)||updata_osd_flag){
					 	osdsetchange(osd_change_info[channel].osd_buf,getcharindex(*(str+k)),pos,size,((channel<2)?0:1));
					 	updata_osd_flag = 1;
					 }
					pos++;
				}
				memset(osd_str_data,0,OSDNum);
				memcpy(osd_str_data,str,strlen(str));
				if(updata_osd_flag){
					MppMdl::Instance()->UpdateOSDInfo(channel, osd_change_info[channel]);
				}
			}
		}
}	

int osd_upgrade_all_data(char* str)
{
	updata_osd_flag=0;
	if(!strcmp(osd_str_data,str)){
		return -1;
	}
	osd_upgrade_data(0,str);
	osd_upgrade_data(1,str);
	osd_upgrade_data(2,str);//子码流
	return 0;
}

int clean_gps_osd_data(char tmp)
{
	XM_MW_OSD_INFOS osd_infos;
	int j = 1;
	
	if(!osd_str_flag){
		return -1;
	}

	if(tmp){
     sprintf(osd_str_data,"%s","N 112.000000 E 22.000000 100km/h");
	}else{
	 memset(osd_str_data,' ',OSDNum);	
	}
  	for (int i = 0; i < 3; i++) {//扩展可变化水印
		for(int k=0;k<OSDNum;k++){
			osdsetchange(osd_change_info[i].osd_buf,getcharindex(osd_str_data[k]),k,OSDWidth,i<2?0:1);
		}
		MppMdl::Instance()->UpdateOSDInfo(i, osd_change_info[i]);		
	}
	return 0;
}
#endif

int osd_time_ofs_y = OSD_GPS_ADJUST_Y;

int osd_data_init(void)
{
	XM_MW_OSD_INFOS osd_infos;

    bool enable=false;
	int i = 0;
	int width_v = 0;
	int height_v = 0;
	int j = 0;
	int move = 0; //偏移
	int osdx = 0; //logo水印x
	int osdy = 0;  //logo水印y
	int osd_x = 0;//时间水印x
	int osd_y = 0;//时间水印y
	int osd_y1 = 0;//后路时间水印y
	int osd_y4 = 0;//子码流时间水印y
	 bool osd_up=false;
	 XM_CONFIG_VALUE cfg_value;
	int ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
	if (ret >= 0) {
       if(cfg_value.int_value==1080){
          width_v=1920;
		  height_v=1080;
		  move=20;
	   }else if(cfg_value.int_value==1440){
          width_v=2560;
		  height_v=1440 - 180;//-128 -上移
		  move=18;
	   }else if(cfg_value.int_value==k3KHeight || cfg_value.int_value==k4KHeight){
        //   if(!MppMdl::Instance()->AdLoss()){
        //    width_v=2560;
		//    height_v=1440;
		//    move=18;
		//   }else{
			if(cfg_value.int_value==k3KHeight){
			 width_v=3008;
		     height_v=k3KHeight;
			 move=16;
			}else{
			 width_v=3840;
		     height_v=k4KHeight-156;//-1024-256-128 +下移
			 move=16;
			}
		//   }
		//osd_up=true;
	   }
	}else{
      width_v=2560;
	  height_v=1440;
	}

	  osd_x=128;  
	  osd_y=8192*(height_v-60-(height_v/360)*8)/height_v;
      osd_y1=8192*(R_Heigth-60-(R_Heigth/360)*8)/R_Heigth;
	  osd_y4=8192*(720-36-(720/360)*8)/720;

#if OSD_SHOW_ADJUST
	  if(gps_online_flag){
	  	
		osd_time_ofs_y = OSD_TIME_ADJUST_Y;
	  }else{

	    osd_time_ofs_y = OSD_GPS_ADJUST_Y;
	  }
#endif

	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Time_Watermark, cfg_value);
	enable=cfg_value.bool_value;

  // MppMdl::Instance()->EnableOsdTime(0,false, osd_x, osd_y);
   MppMdl::Instance()->EnableOsdTime(0,enable, osd_x, osd_y); //前摄时间水印


   MppMdl::Instance()->EnableOsdTime(1,enable, osd_x, 8192*(R_Heigth-60-(R_Heigth/360)*8)/R_Heigth);

#if 0//X2V60_S_DEBUG1
   MppMdl::Instance()->EnableOsdTime(4,enable, osd_x, 8192*(kSubStreamHeight-60-(kSubStreamHeight/360)*8)/kSubStreamHeight + 180); //APP时间水印
#elif 1//OSD_SHOW_ADJUST
   MppMdl::Instance()->EnableOsdTime(4,enable, osd_x, 8192*(kSubStreamHeight-60-(kSubStreamHeight/360)*8)/kSubStreamHeight + osd_time_ofs_y); //APP时间水印
#else
   MppMdl::Instance()->EnableOsdTime(4,enable, osd_x, 8192*(kSubStreamHeight-60-(kSubStreamHeight/360)*8)/kSubStreamHeight); //APP时间水印
#endif

#if 0 //logo水印
      osdx = 128;
	  osdy = 8192*(height_v-60-(height_v/360)*move)/height_v;	
	for (i = 0; i < 3; i++) {//固定的LOGO图片水印，osd_rec0.rgb、osd_rec1.rgb、osd_rec2.rgb
		if( i == 0){//前摄主码流
			osd_infos.osd[i][j].x = osdx;  //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = osdy;// 128+64;//8192/128*116;  //超始位置纵坐标，以8192为基准
			osd_infos.osd[i][j].width =OSDWidth; //不能超过XM_MAX_OSD_WIDTH
			osd_infos.osd[i][j].height =OSDHeight; //不超过XM_MAX_OSD_HEIGHT
			osd_infos.osd[i][j].up = false; //是否放大，true放大1倍
			osd_infos.osd[i][j].enable=enable; //使能

		}else if(i == 1){//后摄主码流
			osd_infos.osd[i][j].x = osdx;  //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = 8192*(R_Heigth-60-(R_Heigth/360)*20)/R_Heigth;   //128+64;//8192/128*116;  //超始位置纵坐标，以8192为基准
			osd_infos.osd[i][j].width =OSDWidth; //不能超过XM_MAX_OSD_WIDTH
			osd_infos.osd[i][j].height = OSDHeight; //不超过XM_MAX_OSD_HEIGHT
			osd_infos.osd[i][j].up = false; //是否放大，true放大1倍
			osd_infos.osd[i][j].enable=enable; //使能

		}else if(i == 2){//前摄子码流，APP直播流
			osd_infos.osd[i][j].x = 128;  //超始位置横坐标，以8192为基准
			osd_infos.osd[i][j].y = 8192*(kSubStreamHeight-115)/kSubStreamHeight;//8192/128*100;  //超始位置纵坐标，以8192为基准
			osd_infos.osd[i][j].width =OSDWidth; //不能超过XM_MAX_OSD_WIDTH
			osd_infos.osd[i][j].height = OSDHeight; //不超过XM_MAX_OSD_HEIGHT
			osd_infos.osd[i][j].up = false; //是否放大，true放大1倍
			osd_infos.osd[i][j].enable=enable; //使能
		}			
		FILE *file=NULL;
		char str[64]={0};
	#if 0	
		memset(str,0,64);
		sprintf(str,"%s","/mnt/tfcard/logo_w.rgb");
		file = fopen(str,"rb");
		if(file){
			//XMLogI("======%d:have %s file",i,str);
			fseek(file,0,SEEK_SET);
			fread(osd_infos.osd[i][j].osd_buf,  1, osd_infos.osd[i][j].width*osd_infos.osd[i][j].height*2, file);
			fclose(file);
			file = NULL;
		}else
	#endif
		{
			memset(str,0,64);
			sprintf(str,"%s","/mnt/custom/Pic/k10.rgb");					
			file = fopen(str,"rb");
			if(file){
				XMLogI("======%d:have %s file",i,str);
				fseek(file,0,SEEK_SET);
				fread(osd_infos.osd[i][j].osd_buf, 1, osd_infos.osd[i][j].width*osd_infos.osd[i][j].height*2, file);
				fclose(file);
				file = NULL;
			}else{					
				XMLogW("======%d:no have %s file",i,str);
			}
		}
	}
#endif	
#if GPS_EN	
	j = 1;
	for(int k=0;k<OSDToalNim;k++){
		FILE *file=NULL;
		char str[64]={0};
		memset(str,0,64);
		/*sprintf(str,"%s%02d%s","/mnt/tfcard/osd_num",k,".rgb");
		file = fopen(str,"rb");
		if(file){
			XMLogI("======%d:have %s file",k,str);
			fseek(file,0,SEEK_SET);
			fread(osd_num_data[k],  1, OSDWidth*OSDHeight*2, file);
			fclose(file);
			file = NULL;
		}else*/{				
			file = fopen(osd_num_path[k],"rb");
			if(file){
				fseek(file,0,SEEK_SET);
				fread(osd_num_data[k],  1, OSDWidth*OSDHeight*2, file);
				fclose(file);
				file = NULL;
			}else{					
				XMLogW("======%d:no have %s file",k,osd_num_path[k]);
			}
		}
	}
	// sprintf(osd_str_data,"%s","N 112.000000 E 22.000000 100km/h");
	for (i = 0; i < 3; i++) {//扩展可变化水印
		memset(osd_infos.osd[i][j].osd_buf,0,XM_MAX_OSD_WIDTH*XM_MAX_OSD_HEIGHT*2);
			if(i==0){
			 osd_infos.osd[i][j].x =128+20*32*8192/width_v;  //超始位置横坐标，以8192为基准	
			 osd_infos.osd[i][j].y = osd_y;  //超始位置纵坐标，以8192为基准
		   }else if(i==1){
			  osd_infos.osd[i][j].x =128+20*32*8192/1920;  //超始位置横坐标，以8192为基准
			  osd_infos.osd[i][j].y = osd_y1;  //超始位置纵坐标，以8192为基准
		   }
			osd_infos.osd[i][j].width = OSDNum*OSDWidth; //不能超过XM_MAX_OSD_WIDTH
			osd_infos.osd[i][j].height = OSDHeight; //不超过XM_MAX_OSD_HEIGHT
			if(osd_up && i==0){
              osd_infos.osd[i][j].up = osd_up; //是否放大，true放大1倍
			  osd_infos.osd[i][j].x =128+20*64*8192/width_v;  //超始位置横坐标，以8192为基准
			}else{
			  osd_infos.osd[i][j].up = false; //是否放大，true放大1倍
			}
			if(i==2){
			  osd_infos.osd[i][j].x =128;  //超始位置横坐标，以8192为基准
			  osd_infos.osd[i][j].y =osd_y4;  //超始位置纵坐标，以8192为基准
			  osd_infos.osd[i][j].width = OSDNum*OSDWidth/2; //不能超过XM_MAX_OSD_WIDTH
		      osd_infos.osd[i][j].height = OSDHeight/2; //不超过XM_MAX_OSD_HEIGHT
		   }
			osd_infos.osd[i][j].enable=enable; //失能
			osd_str_flag=enable;
			memset(osd_str_data,' ',OSDNum);
		for(int k=0;k<strlen(osd_str_data);k++){
			osdsetchange(osd_infos.osd[i][j].osd_buf,getcharindex(osd_str_data[k]),k,strlen(osd_str_data),i<2?0:1);
		}	
		memcpy(osd_change_info[i].osd_buf,osd_infos.osd[i][j].osd_buf,osd_infos.osd[i][j].width*osd_infos.osd[i][j].height*2);	
	}
	MppMdl::Instance()->SetOSDInfos(osd_infos);	
#endif
}

