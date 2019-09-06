#include "stdafx.h"
#include "DataProtocol.h"

DataProtocol::DataProtocol(unsigned int _totalSz, unsigned int _hdSz, unsigned int _dataSz,
	unsigned int _data_format, unsigned int _cols, unsigned int _lins, unsigned int _ccha,
	unsigned int _slcs, unsigned int _cslc, char* _hdBuf, char* _dataBuf)
{
	totalSz = _totalSz;
	hdSz = _hdSz;
	dataSz = _dataSz;
	data_format = _data_format;
	cols = _cols;
	lins = _lins;
	ccha = _ccha;
	slcs = _slcs;
	cslc = _cslc;
	hdBuf = _hdBuf;
	dataBuf = _dataBuf;
}

DataProtocol::~DataProtocol() {}