#pragma once
#ifdef MCU_STM32F103CB
#include "EEPROMM.h"
#define EEPObject EEPROMM
#else
#include <EEPROM.h>
#define EEPObject EEPROM
#endif
#include "Types.h"
#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#define EEPROMCommit EEPObject.commit();
#else
#define EEPROMCommit ;
#endif

class EEPROM2_Class
{
public:
    void PutUint8(uint16& address, uint8 data)
    {
        if (address >= 4000)
            return;
        EEPObject.write(address, data);
        address++;
    }
    void PutUint8Array(uint16& address, uint8* array, uint8 count)
    {
        // count + first entry
        uint8 e0 = 0;
        if (count > 0)
            e0 = array[0];
        Put2Uint8(address, count, e0);
        int saved = 1;
        while (saved < count)
        {
            uint8 ei = array[saved], ei_1 = 0;
            if (count - saved > 1) // 2 or more 
                ei_1 = array[saved + 1];
            Put2Uint8(address, ei, ei_1);
            saved += 2;
        }
    }
    uint8 GetUint8ArrayCount(uint16 address)
    {
        // count + first entry
        uint8 e0 = 0;
        uint8 count = 0;
        Get2Uint8(address, count, e0);
        return count;
    }
    void GetUint8Array(uint16& address, uint8* array)
    {
        // count + first entry
        uint8 e0 = 0;
        uint8 count = 0;
        Get2Uint8(address, count, e0);

        if (count == 0)
            return;

        array[0] = e0;
        int resumed = 1;
        while (resumed < count)
        {
            uint8 ei = 0, ei_1 = 0;
            Get2Uint8(address, ei, ei_1);
            array[resumed] = ei;
            if (count - resumed > 1) // 2 or more 
                array[resumed + 1] = ei_1;
            resumed += 2;
        }
    }
    uint8& GetUint8(uint16& address, uint8& outValue)
    {
        if (address >= 4000)
            return outValue;
        outValue = EEPObject.read(address);
        address++;
        return outValue;
    }
#ifdef ARDUINO_ARCH_STM32F1
    void Put2Uint8(uint16& address, uint8 data1, uint8 data2)
    {
   /*     Serial.print("PA: ");
        Serial.print(address);
        Serial.print(", v1: ");
        Serial.print(data1);
        Serial.print(", v2: ");
        Serial.println(data2);*/
        if (address >= 4000)
            return;
        uint16 data16 = (uint16)data1 + ((uint16)data2 << 8);
        EEPObject.write(address, data16);
        address++;
    }
    void Get2Uint8(uint16& address, uint8& outValue1, uint8& outValue2)
    {
        /*Serial.print("GA: ");
        Serial.print(address);
        Serial.print(", v1: ");
        Serial.print(outValue1);
        Serial.print(", v2: ");
        Serial.println(outValue2);*/
        if (address >= 4000)
            return;
        uint16 data16 = EEPObject.read(address);
        outValue1 = data16;
        outValue2 = data16 >> 8;
        address++;
}
#else 
    void Put2Uint8(uint16& address, uint8 data1, uint8 data2)
    {
        PutUint8(address, data1);
        PutUint8(address, data2);
    }
    void Get2Uint8(uint16& address, uint8& outValue1, uint8& outValue2)
    {
        GetUint8(address, outValue1);
        GetUint8(address, outValue2);
    }
#endif
    void PutInt8(uint16& address, int8 data)
    {
        PutUint8(address, data);
    }
    int8& GetInt8(uint16& address, int8& outValue)
    {
        uint8 outValue2 = 0;
        GetUint8(address, outValue2);
        outValue = outValue2;
        return outValue;
    }
#ifdef ARDUINO_ARCH_STM32F1
    void PutUint16(uint16& address, uint16 data)
    {
        if (address >= 4000)
            return;
        EEPObject.write(address, data);
        address++;
    }
    uint16& GetUint16(uint16& address, uint16& outValue)
    {
        if (address >= 4000)
            return outValue;
        outValue = EEPObject.read(address);
        address++;
        return outValue;
    }
#else
    void PutUint16(uint16& address, uint16 data)
    {
        EEPObject.write(address, (uint8)data);
        EEPObject.write(address + 1, data >> 8);
        address += 2;
    }
    uint16& GetUint16(uint16& address, uint16& outValue)
    {
        outValue = EEPObject.read(address);
        outValue += EEPObject.read(address + 1) << 8;
        address += 2;
        return outValue;
    }
#endif
    void PutInt16(uint16& address, int16 data)
    {
        PutUint16(address, data);
    }
    int16 GetInt16(uint16& address, int16& outValue)
    {
        uint16 outValue2 = 0;
        GetUint16(address, outValue2);
        outValue = outValue2;
        return outValue;
    }
#ifdef ARDUINO_ARCH_STM32F1
    void PutUint32(uint16& address, uint32 data)
    {
        if (address >= 4000 - 1)
            return;
        EEPObject.write(address, data);
        EEPObject.write(address + 1, data >> 16);
        address += 2;
    }
    uint32& GetUint32(uint16& address, uint32& outValue)
    {
        if (address >= 4000 - 1)
            return outValue;
        uint32 b0 = EEPObject.read(address);
        uint32 b1 = EEPObject.read(address + 1);
        address += 2;
        outValue = b0 + (b1 << 16);
        return outValue;
    }
#else
    void PutUint32(uint16& address, uint32 data)
    {
        EEPObject.write(address + 0, (uint8)(data >> 0));
        EEPObject.write(address + 1, (uint8)(data >> 8));
        EEPObject.write(address + 2, (uint8)(data >> 16));
        EEPObject.write(address + 3, (uint8)(data >> 24));
        address += 4;
    }
    uint32& GetUint32(uint16& address, uint32& outValue)
    {
        uint32 b0 = EEPObject.read(address + 0);
        uint32 b1 = EEPObject.read(address + 1);
        uint32 b2 = EEPObject.read(address + 2);
        uint32 b3 = EEPObject.read(address + 3);
        address += 4;
        outValue = b0 + (b1 << 8) + (b2 << 16) + (b3 << 24);
        return outValue;
    }
#endif
    void PutInt32(uint16& address, int32 data)
    {
        PutUint32(address, data);
    }
    int32& GetInt32(uint16& address, int32& outValue)
    {
        uint32 data32 = 0;
        GetUint32(address, data32);
        outValue = data32;
        return outValue;
    }
    void PutFloat(uint16& address, float data)
    {
        uint32* dataPtr32 = reinterpret_cast<uint32*>(&data);
        PutUint32(address, *dataPtr32);
    }
    float& GetFloat(uint16& address, float& outValue)
    {
        uint32 data32 = 0;
        GetUint32(address, data32);
        outValue = *(reinterpret_cast<float*>(&data32));
        return outValue;
    }
#ifdef ARDUINO_ARCH_STM32F1
    void PutString(uint16& address, String& string)
    {
        PutUint8(address, string.length());
        int stored = 0;
        while(stored < string.length())
        {
            uint8 char1 = string[stored], char2 = 0;
            stored++;
            if (stored < string.length())
            {
                char2 = string[stored];
                stored++;
            }

            uint16 u16 = char1 + (char2 << 8);
            PutUint16(address, u16);
        }
    }
    String& GetString(uint16& address, String& S)
    {
        uint8 length = 0;
        GetUint8(address, length);
        char* buffer = new char[length + 1];
        int retrieved = 0;
        while (retrieved < length)
        {
            uint16 data = 0;
            GetUint16(address, data);
            uint8 b0 = data;
            uint8 b1 = data >> 8; // this can be false
            buffer[retrieved] = b0; retrieved++;
            buffer[retrieved] = b1; retrieved++;
        }
        buffer[length] = 0;
        S = String(buffer);
        delete buffer;
        return S;
    }
#else

