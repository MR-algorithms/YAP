#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H
#include <math.h>
#include <complex>
#include <algorithm>


namespace CommonMethod
{
    void split_index(const int index, const int width, const int height, int& column, int& row);
    int GetChannelCountUsed(unsigned int channel_switch);
    bool IsChannelOn(int channel_switch, int channel_index);
    template<typename ElementType>
    bool IsComplexelementZero(ElementType *data, int data_count)
    {

        std::vector<float> temp;
        temp.resize(data_count);
        //method1:
        for(int i=0; i < data_count; i++)
        {
            temp[i] = abs(data[i]);

        }
        auto result = std::minmax_element(temp.data(), temp.data()+data_count);
        float min_data = *result.first;
        float max_data = *result.second;

        bool result1;
        if(max_data < FLT_EPSILON)
            result1 = true;
        else
            result1 = false;

        //method2:
        bool result2 = true;
        for(ElementType *iterater = data; iterater != data + data_count; ++iterater)
        {
            if( abs(*iterater)> FLT_EPSILON)
            {

                result2 = false;
                break;
            }

        }
        assert(result1 == result2);
        return result2;

    }
}

#endif // COMMONMETHOD_H
