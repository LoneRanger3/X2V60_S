#Makefile of adr_pack
TOP_DIR=$(shell pwd)
TOOL_DIR=$(TOP_DIR)/tools
TMP_DIST=$(TOP_DIR)/tmp
TAR_DIR=$(TOP_DIR)/target
TARGET = $(shell pwd)/../middlewaresdk/sample/build/car_demo
PLATFORM = XM650V200
DEVICE = CAR
TYPE = X2V60
TIME = $(shell date +%Y%m%d)
LOGOFILE = 172_320_le.rgb
#SERIAL_NUMBER=800`date "+%y%m%d%H%M"`

BURN_FILE = upall_$(PLATFORM)_$(DEVICE)_$(TYPE).$(TIME).bin
UPGRADER_FILE = netall_$(PLATFORM)_$(DEVICE)_$(TYPE).$(TIME).bin
UPGRADER_FIXED_FILE_NAME = netall_$(PLATFORM)_$(DEVICE)_$(TYPE).bin
OUTPUT_ZIP_FILE = XDSM_$(PLATFORM)_CAR_DASHCAM_$(TYPE)_V1.00_$(TIME).zip

# flash partition
DPT_BOOT_S?=0x0
DPT_BOOT_E?=0x30000
DPT_ENV_E?=0x40000
DPT_KERN_E?=0x260000
DPT_ROOT_E?=0x3a0000
DPT_USR_E?=0xb80000
DPT_CUST_E?=0xe60000
DPT_MTD_E?=0xf60000
DPT_LOGO_E?=0x1000000

DPT_ENV_S=$(DPT_BOOT_E)
DPT_KERN_S=$(DPT_ENV_E)
DPT_ROOT_S=$(DPT_KERN_E)
DPT_USR_S=$(DPT_ROOT_E)
DPT_CUST_S=$(DPT_USR_E)
DPT_MTD_S=$(DPT_CUST_E)
DPT_LOGO_S=$(DPT_MTD_E)

DPT_ALL_S=$(DPT_BOOT_S)
DPT_ALL_E=$(DPT_LOGO_E)

default: pack 

aimg:
	@cp $(TOP_DIR)/uboot/u-boot-x2v60.bin $(TMP_DIST)/
	@cp $(TOP_DIR)/uboot/u-boot-x2v60.env.bin $(TMP_DIST)/
	@echo "Start to make u-boot.env.img ..."
	@$(TOOL_DIR)/mkimage -A arm -O linux -T firmware -C gzip -a $(DPT_ENV_S) \
	-e $(DPT_ENV_E) -n linux -d $(TMP_DIST)/u-boot-x2v60.env.bin $(TMP_DIST)/u-boot.env.img
	@if [ $$? -ne 0 ]; then								\
		echo "make u-boot.env.img failed.";				\
		exit 1;											\
	fi
	@dd if=$(TMP_DIST)/u-boot-x2v60.env.bin of=$(TMP_DIST)/u-boot-x2v60.bin bs=4096 count=1 seek=46 conv=sync

	@echo ""
	@echo "Start to make u-boot.bin.img ..."
	@$(TOOL_DIR)/mkimage -A arm -O linux -T firmware -C gzip -a $(DPT_BOOT_S) \
		-e $(DPT_BOOT_E) -n linux -d $(TMP_DIST)/u-boot-x2v60.bin  $(TMP_DIST)/u-boot.bin.img
	@if [ $$? -ne 0 ]; then								\
		echo "make u-boot.bin.img failed.";				\
		exit 1;											\
	fi
	@echo "$(TMP_DIST)/u-boot.bin.img done"	

kimg:
	@echo "Start make uImage.img..."
	@cp $(TOP_DIR)/kernel/uImage $(TMP_DIST)/
	@$(TOOL_DIR)/mkimage -A arm -O linux -T kernel -C gzip -a $(DPT_KERN_S) \
	-e $(DPT_KERN_E) -n linux -d $(TMP_DIST)/uImage  $(TMP_DIST)/uImage.img
	@if [ $$? -ne 0 ]; then								\
		echo "make uImage.img failed.";					\
		exit 1;											\
	fi
	@echo "$(TMP_DIST)/uImage.img done"	

