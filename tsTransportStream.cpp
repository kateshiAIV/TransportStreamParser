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
    program_clock_reference_extension = 0;
	mStuffingBytes = 0;
    m_PCR = 0;
	m_time = 0.0f; // PCR time in seconds
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

    xTS m_xTS;


    if (Input == nullptr) return -1;
    m_AdaptationFieldLength = Input[4];
    if (m_AdaptationFieldLength == 0) {
        return 0; // No adaptation field
    }
    m_RA = (Input[5] & 0x80) != 0;
    m_DC = (Input[5] & 0x40) != 0;
    m_SP = (Input[5] & 0x20) != 0;
    m_PR = (Input[5] & 0x10) != 0;
    m_OR = (Input[5] & 0x08) != 0;
    m_SF = (Input[5] & 0x04) != 0;
    m_TP = (Input[5] & 0x02) != 0;
    m_EX = (Input[5] & 0x01) != 0;

 

    if (m_PR || m_OR) {
        if (m_AdaptationFieldLength < 7) {
            return m_AdaptationFieldLength;
        }


        uint64_t base = 
            (((uint64_t)Input[6] << 25) |
            ((uint64_t)Input[7] << 17) |
            ((uint64_t)Input[8] << 9) |
            ((uint64_t)Input[9] << 1) |
            ((uint64_t)Input[10] >> 7));

        uint64_t extension = (((uint64_t)Input[10] & 0x01 << 8) | (uint64_t)Input[11]);

		program_clock_reference_base = base;
		program_clock_reference_extension = extension;

        m_PCR = (base * m_xTS.BaseToExtendedClockMultiplier) + extension;

    }
	mStuffingBytes = m_AdaptationFieldLength - 1;
	m_time = static_cast<float>(m_PCR) / m_xTS.ExtendedClockFrequency_Hz;
    return m_AdaptationFieldLength;
    //parsing
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{

       printf(" || AF: L=%3d DC=%d RA=%d SP=%d PR=%d OR=%d SF=%d TP=%d EX=%d",
            m_AdaptationFieldLength,
            m_RA,
            m_DC,
            m_SP,
            m_PR,
            m_OR,
            m_SF,
            m_TP,
            m_EX);

        if (m_PR == true || m_OR == true) {
            printf(" PCR=%d (Time=%fs) Stuffing=0",
                m_PCR,
                m_time);
        }else {
            printf(" Stuffing=%d", m_AdaptationFieldLength - 1);
        }
   
}




void xPES_PacketHeader::Reset() {
    m_PacketStartCodePrefix = 0;
    m_StreamId = 0;
    m_PacketLength = 0;
    return;
}

int32_t xPES_PacketHeader::Parse(const uint8_t* Input) {
    if (Input == nullptr)
        return NOT_VALID;
    m_PacketStartCodePrefix = (static_cast<uint32_t>(Input[0]) << 16) | (static_cast<uint32_t>(Input[1]) << 8) | static_cast<uint32_t>(Input[2]);
    m_StreamId = Input[3];
    m_PacketLength = (static_cast<uint16_t>(Input[4]) << 8) | static_cast<uint16_t>(Input[5]);

    return 6; // Number of bytes parsed
}

void xPES_PacketHeader::Print() const {
    printf("PES: PSCP=%d SID=%d L=%d",
        m_PacketStartCodePrefix, m_StreamId, m_PacketLength);
    return;
}

void xPES_Assembler::xBufferReset() {
    m_LastContinuityCounter = -1;
    m_Started = false;
    m_DataOffset = 0;
    return;
}

void xPES_Assembler::xBufferAppend(const uint8_t* Data, int32_t Size) {
    if (m_Buffer == nullptr) // If buffer not initialized, allocate memory
    {
        m_BufferSize = Size;
        m_Buffer = new uint8_t[Size];
    }
    else // If buffer already initialized, reallocate memory if needed
    {
        uint8_t* tempBuffer = new uint8_t[m_DataOffset + Size];
        std::copy(m_Buffer, m_Buffer + m_DataOffset, tempBuffer);
        delete[] m_Buffer;
        m_Buffer = tempBuffer;
        m_BufferSize = m_DataOffset + Size;
    }
    std::copy(Data, Data + Size, m_Buffer + m_DataOffset);
    m_DataOffset += Size;
    return;
}

