#include <stdafx.h>

#include <Map.h>
#include <Singleton.h>
#include <TextureManager.h>
#include <DataManager.h>
#include <Extract.h>
#include <Column.h>
#include <Cube.h>
#include <Face.h>
#include <Cell.h>
#include <Random.h>

DECLARE_SINGLETON(Map)

Map::Map()
{
    Initialized = false;

	MapSizeX = 0;
	MapSizeY = 0;
	MapSizeZ = 0;

	CellSizeX = 0;
	CellSizeY = 0;
	CellSizeZ = 0;

    CellCount = 0;
    CubeCount = 0;
    FaceCount = 0;
    SlopeCount = 0;

    InitedCellCount = 0;
    InitedCubeCount = 0;
    InitedFaceCount = 0;
    InitedSlopeCount = 0;

    ColumnMatrix = NULL;
}

Map::~Map()
{
    if(MapLoaded)
    {
        ReleaseMap();
    }
}

bool Map::Init()
{
    InitilizeTilePicker();

    Initialized = true;
    MapLoaded = false;

	return true;
}

bool Map::Generate(Uint32 Seed)
{
    RANDOM->Seed(Seed);

    Uint32 TerrainX = MapSizeX + 1;
    Uint32 TerrainY = MapSizeY + 1;

    float** TerrainArray;
    TerrainArray = new float*[TerrainX];

    for(Uint32 i = 0; i < TerrainX; i++)
    {
        TerrainArray[i] = new float[TerrainY];

        for(Uint32 j = 0; j < TerrainY; j++)
        {
            TerrainArray[i][j] = RANDOM->Roll(0, 1);
        }
    }

    Slopping Type = SLOPE_FLAT;

    Cube* NewCube = NULL;
    Cell* TargetCell = NULL;

    for(Uint32 i = 0; i < MapSizeX; i++)
    {
        for(Uint32 j = 0; j < MapSizeY; j++)
        {
            TargetCell = getCubeOwner(i, j, 0);

            if(TargetCell)
            {
                if(!TargetCell->isInitalized())
                {
                    TargetCell->Init();
                }

                NewCube = getCube(i, j, 0);

                if (NewCube)
                {
                    if (NewCube->isInitalized() != true)
                    {
                        NewCube->Init(0);
                        if (Type != SLOPE_FLAT)
                        {
                            NewCube->SetSlope(Type);
                        }
                        NewCube->setVisible(true);
                    }
                }
            }
        }
    }

    for(Uint32 i = 0; i < TerrainX; i++)
    {
        delete TerrainArray[i];
    }

    delete TerrainArray;

    return true;
}

Cell* Map::getCell(Sint32 X, Sint32 Y, Sint32 Z)
{
	if (X >= 0 && X < CellSizeX && Y >= 0 && Y < CellSizeY)
	{
	    if(Z >= ColumnMatrix[X][Y]->BottomLevel() && Z < ColumnMatrix[X][Y]->TopLevel())
	    {
            return ColumnMatrix[X][Y]->getCell(Z);
	    }
	}
	return NULL;
}

Cell* Map::getCubeOwner(Sint32 X, Sint32 Y, Sint32 Z)
{
    if ((X > MapSizeX) || (Y > MapSizeY) || (Z > MapSizeZ))
    {
        return NULL;
    }

    Sint32 CellX = X / CELLEDGESIZE;
    Sint32 CellY = Y / CELLEDGESIZE;

    return getCell(CellX, CellY, Z);
}

Cube* Map::getCube(Sint32 X, Sint32 Y, Sint32 Z)
{
    Cell* TargetCell = getCubeOwner(X, Y, Z);

    if(TargetCell != NULL)
    {
        if(TargetCell->isInitalized())
        {
            Sint32 CubeX = X % CELLEDGESIZE;
            Sint32 CubeY = Y % CELLEDGESIZE;

            return TargetCell->getCube(CubeX, CubeY);
        }
        return NULL;
    }
    return NULL;
}

Face* Map::getFace(Sint32 X, Sint32 Y, Sint32 Z, Facet FaceType)
{
    Cell* TargetCell = getCubeOwner(X, Y, Z);

    if(TargetCell)
    {
        if(TargetCell->isInitalized())
        {
            Sint32 CubeX = X % CELLEDGESIZE;
            Sint32 CubeY = Y % CELLEDGESIZE;

            return TargetCell->getFace(CubeX, CubeY, FaceType);
        }
        return NULL;
    }
    return NULL;
}

