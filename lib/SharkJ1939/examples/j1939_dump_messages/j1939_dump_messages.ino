#include <SPI.h>
#include "mcp2515.h"
#include "SharkJ1939.h"

// This example demonstrates how to set up a table of known Parameter Group Numbers (PGNs) and
// initialize a SharkJ1939 object with an MCP2515 CAN controller for working with J1939 messages

struct can_frame canMsg;
MCP2515 mcp2515(10);

static void tc1_callback(uint8_t dlc, uint8_t *data)
{
  Serial.print(' ');

  // The position of SPN 525 in PG is 3. It is at index 2 of the data array.
  uint8_t spn525 = data[2];

  if (spn525 == 0x7D)
    Serial.print('N'); // Print 'N' for Neutral if SPN 525 is equal to 0x7D
  else if (spn525 == 0xDF)
    Serial.print('R'); // Print 'R' for Reverse if SPN 525 is equal to 0xDF
  else
    Serial.print(spn525, HEX);

  Serial.print(' ');

  // Comment out this line if there is no Safety Header Message.
  Serial.print(SharkJ1939::calculateSDMDataCRC(dlc, data), HEX);
}

static void ccvs1_callback(uint8_t dlc, uint8_t *data)
{
  Serial.print(' ');

  uint16_t spn84;  // Wheel-Based Vehicle Speed
  spn84 = data[2]; // MSB
  spn84 <<= 8;
  spn84 |= data[1]; // LSB

  float mph = spn84 * 0.00390625 / 1.609344; // bits --> km/h --> mph
  Serial.print(mph);
  Serial.print(" MPH");
}

static PGN_KNOWN_ENTRY pgnKnownTable[] = {
    {256, "TC1", tc1_callback}, // PGN 256(=0x0100) Transmission Control 1 with 'tc1_callback()'.
    {3584, "SHM", NULL},        // PGN 3584(=0xE00) Safety Header Message with no callback function
    {48896, "PCM15", NULL},
    {60928, "AC", NULL},
    {61443, "EEC2", NULL},
    {65265, "CCVS1", ccvs1_callback}, // PGN 65265(=0xFEF1) Cruise Control/Vehicle Speed 1
    {126720, "PropA2", NULL},
    {0, NULL, NULL} // The last entry to indicates the end of the table.
};

SharkJ1939 sharkj1939(&mcp2515); // SharkJ1939 object initialized with MCP2515 CAN controller

void setup()
{
  Serial.begin(115200);

  sharkj1939.bindPGNKnownTable(pgnKnownTable); // Binds the known PGN table to the SharkJ1939 object

  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

void loop()
{
  if (Serial.available())
  {
    // Process user input for SharkJ1939.
    // Currently, it only supports pausing and resuming operations.
    sharkj1939.processUserInput();
  }

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)
  {
    sharkj1939.dumpMessageInJ1939Format(canMsg); // Dump received CAN messages with J1939 format to Serial
  }
}
