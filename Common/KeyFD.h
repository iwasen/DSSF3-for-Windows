#define KEYFD_FILENAME	"LICENSE.DAT"
#define LEN_REGISTNO	10

#pragma pack(1)
struct KEYFD {
	char	szRegistNo[LEN_REGISTNO + 1];
	long	nVolumeSerialNo;
	short	nAppFlag;
	short	nValidFlag;
	short	nCheckSum;
};
#pragma pack()
