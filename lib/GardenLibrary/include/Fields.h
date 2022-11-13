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

#ifdef __AVR__
#include  <stdint.h>
#include <Arduino.h>
#else
#include <cstdint>
#include <string>
#include <cstring>
using namespace std;
#endif // __AVR__

/**
 * @brief Base class for handling all Victron Fields.
 * Provides an Id/Data field combo.
 * 
 */
class Field
{
private:
    uint32_t id;
    size_t size;
    void* data;
public:
    /**
     * @brief Construct a new Field object
     * 
     * @param id Unique identifier for the field
     */
    template <typename T> Field(uint32_t id, T data): id(id), size(sizeof(T))
    {
        this->data = malloc(sizeof(T));
        *((T*) this->data) = data;
    };

        /**
     * @brief Construct a new Field object
     * 
     * @param id Unique identifier for the field
     */
    template <typename T> Field(uint32_t id, T* data, size_t size): id(id)
    {
        this->data = malloc(size);
        this->size = size;
        memcpy(this->data, data, size);
    };

    ~Field() 
    {
        free(data);
    };
    
    /**
     * @brief Get the Id object
     * 
     * @return int 
     */
    uint32_t getId() { return id; };
    void* getData() { return data; };
    size_t getSize() { return size; };
};

#endif /* FIELDS */
