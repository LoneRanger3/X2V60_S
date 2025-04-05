#pragma once
class AdBase
{
public:
    AdBase();
    virtual ~AdBase();

    virtual int Init(int channel_num);
    virtual int Start();
    virtual int Stop();
    virtual int Clean();
    virtual int GetCapSolution(int channel, int& width, int& height);
};