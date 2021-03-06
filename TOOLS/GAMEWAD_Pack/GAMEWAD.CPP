#include "GAMEWAD.H"

#include "FILES.H"
#include "IO.H"
#include "CD.H"

#include <cstdio>
#include <cstring>

namespace TombRaiderChronicles
{
	static GAMEWAD_header gameWad;

	void PackGamewad()
	{
		int currentOffset = 0;

		IO::StreamWriter writer(GAMEWAD_FILENAME);
		char* modData = NULL;

		for (int i = 0; i < NUM_GAMEWAD_FILE_ENTRIES; i++)
		{
			writer.seek(currentOffset);

			if (i == NONE)
			{
				//SETUP.MOD
				IO::StreamReader reader(gwFileNames[i]);
				if (reader.m_isOpen)
				{
					gameWad.entries[i].fileSize = (int)reader.m_length;
					gameWad.entries[i].fileOffset = currentOffset;
					currentOffset++;//HACK - So next entry is aligned to 2048 bytes

					if (gameWad.entries[i].fileSize > 0)
					{
						if (modData == NULL)
						{
							modData = new char[gameWad.entries[i].fileSize];
							reader.readRaw(modData, gameWad.entries[i].fileSize);
						}
					}

					reader.close();
				}
			}
			else if (i > CUTSEQ && i < RESERVED_17)
			{
				gameWad.entries[i].fileOffset = currentOffset;
				char buf[32];
				sprintf(buf, "%s.RAW", gwFileNames[i]);
				IO::StreamReader reader(buf);
				int dataSize = (int)reader.m_length;
				char* loadScreenData = new char[dataSize];
				reader.readRaw(loadScreenData, dataSize);
				gameWad.entries[i].fileSize += dataSize;
				reader.close();
				memset(&buf[0], 0, sizeof(buf));
				writer.write(loadScreenData, dataSize);
				delete[] loadScreenData;

				sprintf(buf, "%s_CD.RAW", gwFileNames[i]);
				reader.open(buf);
				dataSize = (int)reader.m_length;
				char* loadCdData = new char[dataSize];
				reader.readRaw(loadCdData, dataSize);
				gameWad.entries[i].fileSize += dataSize;
				reader.close();
				memset(&buf[0], 0, sizeof(buf));
				writer.write(loadCdData, dataSize);
				delete[] loadCdData;

				gameWad.entries[i].fileSize += gameWad.entries[NONE].fileSize;

				if (modData != NULL)
				{
					writer.write(modData, gameWad.entries[NONE].fileSize);

					gameWad.entries[i].fileSize = gameWad.entries[i].fileSize + (CD_SECTOR_SIZE - 1) & ~(CD_SECTOR_SIZE - 1);
					writer.seek((int)writer.m_pos + (CD_SECTOR_SIZE - 1) & ~(CD_SECTOR_SIZE - 1));
				}

				sprintf(buf, "%s.PSX", gwFileNames[i]);
				reader.open(buf);
				dataSize = (int)reader.m_length;
				char* levelData = new char[dataSize];
				reader.readRaw(levelData, dataSize);
				gameWad.entries[i].fileSize += dataSize;
				reader.close();
				writer.write(levelData, dataSize);
				delete[] levelData;

				gameWad.entries[i].fileSize = gameWad.entries[i].fileSize + (CD_SECTOR_SIZE-1) & ~(CD_SECTOR_SIZE-1);
				writer.seek((int)writer.m_pos + (CD_SECTOR_SIZE-1) & ~(CD_SECTOR_SIZE-1));
				reader.open("DATA\\CODE.WAD");
				dataSize = (int)reader.m_length;
				char* codeData = new char[dataSize];
				reader.readRaw(codeData, dataSize);
				gameWad.entries[i].fileSize += dataSize;
				reader.close();
				writer.write(codeData, dataSize);
				delete[] codeData;

				currentOffset += gameWad.entries[i].fileSize;
			}
			else if (i > RESERVED_40 && i < UNKNOWN_49)
			{
				gameWad.entries[i].fileOffset = currentOffset;
				char buf[32];
				sprintf(buf, "%s.RAW", gwFileNames[i]);
				IO::StreamReader reader(buf);
				int dataSize = (int)reader.m_length;
				char* loadScreenData = new char[dataSize];
				reader.readRaw(loadScreenData, dataSize);
				gameWad.entries[i].fileSize += dataSize;
				reader.close();
				memset(&buf[0], 0, sizeof(buf));
				writer.write(loadScreenData, dataSize);
				delete[] loadScreenData;

				sprintf(buf, "%s_CD.RAW", gwFileNames[i]);
				reader.open(buf);
				dataSize = (int)reader.m_length;
				char* loadCdData = new char[dataSize];
				reader.readRaw(loadCdData, dataSize);
				gameWad.entries[i].fileSize += dataSize;
				reader.close();
				memset(&buf[0], 0, sizeof(buf));
				writer.write(loadCdData, dataSize);
				delete[] loadCdData;

				currentOffset += gameWad.entries[i].fileSize;
			}
			else
			{
				//CUTSEQ.JIZ
				IO::StreamReader reader(gwFileNames[i]);
				if (reader.m_isOpen)
				{
					int dataSize = (int)reader.m_length;
					char* fileData = new char[dataSize];
					reader.readRaw(fileData, dataSize);
					gameWad.entries[i].fileSize = dataSize;
					gameWad.entries[i].fileOffset = currentOffset;
					currentOffset += dataSize;
					writer.write(fileData, dataSize);
					delete[] fileData;
					reader.close();
				}
			}
			currentOffset = currentOffset + (CD_SECTOR_SIZE-1) & ~(CD_SECTOR_SIZE-1);
		}

		sprintf(&gameWad.reserved[0], "TR5 DATA FILE (C) 2000 Core Design... 'gamewad.asm' Compiled by Del - %s %s.   this version built by Del, second attempt submission, 1/11/2000....        Hiya to Caroline and Abbie!                       lets hope we get to do a nice ONLINE game next... EVERQUEST RULES...                   ADSL ROCKS, when it works... someone really should kill BT , they suck.                                                        ", __TIME__, __DATE__);

		writer.seek(0);
		writer.write(&gameWad, sizeof(GAMEWAD_header));
		writer.close();
		delete[] modData;
	}
}
