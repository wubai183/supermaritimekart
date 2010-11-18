#ifndef SUPERMARITIMEKART_H
#define SUPERMARITIMEKART_H

#include <dtABC/application.h>

////////////////////////////////////////////////////////////////////////////////

namespace dtGame
{
   class GameManager;
}
namespace dtDAL
{
   class BaseActorObject;
}

namespace net
{
   class Address;
}

class FloatingActor;
class GameFinder;
class SMK_NetworkComponent;

////////////////////////////////////////////////////////////////////////////////

class SuperMaritimeKart : public dtABC::Application
{
   friend class SuperMaritimeKartMessenger;
public:
   SuperMaritimeKart(const std::string& configFilename = "config.xml");

   virtual void Config();

   virtual void PreFrame(const double deltaFrameTime);
   virtual void PostFrame(const double deltaFrameTime);

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int kc);

protected:
   virtual ~SuperMaritimeKart();

   void OnMapLoaded();
   void OnMapUnloaded();

   void StartHosting();
   void ConnectToServer(const net::Address& serverAddress);

private:
   dtCore::RefPtr<dtGame::GameManager> mGameManager;
   GameFinder* mGameFinder;
   typedef std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > ProxyContainer;
   ProxyContainer mInstantiatedPrototypes;
   dtCore::RefPtr<SMK_NetworkComponent> mNetworkComponent;

   void AttachOceanHeightToBoat();

   void GetFloatingActors(std::vector<FloatingActor*>& floatingActors);
   void CreatePickUpItemHandleActors(); //instantiates all PickupItemHandle prototypes in the currently loaded map
};

////////////////////////////////////////////////////////////////////////////////

#endif // SUPERMARITIMEKART_H
