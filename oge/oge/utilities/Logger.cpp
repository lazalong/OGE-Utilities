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

#include "Logger.h"
#include "Memory.h"
#include <string.h>
#include <time.h>

#ifdef _MSC_VER
#   pragma warning(push)
//#   pragma warning(disable:4668) // '__cplusplus' is not defined as preprocessor macro, replacing with '0' for '#if/#elif'
#   pragma warning(disable:4996) // C4996	'fopen': This function or variable may be unsafe.Consider using fopen_s instead.
#   pragma warning(disable:26812) // C26812	The enum type 'OgeLogType' is unscoped. Prefer 'enum class' over 'enum'
#endif

void* OgeCreateLogger(const char* filename, OgeLogType type, long maxLogCount, bool showOnConsole) {
    if (_ogeLogger != NULL) {
        printf("\nWARNING: Log Manager already created!\n");
        OgeLogMessage(OGE_LOG_NORMAL, "Log Manager already created!", __FILE__, __LINE__);
        return _ogeLogger;
    }

    OgeLogger* logger = (OgeLogger*)calloc(OgeLogManagerSize);
    _ogeLogger = logger;

    if (_ogeLogger == NULL) {
        printf("ERROR: Logger not created!\n");
        return NULL;
    }

    logger->logType = type;
    logger->logFile = NULL;
    logger->logCount = 0;
    logger->maxLogCount = maxLogCount;
    logger->showOnConsole = showOnConsole;
    logger->previousStackLevel = 0;

    // Set the methods
    switch ((OgeLogType)type) {
    case OGE_LOGTYPE_JSON:
        _ogeLogger->Log = &OgeLogJSON;
        _ogeLogger->LogAlloc = &OgeLogAllocJSON;
        _ogeLogger->LogHeader = &OgeLogHeaderJSON;
        _ogeLogger->LogFooter = &OgeLogFooterJSON;
        break;
    case OGE_LOGTYPE_HTML:
        _ogeLogger->Log = &OgeLogHTML;
        _ogeLogger->LogAlloc = &OgeLogAllocHTML;
        _ogeLogger->LogHeader = &OgeLogHeaderHTML;
        _ogeLogger->LogFooter = &OgeLogFooterHTML;
        break;
    case OGE_LOGTYPE_TEXT:
    default:
        _ogeLogger->Log = &OgeLogText;
        _ogeLogger->LogAlloc = &OgeLogAllocText;
        _ogeLogger->LogHeader = &OgeLogHeaderText;
        _ogeLogger->LogFooter = &OgeLogFooterText;
        break;
    }

    OgeLogOpenFile(filename);

    return logger;
}

inline
void OgeLogMessage(int level, const char* text, const char* file, int line) {
    if (_ogeLogger->logCount >= _ogeLogger->maxLogCount) {
        if (_ogeLogger->logCount == _ogeLogger->maxLogCount)
            fprintf(_ogeLogger->logFile, "ogeLogger: Logging stopped. Too many lines logged. <br>\n");
        return;
    }
    _ogeLogger->logCount++;
    _ogeLogger->Log(level, text, file, line);
}

inline
void OgeLogAlloc(int allocator, const char* action, long address, long size, const char* file, int line) {
    _ogeLogger->logCount++;
    _ogeLogger->LogAlloc(allocator, action, address, size, file, line);
}

void  OgeLogMessageTest(bool test, int level, const char* text, const char* file, int line) {
    if (test)
        OgeLogMessage(level, text, file, line);
}

// Log frame & time
// TODO break down in OgeLogUpdateText() etc
void  OgeLogUpdate(float deltaTime, int frame) {
    if (_ogeLogger == 0)
        return;

    _ogeLogger->updateCount++;

    // If too many lines logged
    if (_ogeLogger->logCount < _ogeLogger->maxLogCount) {
        // Log frame time
        if (_ogeLogger->logType == OGE_LOGTYPE_HTML)
        {
            fprintf(_ogeLogger->logFile, "<font style=\"FONT-FAMILY: \'Courier New\'\" size=2>\n");
            fprintf(_ogeLogger->logFile, "--------------------------------------------------<br>\n");
            fprintf(_ogeLogger->logFile, "Update: %d - DeltaTime: %f ms<br></font>\n", frame, deltaTime);

        }
        else if (_ogeLogger->logType == OGE_LOGTYPE_JSON) {

        }
        else {
            fprintf(_ogeLogger->logFile, "maxLogCount reached:  Too many lines logged...");
            fprintf(_ogeLogger->logFile, "\n");
        }
    }
    else if (_ogeLogger->logCount == _ogeLogger->maxLogCount) {
        if (_ogeLogger->logType == OGE_LOGTYPE_HTML) {
            fprintf(_ogeLogger->logFile, "<font style=\"FONT-FAMILY: \'Courier New\'\" size=3>\n");
            fprintf(_ogeLogger->logFile, "Max log count reached: stop logging...<br></font>\n");

        }
        else if (_ogeLogger->logType == OGE_LOGTYPE_JSON) {

        }
        else {
            fprintf(_ogeLogger->logFile, "maxLogCount reached:  Too many lines logged...");
            fprintf(_ogeLogger->logFile, "\n");
        }

        return;
    }
}

