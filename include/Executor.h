/*
 * File: Executor.h
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

#ifndef EXECUTOR
#define EXECUTOR

#include <ctime>
#include <unistd.h>

#define MAX_TIMEOUT CLOCKS_PER_SEC

class Executor
{
private:
    clock_t executeDelay;
protected:
    /**
     * @brief Internal execute task to be implemented. The delay returned from this function will be the delay before the next execute.
     * 
     * @return clock_t 
     */
    virtual clock_t doExecute() = 0;
public:
    Executor() : executeDelay(0) {};
    /**
     * @brief execute the main task if enough time has elapsed.
     * The time waited is the returned delay from doExecute
     * 
     */
    void execute()
    {
        clock_t timestamp;

        timestamp = clock();
        if (this->executeDelay < timestamp || (this->executeDelay - timestamp) > MAX_TIMEOUT)
        {
            return;
        }
        this->executeDelay = (this->doExecute() + clock());
    };
};

#endif /* EXECUTOR */
