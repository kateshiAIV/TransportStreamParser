#include <iostream>
#include "tsTransportStream.h"

//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================


/// @brief Reset - reset all TS packet header fields
void xTS_PacketHeader::Reset()
{
    m_SB = 0;
    m_E = 0;  // Transport error indicator
    m_S = 0;  // Payload unit start indicator
    m_T = 0;  // Transport priority
    m_PID = 0; // Packet Identifier
    m_TSC = 0; // Transport scrambling control
    m_AFC = 0; // Adaptation field control
    m_CC = 0; // Continuity counter
}

/**
  @brief Parse all TS packet header fields
  @param Input is pointer to buffer containing TS packet
  @return Number of parsed bytes (4 on success, -1 on failure) 
 */
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{


    if (Input == nullptr) return NOT_VALID;
	m_SB = Input[0];
    m_E = (Input[1] & 0x80) != 0; // Transport error indicator
	m_S = (Input[1] & 0x40) != 0; // Payload unit start indicator
	m_T = (Input[1] & 0x20) != 0; // Transport priority
	m_PID = ((Input[1] & 0x1F) << 8) | Input[2]; // Packet Identifier
	m_TSC = (Input[3] >> 6) & 0x03; // Transport scrambling control m_TSC = (Input[3] >> 6) & 0x03;
	m_AFC = (Input[3] >> 4) & 0x03; // Adaptation field control     m_AFC = (Input[3] >> 4) & 0x03;
	m_CC = (Input[3] & 0x0F); // Continuity counter

  return 4;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const  
{  
    printf("TS: SB=%02d E=%d S=%d P=%d PID=%5d TSC=%d AF=%d CC=%3d",
        m_SB,
        m_E,
        m_S,
        m_T,
        m_PID,
        m_TSC,
        m_AFC,
        m_CC);
}

//=============================================================================================================================================================================
// xTS_AdaptationField
//=============================================================================================================================================================================



// @brief Reset - reset all TS packet header fields
void xTS_AdaptationField::Reset()
{
    //reset
}
/**
@brief Parse adaptation field
@param PacketBuffer is pointer to buffer containing TS packet
@param AdaptationFieldControl is value of Adaptation Field Control field of
corresponding TS packet header
@return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl) // pass from TS packet header
{

	return -1; //TODO: implement
    //parsing
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
    //print print print
}
