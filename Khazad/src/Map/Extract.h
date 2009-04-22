#ifndef EXTRACT_HEADER
#define EXTRACT_HEADER

#include <Singleton.h>
#include <stdafx.h>

class memory_info
{
public:
    char version[20];
    int pe_timestamp;
    int pe_timestamp_offset;
    int map_offset;
    int x_count_offset;
    int y_count_offset;
    int z_count_offset;
    int tile_type_offset;
    int designation_offset;
    int occupancy_offset;
};

class Extractor
{
DECLARE_SINGLETON_CLASS(Extractor)

public:

    bool MapLoaded;

    short int*** Tiles;

    int*** Blocks;

    std::vector<memory_info> meminfo;

    int x_blocks;
    int y_blocks;
    int z_levels;
    int z_active_levels;
    short int *z_level_active;

    bool Init();
    ~Extractor();

    int dumpMemory();
    int loadMap(char* FilePath);

    int readMemoryFile();
    bool FreeMapMemory();

    int picktexture(int);
    int isOpenTerrain(int);
    int isStairTerrain(int);
    int isRampTerrain(int);
    int isFloorTerrain(int);
    int isWallTerrain(int);

};


#define EXTRACT (Extractor::GetInstance())

#endif // EXTRACT_HEADER
