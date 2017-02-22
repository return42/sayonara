#include "StreamRecorderData.h"

StreamRecorderData::StreamRecorderData()
{
	probe_id=0;
	busy = false;
	active = false;
	has_empty_filename = true;
}

StreamRecorderData::~StreamRecorderData()
{
	if(filename){
		g_free(filename);
		filename = nullptr;
	}
}

