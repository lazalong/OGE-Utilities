#include "oge/oge.h"
#include "oge/utilities/Logger.h"
#include "oge/utilities/Memory.h"

int main(int argc, char* argv[]) {
    printf("Oge v%s\n", OGE_VERSION);
    OgeLogger* logMgr = (OgeLogger*) OgeCreateLogger("oge_log.json", OGE_LOGTYPE_JSON, 100, true);

    LOG("test");

    LOGA(0, "add", 1000, 100, __FILE__, __LINE__);
    LOGA(0, "add", 1200, 100, __FILE__, __LINE__);
    LOGA(1, "add", 1000, 100, __FILE__, __LINE__);
    LOGA(1, "add", 1100, 100, __FILE__, __LINE__);
    LOGA(0, "del", 1000, 100, __FILE__, __LINE__);
    LOGA(1, "del", 1100, 100, __FILE__, __LINE__);

    char* test = (char*)malloc(512);
    char* test2 = (char*)malloc(512);

    free(test);


    OgeLogCloseFile();
    OgeMemoryReport(1);

    printf("Type key");
    return 0;
}