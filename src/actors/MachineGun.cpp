#include <actors/MachineGun.h>
#include <actors/PickUpItemHandle.h>
#include <messages/DamageMessage.h>
#include <messages/NetworkMessages.h>
#include <util/SMKUtilFunctions.h>

#include <dtOcean/oceanactor.h>

#include <dtAudio/audiomanager.h>
#include <dtCore/batchisector.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderparamoscillator.h>
#include <dtCore/timer.h>
#include <dtCore/transform.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagefactory.h>
#include <dtUtil/nodecollector.h>

#include <osg/Billboard>
#include <osg/BlendEquation>
#include <osg/BlendFunc>
#include <osg/Texture2D>

#include <osgDB/ReadFile>

#include <cassert>

////////////////////////////////////////////////////////////////////////////////
const std::string MachineGun::MACHINE_GUN_ACTOR_TYPE = "MachineGun";
const float BULLET_TRAIL_HEIGHT = 1.0f;
const float MAX_BULLET_DISTANCE = 1000.0f;

//////////////////////////////////////////////////////////////////////////
MachineGun::MachineGun(const dtDAL::ResourceDescriptor& resource)
: FrontWeapon(resource)
{
   static int id = 0;
   std::string name = MACHINE_GUN_ACTOR_TYPE + dtUtil::ToString(++id);
   SetName(name);
   GetOSGNode()->setName(name);

   SetFiringRate(10.0f);

   // Load any sounds we have
   mpFireSound = LoadSound("/sounds/impact.wav");

   // Setup our damage
   mDamage.SetDamageType(SMK::Damage::DAMAGE_PROJECTILE);
   mDamage.SetAmount(5);
   mDamage.SetRadius(0.0f);

   CreateBulletTrail();
}

