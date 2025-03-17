# SharkJ1939

SharkJ1939 is a library designed to monitor J1939 CAN messages on an Arduino board equipped with the MCP2515. The library offers a built-in feature for calculating SDM Data CRC (SPN 9383), which can be used to verify the integrity of the Safety Header Message. SharkJ1939 is a small program, and as you may have already noticed, its name is inspired by Wireshark.

## Requirements

To use SharkJ1939, you need to install the following Arduino library:

arduino-mcp2515 (https://github.com/autowp/arduino-mcp2515)

## How to use

* Example code on how to use the library can be found in the examples folder of the library's repository. You can create your own PGN (Parameter Group Number) table as shown below:
```c
static PGN_KNOWN_ENTRY pgnKnownTable[] = {
    {256, "TC1", tc1_callback}, // PGN 256(=0x0100) Transmission Control 1
    {3584, "SHM", NULL},        // PGN 3584(=0xE00) Safety Header Message
    {48896, "PCM15", NULL},
    {60928, "AC", NULL},
    {61443, "EEC2", NULL},
    {65265, "CCVS1", ccvs1_callback}, // PGN 65265(=0xFEF1) Cruise Control/Vehicle Speed 1
    {126720, "PropA2", NULL},
    {0, NULL, NULL} // The last entry to indicates the end of the table.
};
```

* Here are some example J1939 messages displayed in the serial console, along with additional information about destination address, PGNs, relevant data values and SDM CRC (if applicable) in the messages. PGN TC1 indicates that the CRC value 9794D9F2 matches 'F2 D9 94 97' in the SHM message. Please note that the below J1939 messages are just for illustration purposes and do not represent actual data.

```text
18EEFF05 8 10 00 40 00 00 05 02 10 1000055821 FF AC
08F18F05 8 01 05 03 00 01 00 FD FF 1000073740 -- 61839
0C0E0305 8 0F FA FC FE F2 D9 94 97 1000073786 03 SHM
0C010305 8 FF FF 7D FF FF 3F 3F FF 1000073788 03 TC1 N 9794D9F2
18FEF100 8 3F 16 92 CF FF FF FF FF 1000215723 -- CCVS1 90.77 MPH
       DLC 1  2  3  4  5  6  7  8  msec      DST PGN
```

* To switch between pausing and resuming the display output, simply press the Enter key.

* Enabling SDM Data CRC (SPN 9383) requires an additional 1102 bytes for code and 1024 bytes for data. If there is insufficient memory and you don't need SHM and SDM, then undefine FEATURE_SDM_CRC32.



