#ifndef CmrPackType_h__
#define CmrPackType_h__

//base type
#define mptNil			0x00
#define mptBool			0x01
#define mptChar			0x02
#define mptUChar		0x03
#define mptInt16		0x04
#define mptUInt16		0x05
#define mptInt32		0x06
#define mptUInt32		0x07
#define mptInt64		0x08
#define mptUInt64		0x09
#define mptFloat		0x0A
#define mptDouble		0x0B
#define mptWChar		0x0C

//second type
#define	mstValue		0xFF01
#define	mstArray		0xFF02
#define mstUniquePtr	0xFF03
#define	mstRefArray		0xFF04
#endif // CmrPackType_h__