rimg:
	tar -zxvf romfs.tar
	@$(TOOL_DIR)/mkcramfs $(TOP_DIR)/romfs $(TMP_DIST)/romfs-x.cramfs
	@echo "Start to make romfs-x.cramfs.img ..."
	@$(TOOL_DIR)/mkimage -A arm -O linux -T kernel -C gzip -a $(DPT_ROOT_S) \
	-e $(DPT_ROOT_E) -n linux -d $(TMP_DIST)/romfs-x.cramfs $(TMP_DIST)/romfs-x.cramfs.img
	@if [ $$? -ne 0 ]; then								\
		echo "make romfs-x.cramfs.img failed";			\
		exit 1;											\
	fi
	@echo "$(TMP_DIST)/romfs-x.cramfs.img done"
	rm romfs -rf

uimg:
	
	@$(TOOL_DIR)/mkcramfs $(TOP_DIR)/usr $(TMP_DIST)/user-x.cramfs
	@echo "Start to make user-x.cramfs.img ..."
	@$(TOOL_DIR)/mkimage -A arm -O linux -T kernel -C gzip -a $(DPT_USR_S) \
	-e $(DPT_USR_E) -n linux -d $(TMP_DIST)/user-x.cramfs $(TMP_DIST)/user-x.cramfs.img
	@if [ $$? -ne 0 ]; then								\
		echo "make user-x.cramfs.img failed.";			\
		exit 1;											\
	fi
	@echo "$(TMP_DIST)/user-x.cramfs.img done"
	