    void PutString(uint16& address, String& string)
    {
        PutUint8(address, string.length());
        for (int i = 0; i < string.length(); i++)
            PutUint8(address, string[i]);
    }
    String& GetString(uint16& address, String& S)
    {
        uint8 length = 0;
        GetUint8(address, length);
        char* buffer = new char[length + 1];
        for (int i = 0; i < length; i++)
        {
            uint8 b = 0;
            GetUint8(address, b);
            buffer[i] = b;
        }
        buffer[length] = 0;
        S = String(buffer);
        delete buffer;
        return S;
    }
#endif

    //template <typename T>
    //const T& put(int idx, const T& t)
    //{
    //    const uint8_t* ptr = (const uint8_t*)&t;
    //    for (int count = sizeof(T); count; --count)
    //        write(idx + sizeof(T) - count, (*ptr++));
    //    return t;
    //}

    //uint8_t read(int idx) 
    //{
    //    //if (idx < 0 || idx >= 1024)
    //    //    return 0;
    //    //uint32 address = EEPROM_PAGE0_BASE + (idx / 2);
    //    //uint16 data = (*(__IO uint16*)address);
    //    //Serial.print(F("read("));
    //    //Serial.print(address, 16);
    //    //Serial.print(F(") = "));
    //    //Serial.print(data);
    //    //Serial.print(F(", "));
    //    //if (idx % 2 == 0) // 0, 2, 4
    //    //    return data & 0xFF;
    //    //else
    //    //    return (data >> 8) & 0xFF;

    //    return EEPObject.read(idx);
    //}
    //void write(int idx, uint8_t val)
    //{
    //    //if (idx < 0 || idx >= 1024)
    //    //    return;
    //    //uint32 address = EEPROM_PAGE0_BASE + (idx / 2);
    //    //uint16 data = (*(__IO uint16*)address);
    //    //if (idx % 2 == 0) // 0, 2, 4
    //    //{
    //    //    data &= 0xFF00;
    //    //    data |= val;
    //    //}
    //    //else
    //    //{
    //    //    data &= 0x00FF;
    //    //    data |= (val << 8);
    //    //}
    //    //Serial.print(F("FLASH_ProgramHalfWord("));
    //    //Serial.print(address, 16);
    //    //Serial.print(F(", "));
    //    //Serial.print(data);
    //    //Serial.print(F(")"));
    //    //FLASH_ProgramHalfWord(address, data);
    //    //delay(1);
    //    EEPObject.write(idx, val);
    //}
};
extern EEPROM2_Class EEPROM2;