void Map::LoadExtract()
{
    if(!EXTRACT->isMapLoaded())
    {
        return;
    }

    if(MapLoaded)
    {
        ReleaseMap();
    }

    DfMap* ExtractedMap = EXTRACT->getMap();

    // Initialize Cells and Cubes
    CellSizeX = ExtractedMap->getXBlocks();
	CellSizeY = ExtractedMap->getYBlocks();
	CellSizeZ = ExtractedMap->getZBlocks();


    ColumnMatrix = new Column**[CellSizeX];

	for (Uint16 i = 0; i < CellSizeX; i++)
	{
		ColumnMatrix[i] = new Column*[CellSizeY];

		for (Uint16 j = 0; j < CellSizeY; j++)
		{
			ColumnMatrix[i][j] = new Column();
			ColumnMatrix[i][j]->Init(i, j);

			for (Uint32 k = 0; k < CellSizeZ; k++)
			{
			    if(ExtractedMap->isBlockInitialized(i, j, k))
			    {
                    Cell* NewCell = new Cell(i * CELLEDGESIZE, j * CELLEDGESIZE, k);
                    NewCell->Init();

                    for(Uint32 l = 0; l < CELLEDGESIZE; l++)
                    {
                        for(Uint32 m = 0; m < CELLEDGESIZE; m++)
                        {
                            LoadCubeData(NewCell, i, j, k, l, m);
                        }
                    }

                    ColumnMatrix[i][j]->PushCell(NewCell, k);
			    }
			}
		}
	}

    MapSizeX = CellSizeX * CELLEDGESIZE;
	MapSizeY = CellSizeY * CELLEDGESIZE;
	MapSizeZ = CellSizeZ;

	for (Uint32 i = 0; i < CellSizeX; i++)
	{
		for (Uint32 j = 0; j < CellSizeY; j++)
		{
			for (Sint32 k = ColumnMatrix[i][j]->BottomLevel(); k < ColumnMatrix[i][j]->TopLevel(); k++)
			{
			    if(getCell(i, j, k) != NULL)
			    {
                    for(Uint32 l = 0; l < CELLEDGESIZE; l++)
                    {
                        for(Uint32 m = 0; m < CELLEDGESIZE; m++)
                        {
                            Cube* TargetCube = getCube((i * CELLEDGESIZE) + l, (j * CELLEDGESIZE) + m, k);
                            if(TargetCube != NULL && TargetCube->isInitalized())
                            {
                                if(TargetCube->isSolid())
                                {
                                    TargetCube->InitFacesSolid();
                                }
                                else
                                {
                                    TargetCube->InitFacesOpen();
                                }

                                if(TargetCube->getSlope() != NULL)
                                {
                                    TargetCube->DetermineSlope();
                                }
                            }
                        }
                    }
			    }
			}
		}
	}

    // Initialize Drawlists
	for(Uint16 Zlevel = 0; Zlevel < getCellSizeZ(); Zlevel++)
	{
        for (Uint32 SizeX = 0; SizeX < getCellSizeX(); SizeX++)
        {
            for (Uint32 SizeY = 0; SizeY < getCellSizeY(); SizeY++)
            {
                Cell* LoopCell = getCell(SizeX, SizeY, Zlevel);

                if(LoopCell != NULL && LoopCell->isActive())
                {
                    if(LoopCell->isDirtyDrawList())
                    {
                        // Rebuild the new Drawlist
                        GLuint DrawListID = LoopCell->getDrawListID();
                        glDeleteLists(DrawListID, 5);

                        for(CameraOrientation Orientation = CAMERA_DOWN; Orientation < NUM_ORIENTATIONS; ++Orientation)
                        {
                            SCREEN->RefreshDrawlist(LoopCell, DrawListID + (GLuint) Orientation, Orientation);
                        }
                        LoopCell->setDirtyDrawList(false);
                    }
                }
            }
        }
	}

    MapLoaded = true;
}

