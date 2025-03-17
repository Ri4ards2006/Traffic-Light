#include <SPI.h>
#include "mcp2515.h"
#include "SharkJ1939.h"

#define J1939_CAN_ID_MASK 0x1FFFFFFF
#define J1939_PGN_MASK 0x03FFFF00
#define J1939_PF_MASK 0x0000FF00
#define J1939_PS_MASK 0x000000FF

// PF <  240: Message is PDU1. (addressable message, PS contains destination address)
// PF >= 240: Message is PDU2. (broadcast message, PS contains group extension)
#define J1939_PDU1_RANGE(PF) (PF < 240)

// static PGN_KNOWN_ENTRY pgnEmptyTable[1] = {{0, NULL, NULL}}; // This caused invalid memory access.
static PGN_KNOWN_ENTRY pgnEmptyTable[2] = {{0, NULL, NULL}, {0, NULL, NULL}};

SharkJ1939::SharkJ1939(MCP2515 &mcp2515) : mcp2515Ref_(mcp2515)
{
  bindPGNKnownTable(&pgnEmptyTable[0]);
}

// Helper function to print hexadecimal values with leading zeros
inline void printHexWithLeadingZero(const uint8_t value)
{
  if (value < 0x10)
  {
    Serial.print('0');
  }
  Serial.print(value, HEX);
}

void SharkJ1939::dumpMessageInJ1939Format(const struct can_frame &j1939Msg)
{
  if (pauseFlag_)
    return;

  // Extract J1939 PGN, PF, and PS from CAN ID and print
  uint32_t j1939_29_bit_can_id = j1939Msg.can_id & J1939_CAN_ID_MASK;
  uint32_t j1939_18_bit_pgn = (j1939_29_bit_can_id & J1939_PGN_MASK) >> 8;
  uint8_t j1939_8_bit_pf = (j1939_18_bit_pgn & J1939_PF_MASK) >> 8;
  uint32_t pgn;

  if (j1939_29_bit_can_id < 0x10000000)
    Serial.print('0');                    // with a leading zero.
  Serial.print(j1939_29_bit_can_id, HEX); // print J1939 29 bit CAN ID

  ringHead_ &= 0x07; // Limit ringHead_ to a range of 0 to 7
  ringBuffer_[ringHead_++] = j1939_29_bit_can_id;

  Serial.print(' ');

  Serial.print(j1939Msg.can_dlc, HEX); // print DLC

  Serial.print(' ');

  for (int i = 0; i < j1939Msg.can_dlc; i++)
  { // print the data
    printHexWithLeadingZero(j1939Msg.data[i]);
    Serial.print(' ');
  }

  Serial.print(millis() + 1000000000); // 1000000000 is to print millis() with a fixed width.

  Serial.print(' ');

  if (J1939_PDU1_RANGE(j1939_8_bit_pf))
  {
    printHexWithLeadingZero(j1939_18_bit_pgn & J1939_PS_MASK); // PDU1 contains destination address.
    pgn = j1939_18_bit_pgn & 0x0003FF00;                       // remove destination address.
  }
  else
  {
    Serial.print("--"); // PDU2 contains group extension.
    pgn = j1939_18_bit_pgn;
  }

  Serial.print(' ');

  for (int index = 0;; index++)
  {
    if (pgnKnownTable_[index].pgn == 0) // The last entry with 0 to indicates the end of the table.
    {
      Serial.print(pgn); // Print the PGN number for unknown PGN.
      break;
    }

    if (pgnKnownTable_[index].pgn != pgn)
    {
      continue;
    }

    Serial.print(pgnKnownTable_[index].pgnAcronym); // Print the acronym (PGN name) for the known PGN.
    if (pgnKnownTable_[index].pgnCallback != NULL)
    {
      pgnKnownTable_[index].pgnCallback(j1939Msg.can_dlc, j1939Msg.data); // Call the callback function with the CAN data.
    }

    break;
  }

  Serial.println();
}

