#include <DOFDrawable.h>

////////////////////////////////////////////////////////////////////////////////
DOFDrawable::DOFDrawable(const std::string& name /*= "DOF Drawable"*/)
: DeltaDrawable(name)
, mpDOFNode(new osgSim::DOFTransform())
{
   mpDOFNode->setName(name);
}

////////////////////////////////////////////////////////////////////////////////
DOFDrawable::~DOFDrawable()
{
   mpDOFNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////
osg::Node* DOFDrawable::GetOSGNode()
{
   return mpDOFNode.get();
}

////////////////////////////////////////////////////////////////////////////////
const osg::Node* DOFDrawable::GetOSGNode() const
{
   return mpDOFNode.get();
}

////////////////////////////////////////////////////////////////////////////////
bool DOFDrawable::AddChild(DeltaDrawable* child)
{
   bool added = dtCore::DeltaDrawable::AddChild(child);
   return mpDOFNode->addChild(child->GetOSGNode()) && added;
}

////////////////////////////////////////////////////////////////////////////////
void DOFDrawable::RemoveChild(DeltaDrawable* child)
{
   mpDOFNode->removeChild(child->GetOSGNode());
   dtCore::DeltaDrawable::RemoveChild(child);
}

////////////////////////////////////////////////////////////////////////////////
