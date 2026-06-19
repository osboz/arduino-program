#include "Labview.h"

void PrintPackageToDisplay(Packet pkt_)
{
    char buffer[64];
    putstrUART0("Packet received:\n");
    SendStrActualXY("Packet received", 0, 0);

    // Print packet length
    sprintf(buffer, "PkLen: %d\n", pkt_.packetLength);
    putstrUART0(buffer);
    SendStrActualXY(buffer, 0, 1);

    // Print type
    sprintf(buffer, "Type: %d\n", pkt_.type);
    putstrUART0(buffer);
    SendStrActualXY(buffer, 0, 2);

    // Print data as hex (not as raw binary)
    putstrUART0("Data (hex): ");
    for (int k = 0; k < pkt_.dataLength; k++)
    {
        sprintf(buffer, "%02X ", pkt_.data[k]);
        putstrUART0(buffer);
        SendStrActualXY(buffer, 2 * k + 1, 3);
    }
    putchUART0('\n');

    // Print CRC
    sprintf(buffer, "CRC: %04X\n", pkt_.crc);
    putstrUART0(buffer);
    SendStrActualXY(buffer, 0, 4);
}

void SendDataToLabViewLRC8(uint16_t dataLength, uint8_t *data, uint8_t type)
{
    uint8_t msb = (uint8_t)((dataLength + 7) >> 8);
    uint8_t lsb = (uint8_t)((dataLength + 7) & 0xFF);

    uint8_t checksum = 0x55 ^ 0xAA ^ msb ^ lsb ^ (uint8_t)type;

    putchUART1(0x55);
    putchUART1(0xAA);
    putchUART1(msb); // Length H (big-endian)
    putchUART1(lsb); // Length L
    putchUART1(type);
    for (size_t i = 0; i < dataLength; i++)
    {
        putchUART1(data[i]);
        checksum ^= data[i];
    }

    putchUART1(0x00);     // High byte of 2-byte checksum (always 0x00 for single-byte result)
    putchUART1(checksum); // Low byte checksum
}

void ProcessLabViewCommand(Packet *pkt)
{
    switch (pkt->type)
    {
    case 0x01: // BTN pressed
        ProcessButtonCommand(pkt);
        break;

    case 0x02: // SEND pressed (sample rate + record length)
        ProcessSendCommand(pkt);
        break;

    case 0x03: // START pressed (Bode plot)
        ProcessStartCommand(pkt);
        break;

    default:
        putstrUART0("Unknown command type\n");
        break;
    }
}

void ProcessButtonCommand(Packet *pkt)
{
    uint8_t btnValue = pkt->data[0]; // 0x00=BTN0, 0x01=BTN1, 0x02=BTN2, 0x03=BTN3 (from LabVIEW)
    uint8_t swValue = pkt->data[1];  // Software value (parameter)

    switch (btnValue)
    {
        // assign switch value
    case 0:
        generatorSettings[generatorSettings[0] + 1] = swValue;
        break;

        // increment button value
    case 1:
        generatorSettings[0] = (generatorSettings[0] + 1) % 3;
        break;

        // toggle run/stop
    case 2:
        generatorSettings[4] = (generatorSettings[4] + 1) % 2;
        break;

        // reset all
    case 3:
        generatorSettings[0] = 0;
        generatorSettings[1] = 0;
        generatorSettings[2] = 0;
        generatorSettings[3] = 0;
        generatorSettings[4] = 0;
        break;

    default:
        putstrUART0("default uhoh");
        break;
    }

    SendDataToLabViewLRC8(4, generatorSettings, 1);
    SendDataToFPGA(generatorSettings);
    SendDataToFPGA(generatorSettings);
}

void ProcessSendCommand(Packet *pkt)
{
    uint16_t sampleRate = ((uint16_t)pkt->data[0] << 8) | (uint16_t)pkt->data[1];
    uint16_t recordLength = ((uint16_t)pkt->data[2] << 8) | (uint16_t)pkt->data[3];

    // Update ADC configuration
    SetFreqTimer1(sampleRate);

    // Send new sample rate and record length to LabVIEW via UART
    uint8_t data[] = {sampleRate >> 8, sampleRate & 0xFF, recordLength >> 8, recordLength & 0xFF};
    SendDataToLabViewLRC8(4, data, 2);

    // Store new sample rate and record length in settings
    oscilloscopeSettings[0] = sampleRate;
    oscilloscopeSettings[1] = recordLength;

    // Send oscilloscope data packet back to LabVIEW (instead of using SendDataToLabViewLRC8)
    // uint8_t samples[] = //spi
}

void ProcessStartCommand(Packet *pkt)
{
    putstrUART0("Bode plot START received\n");
    // Start frequency sweep or trigger action

    SendDataToLabViewLRC8(255, pkt->data, 3);
}

uint8_t CalculateXOR8(uint8_t *data, size_t length)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < length; i++)
    {
        checksum ^= data[i];
    }
    return checksum;
}

void SendDataToFPGA(uint8_t *data)
{
    uint8_t checksum = 0x55;
    SPI_MasterTransfer(0x55);
    for (size_t i = 1; i < 6; i++)
    {
        SPI_MasterTransfer(data[i]);
        checksum ^= data[i];
    }
    SPI_MasterTransfer(checksum);
}