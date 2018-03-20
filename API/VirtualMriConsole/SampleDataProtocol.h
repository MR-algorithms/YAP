#ifndef SAMPLEDATAPROTOCOL_H
#define SAMPLEDATAPROTOCOL_H

#pragma once
#include <complex>
#include <vector>

#define SAMPLE_DATA_START		1
#define SAMPLE_DATA_DATA		2
#define SAMPLE_DATA_END			3

#define SAMPLE_DATA_PROTOCOL_VERSION 1

struct SampleDataStart
{
    uint32_t cmd_id;
	uint32_t version;
	uint32_t scan_id;
	uint32_t dim23456_size;
	uint32_t dim1_size;
	uint32_t dim2_size;
	uint32_t dim3_size;
	uint32_t dim4_size;
	uint32_t dim5_size;
	uint32_t dim6_size;
	uint32_t channel_mask;

    SampleDataStart()
        : cmd_id(SAMPLE_DATA_START)
        , version(SAMPLE_DATA_PROTOCOL_VERSION)
        , scan_id(-1)
        , dim23456_size(0)
        , dim1_size(0)
        , dim2_size(0)
        , dim3_size(0)
        , dim4_size(0)
        , dim5_size(0)
        , dim6_size(0)
        , channel_mask(0)  {  }

    SampleDataStart& operator = (const SampleDataStart& rhs){
       cmd_id = rhs.cmd_id;
       version = rhs.version;
       scan_id = rhs.scan_id;
       dim23456_size = rhs.dim23456_size;
       dim1_size = rhs.dim1_size;
       dim2_size = rhs.dim2_size;
       dim3_size = rhs.dim3_size;
       dim4_size = rhs.dim4_size;
       dim5_size = rhs.dim5_size;
       dim6_size = rhs.dim6_size;
       channel_mask = rhs.channel_mask;
       return *this;
    }


    bool operator == (const SampleDataStart& rhs) const{
       return   cmd_id == rhs.cmd_id &&
                version == rhs.version &&
                scan_id == rhs.scan_id &&
                dim23456_size == rhs.dim23456_size &&
                dim1_size == rhs.dim1_size &&
                dim2_size == rhs.dim2_size &&
                dim3_size == rhs.dim3_size &&
                dim4_size == rhs.dim4_size &&
                dim5_size == rhs.dim5_size &&
                dim6_size == rhs.dim6_size &&
                channel_mask == rhs.channel_mask;

    }




};

struct SampleDataData
{
	uint32_t cmd_id;
    uint32_t rp_id;//
	uint32_t dim23456_index;
    uint32_t rec;//channel Index;
	float coeff;
	std::vector<std::complex<float>> data;

    SampleDataData()
        : cmd_id(SAMPLE_DATA_DATA)
        , rp_id(-1)
        , dim23456_index(0)
        , rec(0)
        , coeff(1)  {}

    SampleDataData& operator = (const SampleDataData& rhs){
        cmd_id = rhs.cmd_id;
        rp_id = rhs.rp_id;
        dim23456_index = rhs.dim23456_index;
        rec = rhs.rec;
        coeff = rhs.coeff;

        data.assign(rhs.data.begin(),rhs.data.end());
        return *this;
    }

    bool operator == (const SampleDataData& rhs) const{
        bool test1 = (  cmd_id == rhs.cmd_id &&
                        rp_id == rhs.rp_id &&
                        dim23456_index == rhs.dim23456_index &&
                        rec == rhs.rec &&
                        coeff == rhs.coeff );

        bool test2 = (data.size() == rhs.data.size());
        if(!test1 || !test2)
        {
            return false;
        }
        else
        {
            for(int i =0; i < static_cast<int>( data.size() ); i ++)
            {
                if(data[i] != rhs.data[i])
                {
                    return false;
                }
            }
            return true;

        }
    }

};


struct SampleDataEnd
{
	uint32_t cmd_id;
	uint32_t error_code;
    SampleDataEnd(): cmd_id(SAMPLE_DATA_END), error_code(0){}

    SampleDataEnd& operator = (const SampleDataEnd& rhs){
        cmd_id = rhs.cmd_id;
        error_code = rhs.error_code;
        return *this;
    }

    bool operator == (const SampleDataEnd& rhs) const{
        return cmd_id == rhs.cmd_id &&
        error_code == rhs.error_code ;
    }
};

#endif //SAMPLEDATAPROTOCOL_H