void  OgeLogOpenFile(const char* filename) {
    if (_ogeLogger->logFile != NULL) {
        assert("Log file exist already");
        OgeLogCloseFile();
    }

    FILE* logfile = NULL;
    if (strlen(filename) == 0) {
        logfile = fopen("OgeLogFile", "w+"); // TODO append .xml, .txt, or .htm
    }
    else {
        logfile = fopen(filename, "w+");
    }

    _ogeLogger->logFile = logfile;

    _ogeLogger->LogHeader();
}

// Close and free
void  OgeLogCloseFile() {
    if (_ogeLogger != NULL && _ogeLogger->logFile != NULL) {
        _ogeLogger->LogFooter();

        fclose(_ogeLogger->logFile);
        _ogeLogger->logFile = 0;
    }

    free(_ogeLogger);
}

//--------------- Text File ---------------------

void OgeLogText(int level, const char* text, const char* file, int line) {
    if (_ogeLogger->showOnConsole)
        printf("%d : %s\n",level, text);

    fprintf(_ogeLogger->logFile, text);
    fprintf(_ogeLogger->logFile, "\n");
}

void OgeLogHeaderText() {
    fprintf(_ogeLogger->logFile, "\nOge - Open Game Engine\n");
    fprintf(_ogeLogger->logFile, "\nVersion : ");
    fprintf(_ogeLogger->logFile, OGE_VERSION);
    fprintf(_ogeLogger->logFile, "\n");
    fprintf(_ogeLogger->logFile, "Logged the ");
    fprintf(_ogeLogger->logFile, OgeLogGetDate());
    fprintf(_ogeLogger->logFile, "\n");
    fprintf(_ogeLogger->logFile, "----------------------------------\n");
}

void OgeLogFooterText() {
    fprintf(_ogeLogger->logFile, "--------------------------------\n");
    fprintf(_ogeLogger->logFile, "Log file closed.\n");
}

void OgeLogAllocText(int allocator, const char* action, long address, long size, const char* file, int line) {
}

//--------------- HTML File ---------------------

void OgeLogHTML(int level, const char* text, const char* file, int line) {
    fprintf(_ogeLogger->logFile, OgeLogGetFont(0)); // default
    fprintf(_ogeLogger->logFile, OgeLogGetDebugLine());
    fprintf(_ogeLogger->logFile, " ");
    fprintf(_ogeLogger->logFile, OgeLogGetTime());
    fprintf(_ogeLogger->logFile, " : ");

    OgeLogWriteIndent();

    fprintf(_ogeLogger->logFile, "</font>");
    fprintf(_ogeLogger->logFile, OgeLogGetFont(level));
    fprintf(_ogeLogger->logFile, "\n");
    fprintf(_ogeLogger->logFile, text);
    fprintf(_ogeLogger->logFile, "</font>\n");

    OgeLogCallStack();

    fprintf(_ogeLogger->logFile, "<font style=\"FONT-FAMILY: \'Courier New\'\" size=2>\n");
    fprintf(_ogeLogger->logFile, " at line");
    fprintf(_ogeLogger->logFile, "%d", line);
    fprintf(_ogeLogger->logFile, " from file ");
    fprintf(_ogeLogger->logFile, "%s", file);
    fprintf(_ogeLogger->logFile, "</font><br>\n");
}

void OgeLogHeaderHTML() {
    fprintf(_ogeLogger->logFile, "<header></header><body>\n");
    fprintf(_ogeLogger->logFile, "<font style=\"FONT-FAMILY: \'Courier New\'\" size=2>\n");
    fprintf(_ogeLogger->logFile, "<br>Oge - Open Game Engine<br>\n");
    fprintf(_ogeLogger->logFile, "<br>Version : ");
    fprintf(_ogeLogger->logFile, OGE_VERSION);
    fprintf(_ogeLogger->logFile, "<br>\n");
    fprintf(_ogeLogger->logFile, "Logged the ");
    fprintf(_ogeLogger->logFile, OgeLogGetDate());
    fprintf(_ogeLogger->logFile, "<br>");
    fprintf(_ogeLogger->logFile, "</font><br>");
    fprintf(_ogeLogger->logFile, "----------------------------------<br>\n");
}

