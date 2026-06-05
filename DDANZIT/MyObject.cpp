#include "MyObject.h"

#include "DDANZITEngine.h"


MyObject::MyObject(Scene* scene) : Draw2D(this)
{
	AddComponent<Script>();
}

GameObject* MyObject::Clone() const
{
	return new MyObject(*this);
}