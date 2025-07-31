#include <iostream>
#include <fstream>
#include <cstdio> // For FILE* and fopen
#include "tsCommon.h"   
#include "tsTransportStream.h"

using namespace std;

//=============================================================================================================================================================================

int main(int argc, char* argv[], char* envp[])
{


   
    FILE* transportStream = std::fopen("example_new.ts", "rb");

    
    if (!transportStream) {
        cout << "File opening failed" << endl;
        return EXIT_FAILURE;
    }
    else {
        cout << "File opened successfully (binary mode)" << endl;
    }

    xTS_PacketHeader TS_PacketHeader;
    xTS_AdaptationField TS_AdaptationField;


    const size_t TS_PACKET_SIZE = 188;
    uint8_t TS_PacketBuffer[TS_PACKET_SIZE];
    int32_t TS_PacketId = 0;

    while (std::fread(TS_PacketBuffer, 1, TS_PACKET_SIZE, transportStream) == TS_PACKET_SIZE) {
        TS_PacketHeader.Reset();
		TS_AdaptationField.Reset();
        TS_PacketHeader.Parse(TS_PacketBuffer);

        if (!(TS_PacketHeader.getAdaptationFieldControl() == 2 || TS_PacketHeader.getAdaptationFieldControl() == 3)) {
            printf("%010d ", TS_PacketId);
            TS_PacketHeader.Print();
            printf("\n");
        }

        if (TS_PacketHeader.getAdaptationFieldControl() == 2 || TS_PacketHeader.getAdaptationFieldControl() == 3)
        {
			TS_AdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.getAdaptationFieldControl());
            printf("%010d ", TS_PacketId);
            TS_PacketHeader.Print();
            TS_AdaptationField.Print();
            printf("\n");
        }

        TS_PacketId++;
    }

    // Close the file
    std::fclose(transportStream);

    return EXIT_SUCCESS;
}

//=============================================================================================================================================================================
