#include <stdafx.h>

#include <Actor.h>
#include <Timer.h>

Actor::Actor(ActorType NewType)
{
	Visible = false;
	DirtyDrawlist = true;
	Type = NewType;
	Dead = false;

	//BirthDate = 0;
}

Actor::Actor()
{
	Visible = false;
	Hidden = false;
	DirtyDrawlist = true;
}

Actor::~Actor()
{

}

bool Actor::Init()
{
	return true;
}

void Actor::setPosition(float X, float Y, float Z)
{
    Position.x = X;
    Position.y = Y;
    Position.z = Z;
}

bool Actor::Update()
{
	return true;
}

bool Actor::Draw(CameraOrientation Orientation)
{
	return true;
}