void OgeLogFooterHTML() {
    fprintf(_ogeLogger->logFile, "<font style=\"FONT-FAMILY: \'Courier New\'\" size=2>\n");
    fprintf(_ogeLogger->logFile, "--------------------------------<br>\n");
    fprintf(_ogeLogger->logFile, "Log file closed. <br>\n");
    fprintf(_ogeLogger->logFile, "</font><br>");
    fprintf(_ogeLogger->logFile, "</body>\n");;
}

void OgeLogAllocHTML(int allocator, const char* action, long address, long size, const char* file, int line) {
     // TODO
}

//--------------- JSON File ---------------------

void replaceChar(char* str, char orig, char rep) {
    char* ix = str;
    int n = 0;
    while (str[n] != '\0') {
        if (str[n] == orig) {
            str[n] = rep;
        }
        n++;
    }
}

void replaceLastOccurence(char* str, char orig, char rep) {
    char* ix = str;
    int n = 0;
    int found = -1;
    while (str[n] != '\0') {
        if (str[n] == orig) {
            found = n;
        }
        n++;
    }

    if (n >= 0) {
        str[n] = rep;
    }
}

// {"type":"log", "p1":"59", "p2":"info", "p3":"engine.cpp:563", "p4":"Starting engine", "p5":"-" },
void OgeLogJSON(int level, const char* text, const char* file, int line) {
    char nb[50];
    
    if (_ogeLogger->showOnConsole)
        printf("%d : %s\n", level, text); // LATER Use the level change font color

    // This is the last part of the PREVIOUS line!
    if (_ogeLogger->logCount > 1)
        fprintf(_ogeLogger->logFile, ",\n");

    fprintf(_ogeLogger->logFile, "{\"type\":\"log\","); // {"type":"log",
    fprintf(_ogeLogger->logFile, "\"p1\":\""); // {"p1":"
    sprintf(nb, "%d", _ogeLogger->updateCount);
    fprintf(_ogeLogger->logFile, nb);
    fprintf(_ogeLogger->logFile, "\", \"p2\":\""); // ", "p2":"
    const char* tmp;
    switch (level)
    {
    case 0:
        tmp = "err";
        break;
    case 1:
        tmp = "rel";
        break;
    case 2:
        tmp = "log"; // normal
        break;
    case 3:
        tmp = "dbg";
        break;
    case 4:
        tmp = "info";
        break;
    case 5:
        tmp = "verb";
        break;
    default:
        tmp = "log";
        break;
    }

    fprintf(_ogeLogger->logFile, tmp);
    fprintf(_ogeLogger->logFile, "\", \"p3\":\""); // ", "p3":"

    char str[100];
    strcpy(str, file);
    replaceChar(str, '\\', '/');
    fprintf(_ogeLogger->logFile, str);
    fprintf(_ogeLogger->logFile, ":"); // :
    sprintf(nb, "%d", line);
    fprintf(_ogeLogger->logFile, nb); // 536
    fprintf(_ogeLogger->logFile, "\", \"p4\":\""); // ", "p4":"
    fprintf(_ogeLogger->logFile, text); // Starting engine
    fprintf(_ogeLogger->logFile, "\", \"p5\":\" - \" }"); // ", "p5":" - " },
    fprintf(_ogeLogger->logFile, "");
}

void OgeLogHeaderJSON() {
    fprintf(_ogeLogger->logFile, "{\"log\":[\n");
}

void OgeLogFooterJSON() {
    fprintf(_ogeLogger->logFile, "\n]}\n");
}

//  {"type":"mem", "p1":"10", "p2":"0", "p3":"add" ,"p4":"101084", "p5":"10000000" },
void OgeLogAllocJSON(int allocator, const char* action, long address, long size, const char* file, int line) {

    // This is the last part of the PREVIOUS line!
    if (_ogeLogger->logCount > 1)
        fprintf(_ogeLogger->logFile, ",\n");

    char nb[50];
    fprintf(_ogeLogger->logFile, "{\"type\":\"mem\","); // {"type":"mem",
    fprintf(_ogeLogger->logFile, "\"p1\":\"");         // {"p1":"
    fprintf(_ogeLogger->logFile, "" + _ogeLogger->updateCount);
    fprintf(_ogeLogger->logFile, "\", \"p2\":\"");      // ", "p2":"
    sprintf(nb, "%d", allocator);
    fprintf(_ogeLogger->logFile, nb);       // Memory Allocator Nb
    fprintf(_ogeLogger->logFile, "\", \"p3\":\"");      // ", "p3":"
    fprintf(_ogeLogger->logFile, action);               // Actions: add, clr, del, ..
    fprintf(_ogeLogger->logFile, "\", \"p4\":\"");      // ", "p4":"
    sprintf(nb, "%d", address);
    fprintf(_ogeLogger->logFile, nb);
    fprintf(_ogeLogger->logFile, "\", \"p5\":\""); // ", "p5":"
    sprintf(nb, "%d", size);
    fprintf(_ogeLogger->logFile, nb);
    fprintf(_ogeLogger->logFile, "\" }");            // " }
    
    // TODO add file + line !!
}

