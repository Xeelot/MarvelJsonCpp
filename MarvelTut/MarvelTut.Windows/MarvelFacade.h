#pragma once

#include <cstdlib>
#include <string>
#include "CharacterDetail.h"

ref class MarvelFacade sealed
{
public:
	MarvelFacade();

	void getMarvelDataFromServer();
   CharacterDetail& getMarvelData();

private:
	Platform::String^ getTimestampString();
	std::string convertPlatformToStd(Platform::String^ text);
	Platform::String^ convertStdToPlatform(std::string text);
   
   CharacterDetail marvelData;
};

