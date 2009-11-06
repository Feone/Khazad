#ifndef MAP__HEADER
#define MAP__HEADER

#include <Singleton.h>
#include <stdafx.h>

#include <Vector3.h>

#define CELLEDGESIZE 16
#define HALFCUBE 0.5

struct MapCoordinates
{
    MapCoordinates()
    {
        X = 0;
        Y = 0;
        Z = 0;
    };

    MapCoordinates(Vector3 Point)
    {
        X = Point.x;
        Y = Point.y;
        Z = Point.z;
    };

    MapCoordinates(Sint32 NewX, Sint32 NewY, Sint32 NewZ)
    {
        X = NewX;
        Y = NewY;
        Z = NewZ;
    };

    MapCoordinates& operator= (const MapCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;
        Z = ArgumentCoordinates.Z;
        return *this;
    };

    Sint32 X;
    Sint32 Y;
    Sint32 Z;
};

struct CellCoordinates
{
    CellCoordinates()
    {
        X = 0;
        Y = 0;
        Z = 0;
    };

    CellCoordinates(Sint16 NewX, Sint16 NewY, Sint16 NewZ)
    {
        X = NewX;
        Y = NewY;
        Z = NewZ;
    };

    CellCoordinates& operator= (const CellCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;
        Z = ArgumentCoordinates.Z;
        return *this;
    };

    CellCoordinates(MapCoordinates SourceCoordinates)
    {
        X = SourceCoordinates.X / CELLEDGESIZE;
        Y = SourceCoordinates.Y / CELLEDGESIZE;
        Z = SourceCoordinates.Z;
    };

    Sint16 X;
    Sint16 Y;
    Sint16 Z;
};

struct CubeCoordinates
{
    CubeCoordinates()
    {
        X = 0;
        Y = 0;
    };

    CubeCoordinates(Uint8 NewX, Uint8 NewY)
    {
        X = NewX;
        Y = NewY;
    };

    CubeCoordinates& operator= (const CubeCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;
        return *this;
    };

    CubeCoordinates(MapCoordinates SourceCoordinates)
    {
        X = SourceCoordinates.X % CELLEDGESIZE;
        Y = SourceCoordinates.Y % CELLEDGESIZE;
    };

    Uint8 X;
    Uint8 Y;
};

struct Face
{
    inline Face()
    {
        PositiveAxisSurfaceTypeID = -1;
        NegativeAxisSurfaceTypeID = -1;

        MaterialTypeID = -1;
    };

    Sint16 PositiveAxisSurfaceTypeID;
    Sint16 NegativeAxisSurfaceTypeID;

    Sint16 MaterialTypeID;
};


struct t_matglossPair;
class Column;
class Cell;
class Actor;
class Face;
class TreeManager;
union t_occupancy;
struct t_construction;
struct t_tree_desc;
struct t_building;
struct Face;
class DFHackAPI;


class Map
{
DECLARE_SINGLETON_CLASS(Map)

public:

	~Map();
	bool Init();

    bool isInitialized()        { return Initialized; }
    bool isMapLoaded()          { return MapLoaded; }

	Cell* getCell(CellCoordinates);
	Cell* getCubeOwner(MapCoordinates);
    bool isCubeInited(MapCoordinates);

    void setCellNeedsReDraw(CellCoordinates, bool NewValue);

    CellCoordinates TranslateMapToCell(MapCoordinates Coordinates);
    CubeCoordinates TranslateMapToCube(MapCoordinates Coordinates);


    bool isCubeSloped(MapCoordinates Coordinates);

    void setCubeShape(MapCoordinates Coordinates, Sint16 TileShape);
    inline Sint16 getCubeShape(MapCoordinates Coordinates);

    void setCubeMaterial(MapCoordinates Coordinates, Sint16 MaterialID);
    inline Sint16 getCubeMaterial(MapCoordinates Coordinates);

    void setCubeSurfaceType(MapCoordinates Coordinates, Sint16 SurfaceID);
    inline Sint16 getCubeSurfaceType(MapCoordinates Coordinates);



