#ifndef SMKBoatActor_h__
#define SMKBoatActor_h__

////////////////////////////////////////////////////////////////////////////////
#include <actors/SMKActorExport.h>
#include <actors/Health.h>

#include <BoatActors/BoatActor.h>

////////////////////////////////////////////////////////////////////////////////

class SMKBoatActorProxy;
class FrontWeaponSlot;
class BackWeaponSlot;
class WeaponSlot;

namespace dtAudio { class Sound; }

namespace dtGame
{
   class DeadReckoningHelper;
   class DRPublishingActComp;
}
namespace SMK
{
   class PickUpItemHandle;
}

class SMK_ACTOR_EXPORT SMKBoatActor : public BoatActor
{
public:  

   /**
   * Constructor
   */
   SMKBoatActor(SMKBoatActorProxy& proxy);

   virtual void TickLocal(const dtGame::Message& msg);

   /**
   * Get the front weapon.
   */
   WeaponSlot* GetFrontWeapon();
   const WeaponSlot* GetFrontWeapon() const;

   /**
   * Get the back weapon.
   */
   WeaponSlot* GetBackWeapon();
   const WeaponSlot* GetBackWeapon() const;

   /**
   * Callback from Scene when a contact occurs. This normally is used to 
   * filter out Transformables that you do not want to perform expensive
   * physics calculations on, but here we use it to fire our Trigger.
   */
   virtual bool FilterContact(dContact* contact, Transformable* collider);

   /**
   * Called when an actor is first placed in the "world"
   */
   virtual void OnEnteredWorld();

   /** 
   * Inherited from FloatingActor.  Called when this actor is about to be nuked.
   */
   virtual void OnRemovedFromWorld();

   virtual void BuildActorComponents();

   virtual void ProcessMessage(const dtGame::Message& message);

   dtGame::DeadReckoningHelper* GetDeadReckoningHelper() { return mDeadReckoningHelper; }
   const dtGame::DeadReckoningHelper* GetDeadReckoningHelper() const { return mDeadReckoningHelper; }
   dtGame::DRPublishingActComp* GetDRPublishingActComp() { return mDRPublishingActComp; }
   const dtGame::DRPublishingActComp* GetDRPublishingActComp() const { return mDRPublishingActComp; }

protected:

   /**
   * Destructor
   */
   virtual ~SMKBoatActor();

   /**
   * Initialize the actor.
   */
   virtual void Initialize();

private:
   void SetupDefaultWeapon();

   void FireFrontWeapon(const dtGame::Message& weaponFiredMessage);
   void FireBackWeapon(const dtGame::Message& weaponFiredMessage);
   bool DoWeWantThisPickUp(const SMK::PickUpItemHandle& pickup) const;

   dtCore::RefPtr<FrontWeaponSlot> mpFrontWeapon;
   dtCore::RefPtr<BackWeaponSlot> mpBackWeapon;
   SMK::Health mHealth;  ///<The current health of this boat
   dtCore::RefPtr<dtAudio::Sound> mPickupAcquireSound;

   dtCore::RefPtr<dtGame::DeadReckoningHelper> mDeadReckoningHelper;
   dtCore::RefPtr<dtGame::DRPublishingActComp> mDRPublishingActComp;
};

class SMK_ACTOR_EXPORT SMKBoatActorProxy : public BoatActorProxy
{
public:

   /**
   * Constructor
   */
   SMKBoatActorProxy();

   /**
   * Adds the properties associated with this actor
   */
   virtual void BuildPropertyMap();

   /**
   * Build invokables
   */
   virtual void BuildInvokables();

   /**
   * Creates the actor
   */
   virtual void CreateActor();

   virtual void OnEnteredWorld();

   /** 
   * Called when this ActorProxy is being removed from the world.  Time to clean
   * up our junk.
   */
   virtual void OnRemovedFromWorld();

   virtual void GetPartialUpdateProperties(std::vector<dtUtil::RefString>& propNamesToFill);

   virtual void NotifyFullActorUpdate();

protected:

   // Destructor
   virtual ~SMKBoatActorProxy();
};

////////////////////////////////////////////////////////////////////////////////

#endif // SMKBoatActor_h__
