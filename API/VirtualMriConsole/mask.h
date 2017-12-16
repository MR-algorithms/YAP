#ifndef CMASK_H
#define CMASK_H
#include <vector>

enum ScanMode
{
    smFullSampling,//全采
    smEqualSampling,//
    smCustom
};
class Mask
{
private:
    std::vector<int> _data;//每个元素表示了采集的相位编码索引。
    unsigned int _currentIndex;//表示_data的索引。

    float _sampleRate;

public:
    Mask();
    bool Initialize(int phaseCount, ScanMode mode, float sampleRate);
    unsigned int GetIndex(){ return _currentIndex;}
    void SetIndex(unsigned int currentIndex){ _currentIndex = currentIndex;}
    unsigned int GetPhaseIndex(){return _data[_currentIndex];}

};

#endif // CMASK_H
