#ifndef _USB_DEVICE_H
#define _USB_DEVICE_H

class UsbDevice
{
public:
    static UsbDevice* Instance();
	static void Uninstance();

    int Init();
    int ConnectServer(const char* server_ip, int signal_port, int media_port);
    int Upgrader(const char* file_name);
    int Realplay(int channel, int stream);
    int SyncSystemTime();
    int GetUpgraderProgress();
    int ForceIFrame(int channel, int stream);
    int CatchPic();
    int SetFPS(int fps);
    int SetOSDenable(int enable, int hour12=1);
    int SetIRMode(int mode);
private:
    UsbDevice();
    ~UsbDevice();
    static UsbDevice* instance_;

private:
    bool upgrader_over_;
};

#endif