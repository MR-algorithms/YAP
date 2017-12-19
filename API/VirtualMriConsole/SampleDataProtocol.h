#pragma once
#include <stdint.h>
#include <complex>
#include <vector>

#define SAMPLE_DATA_START		1
#define SAMPLE_DATA_DATA		2
#define SAMPLE_DATA_END			3

#define SAMPLE_DATA_PROTOCOL_VERSION 1

class QByteArray;

struct HeadItem
{
    uint16_t primary_type;
    uint16_t second_type;
    uint32_t size;
};

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

	SampleDataStart();
    bool Pack(QByteArray &byteArray);
    bool Unpack(QByteArray &byteArray);
};

struct SampleDataData
{
	uint32_t cmd_id;
	uint32_t rp_id;
	uint32_t dim23456_index;
	uint32_t rec;
	float coeff;
	std::vector<std::complex<float>> data;

	SampleDataData();
    bool Pack(QByteArray &byteArray);
    bool Unpack(QByteArray &byteArray);
    void CreatDemoData(double phi0, double freq, double dw, unsigned int dataCount);
};

/*
struct SampleDataDataRef
{
	uint32_t cmd_id;
	uint32_t rp_id;
	uint32_t dim23456_index;
	uint32_t rec;
	float coeff;
	const std::complex<float>* data_ptr;
	int data_size;

	SampleDataDataRef();
	bool Pack(cmr::MsgPack& msg_pack);
	bool Unpack(cmr::MsgUnpack& msg_pack);
};
*/

struct SampleDataEnd
{
	uint32_t cmd_id;
	uint32_t error_code;
	SampleDataEnd(uint32_t error_code);
    bool Pack(QByteArray &byteArray);
    bool Unpack(QByteArray &byteArray);
};

struct FloatArray
{
    int flag;
    std::vector<float> data;
    FloatArray():flag(-1){data.resize(12);}

    bool Pack(QByteArray &byteArray);
    bool Unpack(QByteArray &byteArray);

    void CreateDemoStruct();
    void CreateDemoSinStruct(double phi0, double freq, double dw, unsigned int dataCount);
};

static bool SampleData_TestDemo = false;

