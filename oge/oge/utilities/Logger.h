#ifndef __LOGGER_H__
#define __LOGGER_H__

/*
 *  OGE Open Game Engine
 *  Copyright (c) 2023 Steven Gay (lazalong@gmail.com)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "../Oge.h"
#include <stdbool.h>

// __FILE__ and __LINE__ preprocessor directives not supported
// by old compiler... the next lines should be unnecessary.
#undef LINE_FILE
#ifdef __LINE__
#   ifdef __FILE__
#       define LINE_FILE
#   endif
#endif

typedef enum OgeLogLevel OgeLogLevel;

enum OgeLogLevel
{
    OGE_LOG_ERROR = 1,
    OGE_LOG_RELEASE,
    OGE_LOG_NORMAL,
    OGE_LOG_DEBUG,
    OGE_LOG_INFO,
    OGE_LOG_VERBOSE,

    // Special case
    OGE_LOG_ALLOC,
};

enum OgeLogType
{
    OGE_LOGTYPE_TEXT = 1,
    OGE_LOGTYPE_HTML,
    OGE_LOGTYPE_JSON,
};

typedef enum OgeLogType OgeLogType;

typedef struct OgeLogger OgeLogger;

/**
 Log manager class

  There is two type of logging :
    1 - LOG(e) which log an expression (must be a string).
    2 - LOG(t, e) which log an expression if the test (t) is true.

  For example,

    LOG( mass < 0, "The mass of " + object.getName() + " is less than zero !" );
    LOGR( "Object manager created." );
maxLogCount;
  The next table show the different logging level available:

   level  | Error | Release | Normal | Debug | Info   | Verbose |
          | LOGE  | LOGR    | LOG    | LOGD  | LOGI   | LOGV    |
    Color | Red   | Blue    | Green  | Red   | Orange | Black   |  #define
  ============================================================================
  NONE    |   -   |   -     |   -    |   -   |  -     |   -     |   ---
  RELEASE |   +   |   +     |   -    |   -   |  -     |   -     | LOG_RELEASE
  NORMAL  |   +   |   +     |   +    |   -   |  -     |   -     | LOG_NORMAL
  DEBUG   |   +   |   +     |   +    |   +   |  -     |   -     | LOG_DEBUG
  INFO    |   +   |   +     |   +    |   +   |  +     |   -     | LOG_INFO
  VERBOSE |   +   |   +     |   +    |   +   |  +     |   +     | LOG_VERBOSE

   The DEBUG level should be understood as SPECIAL_DEBUG or FULL_DEBUG.
   When a code is fairly stable we can change a LOG to LOGD,
   and by doing this, decrease the quantity of debug messages.
   We don't need to log all classes, all the time.

   @author: GAY Steven - lazalong at gmail.com
   @version 1.0
*/
struct OgeLogger
{
    unsigned long maxLogCount;
    unsigned long logCount;
    unsigned long updateCount;
    unsigned int  previousStackLevel;
    bool showOnConsole;
    OgeLogType logType;
    FILE* logFile;

    void (*Log)(int level, const char* text, const char* file, int line);
    void (*LogAlloc)(int allocator, const char* action, long address, long size, const char* file, int line);
    void (*LogHeader)(void);
    void (*LogFooter)(void);
};

static OgeLogger* _ogeLogger = NULL;
static size_t OgeLogManagerSize = sizeof(OgeLogger);

extern void* OgeCreateLogger(const char* logname, OgeLogType type, long maxLogCount, bool showOnConsole);
extern void  OgeLogMessageTest(bool test, int level, const char* text, const char* file, int line);
extern void  OgeLogUpdate(float deltaTime, int frame);
extern void  OgeLogOpenFile(const char* filename);
extern void  OgeLogCloseFile();

void OgeLogText(int level, const char* text, const char* file, int line);
void OgeLogAllocText(int allocator, const char* action, long address, long size, const char* file, int line);
void OgeLogHeaderText();
void OgeLogFooterText();

//void OgeLogHeader();
//void OgeLogFooter();

void OgeLogHTML(int level, const char* text, const char* file, int line);
void OgeLogAllocHTML(int allocator, const char* action, long address, long size, const char* file, int line);
void OgeLogHeaderHTML();
void OgeLogFooterHTML();

void OgeLogJSON(int level, const char* text, const char* file, int line);
void OgeLogAllocJSON(int allocator, const char* action, long address, long size, const char* file, int line);
void OgeLogHeaderJSON();
void OgeLogFooterJSON();

void OgeLogWriteIndent();

const char* OgeLogGetDate();
const char* OgeLogGetTime();
const char* OgeLogGetDebugLine();
const char* OgeLogGetFont(int level);
void OgeLogCallStack();