//////////////////////////////////////////////////////////////////////////
MachineGun::~MachineGun()
{
   if (mpQuad.valid())
   {
      mpBeam->removeDrawable(mpQuad.get());
      mpQuad = NULL;
   }

   if (mpBeam.valid())
   {
      GetMatrixNode()->removeChild(mpBeam);
      mpBeam = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////
void MachineGun::FireWeapon()
{
   FrontWeapon::FireWeapon();

   osg::Vec3 hitPoint = CheckForBoatCollision();
   ShowBulletTrail(ConvertAbsoluteToRelativePosition(hitPoint));
}

///////////////////////////////////////////////////////////////////////////////
void MachineGun::CreateBulletTrail()
{
   // Orient beam so that it exposes as much of its face as possible
   mpBeam = new osg::Billboard;
   mpBeam->setMode(osg::Billboard::AXIAL_ROT);
   mpBeam->setAxis(osg::X_AXIS);
   mpBeam->setNormal(osg::Z_AXIS);
   mpBeam->setName("Beam");
   GetMatrixNode()->addChild(mpBeam);

   mpBeamStateSet = new osg::StateSet;

   osg::Image* beamImage       = osgDB::readImageFile("textures/bulletTrail.png");
   osg::Texture2D* beamTexture = new osg::Texture2D(beamImage);

   osg::BlendEquation* blendEquation = new osg::BlendEquation(osg::BlendEquation::FUNC_ADD);

   osg::BlendFunc* blendFunc = new osg::BlendFunc;
   blendFunc->setFunction(osg::BlendFunc::ONE, osg::BlendFunc::ONE);

   // Set the blending parameters
   mpBeamStateSet->setDataVariance(osg::Object::DYNAMIC);
   mpBeamStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   mpBeamStateSet->setBinNumber(100);
   mpBeamStateSet->setTextureAttributeAndModes(0, beamTexture, osg::StateAttribute::ON);
   mpBeamStateSet->setAttributeAndModes(blendEquation, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   mpBeamStateSet->setAttributeAndModes(blendFunc, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   mpBeamStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);

   // Retrieve the shader from the shader manager and assign it to this stateset
   dtCore::ShaderManager&       shaderManager    = dtCore::ShaderManager::GetInstance();
   const dtCore::ShaderProgram* prototypeProgram = shaderManager.FindShaderPrototype("BulletTrailShader");
   dtCore::ShaderProgram*       program          = shaderManager.AssignShaderFromPrototype(*prototypeProgram, *mpBeam.get());
   assert(program);

   mpTimeParam = dynamic_cast<dtCore::ShaderParamOscillator*>(program->FindParameter("time"));
   assert(mpTimeParam.valid());
   mpTimeParam->SetUseRealTime(false);
}

///////////////////////////////////////////////////////////////////////////////
void MachineGun::ShowBulletTrail(osg::Vec3 target)
{   
   osg::Vec3 origin = ConvertAbsoluteToRelativePosition(GetGunfireLocation().getTrans());
   osg::Vec3 heightVec(0.0f, 0.0f, BULLET_TRAIL_HEIGHT);
   osg::Vec3 corner   = origin - (heightVec * 0.5f);
   osg::Vec3 widthVec = target - origin;
   osg::Vec3 rightVec = heightVec ^ widthVec;

   heightVec = widthVec ^ rightVec;
   heightVec.normalize();
   heightVec *= BULLET_TRAIL_HEIGHT;

   if (mpQuad.valid())
   {
      mpBeam->removeDrawable(mpQuad.get());
      mpQuad = NULL;
   }

   mpQuad = osg::createTexturedQuadGeometry(-heightVec * 0.5f, widthVec, heightVec, 0.0f, 0.0f, 1.0f, 1.0f);
   mpQuad->setStateSet(mpBeamStateSet.get());

   widthVec.normalize();
   widthVec *= 0.025f;
   mpQuad->setStateSet(mpBeamStateSet.get());
   mpBeam->addDrawable(mpQuad.get());
   mpBeam->setPosition(0, origin + widthVec);

   osg::Vec3 axis = widthVec;
   osg::Vec3 normal = rightVec;

   axis.normalize();
   normal.normalize();

   mpBeam->setAxis(axis);
   mpBeam->setNormal(normal);

   // Tell the oscillator to start updating the time uniform
   mpTimeParam->TriggerOscillationStart();
}

///////////////////////////////////////////////////////////////////////////////
osg::Vec3 MachineGun::ConvertAbsoluteToRelativePosition(osg::Vec3 relativePos)
{
   //get the parent's world position
   osg::Matrix parentMat;
   GetAbsoluteMatrix(GetOSGNode(), parentMat);

   //calc the difference between xform and the parent's world position
   //child * parent^-1
   relativePos = relativePos * osg::Matrix::inverse(parentMat);

   return relativePos;
}

///////////////////////////////////////////////////////////////////////////////
osg::Vec3 MachineGun::CheckForBoatCollision()
{
   dtGame::GameManager* gm = mpSMKBoatActorProxy->GetGameManager();
   dtCore::RefPtr<dtCore::BatchIsector> iSector = new dtCore::BatchIsector(&gm->GetScene());
   iSector->EnableAndGetISector(0).SetToCheckForClosestDrawable(true);
   dtCore::Transform currentTransform;
   currentTransform.Set(GetGunfireLocation());
   osg::Vec3 location = currentTransform.GetTranslation();
   osg::Vec3 right, up, forward;
   currentTransform.GetOrientation(right, up, forward);
   iSector->EnableAndGetISector(0).SetSectorAsRay(location,
      forward, MAX_BULLET_DISTANCE);
   iSector->Update();

   dtCore::BatchIsector::HitList hitList = iSector->EnableAndGetISector(0).GetHitList();
   for (size_t hitIndex = 0; hitIndex < hitList.size(); ++hitIndex)
   {
      osg::NodePath &nodePath = hitList[hitIndex].getNodePath();
      dtCore::DeltaDrawable* hitDrawable = iSector->MapNodePathToDrawable(nodePath);

      // Ignore our boat, the ocean, and power ups
      if (hitDrawable == this || hitDrawable == mpSMKBoatActorProxy->GetActor() ||
         dynamic_cast<dtOcean::OceanActor*>(hitDrawable) != NULL ||
         dynamic_cast<SMK::PickUpItemHandle*>(hitDrawable) != NULL)
      {
         continue;
      }

      if (hitDrawable != NULL)
      {
         osg::Vec3 hitLocation = hitList[hitIndex].getWorldIntersectPoint();
         if (!mpSMKBoatActorProxy->GetGameActor().IsRemote())
         {
            mDamage.SetLocation(hitLocation);

            dtCore::RefPtr<SMK::DamageMessage> collisionMessage;
            gm->GetMessageFactory().CreateMessage(SMK::SMKNetworkMessages::ACTION_BOAT_HIT, collisionMessage);
            collisionMessage->SetAboutActorId(hitDrawable->GetUniqueId());
            collisionMessage->SetDamage(mDamage);
            gm->SendNetworkMessage(*collisionMessage);
            gm->SendMessage(*collisionMessage);
         }

         return hitLocation;
      }
   }

   return location + forward * MAX_BULLET_DISTANCE;
}

///////////////////////////////////////////////////////////////////////////////
osg::Matrix MachineGun::GetGunfireLocation()
{
   osg::Matrix gunLocation;

   dtCore::RefPtr<dtUtil::NodeCollector> collect = 
      new dtUtil::NodeCollector(GetOSGNode(),
      dtUtil::NodeCollector::MatrixTransformFlag);

   osg::MatrixTransform* gunTransform = collect->GetMatrixTransform("Node_DeckGun");
   if (gunTransform != NULL)
   {
      gunLocation = SMK::GetAbsoluteMatrix(GetOSGNode(), gunTransform);
   }
   else
   {
      dtCore::Transform currentTransform;
      GetTransform(currentTransform);
      currentTransform.Get(gunLocation);
   }

   return gunLocation;
}

////////////////////////////////////////////////////////////////////////////////
