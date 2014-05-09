#ifndef _NETWORK_DATA_H_
#define _NETWORK_DATA_H_

namespace clearsky
{
	typedef unsigned short CLIENT_UID;
	typedef char PROT_MESSAGE_TYPE;
	typedef unsigned short MAL;
	typedef char MSG_ARGS;

	const static unsigned int ND_ARG_SIZE = 128;
	const static unsigned char PROTOCOL_VERSION = 1;
	const static CLIENT_UID UID_UNDEF = 0;

	const static char MT_REQUEST_UID = (char)2;

	struct CLEARSKY_API NetworkData
	{
		unsigned char PROTOCOL;
		CLIENT_UID UID;

		PROT_MESSAGE_TYPE MESSAGE_TYPE;
		MAL MESSAGE_ARGS_LENGTH;
		MSG_ARGS MESSAGE_ARGS[ND_ARG_SIZE];
	};

	union CLEARSKY_API NetworkDataMessage
	{
		NetworkData NetData;
		char asChar[sizeof(NetworkData)];
	};

	const static unsigned int ND_SIZE = sizeof(NetworkData);
}

#endif