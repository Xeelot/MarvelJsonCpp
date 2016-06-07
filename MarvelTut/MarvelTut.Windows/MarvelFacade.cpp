#include "MarvelFacade.h"
#include "MD5.h"
#include <ctime>
#include <codecvt>
#include "curl.h"
#include "CharacterDetail.h"
#include "JsonSerializer.h"

#define CURL_ENABLED 1

Platform::String^ PUBLIC_KEY("a1b90cc88dd35e587205574bc1d6cc68");
Platform::String^ PRIVATE_KEY("2f76b7ca6d77e03d589f86021d653bdf4956fdf7");
const int MAX_CHARACTERS(1500);
const int MAX_LIMIT(2);



std::string data; //will hold the url's contents

size_t characterCallback(char* buf, size_t size, size_t nmemb, void* up)
{ //callback must have this declaration
  //buf is a pointer to the data that curl has for us
  //size*nmemb is the size of the buffer

   for (UINT i = 0; i < (size * nmemb); ++i)
   {
      data.push_back(buf[i]);
   }
   return (size * nmemb); //tell curl how many bytes we handled
}


MarvelFacade::MarvelFacade()
{
   curl_global_init(CURL_GLOBAL_ALL);
}


void MarvelFacade::getMarvelDataFromServer()
{
   // Generate random number
   //int randNum = rand() % MAX_CHARACTERS;
   int randNum = 41;

   // Get the MD5 Hash
   Platform::String^ timeStampString(getTimestampString());
   std::string timeStd = convertPlatformToStd(timeStampString + PRIVATE_KEY + PUBLIC_KEY);
   std::string md5Hash(MD5(timeStd).hexdigest());

   // Assemble URL string
   Platform::String^ urlString("http://gateway.marvel.com:80/v1/public/characters?limit=" + MAX_LIMIT +
      "&offset=" + randNum + "&apikey=" + PUBLIC_KEY + "&ts=" + timeStampString + "&hash=" + convertStdToPlatform(md5Hash));

   // Get URL call to Marvel
#ifdef CURL_ENABLED
   CURL* curl;
   curl = curl_easy_init();

   curl_easy_setopt(curl, CURLOPT_URL, convertPlatformToStd(urlString).c_str());
   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &characterCallback);
   curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //tell curl to output its progress

   curl_easy_perform(curl);
   curl_easy_cleanup(curl);
#endif // CURL_ENABLED

   // Parse returned JSON
   CJsonSerializer::Deserialize(&marvelData, data);
}


CharacterDetail& MarvelFacade::getMarvelData()
{
   return marvelData;
}


Platform::String^ MarvelFacade::getTimestampString()
{
   // Call std::time to get the timestamp
   std::time_t timeStamp = std::time(nullptr);
   return timeStamp.ToString();
}


std::string MarvelFacade::convertPlatformToStd(Platform::String ^ text)
{
   // convert to wide string
   std::wstring textW(text->Begin());
   // convert to std::string
   std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
   std::string textStd = converter.to_bytes(textW);
   return textStd;
}


Platform::String^ MarvelFacade::convertStdToPlatform(std::string text)
{
   // convert to wide string
   std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
   std::wstring textW = converter.from_bytes(text);
   // convert to Platform::String^
   const wchar_t* textWChar = textW.c_str();
   Platform::String^ textPlatform = ref new Platform::String(textWChar);
   return textPlatform;
}
