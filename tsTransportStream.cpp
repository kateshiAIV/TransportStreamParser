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
	m_AdaptationFieldLength = 0;
    m_DC = 0; // Discontinuity indicator
    m_RA = 0; // Random access indicator
    m_SP = 0; // Elementary stream priority indicator 
    m_PR = 0; // Program Clock Reference flag // PCR_flag
	m_OR = 0; // Original Program Clock Reference flag // OPCR_flag
    m_SF = 0; // Splicing point flag
    m_TP = 0; // Transport private data flag 
    m_EX = 0; // Adaptation field extension flag
    program_clock_reference_base = 0;
}
/**
@brief Parse adaptation field
@param PacketBuffer is pointer to buffer containing TS packet
@param AdaptationFieldControl is value of Adaptation Field Control field of
corresponding TS packet header
@return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* Input, uint8_t AdaptationFieldControl) // pass from TS packet header
{


    if (Input == nullptr) return -1;
    m_AdaptationFieldLength = Input[4];
    m_RA = (Input[5] & 0x80) != 0;
    m_DC = (Input[5] & 0x40) != 0;
    m_SP = (Input[5] & 0x20) != 0;
    m_PR = (Input[5] & 0x10) != 0;
    m_OR = (Input[5] & 0x08) != 0;
    m_SF = (Input[5] & 0x04) != 0;
    m_TP = (Input[5] & 0x02) != 0;
    m_EX = (Input[5] & 0x01) != 0;
    //0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
    // ??
    if (m_PR == 1) {
        m_PCR |= static_cast<uint64_t>(Input[6]) << 25;
        m_PCR |= static_cast<uint64_t>(Input[7]) << 17;
        m_PCR |= static_cast<uint64_t>(Input[8]) << 9;
        m_PCR |= static_cast<uint64_t>(Input[9]) << 1;
        m_PCR |= static_cast<uint64_t>(Input[10]) >> 7;
    }
    
    if (m_PR == 1) {
        uint64_t base =
            ((uint64_t)Input[6] << 25) |
            ((uint64_t)Input[7] << 17) |
            ((uint64_t)Input[8] << 9) |
            ((uint64_t)Input[9] << 1) |
            ((uint64_t)(Input[10]) >> 7);

        uint16_t extension =
            ((Input[10] & 0x01) << 8) | Input[11];

        program_clock_reference_base = base;
        program_clock_reference_extension = extension;
        m_PCR = (base);

        //101101010000010000000000000 // 94904320
        //11010100001000001011000000 // 55608000
//base  //101101010000010000 ?? wtf
    }




	return m_AdaptationFieldLength; 
    //parsing
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
    if (m_PR == 1) {
        printf(" || AF: L=%3d DC=%d RA=%d SP=%d PR=%d OR=%d SF=%d TP=%d EX=%d PCR=%10d",
            m_AdaptationFieldLength,
            m_RA,
            m_DC,
            m_SP,
            m_PR,
            m_OR,
            m_SF,
            m_TP,
            m_EX,
            m_PCR);
    }
}
