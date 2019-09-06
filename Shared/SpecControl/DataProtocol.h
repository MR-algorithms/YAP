#pragma once
#define DATA_FORMAT_CXFL 1
#define DATA_FORMAT_FL 2
#define DATA_FORMAT_SH 3

struct DataProtocol
{
	unsigned int totalSz;
	unsigned int hdSz;
	unsigned int dataSz;
	unsigned int data_format;
	unsigned int cols;
	unsigned int lins;
	unsigned int ccha;
	unsigned int slcs;
	unsigned int cslc;

	char* hdBuf;
	char* dataBuf;
	DataProtocol(unsigned int _totalSz, unsigned int _hdSz, unsigned int _dataSz,
		unsigned int _data_format, unsigned int _cols, unsigned int _lins, unsigned int _ccha,
		unsigned int _slcs, unsigned int _cslc, char* _hdBuf, char* _dataBuf);
	~DataProtocol();
};