    Face* getFace(MapCoordinates Coordinates, Facet FacetType);
    bool hasFace(MapCoordinates Coordinates, Facet FacetType);

    bool removeFace(MapCoordinates Coordinates, Facet FacetType);
    Face* addFace(MapCoordinates Coordinates, Facet FacetType);

    void setFaceMaterial(MapCoordinates Coordinates, Facet FacetType, Sint16 MaterialID);
    inline Sint16 getFaceMaterial(MapCoordinates Coordinates, Facet FacetType);

    void setFaceSurfaceType(MapCoordinates Coordinates, Facet FacetType, Sint16 SurfaceID);
    inline Sint16 getFaceSurfaceType(MapCoordinates Coordinates, Facet FacetType);
    void setBothFaceSurfaceTypes(MapCoordinates Coordinates, Facet FacetType, Sint16 SurfaceID);


    bool isCubeHidden(MapCoordinates Coordinates);
    void setCubeHidden(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSubTerranean(MapCoordinates Coordinates);
    void setCubeSubTerranean(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSkyView(MapCoordinates Coordinates);
    void setCubeSkyView(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSunLit(MapCoordinates Coordinates);
    void setCubeSunLit(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSolid(MapCoordinates Coordinates);
    void setCubeSolid(MapCoordinates Coordinates, bool NewValue);



    void InitilizeTilePicker(DFHackAPI & DF);
    Sint16 PickMaterial(Sint16 TileType, Sint16 basematerial, Sint16 veinmaterial, t_matglossPair constructionmaterial, t_occupancy occupancy);
    Sint16 ResolveMatGlossPair(t_matglossPair MatPair);

    bool Generate(Uint32 Seed);

	Uint32 getMapSizeX() { return MapSizeX; }
	Uint32 getMapSizeY() { return MapSizeY; }
	Uint32 getMapSizeZ() { return MapSizeZ; }

	Uint16 getCellSizeX() { return CellSizeX; }
	Uint16 getCellSizeY() { return CellSizeY; }
	Uint16 getCellSizeZ() { return CellSizeZ; }

    bool Extract();
    bool Load(string filename);
    void Save(string filename);

    void ReleaseMap();

    void DigChannel(MapCoordinates Coordinates);
    void DigSlope(MapCoordinates Coordinates);
    void Dig(MapCoordinates Coordinates);

    void LoadCellData(DFHackAPI & context,
                      vector< vector <uint16_t> >& layerassign,
                      Cell* TargetCell,
                      map<uint64_t, t_construction> & constuctions,
                      map<uint64_t, t_tree_desc> & vegetation,
                      map<uint64_t, t_building> & buildings,
                      CellCoordinates NewCellCoordinates);

    void ChangeCellCount(Sint8 Change)      { CellCount += Change; }
    Uint32 getCellCount()                   { return CellCount; }

    void ChangeInitedCellCount(Sint8 Change)      { InitedCellCount += Change; }
    Uint32 getInitedCellCount()                   { return InitedCellCount; }

    void ChangeFaceCount(Sint8 Change)      { FaceCount += Change; }
    Uint32 getFaceCount()                   { return FaceCount; }


    MapCoordinates getMapCenter();

    TreeManager* TreeMan;

protected:

    bool Initialized;
    bool MapLoaded;

    Uint32 MapSizeX;
    Uint32 MapSizeY;
    Uint32 MapSizeZ;

    Uint16 CellSizeX;
    Uint16 CellSizeY;
    Uint16 CellSizeZ;

    Column*** ColumnMatrix;

    Sint16 TileShapePicker[600];
    Sint16 TileSurfacePicker[600];
    Sint16 TileMaterialPicker[600];
    Sint16 TileMaterialClassPicker[600];

    vector <int16_t> StoneMatGloss;
    vector <int16_t> MetalMatGloss;

    Uint32 CellCount;
    Uint32 FaceCount;

    Uint32 InitedCellCount;
};

#define MAP (Map::GetInstance())

#endif // MAP__HEADER
