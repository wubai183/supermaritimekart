#include <Damage.h>
#include <limits>

using namespace SMK;

////////////////////////////////////////////////////////////////////////////////
Damage::Damage():
 mAmount(UCHAR_MAX)
,mDamageType(DAMAGE_IMPACT)
,mLocation(0.f,0.f,0.f)
,mRadius(1.f)
{
}

////////////////////////////////////////////////////////////////////////////////
DamageAmountType Damage::GetAmount() const
{
   return mAmount;
}

////////////////////////////////////////////////////////////////////////////////
void Damage::SetAmount(const DamageAmountType& val)
{
   mAmount = val;
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 Damage::GetLocation() const
{
   return mLocation;
}

////////////////////////////////////////////////////////////////////////////////
void Damage::SetLocation(const osg::Vec3& val)
{
   mLocation = val;
}

////////////////////////////////////////////////////////////////////////////////
float Damage::GetRadius() const
{
   return mRadius;
}

////////////////////////////////////////////////////////////////////////////////
void Damage::SetRadius(const float& val)
{
   mRadius = val;
}

////////////////////////////////////////////////////////////////////////////////
Damage::DamageType Damage::GetDamageType() const
{
   return mDamageType;
}

////////////////////////////////////////////////////////////////////////////////
void Damage::SetDamageType(const Damage::DamageType& val)
{
   mDamageType = val;
}
