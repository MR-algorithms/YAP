#ifndef CMASK_H
#define CMASK_H
#include <vector>

enum ScanMode
{
    smFullSampling,
    smEqualSampling,// 你好
    smCustom
};

class Mask
{
private:
    std::vector<int> _data;
    unsigned int _currentIndex;

    float _sampleRate;

public:
    Mask();
    bool Initialize(int phaseCount, ScanMode mode, float sampleRate);
    unsigned int GetIndex(){ return _currentIndex;}
    void SetIndex(unsigned int currentIndex){ _currentIndex = currentIndex;}
    unsigned int GetPhaseIndex(){return _data[_currentIndex];}

};

#endif // CMASK_H