wimg:
	@if [ ! -d $(TMP_DIST)/web ]; \
		then mkdir $(TMP_DIST)/web; \
	else \
		rm -rf $(TMP_DIST)/web/*; \
	fi
	@$(TOOL_DIR)/mkcramfs $(TMP_DIST)/web $(TMP_DIST)/web-x.cramfs
	@if [ $$? -ne 0 ]; then                              \
		echo "make web-x.cramfs fail!";					\
		exit 1;                                         \
	fi;
	@echo ""
	@echo "Start to make web-x.cramfs.img ..."
	@$(TOOL_DIR)/mkimage  -A arm -O linux -T standalone -C gzip -a $(DPT_WEB_S) \
	-e $(DPT_WEB_E) -n linux -d $(TMP_DIST)/web-x.cramfs $(TMP_DIST)/web-x.cramfs.img

	@if [ $$? -ne 0 ]; then					\
		echo "make web-x.cramfs.img fail.";	\
		exit 1;								\
	fi
	
	@echo "$(TMP_DIST)/web-x.cramfs.img done"
cimg:
	#@if [ ! -d $(TMP_DIST)/custom ]; \
	#	then mkdir $(TMP_DIST)/custom; \
	#else \
	#	rm -rf $(TMP_DIST)/custom/*; \
	#fi
	#拷贝资源文件
	#@cp -rf $(TOP_DIR)/Resource/$(RESOUCE_DIR)/* $(TMP_DIST)/custom
	@cp -rf $(TOP_DIR)/custom $(TMP_DIST)/custom
	@$(TOOL_DIR)/mkcramfs $(TMP_DIST)/custom $(TMP_DIST)/custom-x.cramfs
	
	@if [ $$? -ne 0 ]; then								\
		echo "make $(TMP_DIST)/custom-x.cramfs fail!";	\
		exit 1;											\
	fi
	@echo ""
	@echo "Start to make custom-x.cramfs.img ..."
	@$(TOOL_DIR)/mkimage -A arm -O linux -T standalone -C gzip -a $(DPT_CUST_S) \
		-e $(DPT_CUST_E) -n linux -d $(TMP_DIST)/custom-x.cramfs $(TMP_DIST)/custom-x.cramfs.img
	@if [ $$? -ne 0 ]; then									\
		echo "make $(TMP_DIST)/custom-x.cramfs.img fail."	\
		exit 1;												\
	fi
	
	@echo "$(TMP_DIST)/custom-x.cramfs.img done"
limg:
	@cp -rf $(TOP_DIR)/logo $(TMP_DIST)/
	@if [ ! -d $(TMP_DIST)/logo ]; \
		then mkdir $(TMP_DIST)/logo; \
	#else \
	#	rm -rf $(TMP_DIST)/logo/*; \
	fi
	#@$(TOOL_DIR)/mkcramfs $(TMP_DIST)/logo $(TMP_DIST)/logo-x.cramfs
	
	@echo ""
	@echo "Start to make logo-x.cramfs.img ..."
	@$(TOOL_DIR)/mkimage -A arm -O linux -T kernel -C gzip -a $(DPT_LOGO_S) \
	-e $(DPT_LOGO_E) -n linux -d $(TMP_DIST)/logo/$(LOGOFILE)  $(TMP_DIST)/logo.img
	@if [ $$? -ne 0 ]; then								\
		echo "make logo-x.cramfs.img fail.";			\
		exit 1;											\
	fi

	@echo "$(TMP_DIST)/logo-x.cramfs.img done"

dimg:
	@echo "Start to make mtd-x.jffs2 ..."
	@cp -rf $(TOP_DIR)/mtd $(TMP_DIST)/mtd
	@$(TOOL_DIR)/mkfs.jffs2 -d $(TMP_DIST)/mtd -l -e 0x10000 -o $(TMP_DIST)/mtd-x.jffs2
	@if [ $$? -ne 0 ]; then								\
		echo "make $(TMP_DIST)/mtd-x.jffs2 failed!";	\
		exit 1;											\
	fi
	
	@echo "Start to make mtd-x.jffs2.img ..."
	@$(TOOL_DIR)/mkimage -A arm -O linux -T standalone -C gzip -a $(DPT_MTD_S) \
		-e $(DPT_MTD_E) -n linux -d $(TMP_DIST)/mtd-x.jffs2 $(TMP_DIST)/mtd-x.jffs2.img
	@if [ $$? -ne 0 ]; then									\
		echo "make $(TMP_DIST)/mtd-x.jffs2.img failed."		\
		exit 1;												\
	fi
	@chmod 777 $(TMP_DIST)/mtd-x.jffs2*
	@echo "$(TMP_DIST)/mtd-x.jffs2.img done"	
makCardemo:
	@if [ -e $(TARGET) ]; \
		then cp $(TARGET) $(TOP_DIR)/usr/bin/; \
	fi
preBuild:
	mkdir -p $(TMP_DIST)
	mkdir -p $(TAR_DIR)
	@chmod +x $(TOP_DIR)/usr/sbin/run_app.sh 			\
	$(TOP_DIR)/usr/sbin/usb_config.sh 					\
	$(TOP_DIR)/usr/sbin/hostapd							\
	$(TOP_DIR)/usr/etc/loadmod 							\
	$(TOP_DIR)/usr/lib/modules/pll.sh					\
	$(TOP_DIR)/usr/bin/car_demo     					\
	$(TOP_DIR)/usr/bin/exfatfsck						\
	$(TOP_DIR)/usr/bin/fsck.exfat						\
	$(TOP_DIR)/usr/bin/upgrader							\
	$(TOP_DIR)/usr/lib/modules/loadxm530v200			\
	$(TOP_DIR)/usr/etc/loadmod							\
	$(TOP_DIR)/usr/etc/loadpublic						\
	$(TOP_DIR)/tools/mkall_16M							\
	$(TOP_DIR)/tools/mkpkt								\
	$(TOP_DIR)/tools/zip								\
	$(TOP_DIR)/tools/mkcramfs							\
	$(TOP_DIR)/tools/mkimage							\
	$(TOP_DIR)/tools/mkfs.jffs2
postBuild:
	cp $(TOP_DIR)/InstallDesc $(TMP_DIST)/
	echo $(shell date +%Y%m%d%H%M%S) >> $(TMP_DIST)/InstallDesc
	$(TOOL_DIR)/zip -j $(TMP_DIST)/$(UPGRADER_FILE) \
				$(TMP_DIST)/u-boot.bin.img \
				$(TMP_DIST)/u-boot.env.img \
				$(TMP_DIST)/uImage.img \
				$(TMP_DIST)/user-x.cramfs.img \
				$(TMP_DIST)/custom-x.cramfs.img \
				$(TMP_DIST)/romfs-x.cramfs.img \
				$(TMP_DIST)/mtd-x.jffs2.img	\
				$(TMP_DIST)/logo.img \
				$(TMP_DIST)/InstallDesc
				
	$(TOOL_DIR)/zip -j $(TMP_DIST)/$(UPGRADER_FIXED_FILE_NAME) \
				$(TMP_DIST)/u-boot.bin.img \
				$(TMP_DIST)/u-boot.env.img \
				$(TMP_DIST)/uImage.img \
				$(TMP_DIST)/user-x.cramfs.img \
				$(TMP_DIST)/custom-x.cramfs.img \
				$(TMP_DIST)/romfs-x.cramfs.img \
				$(TMP_DIST)/logo.img \
				$(TMP_DIST)/InstallDesc
				
	$(TOOL_DIR)/mkpkt $(TMP_DIST)/update.img \
				$(TMP_DIST)/uImage.img \
				$(TMP_DIST)/romfs-x.cramfs.img \
				$(TMP_DIST)/user-x.cramfs.img \
				$(TMP_DIST)/custom-x.cramfs.img \
				$(TMP_DIST)/logo.img
				
	$(TOOL_DIR)/mkall_16M $(TMP_DIST)/$(BURN_FILE)	\
				$(TMP_DIST)/u-boot-x2v60.bin	    	$(DPT_BOOT_S)	 	$(DPT_BOOT_E)	\
				$(TMP_DIST)/u-boot-x2v60.env.bin		$(DPT_ENV_S)       	$(DPT_ENV_E) 	\
				$(TMP_DIST)/uImage           	$(DPT_KERN_S)       $(DPT_KERN_E) 	\
				$(TMP_DIST)/romfs-x.cramfs		$(DPT_ROOT_S)       $(DPT_ROOT_E)  	\
				$(TMP_DIST)/user-x.cramfs		$(DPT_USR_S)       	$(DPT_USR_E)  	\
				$(TMP_DIST)/custom-x.cramfs  	$(DPT_CUST_S)       $(DPT_CUST_E) 	\
				$(TMP_DIST)/mtd-x.jffs2			$(DPT_MTD_S)        $(DPT_MTD_E) 	\
				$(TMP_DIST)/logo/$(LOGOFILE)	$(DPT_LOGO_S)       $(DPT_LOGO_E)
	
	@cp $(TMP_DIST)/$(UPGRADER_FILE) $(TAR_DIR)/
	@cp $(TMP_DIST)/$(UPGRADER_FIXED_FILE_NAME) $(TAR_DIR)/
	@cp $(TMP_DIST)/$(BURN_FILE) $(TAR_DIR)/

	$(TOOL_DIR)/zip -j $(TAR_DIR)/$(OUTPUT_ZIP_FILE) \
				$(TAR_DIR)/$(UPGRADER_FIXED_FILE_NAME) \
				$(TAR_DIR)/$(BURN_FILE) \
				$(TOP_DIR)/lib_version.txt

#	@rm -rf $(TMP_DIST)

.PHONY : clean pack

pack: preBuild makCardemo dimg aimg uimg kimg cimg limg rimg postBuild

clean:	
	rm -fr $(TAR_DIR)
	rm -fr $(TMP_DIST)
