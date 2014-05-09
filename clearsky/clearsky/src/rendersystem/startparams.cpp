/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/startparams.h"

namespace clearsky
{
	StartParams::StartParams()
	{
		strncpy_s(this->title,"clearsky",149);
		//memset(this->driverType,0,sizeof(this->driverType));
		//memset(this->featureLevel,0,sizeof(this->featureLevel));
		strncpy_s(this->driverType,"D3D_DRIVER_TYPE_HARDWARE",50);
		strncpy_s(this->featureLevel,"D3D_FEATURE_LEVEL_11_0",50);
		this->fullscreen = false;
		this->windowWidth = 1024;
		this->windowHeight = 768;
		this->multiSamplingCount = 1;
		this->multiSamplingLevel = 0;
	}

	StartParams::~StartParams()
	{

	}

	RETURN_VALUE StartParams::load(const char *filename)
	{
		FILE *inputFile=NULL;
		fopen_s(&inputFile,filename,"r");
		if(!inputFile)
			return RETURN_ERROR;

		char lineBuffer[256];

		while(fgets(lineBuffer,256,inputFile))
		{
			if(strlen(lineBuffer)>0)
			{
				char key[50];
				char value[150];

				sscanf_s(lineBuffer,"%s = %s",key,49,value,149);

				if(strncmp(key,"title",140)==0)
				{
					strncpy_s(this->title,value,149);
				}
				else if(strncmp(key,"windowWidth",50)==0)
				{
					this->windowWidth = atoi(value);
				}
				else if(strncmp(key,"windowHeight",50)==0)
				{
					this->windowHeight = atoi(value);
				}
				else if(strncmp(key,"fullscreen",50)==0)
				{
					this->fullscreen = (atoi(value)!=0);
				}
				else if(strncmp(key,"multiSamplingCount",50)==0)
				{
					this->multiSamplingCount = atoi(value);
				}
				else if(strncmp(key,"multiSamplingLevel",50)==0)
				{
					this->multiSamplingLevel = atoi(value);
				}
				else if(strncmp(key,"driverType",140)==0)
				{
					strncpy_s(this->driverType,value,149);
				}
				else if(strncmp(key,"featureLevel",140)==0)
				{
					strncpy_s(this->featureLevel,value,149);
				}
			}
		}

		fclose(inputFile);

		return RETURN_OK;
	}
}




