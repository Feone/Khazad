#include <stdafx.h>

#include <Cell.h>
#include <Cube.h>
#include <Face.h>
#include <Game.h>
#include <TextureManager.h>
#include <ScreenManager.h>
#include <DataManager.h>
#include <Random.h>


Face::Face()
{
	setType(FACE_ACTOR);

	Initalized = false;
	Visible = false;

	FirstOwner = NULL;
	SecondOwner = NULL;

    Constructed = false;
    Decorated = false;

    Material = 6;

    Rotation = RANDOM->Roll(0, 255);

	//GAME->ActorList.push_back(this);
	//ID = (Uint32) GAME->ActorList.size();
}

Face::~Face()
{
    //GAME->RemoveActor(ID);
}

bool Face::Init(Cube* First, Cube* Second, Facet Type, Uint16 MaterialType)
{
    Initalized = true;

	FirstOwner = First;
    SecondOwner = Second;
	FacetType = Type;
	Visible = true;
	Material = MaterialType;

    if(SecondOwner != NULL)
    {
        //SecondOwner->setFacet(FacetType, this);
        SecondOwner->setFacet(Cube::OpositeFace(FacetType), this);
    }

	switch(FacetType)
	{
		case FACET_TOP:
		{
			Points[0].x = - 0.5;
			Points[0].y = + 0.5;
			Points[0].z = + 0.5;

			Points[1].x = + 0.5;
			Points[1].y = + 0.5;
			Points[1].z = + 0.5;

			Points[2].x = + 0.5;
			Points[2].y = - 0.5;
			Points[2].z = + 0.5;

			Points[3].x = - 0.5;
			Points[3].y = - 0.5;
			Points[3].z = + 0.5;

			break;
		}
		case FACET_BOTTOM:
		{
			Points[0].x = - 0.5;
			Points[0].y = + 0.5;
			Points[0].z = - 0.5;

			Points[1].x = + 0.5;
			Points[1].y = + 0.5;
			Points[1].z = - 0.5;

			Points[2].x = + 0.5;
			Points[2].y = - 0.5;
			Points[2].z = - 0.5;

			Points[3].x = - 0.5;
			Points[3].y = - 0.5;
			Points[3].z = - 0.5;

			break;
		}
		case FACET_NORTH_EAST:
		{
			Points[0].x = - 0.5;
			Points[0].y = - 0.5;
			Points[0].z = - 0.5;

			Points[1].x = + 0.5;
			Points[1].y = - 0.5;
			Points[1].z = - 0.5;

			Points[2].x = + 0.5;
			Points[2].y = - 0.5;
			Points[2].z = + 0.5;

			Points[3].x = - 0.5;
			Points[3].y = - 0.5;
			Points[3].z = + 0.5;

			break;
		}
		case FACET_SOUTH_EAST:
		{
			Points[0].x = + 0.5;
			Points[0].y = - 0.5;
			Points[0].z = - 0.5;

			Points[1].x = + 0.5;
			Points[1].y = + 0.5;
			Points[1].z = - 0.5;

			Points[2].x = + 0.5;
			Points[2].y = + 0.5;
			Points[2].z = + 0.5;

			Points[3].x = + 0.5;
			Points[3].y = - 0.5;
			Points[3].z = + 0.5;

			break;
		}
		case FACET_SOUTH_WEST:
		{
			Points[0].x = + 0.5;
			Points[0].y = + 0.5;
			Points[0].z = - 0.5;

			Points[1].x = - 0.5;
			Points[1].y = + 0.5;
			Points[1].z = - 0.5;

			Points[2].x = - 0.5;
			Points[2].y = + 0.5;
			Points[2].z = + 0.5;

			Points[3].x = + 0.5;
			Points[3].y = + 0.5;
			Points[3].z = + 0.5;

			break;
		}
		case FACET_NORTH_WEST:
		{
			Points[0].x = - 0.5;
			Points[0].y = + 0.5;
			Points[0].z = - 0.5;

			Points[1].x = - 0.5;
			Points[1].y = - 0.5;
			Points[1].z = - 0.5;

			Points[2].x = - 0.5;
			Points[2].y = - 0.5;
			Points[2].z = + 0.5;

			Points[3].x = - 0.5;
			Points[3].y = + 0.5;
			Points[3].z = + 0.5;

			break;
		}
	}

	return true;
}

bool Face::CheckRemoval()
{
	if (FirstOwner != NULL && FirstOwner->isInitalized())
	{
        if (SecondOwner != NULL && SecondOwner->isInitalized())
        {
            return true;
        }
	}
	return false;
}

bool Face::Update()
{
	return true;
}

bool Face::Draw(float xTranslate, float yTranslate)
{
    if(Visible)
    {
        TEXTURE->BindTexturePoint(Material, 0 + Rotation);         glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
        TEXTURE->BindTexturePoint(Material, 1 + Rotation);         glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
        TEXTURE->BindTexturePoint(Material, 2 + Rotation);         glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

        TEXTURE->BindTexturePoint(Material, 2 + Rotation);         glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);
        TEXTURE->BindTexturePoint(Material, 3 + Rotation);         glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);
        TEXTURE->BindTexturePoint(Material, 0 + Rotation);         glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);

        SCREEN->IncrementTriangles(2);
    }

	return true;
}