//------------------------------------------------

// Return something like "Mon Jun 8 15:49:35 2020"
const char* OgeLogGetDate() {
    struct tm* pTime;
    time_t ctTime;
    time(&ctTime);
    pTime = localtime(&ctTime);

    /*
      char* str;

      std::string str;
      str += toString(pTime->tm_mday, 2, '0');
      str += ":";
      str += toString(pTime->tm_mon, 2, '0');
      str += ":";
      str += toString(pTime->tm_year + 1900);

      return str;
      */

    return asctime(pTime);
}

const char* OgeLogGetTime() {
    struct tm* pTime;
    time_t ctTime;
    time(&ctTime);
    pTime = localtime(&ctTime);

    static char str[9];

    // LATER check http://www.cplusplus.com/reference/ctime/strftime/
    sprintf(str, "%02d:%02d:%02d", pTime->tm_hour, pTime->tm_min, pTime->tm_sec);

    return str;
}

const char* OgeLogGetDebugLine() {
    static char str[6];
    sprintf(str, "#%04d", _ogeLogger->logCount);
    return str;
}

const char* OgeLogGetFont(int level) {
    switch (level) {
    case OGE_LOG_ERROR:
        return "<font color=\"white\" style=\"FONT-FAMILY: \'Courier New\';BACKGROUND-COLOR:red\" size=2>";
    case OGE_LOG_RELEASE:
        return "<font color=\"blue\" style=\"FONT-FAMILY: \'Courier New\';BACKGROUND-COLOR:white\" size=2>";
    case OGE_LOG_NORMAL:
        return "<font color=\"green\" style=\"FONT-FAMILY: \'Courier New\';BACKGROUND-COLOR:white\" size=2>";
    case OGE_LOG_DEBUG:
        return "<font color=\"red\" style=\"FONT-FAMILY: \'Courier New\';BACKGROUND-COLOR:white\" size=2>";
    case OGE_LOG_INFO:
        return "<font color=\"orange\" style=\"FONT-FAMILY: \'Courier New\';BACKGROUND-COLOR:white\" size=2>";
    case OGE_LOG_VERBOSE:
        return "<font color=\"black\" style=\"FONT-FAMILY: \'Courier New\';BACKGROUND-COLOR:white\" size=2>";
    default:
        break;
    }

    return "<font color=\"black\" style=\"FONT-FAMILY: \'Courier New\';BACKGROUND-COLOR:white\" size=2>";
}

void OgeLogWriteIndent() {
    if (_ogeLogger->logType == OGE_LOGTYPE_HTML) {
        for (unsigned int i = 0; i < _ogeLogger->previousStackLevel; i++)
            fprintf(_ogeLogger->logFile, "|  ");
    }
    else if (_ogeLogger->logType == OGE_LOGTYPE_JSON) {
        // LATER
    }
    else {
        for (unsigned int i = 0; i < _ogeLogger->previousStackLevel; i++)
            fprintf(_ogeLogger->logFile, "|  ");
    }
}

void OgeLogPushFunction(const char* name) {
    // NOW mCallStack.push_back(name);
}

void OgeLogPopFunction() {
    /* LATER
    if (mCallStack.empty())
        return;
    mCallStack.pop_back();
    if (mPreviousStackLevel >= mCallStack.size())
        logCallStack();
     */
}

void OgeLogCallStack() {
    // LATER
    /*
        unsigned int currentStackLevel = mCallStack.size();

    while(mPreviousStackLevel < currentStackLevel)
    {
        // TODO only html
        htmlFunctionStart(mCallStack[mPreviousStackLevel]);
        mPreviousStackLevel++;
    }

    while(mPreviousStackLevel > currentStackLevel)
    {
        htmlFunctionEnd(); // TODO only html
        mPreviousStackLevel--;
    }
    */
}

void OgeLogFunctionStart(const char* fn) {
    /* LATER
    // Replace "#0002 14:21:24 :" = 17 non-breaking space
    mFileLog << "<font style=\"FONT-FAMILY: \'Courier New\'\" size=2>" << std::endl;
    mFileLog << "<br>";

    //THE NEXT LINE CONTAINS 17 non-breaking spaces:
    You must write 17 times "& n b s p ;" in one word without spaces!
        mFileLog << "                 ";

    writeIndent();
    mFileLog << "in " << fn;
    mFileLog << "</font>" << std::endl;
    mFileLog.flush();
    */
}

void OgeLogHtmlFunctionEnd() {
    // ! no need...  writeIndent();
}


#ifdef _MSC_VER
#   pragma warning(pop)
#endif