void SharkJ1939::processUserInput(void)
{
  if (Serial.read() == '\n')
  {
    Serial.println(F("       DLC 1  2  3  4  5  6  7  8  msec      DST PGN"));
    pauseFlag_ = !pauseFlag_;
  }
}

#ifdef FEATURE_SDM_CRC32
// This lookup table was created using http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
// The CRC polynomial used: 0x6938392D
static const uint32_t sdm_crc32_lookup_table[] = {
    0x00000000, 0x6938392D, 0xD270725A, 0xBB484B77, 0xCDD8DD99, 0xA4E0E4B4, 0x1FA8AFC3, 0x769096EE,
    0xF289821F, 0x9BB1BB32, 0x20F9F045, 0x49C1C968, 0x3F515F86, 0x566966AB, 0xED212DDC, 0x841914F1,
    0x8C2B3D13, 0xE513043E, 0x5E5B4F49, 0x37637664, 0x41F3E08A, 0x28CBD9A7, 0x938392D0, 0xFABBABFD,
    0x7EA2BF0C, 0x179A8621, 0xACD2CD56, 0xC5EAF47B, 0xB37A6295, 0xDA425BB8, 0x610A10CF, 0x083229E2,
    0x716E430B, 0x18567A26, 0xA31E3151, 0xCA26087C, 0xBCB69E92, 0xD58EA7BF, 0x6EC6ECC8, 0x07FED5E5,
    0x83E7C114, 0xEADFF839, 0x5197B34E, 0x38AF8A63, 0x4E3F1C8D, 0x270725A0, 0x9C4F6ED7, 0xF57757FA,
    0xFD457E18, 0x947D4735, 0x2F350C42, 0x460D356F, 0x309DA381, 0x59A59AAC, 0xE2EDD1DB, 0x8BD5E8F6,
    0x0FCCFC07, 0x66F4C52A, 0xDDBC8E5D, 0xB484B770, 0xC214219E, 0xAB2C18B3, 0x106453C4, 0x795C6AE9,
    0xE2DC8616, 0x8BE4BF3B, 0x30ACF44C, 0x5994CD61, 0x2F045B8F, 0x463C62A2, 0xFD7429D5, 0x944C10F8,
    0x10550409, 0x796D3D24, 0xC2257653, 0xAB1D4F7E, 0xDD8DD990, 0xB4B5E0BD, 0x0FFDABCA, 0x66C592E7,
    0x6EF7BB05, 0x07CF8228, 0xBC87C95F, 0xD5BFF072, 0xA32F669C, 0xCA175FB1, 0x715F14C6, 0x18672DEB,
    0x9C7E391A, 0xF5460037, 0x4E0E4B40, 0x2736726D, 0x51A6E483, 0x389EDDAE, 0x83D696D9, 0xEAEEAFF4,
    0x93B2C51D, 0xFA8AFC30, 0x41C2B747, 0x28FA8E6A, 0x5E6A1884, 0x375221A9, 0x8C1A6ADE, 0xE52253F3,
    0x613B4702, 0x08037E2F, 0xB34B3558, 0xDA730C75, 0xACE39A9B, 0xC5DBA3B6, 0x7E93E8C1, 0x17ABD1EC,
    0x1F99F80E, 0x76A1C123, 0xCDE98A54, 0xA4D1B379, 0xD2412597, 0xBB791CBA, 0x003157CD, 0x69096EE0,
    0xED107A11, 0x8428433C, 0x3F60084B, 0x56583166, 0x20C8A788, 0x49F09EA5, 0xF2B8D5D2, 0x9B80ECFF,
    0xAC813501, 0xC5B90C2C, 0x7EF1475B, 0x17C97E76, 0x6159E898, 0x0861D1B5, 0xB3299AC2, 0xDA11A3EF,
    0x5E08B71E, 0x37308E33, 0x8C78C544, 0xE540FC69, 0x93D06A87, 0xFAE853AA, 0x41A018DD, 0x289821F0,
    0x20AA0812, 0x4992313F, 0xF2DA7A48, 0x9BE24365, 0xED72D58B, 0x844AECA6, 0x3F02A7D1, 0x563A9EFC,
    0xD2238A0D, 0xBB1BB320, 0x0053F857, 0x696BC17A, 0x1FFB5794, 0x76C36EB9, 0xCD8B25CE, 0xA4B31CE3,
    0xDDEF760A, 0xB4D74F27, 0x0F9F0450, 0x66A73D7D, 0x1037AB93, 0x790F92BE, 0xC247D9C9, 0xAB7FE0E4,
    0x2F66F415, 0x465ECD38, 0xFD16864F, 0x942EBF62, 0xE2BE298C, 0x8B8610A1, 0x30CE5BD6, 0x59F662FB,
    0x51C44B19, 0x38FC7234, 0x83B43943, 0xEA8C006E, 0x9C1C9680, 0xF524AFAD, 0x4E6CE4DA, 0x2754DDF7,
    0xA34DC906, 0xCA75F02B, 0x713DBB5C, 0x18058271, 0x6E95149F, 0x07AD2DB2, 0xBCE566C5, 0xD5DD5FE8,
    0x4E5DB317, 0x27658A3A, 0x9C2DC14D, 0xF515F860, 0x83856E8E, 0xEABD57A3, 0x51F51CD4, 0x38CD25F9,
    0xBCD43108, 0xD5EC0825, 0x6EA44352, 0x079C7A7F, 0x710CEC91, 0x1834D5BC, 0xA37C9ECB, 0xCA44A7E6,
    0xC2768E04, 0xAB4EB729, 0x1006FC5E, 0x793EC573, 0x0FAE539D, 0x66966AB0, 0xDDDE21C7, 0xB4E618EA,
    0x30FF0C1B, 0x59C73536, 0xE28F7E41, 0x8BB7476C, 0xFD27D182, 0x941FE8AF, 0x2F57A3D8, 0x466F9AF5,
    0x3F33F01C, 0x560BC931, 0xED438246, 0x847BBB6B, 0xF2EB2D85, 0x9BD314A8, 0x209B5FDF, 0x49A366F2,
    0xCDBA7203, 0xA4824B2E, 0x1FCA0059, 0x76F23974, 0x0062AF9A, 0x695A96B7, 0xD212DDC0, 0xBB2AE4ED,
    0xB318CD0F, 0xDA20F422, 0x6168BF55, 0x08508678, 0x7EC01096, 0x17F829BB, 0xACB062CC, 0xC5885BE1,
    0x41914F10, 0x28A9763D, 0x93E13D4A, 0xFAD90467, 0x8C499289, 0xE571ABA4, 0x5E39E0D3, 0x3701D9FE};
#endif // FEATURE_SDM_CRC32

static uint32_t SharkJ1939::calculateSDMDataCRC(const uint8_t dlc, const uint8_t *data)
{
#ifdef FEATURE_SDM_CRC32
  // The SDM Data CRC (SPN 9383) is the 32-bit CRC of the data field of the corresponding SDM.
  // The CRC polynomial used: 0x6938392D
  // Initial value: 0xFFFFFFFF, Final XOR value: 0x00000000
  // Inputs are not reflected. Results are not reflected.
  // Example CRC calculation:
  // - Input data: 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07
  //   CRC result: 0xC550537D
  // - Input data: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
  //   CRC result: 0xBB1584F9

  uint32_t crc32 = 0xffffffff;

  for (int i = 0; i < dlc; i++)
  {
    uint8_t pos = ((crc32 >> 24) ^ data[i]) & 0xFF;
    crc32 = (crc32 << 8) ^ sdm_crc32_lookup_table[pos];
  }

  return crc32;
#else
  return 0;
#endif
}
