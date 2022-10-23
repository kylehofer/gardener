/*
 * File: Fields.h
 * Project: gardener
 * Created Date: Friday October 21st 2022
 * Author: Kyle Hofer
 * 
 * MIT License
 * 
 * Copyright (c) 2022 Kyle Hofer
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * HISTORY:
 */

#ifndef FIELDS
#define FIELDS

#include <cstdint>
#include <string>

/**
 * @brief Base class for handling all Victron Fields.
 * Provides an Id/Data field combo.
 * 
 */
class Field
{
private:
    uint32_t id;
public:
    /**
     * @brief Construct a new Field object
     * 
     * @param id Unique identifier for the field
     */
    Field(uint32_t id): id(id) {};
    virtual ~Field() {};
    /**
     * @brief Get the Id object
     * 
     * @return int 
     */
    uint32_t getId() { return id; };
    /**
     * @brief Get the Data object
     * 
     * @param data Pointer to memory address to load the data into
     */
    virtual void getData(void* data) = 0;
};

class StringField : Field
{
private:
    std::string data;
public:
    /**
     * @brief Construct a new String Field object
     * 
     * @param id Unique identifier for the field
     * @param data String data for the field
     */
    StringField(uint32_t id, std::string data) : Field(id), data(data) {};
    ~StringField() {};
    void getData(void* data) { *((std::string*) data) = this->data; };
};

class DoubleField : Field
{
private:
    double data;
public:
    /**
     * @brief Construct a new Double Field object
     * 
     * @param id Unique identifier for the field
     * @param data Double data for the field
     */
    DoubleField(uint32_t id, double data) : Field(id), data(data) {};
    ~DoubleField() {};
    void getData(void* data) { *((double*) data) = this->data; };
};

class FloatField : Field
{
private:
    float data;
public:
    /**
     * @brief Construct a new Float Field object
     * 
     * @param id Unique identifier for the field
     * @param data Float data for the field
     */
    FloatField(uint32_t id, float data) : Field(id), data(data) {};
    ~FloatField() {};
    void getData(void* data) { *((float*) data) = this->data; };
};

class UInt32Field : Field
{
private:
    uint32_t data;
public:
    /**
     * @brief Construct a new UInt32 Field object
     * 
     * @param id Unique identifier for the field
     * @param data UInt32 data for the field
     */
    UInt32Field(uint32_t id, uint32_t data) : Field(id), data(data) {};
    ~UInt32Field() {};
    void getData(void* data) { *((uint32_t*) data) = this->data; };
};

class Int32Field : Field
{
private:
    int32_t data;
public:
    /**
     * @brief Construct a new Int32 Field object
     * 
     * @param id Unique identifier for the field
     * @param data Int32 data for the field
     */
    Int32Field(uint32_t id, int32_t data) : Field(id), data(data) {};
    ~Int32Field() {};
    void getData(void* data) { *((int32_t*) data) = this->data; };
};

class UInt16Field : Field
{
private:
    uint16_t data;
public:
    /**
     * @brief Construct a new UInt16 Field object
     * 
     * @param id Unique identifier for the field
     * @param data UInt16 data for the field
     */
    UInt16Field(uint32_t id, uint16_t data) : Field(id), data(data) {};
    ~UInt16Field() {};
    void getData(void* data) { *((uint16_t*) data) = this->data; };
};

class Int16Field : Field
{
private:
    int16_t data;
public:
    /**
     * @brief Construct a new Int16 Field object
     * 
     * @param id Unique identifier for the field
     * @param data Int16 data for the field
     */
    Int16Field(uint32_t id, int16_t data) : Field(id), data(data) {};
    ~Int16Field() {};
    void getData(void* data) { *((int16_t*) data) = this->data; };
};

class UInt8Field : Field
{
private:
    uint8_t data;
public:
    /**
     * @brief Construct a new UInt8 Field object
     * 
     * @param id Unique identifier for the field
     * @param data UInt8 data for the field
     */
    UInt8Field(uint32_t id, uint8_t data) : Field(id), data(data) {};
    ~UInt8Field() {};
    void getData(void* data) { *((uint8_t*) data) = this->data; };
};

class Int8Field : Field
{
private:
    int8_t data;
public:
    /**
     * @brief Construct a new Int8 Field object
     * 
     * @param id Unique identifier for the field
     * @param data Int8 data for the field
     */
    Int8Field(uint32_t id, int8_t data) : Field(id), data(data) {};
    ~Int8Field() {};
    void getData(void* data) { *((int8_t*) data) = this->data; };
};

#endif /* FIELDS */