void Map::LoadCubeData(Cell* TargetCell, Uint32 CellX, Uint32 CellY, Uint32 CellZ, Uint32 CubeX, Uint32 CubeY)
{
    Uint32 MapX = (CellX * CELLEDGESIZE) + CubeX;
	Uint32 MapY = (CellY * CELLEDGESIZE) + CubeY;
	Uint32 MapZ = CellZ;

    DfMap *df_map = EXTRACT->getMap();

    int TileType = df_map->getTileType(CellX, CellY, CellZ, CubeX, CubeY);

    bool IsFloor = df_map->isFloorTerrain(TileType);
    bool IsWall = df_map->isWallTerrain(TileType);
    bool IsOpen = df_map->isOpenTerrain(TileType);
    bool IsRamp = df_map->isRampTerrain(TileType);
    bool IsStairs = df_map->isStairTerrain(TileType);

    int Liquid = df_map->getLiquidLevel(MapX, MapY, MapZ);

    if(IsFloor || IsWall || IsOpen || IsRamp || IsStairs)
    {
        Cube* TargetCube = TargetCell->getCube(CubeX, CubeY);

        TargetCube = new Cube();
        TargetCell->setCube(TargetCube, CubeX, CubeY);
        TargetCube->setPosition((float) MapX, (float) MapY, (float) MapZ);

        Uint16 Material = PickTexture(TileType);

        bool Hidden = df_map->isHidden(MapX, MapY, MapZ);
        TargetCube->setHidden(Hidden);

        TargetCube->setSubTerranean(df_map->isSubterranean( MapX, MapY, MapZ));
        TargetCube->setSkyView(df_map->isSkyView( MapX, MapY, MapZ));
        TargetCube->setSunLit(df_map->isSunLit( MapX, MapY, MapZ));

        if(IsWall)
        {
            TargetCube->Init(Material);
        }
        if(IsOpen)
        {
            TargetCube->Open();
        }
        if(IsRamp)
        {
            TargetCube->Init(Material);
            TargetCube->Open();
            TargetCube->SetSlope(SLOPE_FLAT);  // Prime the Slope, the type can not yet be determined
        }
        if(IsFloor)
        {
            TargetCube->InitConstructedFace(FACET_BOTTOM, Material);
        }
        if(IsStairs)
        {
            TargetCube->Init(Material);
            TargetCube->Open();
            TargetCube->SetSlope(SLOPE_FLAT);
            //TODO render stairs differently
        }
        if(Liquid)
        {
            //TargetCube->Open();
            TargetCube->setLiquid((Uint8) Liquid);

            if(df_map->isMagma( MapX, MapY, MapZ))
            {
                TargetCube->InitConstructedFace(FACET_TOP, DATA->getLabelIndex("MATERIAL_LAVA"));
            }
            else
            {
                TargetCube->InitConstructedFace(FACET_TOP, DATA->getLabelIndex("MATERIAL_WATER"));
            }
        }
        TargetCube->setVisible(true);
    }
}

void Map::InitilizeTilePicker()
{
    for(int i = 0; i < 600; ++i)
    {
        TilePicker[i] = DATA->getLabelIndex("TEXTURE_NEHE");
    }

    for(Uint32 i = 0; i < DATA->getNumMaterials(); ++i)
    {
        for(Uint32 j = 0; j < DATA->getMaterialData(i)->TileTypes.size(); ++j)
        {
            int Tile = DATA->getMaterialData(i)->TileTypes[j];
            TilePicker[Tile] = i;
        }
    }
}

Uint32 Map::PickTexture(int TileType)
{
    return TilePicker[TileType];
}

Vector3 Map::getMapCenter()
{
    Vector3 CenterPoint;

    CenterPoint.x = getMapSizeX() / 2;
    CenterPoint.y = getMapSizeY() / 2;
    CenterPoint.z = getMapSizeZ();

    return CenterPoint;
}

void Map::ReleaseMap()
{
    MapLoaded = false;

    if (ColumnMatrix != NULL)
    {
        for (Uint32 x = 0; x < CellSizeX; ++x)
        {
            if (ColumnMatrix[x] != NULL)
            {
                for (Uint32 y = 0; y < CellSizeY; ++y)
                {
                    if (ColumnMatrix[x][y] != NULL)
                    {
                        delete ColumnMatrix[x][y];
                    }
                }
                delete[] ColumnMatrix[x];
            }
        }
        delete[] ColumnMatrix;
        ColumnMatrix = NULL;
    }
}

