// main.cpp : defines the entry point for the console application.

#include <SuperMaritimeKart.h>

#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>

#include <conio.h>
#include <direct.h>


int main(int argc, char** argv)
{
   const std::string relativeConfigPath("data/config.xml");

   // Attempt to automatically set the correct working directory
   if (dtUtil::FileUtils::GetInstance().FileExists("../" + relativeConfigPath))
   {
      chdir("../");
   }

   if (dtUtil::FileUtils::GetInstance().FileExists(relativeConfigPath))
   {
      std::string dataPath = dtUtil::GetDeltaDataPathList();
      dtUtil::SetDataFilePathList(dataPath + ";" + dtUtil::FileUtils::GetInstance().CurrentDirectory() + "/data");

      dtCore::RefPtr<SuperMaritimeKart> app = new SuperMaritimeKart(argc, argv, "config.xml");
      app->Config();
      app->Run();
   }
   else
   {
      LOG_ALWAYS("Unable to find config file, working directory may need to be set.");

      // Give users a chance to see the console
      LOG_ALWAYS("Hit a key to exit...");
      while (!_kbhit()){}
   }

   return 0;
}