void xPES_Assembler::Init(int32_t PID) {
    m_PID = PID;
    m_Buffer = nullptr;
    m_BufferSize = 0;
    m_DataOffset = 0;
    m_LastContinuityCounter = -1;
    m_Started = false;
    return;
}

xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField) {
    const uint8_t* payloadData = TransportStreamPacket + xTS::TS_HeaderLength;
    int32_t payloadSize = xTS::TS_PacketLength - xTS::TS_HeaderLength;

    // Учитываем адаптационное поле
    if (PacketHeader->hasAdaptationField()) {
        payloadData += AdaptationField->getNumBytes();
        payloadSize -= AdaptationField->getNumBytes();
    }

    // Если нет полезной нагрузки, пропускаем
    if (!PacketHeader->hasPayload() || payloadSize <= 0) {
        return eResult::StreamPackedLost;
    }

    // Обработка начала PES-пакета
    if (PacketHeader->getPID() != m_PID) {
        Init(PacketHeader->getPID());
    }

    // Новый PES-пакет начинается только если установлен PUSI
    if (PacketHeader->getSyncByte() == 0x47 && PacketHeader->hasPayload() && PacketHeader->getPID() == m_PID && PacketHeader->getCC() != m_LastContinuityCounter) {
        if (PacketHeader->getCC() != ((m_LastContinuityCounter + 1) & 0x0F)) {
            m_Started = false;
            xBufferReset();
            return eResult::StreamPackedLost;
        }
    }

    if (PacketHeader->hasPayload() && PacketHeader->getPID() == m_PID && PacketHeader->getSyncByte() == 0x47) {
        if (PacketHeader->getCC() != ((m_LastContinuityCounter + 1) & 0x0F)) {
            m_Started = false;
            xBufferReset();
            return eResult::StreamPackedLost;
        }
    }

    if (PacketHeader->hasPayload() && PacketHeader->getPID() == m_PID && PacketHeader->getSyncByte() == 0x47 && PacketHeader->hasPayload() && PacketHeader->getCC() == m_LastContinuityCounter) {
        // Повтор пакета — игнорируем
        return eResult::AssemblingContinue;
    }

    // Если начало нового PES-пакета
    if (PacketHeader->hasPayload() && PacketHeader->getPID() == m_PID && PacketHeader->getSyncByte() == 0x47 && PacketHeader->hasPayload() && PacketHeader->getCC() != m_LastContinuityCounter && PacketHeader->getPID() == m_PID && PacketHeader->hasPayload() && PacketHeader->getSyncByte() == 0x47 && PacketHeader->hasPayload()) {
        bool PUSI = PacketHeader->getPayloadUnitStartIndicator();

        if (PUSI) {
            // Сброс буфера и парсинг заголовка
            xBufferReset();
            m_Started = true;
            m_LastContinuityCounter = PacketHeader->getCC();

            m_PESH.Reset();
            int32_t headerLen = m_PESH.Parse(payloadData);

            if (headerLen <= 0) {
                m_Started = false;
                return eResult::StreamPackedLost;
            }

            xBufferAppend(payloadData, payloadSize);  // Сохраняем всю полезную нагрузку
            return eResult::AssemblingStarted;
        }
        else if (m_Started) {
            m_LastContinuityCounter = PacketHeader->getCC();
            xBufferAppend(payloadData, payloadSize);

            // Проверка завершения по длине PES-пакета
            if (m_PESH.getPacketLength() > 0 && getNumPacketBytes() >= (m_PESH.getPacketLength() + xTS::PES_HeaderLength)) {
                m_Started = false;
                return eResult::AssemblingFinished;
            }

            return eResult::AssemblingContinue;
        }
    }

    return eResult::StreamPackedLost;
}