extern void OgeLogMessage(int level, const char* text, const char* file, int line);
// Action values should be add/rem/clr/del/err to be used with my HeapLogViewer
extern void OgeLogAlloc(int allocator, const char* action, long address, long size, const char* file, int line);

#ifdef LINE_FILE
#   define LOGE(e)     OgeLogMessage(OGE_LOG_ERROR, e, __FILE__, __LINE__);
#   define LOGR(e)     OgeLogMessage(OGE_LOG_RELEASE, e, __FILE__, __LINE__);
#   define LOG(e)      OgeLogMessage(OGE_LOG_NORMAL, e, __FILE__, __LINE__);
#   define LOGD(e)     OgeLogMessage(OGE_LOG_DEBUG, e, __FILE__, __LINE__);
#   define LOGI(e)     OgeLogMessage(OGE_LOG_INFO, e, __FILE__, __LINE__);
#   define LOGV(e)     OgeLogMessage(OGE_LOG_VERBOSE, e, __FILE__, __LINE__);
#   define LOGEC(t, e) OgeLogMessageTest(OGE_LOG_ERROR, t, e, __FILE__, __LINE__);
#   define LOGRC(t, e) OgeLogMessageTest(OGE_LOG_RELEASE, t, e, __FILE__, __LINE__);
#   define LOGC(t, e)  OgeLogMessageTest(OGE_LOG_NORMAL, t, e, __FILE__, __LINE__);
#   define LOGDC(t, e) OgeLogMessageTest(OGE_LOG_DEBUG, t, e, __FILE__, __LINE__);
#   define LOGIC(t, e) OgeLogMessageTest(OGE_LOG_INFO, t, e, __FILE__, __LINE__);
#   define LOGVC(t, e) OgeLogMessageTest(OGE_LOG_VERBOSE, t, e, __FILE__, __LINE__);
#   define LOGU(t, f)  OgeLogUpdate(t, f); // Update
#   define LOGA(allocator, action, address, size, file, line)  OgeLogAlloc(allocator, action, (long)address, (long)size, file, line);
#else
#   define LOGE(e)     OgeLogMessage(OGE_LOG_ERROR, e, "", 0);
#   define LOGR(e)     OgeLogMessage(OGE_LOG_RELEASE, e, "", 0);
#   define LOG(e)      OgeLogMessage(OGE_LOG_NORMAL, e, "", 0);
#   define LOGD(e)     OgeLogMessage(OGE_LOG_DEBUG, e, "", 0);
#   define LOGI(e)     OgeLogMessage(OGE_LOG_INFO, e, "", 0);
#   define LOGV(e)     OgeLogMessage(OGE_LOG_VERBOSE, e, "", 0);
#   define LOGEC(t, e) OgeLogMessageTest(OGE_LOG_ERROR, t, e, "", 0);
#   define LOGRC(t, e) OgeLogMessageTest(OGE_LOG_RELEASE, t, e, "", 0);
#   define LOGC(t, e)  OgeLogMessageTest(OGE_LOG_NORMAL, t, e, "", 0);
#   define LOGDC(t, e) OgeLogMessageTest(OGE_LOG_DEBUG, t, e, "", 0);
#   define LOGIC(t, e) OgeLogMessageTest(OGE_LOG_INFO, t, e, "", 0);
#   define LOGVC(t, e) OgeLogMessageTest(OGE_LOG_VERBOSE, t, e, "", 0);
#   define LOGU(t, f)  OgeLogUpdate(t, f); // Update
#   define LOGA(allocator, action, address, size, file, line)  OgeLogAlloc(allocator, action, (long)address, (long)size, file, line);
#endif

//#define FN(e) LogManager::EventLogFN obj____unique_name(e)
//#define LOG_UPDATE(e) LogManager::getSingleton().update(e);

// Put a #define in Oge.h
#ifndef LOG_VERBOSE
#   undef LOGV
#   undef LOGVC
#   define LOGV(e)
#   define LOGVC(t, e)

#   ifndef LOG_INFO
#       undef LOGI
#       undef LOGIC
#       define LOGI(e)
#       define LOGIC(t, e)

#       ifndef LOG_DEBUG
#       undef LOGD
#       undef LOGDC
#       define LOGD(e)
#       define LOGDC(t, e)

#       ifndef LOG_NORMAL
#           undef LOG
#           undef LOGC
#           define LOG(e)
#           define LOGC(t, e)

#           undef FN
#           define FN(e)

#           undef LOGUP
#           define LOGU(t, f) // Update

#           ifndef LOG_RELEASE
                // Then we are in LOG_NONE !
#               undef LOGR
#               undef LOGRC
#               define LOGR(e)
#               define LOGRC(t, e)

#               undef LOGE
#               undef LOGEC
#               define LOGE(e)
#               define LOGEC(t, e)

#               endif
#           endif
#       endif
#   endif
#endif

#endif // !__LOGGER_H__
