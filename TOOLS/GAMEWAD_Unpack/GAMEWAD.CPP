#include "GAMEWAD.H"

#include "FILES.H"
#include "IO.H"
#include "CD.H"

#include <cstdio>
#include <cstring>

namespace TombRaiderChronicles
{
	static GAMEWAD_header gameWad;

	void UnpackGamewad(const char* fileName)
	{
		IO::StreamReader reader(fileName);
		if (!reader.m_isOpen)
		{
			printf("Warning: failed to open file: %s\n", fileName);
			return;
		}

		char* fileData = new char[NUM_GAMEWAD_FILE_ENTRIES * sizeof(GAMEWAD_entry)];
		reader.readRaw(fileData, NUM_GAMEWAD_FILE_ENTRIES * sizeof(GAMEWAD_entry));
		GAMEWAD_entry* entry = (GAMEWAD_entry*)fileData;

		for (int i = 0; i < NUM_GAMEWAD_FILE_ENTRIES; i++)
		{
			//Skip null entries
			if (entry[i].fileOffset > 0 && entry[i].fileSize > 0)
			{
				reader.seek(entry[i].fileOffset);

				//Level file
				if (i > CUTSEQ && i < RESERVED_17)
				{
					//Loading Screen
					char* loadingScreenData = new char[LOADING_SCREEN_IMG_SIZE];
					reader.readRaw(loadingScreenData, LOADING_SCREEN_IMG_SIZE);
					char buf[32];
					sprintf(buf, "%s.RAW", gwFileNames[i]);
					IO::StreamWriter writer(buf);
					writer.write(loadingScreenData, LOADING_SCREEN_IMG_SIZE);
					delete[] loadingScreenData;
					writer.close();
					memset(&buf[0], 0, sizeof(buf));

					//Loading CD
					char* loadingCdData = new char[LOADING_CD_IMG_SIZE];
					reader.readRaw(loadingCdData, LOADING_CD_IMG_SIZE);
					sprintf(buf, "%s_CD.RAW", gwFileNames[i]);
					writer.open(buf);
					writer.write(loadingCdData, LOADING_CD_IMG_SIZE);
					delete[] loadingCdData;
					writer.close();
					memset(&buf[0], 0, sizeof(buf));

					//SETUP.MOD
					char* modData = new char[entry[NONE].fileSize];
					reader.readRaw(modData, entry[NONE].fileSize);
					writer.open(gwFileNames[NONE]);
					writer.write(modData, entry[NONE].fileSize);
					delete[] modData;
					writer.close();

					//Level data
					reader.seek((int)reader.m_pos + (CD_SECTOR_SIZE - 1) & ~(CD_SECTOR_SIZE - 1));
					int dataSize = entry[i].fileSize - ((int)reader.m_pos - entry[i].fileOffset) - 0x691B4;///@HACK
					char* levelData = new char[dataSize];
					reader.readRaw(levelData, dataSize);
					sprintf(buf, "%s.PSX", gwFileNames[i]);
					writer.open(buf);
					writer.write(levelData, dataSize);
					delete[] levelData;
					writer.close();

					//Code modules
					reader.seek((int)reader.m_pos + (CD_SECTOR_SIZE - 1) & ~(CD_SECTOR_SIZE - 1));

					/*
					 * Getting size of codewad
					 */
					int codeWadStart = (int)reader.m_pos;
					reader.seek(8 * 146 + (unsigned int)reader.m_pos);
					int dataOffset;
					reader.read(dataOffset);
					reader.read(dataSize);
					reader.seek(codeWadStart);
					dataSize += dataOffset;

					char* codeData = new char[dataSize];//Sizeof codewad
					reader.readRaw(codeData, dataSize);
					writer.open("DATA\\CODE.WAD");
					writer.write(codeData, dataSize);
					delete[] codeData;
					writer.close();

				}
				else if (i > RESERVED_40 && i < UNKNOWN_49)
				{
					//Loading Screen
					char* loadingScreenData = new char[LOADING_SCREEN_IMG_SIZE];
					reader.readRaw(loadingScreenData, LOADING_SCREEN_IMG_SIZE);
					char buf[32];
					sprintf(buf, "%s.RAW", gwFileNames[i]);
					IO::StreamWriter writer(buf);
					writer.write(loadingScreenData, LOADING_SCREEN_IMG_SIZE);
					delete[] loadingScreenData;
					writer.close();
					memset(&buf[0], 0, sizeof(buf));

					//Loading CD
					char* loadingCdData = new char[LOADING_CD_IMG_SIZE];
					reader.readRaw(loadingCdData, LOADING_CD_IMG_SIZE);
					sprintf(buf, "%s_CD.RAW", gwFileNames[i]);
					writer.open(buf);
					writer.write(loadingCdData, LOADING_CD_IMG_SIZE);
					delete[] loadingCdData;
					writer.close();
					memset(&buf[0], 0, sizeof(buf));
				}
				else
				{
					char* entryData = new char[entry[i].fileSize];
					reader.readRaw(entryData, entry[i].fileSize);
					IO::StreamWriter writer(gwFileNames[i]);
					writer.write(entryData, entry[i].fileSize);
					delete[] entryData;
					writer.close();
				}
			}
		}

		delete[] fileData;
		reader.close();
	}
}
