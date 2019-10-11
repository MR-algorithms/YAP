#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H


namespace CommonMethod
{
    void split_index(const int index, const int width, const int height, int& column, int& row);
    int GetChannelCountUsed(unsigned int channel_switch);
    bool IsChannelOn(int channel_switch, int channel_index);
}

#endif // COMMONMETHOD_H
