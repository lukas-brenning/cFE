/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
** File:
**    es_UT.c
**
** Purpose:
**    Executive Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/

/*
** Includes
*/
#include "es_UT.h"

extern CFE_ES_PerfData_t     *Perf;
extern CFE_ES_Global_t       CFE_ES_Global;
extern CFE_ES_CDSBlockDesc_t CFE_ES_CDSBlockDesc;
extern CFE_ES_TaskData_t     CFE_ES_TaskData;
extern CFE_ES_CDSPool_t      CFE_ES_CDSMemPool;

extern int32 dummy_function(void);

/*
** Global variables
*/
/* Create a startup script buffer for a maximum of 5 lines * 80 chars/line */
char StartupScript[MAX_STARTUP_SCRIPT];

/* Number of apps instantiated */
uint32 ES_UT_NumApps;

static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_NOOP_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_NOOP_CC
};

static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_RESET_COUNTERS_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_RESET_COUNTERS_CC
};

static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_RESTART_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_RESTART_CC
};

static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_START_APP_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_START_APP_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_STOP_APP_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_STOP_APP_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_RESTART_APP_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_RESTART_APP_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_RELOAD_APP_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_RELOAD_APP_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_QUERY_ONE_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_QUERY_ONE_CC 
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_QUERY_ALL_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_QUERY_ALL_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_QUERY_ALL_TASKS_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_CLEAR_SYSLOG_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_CLEAR_SYSLOG_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_WRITE_SYSLOG_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_OVER_WRITE_SYSLOG_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_CLEAR_ER_LOG_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_CLEAR_ER_LOG_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_WRITE_ER_LOG_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_START_PERF_DATA_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_STOP_PERF_DATA_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_SET_PERF_FILTER_MASK_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_SET_PERF_TRIGGER_MASK_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_RESET_PR_COUNT_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_RESET_PR_COUNT_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_SET_MAX_PR_COUNT_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_SET_MAX_PR_COUNT_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_DELETE_CDS_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_DELETE_CDS_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_SEND_MEM_POOL_STATS_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_DUMP_CDS_REGISTRY_CC
};

static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_CMD_INVALID_CC =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
        .CommandCode = CFE_ES_DUMP_CDS_REGISTRY_CC + 2
};

static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_ES_SEND_HK =
{
        .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_SEND_HK_MID)
};

/*
** Functions
*/
CFE_ES_ResourceID_t ES_UT_MakeAppIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up AppID values in a manner similar to FSW.
     * Real apps should never do this. */
    return CFE_ES_ResourceID_FromInteger(ArrayIdx + CFE_ES_APPID_BASE);
}

CFE_ES_ResourceID_t ES_UT_MakeTaskIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up TaskID values in a manner similar to FSW.
     * Real apps should never do this. */
    return CFE_ES_ResourceID_FromInteger(ArrayIdx + 0x40010000);
}

CFE_ES_ResourceID_t ES_UT_MakeLibIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up LibID values in a manner similar to FSW.
     * Real apps should never do this. */
    return CFE_ES_ResourceID_FromInteger(ArrayIdx + CFE_ES_LIBID_BASE);
}

/*
 * Helper function to setup a single app ID in the given state, along with
 * a main task ID.  A pointer to the App and Task record is output so the
 * record can be modified
 */
void ES_UT_SetupSingleAppId(CFE_ES_AppType_Enum_t AppType, CFE_ES_AppState_Enum_t AppState,
        const char *AppName, CFE_ES_AppRecord_t **OutAppRec, CFE_ES_TaskRecord_t **OutTaskRec)
{
    osal_id_t UtOsalId;
    CFE_ES_ResourceID_t UtTaskId;
    CFE_ES_ResourceID_t UtAppId;
    CFE_ES_AppRecord_t *LocalAppPtr;
    CFE_ES_TaskRecord_t *LocalTaskPtr;

    OS_TaskCreate(&UtOsalId, "UT", NULL, NULL, 0, 0, 0);
    UtTaskId = CFE_ES_ResourceID_FromOSAL(UtOsalId);
    UtAppId = ES_UT_MakeAppIdForIndex(ES_UT_NumApps);
    ++ES_UT_NumApps;

    LocalTaskPtr = CFE_ES_LocateTaskRecordByID(UtTaskId);
    LocalAppPtr = CFE_ES_LocateAppRecordByID(UtAppId);
    CFE_ES_TaskRecordSetUsed(LocalTaskPtr, UtTaskId);
    CFE_ES_AppRecordSetUsed(LocalAppPtr, UtAppId);
    LocalTaskPtr->AppId = UtAppId;
    LocalAppPtr->TaskInfo.MainTaskId = UtTaskId;
    LocalAppPtr->AppState = AppState;
    LocalAppPtr->Type = AppType;

    if (AppName)
    {
        strncpy(LocalAppPtr->StartParams.Name, AppName,
                sizeof(LocalAppPtr->StartParams.Name)-1);
        LocalAppPtr->StartParams.Name[sizeof(LocalAppPtr->StartParams.Name)-1] = 0;
        strncpy(LocalTaskPtr->TaskName, AppName,
                sizeof(LocalTaskPtr->TaskName)-1);
        LocalTaskPtr->TaskName[sizeof(LocalTaskPtr->TaskName)-1] = 0;
    }

    if (OutAppRec)
    {
        *OutAppRec = LocalAppPtr;
    }
    if (OutTaskRec)
    {
        *OutTaskRec = LocalTaskPtr;
    }

    if (AppType == CFE_ES_AppType_CORE)
    {
        ++CFE_ES_Global.RegisteredCoreApps;
    }
    if (AppType == CFE_ES_AppType_EXTERNAL)
    {
        ++CFE_ES_Global.RegisteredExternalApps;
    }
    ++CFE_ES_Global.RegisteredTasks;
}

/*
 * Helper function to setup a child task ID associated with the given
 * app record.  A pointer to the Task record is output so the record
 * can be modified
 */
void ES_UT_SetupChildTaskId(const CFE_ES_AppRecord_t *ParentApp, const char *TaskName, CFE_ES_TaskRecord_t **OutTaskRec)
{
    osal_id_t UtOsalId;
    CFE_ES_ResourceID_t UtTaskId;
    CFE_ES_ResourceID_t UtAppId;
    CFE_ES_TaskRecord_t *LocalTaskPtr;

    UtAppId = CFE_ES_AppRecordGetID(ParentApp);

    OS_TaskCreate(&UtOsalId, "C", NULL, NULL, 0, 0, 0);
    UtTaskId = CFE_ES_ResourceID_FromOSAL(UtOsalId);

    LocalTaskPtr = CFE_ES_LocateTaskRecordByID(UtTaskId);
    CFE_ES_TaskRecordSetUsed(LocalTaskPtr, UtTaskId);
    LocalTaskPtr->AppId = UtAppId;

    if (TaskName)
    {
        strncpy(LocalTaskPtr->TaskName, TaskName,
                sizeof(LocalTaskPtr->TaskName)-1);
        LocalTaskPtr->TaskName[sizeof(LocalTaskPtr->TaskName)-1] = 0;
    }

    if (OutTaskRec)
    {
        *OutTaskRec = LocalTaskPtr;
    }

    ++CFE_ES_Global.RegisteredTasks;
}

void ES_UT_SetupSingleCDSRegistry(const char *CDSName, bool IsTable,
        CFE_ES_CDS_RegRec_t **OutRegRec)
{
    CFE_ES_CDS_RegRec_t *LocalRegRecPtr;

    /*
     * Note - because the "ES_ResetUnitTest()" routine wipes the entire
     * global data structure, this runtime var needs to be re-initialized.
     */
    CFE_ES_Global.CDSVars.MaxNumRegEntries =
            sizeof(CFE_ES_Global.CDSVars.Registry) / sizeof(CFE_ES_CDS_RegRec_t);

    LocalRegRecPtr = &CFE_ES_Global.CDSVars.Registry[0];
    if (CDSName != NULL)
    {
        strncpy(LocalRegRecPtr->Name, CDSName,
                sizeof(LocalRegRecPtr->Name) - 1);
        LocalRegRecPtr->Name[sizeof(LocalRegRecPtr->Name) - 1] = 0;
    }
    else
    {
        LocalRegRecPtr->Name[0] = 0;
    }
    LocalRegRecPtr->Taken = true;
    LocalRegRecPtr->Table = IsTable;
    LocalRegRecPtr->MemHandle =
        sizeof(CFE_ES_Global.CDSVars.ValidityField);

    if (OutRegRec)
    {
        *OutRegRec = LocalRegRecPtr;
    }
}


int32 ES_UT_SetupOSCleanupHook(void *UserObj, int32 StubRetcode,
                               uint32 CallCount,
                               const UT_StubContext_t *Context)
{
    osal_id_t ObjList[7];

    /* On the first call, Use the stub functions to generate one object of
     * each type
     */
    if (CallCount == 0)
    {
        OS_TaskCreate(&ObjList[0], NULL, NULL, NULL, 0, 0, 0);
        OS_QueueCreate(&ObjList[1], NULL, 0, 0, 0);
        OS_MutSemCreate(&ObjList[2], NULL, 0);
        OS_BinSemCreate(&ObjList[3], NULL, 0, 0);
        OS_CountSemCreate(&ObjList[4], NULL, 0, 0);
        OS_TimerCreate(&ObjList[5], NULL, NULL, NULL);
        OS_OpenCreate(&ObjList[6], NULL, 0, 0);

        UT_SetDataBuffer((UT_EntryKey_t)&OS_ForEachObject, ObjList,
                          sizeof(ObjList), true);
    }

    return StubRetcode;
}

static void ES_UT_SetupForOSCleanup(void)
{
    UT_SetHookFunction(UT_KEY(OS_ForEachObject), ES_UT_SetupOSCleanupHook,
                       NULL);
}

typedef struct
{
    uint32 AppType;
    uint32 AppState;
} ES_UT_SetAppStateHook_t;

static int32 ES_UT_SetAppStateHook(void *UserObj, int32 StubRetcode,
                                           uint32 CallCount,
                                           const UT_StubContext_t *Context)
{
    ES_UT_SetAppStateHook_t *StateHook = UserObj;
    uint32 i;
    CFE_ES_AppRecord_t *AppRecPtr;


    AppRecPtr = CFE_ES_Global.AppTable;
    for (i=0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; ++i)
    {
        if ( CFE_ES_AppRecordIsUsed(AppRecPtr) )
        {
            /* If no filter object supplied, set all apps to RUNNING */
            if (StateHook == NULL)
            {
                AppRecPtr->AppState = CFE_ES_AppState_RUNNING;
            }
            else if (StateHook->AppType == 0 || AppRecPtr->Type == StateHook->AppType)
            {
                AppRecPtr->AppState = StateHook->AppState;
            }
        }
        ++AppRecPtr;
    }

    return StubRetcode;
}

void UtTest_Setup(void)
{
    UT_Init("es");
    UtPrintf("cFE ES Unit Test Output File\n\n");

    /* Set up the performance logging variable */
    Perf = (CFE_ES_PerfData_t *) &CFE_ES_ResetDataPtr->Perf;

    UT_ADD_TEST(TestInit);
    UT_ADD_TEST(TestStartupErrorPaths);
    UT_ADD_TEST(TestApps);
    UT_ADD_TEST(TestLibs);
    UT_ADD_TEST(TestERLog);
    UT_ADD_TEST(TestTask);
    UT_ADD_TEST(TestPerf);
    UT_ADD_TEST(TestAPI);
    UT_ADD_TEST(TestGenericCounterAPI);
    UT_ADD_TEST(TestCDS);
    UT_ADD_TEST(TestCDSMempool);
    UT_ADD_TEST(TestESMempool);
    UT_ADD_TEST(TestSysLog);
    UT_ADD_TEST(TestBackground);
}

/*
** Reset variable values prior to a test
*/
void ES_ResetUnitTest(void)
{
    UT_InitData();

    memset(&CFE_ES_Global, 0, sizeof(CFE_ES_Global));
    ES_UT_NumApps = 0;
} /* end ES_ResetUnitTest() */

void TestInit(void)
{
    UtPrintf("Begin Test Init");

    UT_SetCDSSize(128 * 1024);
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));

    /* Set up the reset area */
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_ToneSignalSelect_PRIMARY);

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));

    /* Go through ES_Main and cover normal paths */
    UT_SetDummyFuncRtn(OS_SUCCESS);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, NULL);
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1,
                CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 0,
              "CFE_ES_Main",
              "Normal startup");
}

void TestStartupErrorPaths(void)
{
    int j;
    ES_UT_SetAppStateHook_t StateHook;
    uint32 PanicStatus;
    uint32 ResetType;
    CFE_ES_TaskRecord_t *TaskRecPtr;
    CFE_ES_AppRecord_t *AppRecPtr;

    UtPrintf("Begin Test Startup Error Paths");

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';

    /* Perform ES main startup with a mutex creation failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_MutSemCreate), OS_ERROR);
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Panic), &PanicStatus, sizeof(PanicStatus), false);
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, 1, 1,
                CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              PanicStatus == CFE_PSP_PANIC_STARTUP_SEM &&
              UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 1,
              "CFE_ES_Main",
              "Mutex create failure");

    /* Perform ES main startup with a file open failure */
    ES_ResetUnitTest();
    UT_SetDummyFuncRtn(OS_SUCCESS);
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, NULL);
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, 1, 1,
                (char *) CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_OPEN_ES_APP_STARTUP]),
              "CFE_ES_Main",
              "File open failure");

    /* Perform ES main startup with a startup sync failure */
    ES_ResetUnitTest();
    StateHook.AppState = CFE_ES_AppState_RUNNING;
    StateHook.AppType = CFE_ES_AppType_CORE;    /* by only setting core apps, it will appear as if external apps did not start */
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, &StateHook);
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, 1, 1,
                (char *) CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_STARTUP_SYNC_FAIL_1]),
              "CFE_ES_Main",
              "Startup sync failure 1");
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_STARTUP_SYNC_FAIL_2]),
              "CFE_ES_Main",
              "Startup sync failure 2");

    /* Perform a power on reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_POWERON,
                               CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_HW_SPECIAL]),
              "CFE_ES_SetupResetVariables",
              "Power on reset - Hardware special command");

    /* Perform a processor reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_PROC_RESET_MAX_HW_SPECIAL]),
              "CFE_ES_SetupResetVariables",
              "Processor reset - hardware special command");

    /* Perform a power on reset with an "other cause" sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_POWERON, -1, 1);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_OTHER]),
              "CFE_ES_SetupResetVariables",
              "Other cause reset");

    /* Perform the maximum number of processor resets */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = false;

    for (j = 0; j < CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS; j++)
    {
        CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                                   CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    }

    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount ==
                  CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS,
              "CFE_ES_SetupResetVariables",
              "Maximum processor resets");

    /* Attempt another processor reset after the maximum have occurred */
    ES_ResetUnitTest();
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Restart), &ResetType, sizeof(ResetType), false);
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount ==
                  CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS + 1 &&
              ResetType == CFE_PSP_RST_TYPE_POWERON &&
              UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1,
              "CFE_ES_SetupResetVariables",
              "Processor reset - power cycle; exceeded maximum "
                  "processor resets");

    /* Perform a power on reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_MAX_HW_SPECIAL]),
              "CFE_ES_SetupResetVariables",
              "Processor reset - hardware special command; exceeded maximum "
                  "processor resets");

    /* Perform a processor reset with an reset area failure */
    ES_ResetUnitTest();
    UT_SetStatusBSPResetArea(OS_ERROR, 0, CFE_TIME_ToneSignalSelect_PRIMARY);
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Panic), &PanicStatus, sizeof(PanicStatus), false);
    CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = true;
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              PanicStatus == CFE_PSP_PANIC_MEMORY_ALLOC &&
              UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 1,
              "CFE_ES_SetupResetVariables",
              "Get reset area error");

    /* Perform a processor reset triggered by ES */
    /* Added for coverage, as the "panic" case will should not cover this one */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = true;
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 0,
              "CFE_ES_SetupResetVariables",
              "Processor Reset caused by ES");

    /* Perform a processor reset with the size of the reset area too small */
    ES_ResetUnitTest();
    UT_SetSizeofESResetArea(0);
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Panic), &PanicStatus, sizeof(PanicStatus), false);
    UT_SetStatusBSPResetArea(OS_SUCCESS, 0, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR,
                               CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              PanicStatus == CFE_PSP_PANIC_MEMORY_ALLOC &&
              UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 1,
              "CFE_ES_SetupResetVariables",
              "Reset area too small");

    /* Test initialization of the file systems specifying a power on reset
     * following a failure to create the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_initfs), OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_mount), OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_mkfs), OS_ERROR);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_POWERON);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CREATE_VOLATILE]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]) && 
              UT_GetStubCount(UT_KEY(OS_printf)) == 2,
              "CFE_ES_InitializeFileSystems",
              "Power on reset; error creating volatile (RAM) volume");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to reformat the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_initfs), OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_mount), OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_mkfs), OS_ERROR);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CREATE_VOLATILE]) && 
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INIT_VOLATILE]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REFORMAT_VOLATILE]) &&
              UT_GetStubCount(UT_KEY(OS_printf)) == 7,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error reformatting volatile (RAM) volume");

    /* Test initialization of the file systems specifying a processor reset
     * following failure to get the volatile disk memory
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_GetVolatileDiskMem), CFE_PSP_ERROR);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
              UT_GetStubCount(UT_KEY(OS_printf)) == 3,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; cannot get memory for volatile disk");

    /* Test initialization of the file systems where the number of free blocks
     * reported is greater than the number of RAM disk sectors
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_mount), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_fsBlocksFree), 1, CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS + 1);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]) &&
                UT_GetStubCount(UT_KEY(OS_printf)) == 2,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; truncate free block count");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to remove the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_rmfs), OS_ERROR);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
               UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REMOVE_VOLATILE]) && 
               UT_GetStubCount(UT_KEY(OS_printf)) == 3,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error removing volatile (RAM) volume");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to unmount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_unmount), 1, -1);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_UNMOUNT_VOLATILE]) && 
              UT_GetStubCount(UT_KEY(OS_printf)) == 3,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error unmounting volatile (RAM) volume");

    /* Test successful initialization of the file systems */
    ES_ResetUnitTest();
    CFE_ES_InitializeFileSystems(4564564);
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(OS_printf)) == 0,
              "CFE_ES_InitializeFileSystems",
              "Initialize file systems; successful");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to remount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_mount), OS_ERROR);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]) && 
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REMOUNT_VOLATILE]) &&
              UT_GetStubCount(UT_KEY(OS_printf)) == 4,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error remounting volatile (RAM) volume");

    /* Test initialization of the file systems with an error determining the
     * number of blocks that are free on the volume
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_fsBlocksFree), 1, -1);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_DETERMINE_BLOCKS]) &&
                  UT_GetStubCount(UT_KEY(OS_printf)) == 1,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error determining blocks free on volume");

    /* Test reading the object table where a record used error occurs */
    ES_ResetUnitTest();
    TaskRecPtr = CFE_ES_Global.TaskTable;
    for (j = 0; j < OS_MAX_TASKS; j++)
    {
        /* Mark all entries as "used" to ensure that the log message gets
         * output
         */
        CFE_ES_TaskRecordSetUsed(TaskRecPtr, ES_UT_MakeTaskIdForIndex(j));
        ++TaskRecPtr;
    }

    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_RECORD_USED]) &&
                  UT_GetStubCount(UT_KEY(OS_printf)) == 23,
              "CFE_ES_CreateObjects",
              "Record used error");

    /* Test reading the object table where an error occurs when
     * calling a function
     */
    ES_ResetUnitTest();
    TaskRecPtr = CFE_ES_Global.TaskTable;
    for (j = 0; j < OS_MAX_TASKS; j++)
    {
        /* Mark all entries as "used" to ensure that the log message gets
         * output
         */
        CFE_ES_TaskRecordSetUsed(TaskRecPtr, ES_UT_MakeAppIdForIndex(j));
        ++TaskRecPtr;
    }

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_EarlyInit), 1, -1);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_RECORD_USED]) && 
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_EARLYINIT]) &&
              UT_GetStubCount(UT_KEY(OS_printf)) == 24,
              "CFE_ES_CreateObjects",
              "Error returned when calling function");

    /* Test reading the object table where an error occurs when
     * creating a core app
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_TaskCreate), OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_BinSemCreate), OS_ERROR);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CORE_APP_CREATE]) &&
                  UT_GetStubCount(UT_KEY(OS_printf)) == 13,
              "CFE_ES_CreateObjects",
              "Error creating core application");

    /* Test reading the object table where all app slots are taken */
    ES_ResetUnitTest();
    AppRecPtr = CFE_ES_Global.AppTable;
    for (j = 0; j < CFE_PLATFORM_ES_MAX_APPLICATIONS; j++)
    {
       CFE_ES_AppRecordSetUsed(AppRecPtr, ES_UT_MakeAppIdForIndex(j));
       ++AppRecPtr;
    }

    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_NO_FREE_CORE_APP_SLOTS]) == 5,
              "CFE_ES_CreateObjects",
              "No free application slots available, message");
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(OS_printf)) == 18,
              "CFE_ES_CreateObjects",
              "No free application slots available, printf count");

    /* Test reading the object table with a NULL function pointer */
    ES_ResetUnitTest();
    AppRecPtr = CFE_ES_Global.AppTable;
    for (j = 0; j < CFE_PLATFORM_ES_MAX_APPLICATIONS; j++)
    {
        CFE_ES_AppRecordSetUsed(AppRecPtr, ES_UT_MakeAppIdForIndex(j));
       ++AppRecPtr;
    }

    CFE_ES_ObjectTable[1].ObjectType = CFE_ES_FUNCTION_CALL;
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_NO_FREE_CORE_APP_SLOTS]) == 5,
              "CFE_ES_CreateObjects",
              "Bad function pointer, app slots message");
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_FUNCTION_POINTER]),
              "CFE_ES_CreateObjects",
              "Bad function pointer message");
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(OS_printf)) == 19,
              "CFE_ES_CreateObjects",
              "Bad function pointer, printf count");

    /* Test response to an invalid startup type */
    ES_ResetUnitTest();
    CFE_ES_Global.DebugVars.DebugFlag = 1;
    CFE_ES_SetupResetVariables(-1, CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_Global.DebugVars.DebugFlag == 1,
              "CFE_ES_SetupResetVariables",
              "Invalid startup type");
    CFE_ES_Global.DebugVars.DebugFlag = 0;

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to initialize and mount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_initfs), OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_mount), OS_ERROR);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INIT_VOLATILE]) && 
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REMOUNT_VOLATILE]) &&
              UT_GetStubCount(UT_KEY(OS_printf)) == 6,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error initializing and mounting volatile "
                "(RAM) volume");

    /* Test application sync delay where the operation times out */
    ES_ResetUnitTest();
    /* This prep is necessary so GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppType_CORE, NULL, &AppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    UT_Report(__FILE__, __LINE__,
            CFE_ES_WaitForSystemState(CFE_ES_SystemState_OPERATIONAL,
                                        CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) ==
                                          CFE_ES_OPERATION_TIMED_OUT,
              "CFE_ES_ApplicationSyncDelay",
              "Operation timed out");
    
    
    /* Test startup sync with alternate minimum system state
     * of CFE_ES_SystemState_SHUTDOWN
     */
    ES_ResetUnitTest();
    /* This prep is necessary so GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppType_CORE, NULL, &AppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    UT_Report(__FILE__, __LINE__, CFE_ES_WaitForSystemState(CFE_ES_SystemState_SHUTDOWN,
                                       CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) ==
                                          CFE_ES_OPERATION_TIMED_OUT &&
                                    AppRecPtr->AppState ==
                                    CFE_ES_AppState_STOPPED,
              "CFE_ES_WaitForSystemState",
              "Min System State is CFE_ES_SystemState_SHUTDOWN");
    

    /* Test startup sync with alternate minimum system state
     * of CFE_ES_SystemState_APPS_INIT
     */
    ES_ResetUnitTest();
    /* This prep is necessary so GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppType_CORE, NULL, &AppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    
    UT_Report(__FILE__, __LINE__,
            CFE_ES_WaitForSystemState(CFE_ES_SystemState_APPS_INIT,
                                        CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) ==
                                          CFE_ES_OPERATION_TIMED_OUT && 
                                        AppRecPtr->AppState ==
                                        CFE_ES_AppState_LATE_INIT,
              "CFE_ES_WaitForSystemState",
              "Min System State is CFE_ES_SystemState_APPS_INIT");
}

void TestApps(void)
{
    int NumBytes;
    int Return;
    int j;
    CFE_ES_AppInfo_t AppInfo;
    CFE_ES_ResourceID_t Id;
    CFE_ES_TaskRecord_t *UtTaskRecPtr;
    CFE_ES_AppRecord_t *UtAppRecPtr;

    UtPrintf("Begin Test Apps");

    /* Test starting an application where the startup script is too long */
    ES_ResetUnitTest();
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_fghfghfghfghfg"
            "hfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghLIB, "
            "0, 0, 0x0, 1; CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, "
            "70, 4096, 0x0, 1; CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, "
            "SCH_APP, 120, 4096, 0x0, 1; CFE_APP, /cf/apps/to.bundle, "
            "TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_FILE_LINE_TOO_LONG]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_ES_APP_STARTUP_OPEN]) && 
              UT_GetStubCount(UT_KEY(OS_printf)) == 8,
              "CFE_ES_StartApplications",
              "Line too long");

    /* Create a valid startup script for subsequent tests */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);

    /* Test starting an application with an error reading the startup file */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, -1);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_STARTUP_READ]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_ES_APP_STARTUP_OPEN]) && 
              UT_GetStubCount(UT_KEY(OS_printf)) == 2,
              "CFE_ES_StartApplications",
              "Error reading startup file");

    /* Test starting an application with an end-of-file returned by the
     * OS read
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, 0);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_ES_APP_STARTUP_OPEN]) &&
              UT_GetStubCount(UT_KEY(OS_printf)) == 1,
              "CFE_ES_StartApplications",
              "End of file reached");

    /* Test starting an application with an open failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_OPEN_ES_APP_STARTUP]) &&
                 UT_GetStubCount(UT_KEY(OS_printf)) == 2,
              "CFE_ES_StartApplications",
              "Can't open ES application startup file");

    /* Test successfully starting an application */
    ES_ResetUnitTest();
    UT_SetReadBuffer(StartupScript, NumBytes);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR,
                             CFE_PLATFORM_ES_NONVOL_STARTUP_FILE);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_ES_APP_STARTUP_OPEN]) &&
                 UT_GetStubCount(UT_KEY(OS_printf)) == 8,
              "CFE_ES_StartApplications",
              "Start application; successful");

    /* Test parsing the startup script with an unknown entry type */
    ES_ResetUnitTest();
    {
        const char *TokenList[] =
        {
                "UNKNOWN",
                "/cf/apps/tst_lib.bundle",
                "TST_LIB_Init",
                "TST_LIB",
                "0",
                "0",
                "0x0",
                "1"
        };
        UT_Report(__FILE__, __LINE__,
                CFE_ES_ParseFileEntry(TokenList, 8) == CFE_ES_ERR_APP_CREATE,
                "CFE_ES_ParseFileEntry",
                "Unknown entry type");
    }

    /* Test parsing the startup script with an invalid file entry */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ParseFileEntry(NULL, 0) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_ParseFileEntry",
              "Invalid file entry");

    /* Test application loading and creation with a task creation failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_TaskCreate), OS_ERROR);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename",
                              "EntryPoint",
                              "AppName",
                              170,
                              4096,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_APP_CREATE]),
              "CFE_ES_AppCreate",
              "Task create failure");
    
    /* Test application creation with NULL file name */
    ES_ResetUnitTest();
    Return = CFE_ES_AppCreate(&Id,
                              NULL,
                              "EntryPoint",
                              "AppName",
                              170,
                              4096,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_AppCreate",
              "NULL file name");
    
    /* Test successful application loading and creation  */
    ES_ResetUnitTest();
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS,
              "CFE_ES_AppCreate",
              "Application load/create; successful");

    /* Test application loading and creation where the file cannot be loaded */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleLoad), 1, -1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
                UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_EXTRACT_FILENAME_UT55]),
              "CFE_ES_AppCreate",
              "File load failure");

    /* Test application loading and creation where all app slots are taken */
    ES_ResetUnitTest();
    UtAppRecPtr = CFE_ES_Global.AppTable;
    for (j = 0; j < CFE_PLATFORM_ES_MAX_APPLICATIONS; j++)
    {
       CFE_ES_AppRecordSetUsed(UtAppRecPtr, ES_UT_MakeAppIdForIndex(j));
       ++UtAppRecPtr;
    }

    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
                UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_NO_FREE_APP_SLOTS]),
              "CFE_ES_AppCreate",
              "No free application slots available");

    /* Test application loading and creation where the entry point symbol
     * cannot be found
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_SymbolLookup), 1, -1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_FIND_SYMBOL]),
              "CFE_ES_AppCreate",
              "Entry point symbol lookup failure");


    /* Test application loading and creation where the entry point symbol
     * cannot be found and module unload fails
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_SymbolLookup), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleUnload), 1, -1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APP_CREATE &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_FIND_SYMBOL]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MODULE_UNLOAD_FAILED]),
              "CFE_ES_AppCreate",
              "Module unload failure after entry point lookup failure");

    /* Test scanning and acting on the application table where the timer
     * expires for a waiting application
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_WAITING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppTimerMsec = 0;
    memset(&CFE_ES_TaskData.BackgroundAppScanState, 0, sizeof(CFE_ES_TaskData.BackgroundAppScanState));
    CFE_ES_RunAppTableScan(0, &CFE_ES_TaskData.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PCR_ERR2_EID) &&
              UtAppRecPtr->ControlReq.AppTimerMsec == 0 &&
              UtAppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_SYS_DELETE,
              "CFE_ES_RunAppTableScan",
              "Waiting; process control request");

    /* Test scanning and acting on the application table where the timer
     * has not expired for a waiting application
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_WAITING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_EXIT;
    UtAppRecPtr->ControlReq.AppTimerMsec = 5000;
    CFE_ES_RunAppTableScan(1000, &CFE_ES_TaskData.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__,
              UtAppRecPtr->ControlReq.AppTimerMsec == 4000 &&
              UtAppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_APP_EXIT,
              "CFE_ES_RunAppTableScan",
              "Decrement timer");

    /* Test scanning and acting on the application table where the application
     * has stopped and is ready to be acted on
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppTimerMsec = 0;
    CFE_ES_RunAppTableScan(0, &CFE_ES_TaskData.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PCR_ERR2_EID) &&
              UtAppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_SYS_DELETE &&
              UtAppRecPtr->ControlReq.AppTimerMsec == 0,
              "CFE_ES_RunAppTableScan",
              "Stopped; process control request");

    /* Test scanning and acting on the application table where the application
     * has stopped and is ready to be acted on
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_EARLY_INIT, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppTimerMsec = 5000;
    CFE_ES_RunAppTableScan(0, &CFE_ES_TaskData.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__,
              UT_GetNumEventsSent() == 0 &&
              UtAppRecPtr->ControlReq.AppTimerMsec == 5000,
              "CFE_ES_RunAppTableScan",
              "Initializing; process control request");

   /* Test a control action request on an application with an
     * undefined control request state
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = 0x12345;
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PCR_ERR2_EID),
              "CFE_ES_ProcessControlRequest",
              "Unknown state (default)");

    /* Test a successful control action request to exit an application */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    strncpy((char *) UtAppRecPtr->StartParams.FileName,
            "/ram/Filename", OS_MAX_PATH_LEN);
    UtAppRecPtr->StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) UtAppRecPtr->StartParams.EntryPoint,
            "NotNULL", OS_MAX_API_NAME);
    UtAppRecPtr->StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    UtAppRecPtr->StartParams.Priority = 255;
    UtAppRecPtr->StartParams.StackSize = 8192;
    UtAppRecPtr->StartParams.ExceptionAction = 0;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest =
      CFE_ES_RunStatus_APP_EXIT;
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_EXIT_APP_INF_EID),
              "CFE_ES_ProcessControlRequest",
              "Exit application; successful");

    /* Test a control action request to exit an application where the
     * request fails
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_EXIT;
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_EXIT_APP_ERR_EID),
              "CFE_ES_ProcessControlRequest",
              "Exit application failure");

    /* Test a control action request to stop an application where the
     * request fails
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_DELETE;
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_STOP_ERR3_EID),
              "CFE_ES_ProcessControlRequest",
              "Stop application failure");

    /* Test a control action request to restart an application where the
     * request fails due to a CleanUpApp error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_RESTART;
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR4_EID),
              "CFE_ES_ProcessControlRequest",
              "Restart application failure; CleanUpApp error");

    /* Test a control action request to restart an application where the
     * request fails due to an AppCreate error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_RESTART;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    UT_SetForceFail(UT_KEY(OS_TaskCreate), OS_ERROR);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR3_EID),
              "CFE_ES_ProcessControlRequest",
              "Restart application failure; AppCreate error");

    /* Test a control action request to reload an application where the
     * request fails due to a CleanUpApp error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_RELOAD;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR4_EID),
              "CFE_ES_ProcessControlRequest",
              "Reload application failure; CleanUpApp error");

    /* Test a control action request to reload an application where the
     * request fails due to an AppCreate error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_RELOAD;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    UT_SetForceFail(UT_KEY(OS_TaskCreate), OS_ERROR);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR3_EID),
              "CFE_ES_ProcessControlRequest",
              "Reload application failure; AppCreate error");

    /* Test a successful control action request to exit an application that
     * has an error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    strncpy((char *) UtAppRecPtr->StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    UtAppRecPtr->StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) UtAppRecPtr->StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    UtAppRecPtr->StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    UtAppRecPtr->StartParams.Priority = 255;
    UtAppRecPtr->StartParams.StackSize = 8192;
    UtAppRecPtr->StartParams.ExceptionAction = 0;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest =
      CFE_ES_RunStatus_APP_ERROR;
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ERREXIT_APP_INF_EID),
              "CFE_ES_ProcessControlRequest",
              "Exit application on error; successful");

    /* Test a control action request to exit an application that
     * has an error where the request fails
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    UtAppRecPtr->ControlReq.AppControlRequest =
      CFE_ES_RunStatus_APP_ERROR;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ERREXIT_APP_ERR_EID),
              "CFE_ES_ProcessControlRequest",
              "Exit application on error failure");

    /* Test a successful control action request to stop an application */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    strncpy((char *) UtAppRecPtr->StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    UtAppRecPtr->StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) UtAppRecPtr->StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    UtAppRecPtr->StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    UtAppRecPtr->StartParams.Priority = 255;
    UtAppRecPtr->StartParams.StackSize = 8192;
    UtAppRecPtr->StartParams.ExceptionAction = 0;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_DELETE;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_STOP_INF_EID),
              "CFE_ES_ProcessControlRequest",
              "Stop application; successful");

    /* Test a successful control action request to restart an application */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    strncpy((char *) UtAppRecPtr->StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    UtAppRecPtr->StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) UtAppRecPtr->StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    UtAppRecPtr->StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    UtAppRecPtr->StartParams.Priority = 255;
    UtAppRecPtr->StartParams.StackSize = 8192;
    UtAppRecPtr->StartParams.ExceptionAction = 0;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_RESTART;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RESTART_APP_INF_EID),
              "CFE_ES_ProcessControlRequest",
              "Restart application; successful");

    /* Test a successful control action request to reload an application */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    strncpy((char *) UtAppRecPtr->StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    UtAppRecPtr->StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) UtAppRecPtr->StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    UtAppRecPtr->StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    UtAppRecPtr->StartParams.Priority = 255;
    UtAppRecPtr->StartParams.StackSize = 8192;
    UtAppRecPtr->StartParams.ExceptionAction = 0;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_RELOAD;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RELOAD_APP_INF_EID),
              "CFE_ES_ProcessControlRequest",
              "Reload application; successful");

    /* Test a control action request for an application that has an invalid
     * state (exception)
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    strncpy((char *) UtAppRecPtr->StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    UtAppRecPtr->StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) UtAppRecPtr->StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    UtAppRecPtr->StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    UtAppRecPtr->StartParams.Priority = 255;
    UtAppRecPtr->StartParams.StackSize = 8192;
    UtAppRecPtr->StartParams.ExceptionAction = 0;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_SYS_EXCEPTION;
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    CFE_ES_ProcessControlRequest(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PCR_ERR1_EID),
              "CFE_ES_ProcessControlRequest",
              "Invalid state");

    /* Test populating the application information structure with data */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    Id = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(&AppInfo, Id) == CFE_SUCCESS,
              "CFE_ES_GetAppInfo",
              "Get application information; successful");

    /* Test populating the application information structure with data using
     * a null application information pointer
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    Id = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(NULL, Id) == CFE_ES_ERR_BUFFER,
              "CFE_ES_GetAppInfo",
              "Null application information pointer");

    /* Test populating the application information structure using an
     * inactive application ID
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    Id = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_AppRecordSetFree(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(&AppInfo, Id) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppInfo",
              "Application ID not active");

    /* Test populating the application information structure using an
     * application ID value greater than the maximum allowed
     */
    ES_ResetUnitTest();
    Id = ES_UT_MakeAppIdForIndex(99999);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(&AppInfo, Id) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppInfo",
              "Application ID exceeds maximum");

    /* Test populating the application information structure using a valid
     * application ID, but with a failure to retrieve the module information
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    Id = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleInfo), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppInfo(&AppInfo, Id) == CFE_SUCCESS,
              "CFE_ES_GetAppInfo",
              "Module not found");

    /* Test deleting an application and cleaning up its resources with OS
     * delete and close failures
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupForOSCleanup();

    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    UT_SetForceFail(UT_KEY(OS_TaskDelete), OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_close), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(UtAppRecPtr) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Task OS delete and close failure");

    /* Test deleting an application and cleaning up its resources with a
     * mutex delete failure
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(UtAppRecPtr) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Task mutex delete failure");

    /* Test deleting an application and cleaning up its resources with a
     * failure to unload the module
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleUnload), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(UtAppRecPtr) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Module unload failure");

    /* Test deleting an application and cleaning up its resources where the
     * EVS application cleanup fails
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(UtAppRecPtr) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "EVS application cleanup failure");

    /* Test cleaning up the OS resources for a task with a failure
     *  deleting mutexes
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) ==
                CFE_ES_MUT_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Mutex delete failure");

    /* Test cleaning up the OS resources for a task with a failure deleting
     * binary semaphores
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) ==
                CFE_ES_BIN_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Binary semaphore delete failure");

    /* Test cleaning up the OS resources for a task with a failure deleting
     * counting semaphores
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemDelete), 1, OS_ERROR);
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) ==
                CFE_ES_COUNT_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Counting semaphore failure");

    /* Test cleaning up the OS resources for a task with a failure
     * deleting queues
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_QueueDelete), 1, OS_ERROR);
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) ==
                CFE_ES_QUEUE_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Queue delete failure");

    /* Test cleaning up the OS resources for a task with a failure
     * deleting timers
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    /* Just set OS_TimerDelete to fail.  There is no requirement
     * that the code call OS_TimerGetInfo first.
     */
    UT_SetDeferredRetcode(UT_KEY(OS_TimerDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) ==
                CFE_ES_TIMER_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Timer delete failure");

    /* Test cleaning up the OS resources for a task with a failure
     * closing files
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_close), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) != CFE_SUCCESS,
              "CFE_ES_CleanupTaskResources",
              "File close failure");

    /* Test cleaning up the OS resources for a task with a failure
     * to delete the task
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_SetForceFail(UT_KEY(OS_TaskDelete), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) == CFE_ES_TASK_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Task delete failure");

    /* Test successfully cleaning up the OS resources for a task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) == CFE_SUCCESS,
              "CFE_ES_CleanupTaskResources",
              "Clean up task OS resources; successful");

    /* Test parsing the startup script for a cFE application and a restart
     * application exception action
     */
    ES_ResetUnitTest();
    {
        const char *TokenList[] =
        {
                "CFE_APP",
                "/cf/apps/tst_lib.bundle",
                "TST_LIB_Init",
                "TST_LIB",
                "0",
                "0",
                "0x0",
                "0"
        };
        UT_Report(__FILE__, __LINE__,
                  CFE_ES_ParseFileEntry(TokenList, 8) == CFE_SUCCESS,
                  "CFE_ES_ParseFileEntry",
                  "CFE application; restart application on exception");
    }

    /* Test scanning and acting on the application table where the timer
     * expires for a waiting application
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_WAITING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppTimerMsec = 0;
    CFE_ES_RunAppTableScan(0, &CFE_ES_TaskData.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__,
              UT_GetNumEventsSent() == 0 &&
              UtAppRecPtr->ControlReq.AppTimerMsec == 0,
              "CFE_ES_RunAppTableScan",
              "Waiting; process control request");

    /* Test scanning and acting on the application table where the application
     * is already running
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppTimerMsec = 0;
    CFE_ES_RunAppTableScan(0, &CFE_ES_TaskData.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__,
              UT_GetNumEventsSent() == 0 &&
              UtAppRecPtr->ControlReq.AppTimerMsec == 0,
              "CFE_ES_RunAppTableScan",
              "Running; process control request");

    /* Test deleting an application and cleaning up its resources where the
     * application ID matches the main task ID
     */
    ES_ResetUnitTest();
    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    /* Setup a second entry which will NOT be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    /* Associate a child task with the app to be deleted */
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, NULL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(UtAppRecPtr) == CFE_SUCCESS,
              "CFE_ES_CleanUpApp",
              "Main task ID matches task ID, nominal");
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskRecordIsUsed(UtTaskRecPtr),
              "CFE_ES_CleanUpApp",
              "Main task ID matches task ID, other task unaffected");

    /* Test deleting an application and cleaning up its resources where the
     * application ID doesn't match the main task ID
     */
    ES_ResetUnitTest();

    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    /* Setup a second entry which will NOT be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, NULL, NULL);
    /* Associate a child task with the app to be deleted */
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, NULL);

    /* switch the main task association (makes it wrong) */
    UtAppRecPtr->TaskInfo.MainTaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);

    UT_SetForceFail(UT_KEY(OS_TaskDelete), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(UtAppRecPtr) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Main task ID doesn't match task ID, CFE_ES_APP_CLEANUP_ERR");
    UT_Report(__FILE__, __LINE__,
              !CFE_ES_TaskRecordIsUsed(UtTaskRecPtr),
              "CFE_ES_CleanUpApp",
              "Main task ID doesn't match task ID, second task unchanged");

    /* Test deleting an application and cleaning up its resources where the
     * application ID doesn't match and the application is a core application
     */
    ES_ResetUnitTest();
    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    /* Setup a second entry which will NOT be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    /* Associate a child task with the app to be deleted */
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, NULL);

    /* switch the main task association (makes it wrong) */
    UtAppRecPtr->TaskInfo.MainTaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);

    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(UtAppRecPtr) == CFE_SUCCESS,
              "CFE_ES_CleanUpApp",
              "Application ID mismatch; core application");

    UT_Report(__FILE__, __LINE__,
              !CFE_ES_TaskRecordIsUsed(UtTaskRecPtr),
              "CFE_ES_CleanUpApp",
              "Application ID mismatch; core application");

    UT_Report(__FILE__, __LINE__,
              CFE_ES_Global.RegisteredExternalApps == 1,
              "CFE_ES_CleanUpApp",
              "Application ID mismatch; core application");

    /* Test successfully deleting an application and cleaning up its resources
     * and the application is an external application
     */
    ES_ResetUnitTest();

    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);

    OS_ModuleLoad(&UtAppRecPtr->StartParams.ModuleId, "UT",
                  "ut-module");
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(UtAppRecPtr) == CFE_SUCCESS &&
              !CFE_ES_TaskRecordIsUsed(UtTaskRecPtr) &&
              CFE_ES_Global.RegisteredExternalApps == 0,
              "CFE_ES_CleanUpApp",
              "Successful application cleanup; external application");

    /* Test cleaning up the OS resources for a task with failure to
     * obtain information on mutex, binary, and counter semaphores, and
     * queues, timers, and file descriptors
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    Id = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_FDGetInfo), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanupTaskResources(Id) == CFE_SUCCESS,
              "CFE_ES_CleanupTaskResources",
              "Get OS information failures");
}

void TestLibs(void)
{
    CFE_ES_LibRecord_t *UtLibRecPtr;
    char LongLibraryName[sizeof(UtLibRecPtr->LibName)+1];
    CFE_ES_ResourceID_t Id;
    uint32 j;
    int32 Return;

    /* Test shared library loading and initialization where the initialization
     * routine returns an error
     */
    ES_ResetUnitTest();
    UT_SetDummyFuncRtn(-444);
    Return = CFE_ES_LoadLibrary(&Id,
                                "filename",
                                "EntryPoint",
                                "LibName");
    UT_Report(__FILE__, __LINE__,
              Return == -444 &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_SHARED_LIBRARY_INIT]),
              "CFE_ES_LoadLibrary",
              "Load shared library initialization failure");


    /* Test Load library returning an error on a too long library name */
    memset(&LongLibraryName[0], 'a', sizeof(UtLibRecPtr->LibName));
    LongLibraryName[sizeof(UtLibRecPtr->LibName)] = '\0';
    Return = CFE_ES_LoadLibrary(&Id,
                                "filename",
                                "EntryPoint",
                                &LongLibraryName[0]);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_LoadLibrary",
              "Load shared library bad argument (library name too long)");

    /* Test successful shared library loading and initialization */
    UT_InitData();
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS,
              "CFE_ES_LoadLibrary",
              "successful");

    UtLibRecPtr = CFE_ES_LocateLibRecordByID(Id);
    UtAssert_True(UtLibRecPtr != NULL, "CFE_ES_LoadLibrary() return valid ID");
    UtAssert_True(CFE_ES_LibRecordIsUsed(UtLibRecPtr), "CFE_ES_LoadLibrary() record used");

    /* Try loading same library again, should return the DUPLICATE code */
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_LIB_ALREADY_LOADED,
              "CFE_ES_LoadLibrary",
              "Duplicate");
    UtAssert_True(CFE_ES_ResourceID_Equal(Id, CFE_ES_LibRecordGetID(UtLibRecPtr)),
            "CFE_ES_LoadLibrary() returned previous ID");

    /* Test shared library loading and initialization where the library
     * fails to load
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleLoad), 1, -1);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB,
              "CFE_ES_LoadLibrary",
              "Load shared library failure");

    /* Test shared library loading and initialization where the library
     * entry point symbol cannot be found
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_SymbolLookup), 1, -1);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB,
              "CFE_ES_LoadLibrary",
              "Could not find library initialization symbol");

    /* Test shared library loading and initialization where the library
     * initialization function fails and then must be cleaned up
     */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_remove), OS_ERROR); /* for coverage of error path */
    UT_SetForceFail(UT_KEY(dummy_function), -555);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle",
                                "dummy_function",
                            "TST_LIB");
    UT_Report(__FILE__, __LINE__,
              Return == -555,
              "CFE_ES_LoadLibrary",
              "Library initialization function failure");

    /* Test shared library loading and initialization where there are no
     * library slots available
     */
    ES_ResetUnitTest();
    UtLibRecPtr = CFE_ES_Global.LibTable;
    for (j = 0; j < CFE_PLATFORM_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_LibRecordSetUsed(UtLibRecPtr,
                ES_UT_MakeLibIdForIndex(j));
        ++UtLibRecPtr;
    }

    Return = CFE_ES_LoadLibrary(&Id,
                                "filename",
                                "EntryPoint",
                                "LibName");
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_LOAD_LIB &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_LIBRARY_SLOTS]),
              "CFE_ES_LoadLibrary",
              "No free library slots");

}

void TestERLog(void)
{
    int Return;

    UtPrintf("Begin Test Exception and Reset Log");

    /* Test initial rolling over log entry,
     * null description,
     * and non-null context with small size
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ERLogIndex = CFE_PLATFORM_ES_ER_LOG_ENTRIES + 1;
    Return = CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE,
                                 CFE_PSP_RST_TYPE_POWERON,
                                 1,
                                 NULL);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS &&
              !strcmp(CFE_ES_ResetDataPtr->ERLog[0].BaseInfo.Description,
                      "No Description String Given.") &&
              CFE_ES_ResetDataPtr->ERLogIndex == 1,
              "CFE_ES_WriteToERLog",
              "Log entries exceeded; no description; valid context size");


    /* Test non-rolling over log entry,
     * null description,
     * and null context
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ERLogIndex = 0;
    Return = CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE,
                                 CFE_PSP_RST_TYPE_POWERON,
                                 1,
                                 NULL);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS &&
            CFE_ES_ResetDataPtr->ERLogIndex == 1,
              "CFE_ES_WriteToERLog",
              "No log entry rollover; no description; no context");
}

void TestTask(void)
{
    uint32                      ResetType;
    uint32                      UT_ContextData;
    osal_id_t                   UT_ContextTask;
    union
    {
        CFE_SB_Msg_t             Msg;
        CFE_ES_NoArgsCmd_t       NoArgsCmd;
        CFE_ES_Restart_t         RestartCmd;
        CFE_ES_StartApp_t        StartAppCmd;
        CFE_ES_StopApp_t         StopAppCmd;
        CFE_ES_RestartApp_t      RestartAppCmd;
        CFE_ES_ReloadApp_t       ReloadAppCmd;
        CFE_ES_QueryOne_t        QueryOneCmd;
        CFE_ES_QueryAll_t        QueryAllCmd;
        CFE_ES_OverWriteSyslog_t OverwriteSysLogCmd;
        CFE_ES_WriteSyslog_t     WriteSyslogCmd;
        CFE_ES_WriteERLog_t      WriteERlogCmd;
        CFE_ES_SetMaxPRCount_t   SetMaxPRCountCmd;
        CFE_ES_DeleteCDS_t       DeleteCDSCmd;
        CFE_ES_SendMemPoolStats_t TlmPoolStatsCmd;
        CFE_ES_DumpCDSRegistry_t DumpCDSRegCmd;
        CFE_ES_QueryAllTasks_t   QueryAllTasksCmd;
    } CmdBuf;
    Pool_t                      UT_TestPool;
    CFE_ES_AppRecord_t          *UtAppRecPtr;
    CFE_ES_TaskRecord_t         *UtTaskRecPtr;
    CFE_ES_CDS_RegRec_t         *UtCDSRegRecPtr;

    UtPrintf("Begin Test Task");

    /* Reset the log index; CFE_ES_TaskMain() calls CFE_ES_TaskInit() which
     * sets SystemLogMode to DISCARD, which can result in a log overflow
     * depending on the value that the index has reached from previous tests
     */
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_ES_ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_ResetDataPtr->SystemLogEndIdx = CFE_ES_ResetDataPtr->SystemLogWriteIdx;

    /* Test task main process loop with a command pipe error */
    ES_ResetUnitTest();
    /* this is needed so CFE_ES_GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    CFE_ES_TaskMain();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_COMMAND_PIPE]),
              "CFE_ES_TaskMain",
              "Command pipe error, UT_OSP_COMMAND_PIPE message");
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(OS_printf)) == 2,
              "CFE_ES_TaskMain",
              "Command pipe error, printf count");

    /* Test task main process loop with an initialization failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_TaskRegister), OS_ERROR);
    CFE_ES_TaskMain();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_APP_INIT]),
              "CFE_ES_TaskMain",
              "Task initialization fail, UT_OSP_APP_INIT message");
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REGISTER_APP]),
              "CFE_ES_TaskMain",
              "Task initialization fail, UT_OSP_REGISTER_APP message");
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(OS_printf)) == 2,
              "CFE_ES_TaskMain",
              "Task initialization fail, printf count");

    /* Test task main process loop with bad checksum information */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_GetCFETextSegmentInfo), 1, -1);
    /* this is needed so CFE_ES_GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_SUCCESS,
              "CFE_ES_TaskInit",
              "Checksum fail, task init result");
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum == 0xFFFF,
              "CFE_ES_TaskInit",
              "Checksum fail, checksum value");

    /* Test successful task main process loop - Power On Reset Path */
    ES_ResetUnitTest();
    /* this is needed so CFE_ES_GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    CFE_ES_ResetDataPtr->ResetVars.ResetType = 2;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_SUCCESS &&
              CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum != 0xFFFF,
              "CFE_ES_TaskInit",
              "Checksum success, POR Path");

    /* Test successful task main process loop - Processor Reset Path */
    ES_ResetUnitTest();
    /* this is needed so CFE_ES_GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    CFE_ES_ResetDataPtr->ResetVars.ResetType = 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_SUCCESS &&
              CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum != 0xFFFF,
              "CFE_ES_TaskInit",
              "Checksum success, PR Path");

    /* Test task main process loop with a register app failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_TaskRegister), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_ES_ERR_APP_REGISTER,
             "CFE_ES_TaskInit",
              "Register application fail");

    /* Test task main process loop with a with an EVS register failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -1,
              "CFE_ES_TaskInit",
              "EVS register fail");

    /* Test task main process loop with a SB pipe create failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -2);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -2,
              "CFE_ES_TaskInit",
              "SB pipe create fail");

    /* Test task main process loop with a HK packet subscribe failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_SubscribeEx), 1, -3);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -3,
              "CFE_ES_TaskInit",
              "HK packet subscribe fail");

    /* Test task main process loop with a ground command subscribe failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_SubscribeEx), 2, -4);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -4,
              "CFE_ES_TaskInit",
              "Ground command subscribe fail");

    /* Test task main process loop with an init event send failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, -5);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -5,
              "CFE_ES_TaskInit",
              "Initialization event send fail");

    /* Test task main process loop with version event send failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 2, -6);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -6,
              "CFE_ES_TaskInit",
              "Version event send fail");

    /* Test task init with background init fail */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, -7);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == -7,
              "CFE_ES_TaskInit",
              "Background init fail");

    /* Set the log mode to OVERWRITE; CFE_ES_TaskInit() sets SystemLogMode to
     * DISCARD, which can result in a log overflow depending on the value that
     * the index reaches during subsequent tests
     */
    CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LogMode_OVERWRITE;

    /* Test a successful HK request */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd),
            UT_TPID_CFE_ES_SEND_HK);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskData.HkPacket.Payload.HeapBytesFree > 0,
              "CFE_ES_HousekeepingCmd",
              "HK packet - get heap successful");

    /* Test the HK request with a get heap failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_HeapGetInfo), 1, -1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd),
            UT_TPID_CFE_ES_SEND_HK);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskData.HkPacket.Payload.HeapBytesFree == 0,
              "CFE_ES_HousekeepingCmd",
              "HK packet - get heap fail");

    /* Test successful no-op command */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd),
            UT_TPID_CFE_ES_CMD_NOOP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_NOOP_INF_EID),
            "CFE_ES_NoopCmd",
            "No-op");

    /* Test successful reset counters command */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd),
            UT_TPID_CFE_ES_CMD_RESET_COUNTERS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RESET_INF_EID),
              "CFE_ES_ResetCountersCmd",
              "Reset counters");

    /* Test successful cFE restart */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CmdBuf.RestartCmd.Payload.RestartType = CFE_PSP_RST_TYPE_PROCESSOR;
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Restart), &ResetType, sizeof(ResetType), false);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartCmd),
            UT_TPID_CFE_ES_CMD_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              ResetType == CFE_PSP_RST_TYPE_PROCESSOR &&
              UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1,
              "CFE_ES_RestartCmd",
              "Restart cFE");

    /* Test cFE restart with bad restart type */
    ES_ResetUnitTest();
    CmdBuf.RestartCmd.Payload.RestartType = 4524;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_Restart_t),
            UT_TPID_CFE_ES_CMD_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_BOOT_ERR_EID),
              "CFE_ES_RestartCmd",
              "Invalid restart type");

    /* Test successful app create */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppFileName, "filename",
            sizeof(CmdBuf.StartAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint));
    memset(CmdBuf.StartAppCmd.Payload.Application, 'x', 
            sizeof(CmdBuf.StartAppCmd.Payload.Application));
    CmdBuf.StartAppCmd.Payload.Priority = 160;
    CmdBuf.StartAppCmd.Payload.StackSize = 8192;
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_INF_EID),
              "CFE_ES_StartAppCmd",
              "Start application from file name");

    /* Test app create with an OS task create failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_TaskCreate), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_ERR_EID),
              "CFE_ES_StartAppCmd",
              "Start application from file name fail");

    /* Test app create with the file name too short */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppFileName, "123",
            sizeof(CmdBuf.StartAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.Application, "appName",
            sizeof(CmdBuf.StartAppCmd.Payload.Application));
    CmdBuf.StartAppCmd.Payload.Priority = 160;
    CmdBuf.StartAppCmd.Payload.StackSize = 12096;
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_INVALID_FILENAME_ERR_EID),
              "CFE_ES_StartAppCmd",
              "Invalid file name");

    /* Test app create with a null application entry point */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppFileName, "filename",
            sizeof(CmdBuf.StartAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppEntryPoint, "",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.Application, "appName",
            sizeof(CmdBuf.StartAppCmd.Payload.Application));
    CmdBuf.StartAppCmd.Payload.Priority = 160;
    CmdBuf.StartAppCmd.Payload.StackSize = 12096;
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID),
              "CFE_ES_StartAppCmd",
              "Application entry point null");

    /* Test app create with a null application name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppFileName, "filename",
            sizeof(CmdBuf.StartAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.Application, "",
            sizeof(CmdBuf.StartAppCmd.Payload.Application));
    CmdBuf.StartAppCmd.Payload.Priority = 160;
    CmdBuf.StartAppCmd.Payload.StackSize = 12096;
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_NULL_APP_NAME_ERR_EID),
              "CFE_ES_StartAppCmd",
              "Application name null");

    /* Test app create with with an invalid exception action */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppFileName, "filename",
            sizeof(CmdBuf.StartAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.Application, "appName",
            sizeof(CmdBuf.StartAppCmd.Payload.Application));
    CmdBuf.StartAppCmd.Payload.Priority = 160;
    CmdBuf.StartAppCmd.Payload.StackSize = 12096;
    CmdBuf.StartAppCmd.Payload.ExceptionAction = 255;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_EXC_ACTION_ERR_EID),
              "CFE_ES_StartAppCmd",
              "Invalid exception action");

    /* Test app create with a bad stack size */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppFileName, "filename",
            sizeof(CmdBuf.StartAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.Application, "appName",
            sizeof(CmdBuf.StartAppCmd.Payload.Application));
    CmdBuf.StartAppCmd.Payload.Priority = 160;
    CmdBuf.StartAppCmd.Payload.StackSize = 0;
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_STACK_ERR_EID),
              "CFE_ES_StartAppCmd",
              "Stack size too small");

    /* Test app create with a bad priority */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppFileName, "filename",
            sizeof(CmdBuf.StartAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.Application, "appName",
            sizeof(CmdBuf.StartAppCmd.Payload.Application));
    CmdBuf.StartAppCmd.Payload.Priority = 1000;
    CmdBuf.StartAppCmd.Payload.StackSize = 12096;
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_PRIORITY_ERR_EID),
              "CFE_ES_StartAppCmd",
              "Priority is too large");

    /* Test successful app stop */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.StopAppCmd.Payload.Application, "CFE_ES",
            sizeof(CmdBuf.StopAppCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StopApp_t),
            UT_TPID_CFE_ES_CMD_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_STOP_DBG_EID),
              "CFE_ES_StopAppCmd",
              "Stop application initiated");

    /* Test app stop failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_WAITING, "CFE_ES", NULL, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StopApp_t),
            UT_TPID_CFE_ES_CMD_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_STOP_ERR1_EID),
              "CFE_ES_StopAppCmd",
              "Stop application failed");

    /* Test app stop with a bad app name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StopAppCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(CmdBuf.StopAppCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StopApp_t),
            UT_TPID_CFE_ES_CMD_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_STOP_ERR2_EID),
              "CFE_ES_StopAppCmd",
              "Stop application bad name");

    /* Test successful app restart */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.RestartAppCmd.Payload.Application, "CFE_ES",
            sizeof(CmdBuf.RestartAppCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_RestartApp_t),
            UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RESTART_APP_DBG_EID),
              "CFE_ES_RestartAppCmd",
              "Restart application initiated");

    /* Test app restart with a bad app name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.RestartAppCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(CmdBuf.RestartAppCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_RestartApp_t),
            UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR2_EID),
              "CFE_ES_RestartAppCmd",
              "Restart application bad name");

    /* Test failed app restart */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_WAITING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.RestartAppCmd.Payload.Application, "CFE_ES",
        sizeof(CmdBuf.RestartAppCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_RestartApp_t),
            UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR1_EID),
              "CFE_ES_RestartAppCmd",
              "Restart application failed");

    /* Test successful app reload */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.ReloadAppCmd.Payload.AppFileName, "New_Name",
            sizeof(CmdBuf.ReloadAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.ReloadAppCmd.Payload.Application, "CFE_ES",
            sizeof(CmdBuf.ReloadAppCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_ReloadApp_t),
            UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RELOAD_APP_DBG_EID),
              "CFE_ES_ReloadAppCmd",
              "Reload application initiated");

    /* Test app reload with a bad app name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.ReloadAppCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(CmdBuf.ReloadAppCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_ReloadApp_t),
            UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR2_EID),
              "CFE_ES_ReloadAppCmd",
              "Reload application bad name");

    /* Test failed app reload */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_WAITING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.ReloadAppCmd.Payload.Application, "CFE_ES",
            sizeof(CmdBuf.ReloadAppCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_ReloadApp_t),
            UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR1_EID),
              "CFE_ES_ReloadAppCmd",
              "Reload application failed");

    /* Test successful telemetry packet request for single app data */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_WAITING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.QueryOneCmd.Payload.Application, "CFE_ES",
            sizeof(CmdBuf.QueryOneCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryOne_t),
            UT_TPID_CFE_ES_CMD_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ONE_APP_EID),
              "CFE_ES_QueryOneCmd",
              "Query application - success");

    /* Test telemetry packet request for single app data with failure of
     * CFE_SB_SendMsg
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.QueryOneCmd.Payload.Application, "CFE_ES",
            sizeof(CmdBuf.QueryOneCmd.Payload.Application));
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_SendMsg), 1, -1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryOne_t),
            UT_TPID_CFE_ES_CMD_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ONE_ERR_EID),
              "CFE_ES_QueryOneCmd",
              "Query application - SB send message fail");

    /* Test telemetry packet request for single app data with a bad app name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.QueryOneCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(CmdBuf.QueryOneCmd.Payload.Application));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryOne_t),
            UT_TPID_CFE_ES_CMD_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ONE_APPID_ERR_EID),
              "CFE_ES_QueryOneCmd",
              "Query application - bad application name");

    /* Test successful write of all app data to file */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy((char *) CmdBuf.QueryAllCmd.Payload.FileName, "AllFilename",
            sizeof(CmdBuf.QueryAllCmd.Payload.FileName));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAll_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ALL_APPS_EID),
              "CFE_ES_QueryAllCmd",
              "Query all applications - success");

    /* Test write of all app data to file with a null file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAll_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ALL_APPS_EID),
              "CFE_ES_QueryAllCmd",
              "Query all applications - null file name");

    /* Test write of all app data to file with a write header failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAll_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_WRHDR_ERR_EID),
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; write header");

    /* Test write of all app data to file with a file write failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_SetForceFail(UT_KEY(OS_write), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAll_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_TASKWR_ERR_EID),
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; task write");

    /* Test write of all app data to file with a file create failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAll_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_OSCREATE_ERR_EID),
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; OS create");

    /* Test successful write of all task data to a file */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAllTasks_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_TASKINFO_EID),
              "CFE_ES_QueryAllTasksCmd",
              "Task information file written");

    /* Test write of all task data to a file with write header failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.QueryAllTasksCmd.Payload.FileName, "filename",
            sizeof(CmdBuf.QueryAllTasksCmd.Payload.FileName));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, -1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAllTasks_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_TASKINFO_WRHDR_ERR_EID),
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; write header");

    /* Test write of all task data to a file with a task write failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetForceFail(UT_KEY(OS_write), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAllTasks_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_TASKINFO_WR_ERR_EID),
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; task write");

    /* Test write of all task data to a file with an OS create failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAllTasks_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_TASKINFO_OSCREATE_ERR_EID),
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; OS create");

    /* Test successful clearing of the system log */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_ClearSyslog_t),
            UT_TPID_CFE_ES_CMD_CLEAR_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_SYSLOG1_INF_EID),
              "CFE_ES_ClearSyslogCmd",
              "Clear ES log data");

    /* Test successful overwriting of the system log using discard mode */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.OverwriteSysLogCmd.Payload.Mode = CFE_ES_LogMode_OVERWRITE;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_OverWriteSyslog_t),
            UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_SYSLOGMODE_EID),
              "CFE_ES_OverWriteSyslogCmd",
              "Overwrite system log received (discard mode)");

    /* Test overwriting the system log using an invalid mode */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.OverwriteSysLogCmd.Payload.Mode = 255;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_OverWriteSyslog_t),
            UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ERR_SYSLOGMODE_EID),
              "CFE_ES_OverWriteSyslogCmd",
              "Overwrite system log using invalid mode");

    /* Test successful writing of the system log */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.WriteSyslogCmd.Payload.FileName, "filename",
            sizeof(CmdBuf.WriteSyslogCmd.Payload.FileName));
    CFE_ES_TaskData.HkPacket.Payload.SysLogEntries = 123;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_WriteSyslog_t),
            UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_SYSLOG2_EID),
              "CFE_ES_WriteSyslogCmd",
              "Write system log; success");

    /* Test writing the system log using a null file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.WriteSyslogCmd.Payload.FileName[0] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_WriteSyslog_t),
            UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_SYSLOG2_EID),
              "CFE_ES_WriteSyslogCmd",
              "Write system log; null file name");

    /* Test writing the system log with an OS create failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    strncpy((char *) CmdBuf.WriteSyslogCmd.Payload.FileName, "",
            sizeof(CmdBuf.WriteSyslogCmd.Payload.FileName));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_WriteSyslog_t),
            UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_SYSLOG2_ERR_EID),
              "CFE_ES_WriteSyslogCmd",
              "Write system log; OS create");

    /* Test writing the system log with an OS write failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetForceFail(UT_KEY(OS_write), OS_ERROR);
    CFE_ES_ResetDataPtr->SystemLogWriteIdx = snprintf(CFE_ES_ResetDataPtr->SystemLog,
            sizeof(CFE_ES_ResetDataPtr->SystemLog),
            "0000-000-00:00:00.00000 Test Message\n");
    CFE_ES_ResetDataPtr->SystemLogEndIdx = CFE_ES_ResetDataPtr->SystemLogWriteIdx;
    strncpy((char *) CmdBuf.WriteSyslogCmd.Payload.FileName, "",
            sizeof(CmdBuf.WriteSyslogCmd.Payload.FileName));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_WriteSyslog_t),
            UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID),
              "CFE_ES_WriteSyslogCmd",
              "Write system log; OS write");

    /* Test writing the system log with a write header failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_WriteSyslog_t),
            UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID),
              "CFE_ES_WriteSyslogCmd",
              "Write system log; write header");

    /* Test successful clearing of the E&R log */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_ClearERLog_t),
            UT_TPID_CFE_ES_CMD_CLEAR_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ERLOG1_INF_EID),
              "CFE_ES_ClearERLogCmd",
              "Clear E&R log");

    /* Test successful writing of the E&R log */
    /* In the current implementation, it does not directly write the file,
     * this just sets a flag for the background task */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.WriteERlogCmd.Payload.FileName, "filename",
            sizeof(CmdBuf.WriteERlogCmd.Payload.FileName));
    CFE_ES_TaskData.BackgroundERLogDumpState.IsPending = false;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_WriteERLog_t),
            UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__,
            CFE_ES_TaskData.BackgroundERLogDumpState.IsPending,
              "CFE_ES_WriteERLogCmd",
              "Write E&R log command; pending");
    UT_Report(__FILE__, __LINE__,
            !UT_EventIsInHistory(CFE_ES_ERLOG_PENDING_ERR_EID),
              "CFE_ES_WriteERLogCmd",
              "Write E&R log command; no events");

    /* sending the same command a second time should fail with an event
     * indicating a file write is already pending. */
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_WriteERLog_t),
            UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__,
            UT_EventIsInHistory(CFE_ES_ERLOG_PENDING_ERR_EID),
              "CFE_ES_WriteERLogCmd",
              "Write E&R log command; already pending event");

    /* calling the background job when no write pending should immediately return false, no event */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundERLogDumpState, 0, sizeof(CFE_ES_TaskData.BackgroundERLogDumpState));
    UT_Report(__FILE__, __LINE__,
              !CFE_ES_RunERLogDump(0, &CFE_ES_TaskData.BackgroundERLogDumpState),
              "CFE_ES_RunERLogDump",
              "Write E&R log; not pending");
    UT_Report(__FILE__, __LINE__,
            !UT_EventIsInHistory(CFE_ES_ERLOG2_EID),
              "CFE_ES_WriteERLogCmd",
              "Write E&R log command; no file written event");

    /* nominal condition - still returns false, but generates event */
    ES_ResetUnitTest();
    UT_ContextData = 42;
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Exception_CopyContext),&UT_ContextData, sizeof(UT_ContextData), false);
    CFE_ES_TaskData.BackgroundERLogDumpState.IsPending = true;
    CFE_ES_RunERLogDump(0, &CFE_ES_TaskData.BackgroundERLogDumpState);
    UT_Report(__FILE__, __LINE__,
            !CFE_ES_TaskData.BackgroundERLogDumpState.IsPending,
              "CFE_ES_RunERLogDump",
              "Write E&R log; nominal, clear flag");
    UT_Report(__FILE__, __LINE__,
            UT_EventIsInHistory(CFE_ES_ERLOG2_EID),
              "CFE_ES_WriteERLogCmd",
              "Write E&R log command; file written event");

    /* Test writing the E&R log with an OS create failure */
    ES_ResetUnitTest();
    CFE_ES_TaskData.BackgroundERLogDumpState.IsPending = true;
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    CFE_ES_RunERLogDump(0, &CFE_ES_TaskData.BackgroundERLogDumpState);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ERLOG2_ERR_EID),
              "CFE_ES_RunERLogDump",
              "Write E&R log; OS create");

    /* Test writing the E&R log with an OS write failure */
    ES_ResetUnitTest();
    CFE_ES_TaskData.BackgroundERLogDumpState.IsPending = true;
    UT_SetForceFail(UT_KEY(OS_write), OS_ERROR);
    CFE_ES_RunERLogDump(0, &CFE_ES_TaskData.BackgroundERLogDumpState);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID),
              "CFE_ES_RunERLogDump",
              "Write E&R log; OS write");

    /* Test writing the E&R log with a write header failure */
    ES_ResetUnitTest();
    CFE_ES_TaskData.BackgroundERLogDumpState.IsPending = true;
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, OS_ERROR);
    CFE_ES_RunERLogDump(0, &CFE_ES_TaskData.BackgroundERLogDumpState);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID),
              "CFE_ES_WriteERLogCmd",
              "Write E&R log; write header");

    /* Test scan for exceptions in the PSP, should invoke a Processor Reset */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_Exception_GetCount), 1);
    CFE_ES_RunExceptionScan(0, NULL);
    UT_Report(__FILE__, __LINE__,
            UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1,
              "CFE_ES_RunExceptionScan",
              "Scan for exceptions; processor restart");

    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = 1;
    UT_SetForceFail(UT_KEY(CFE_PSP_Exception_GetCount), 1);
    CFE_ES_RunExceptionScan(0, NULL);
    /* first time should do a processor restart (limit reached) */
    UT_Report(__FILE__, __LINE__,
            UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1,
              "CFE_ES_RunExceptionScan",
              "Scan for exceptions; processor restart");
    /* next time should do a poweron restart (limit reached) */
    CFE_ES_RunExceptionScan(0, NULL);
    UT_Report(__FILE__, __LINE__,
            UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 2,
              "CFE_ES_RunExceptionScan",
              "Scan for exceptions; poweron restart");

    /* nominal for app restart - associate exception with a task ID */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_Exception_GetCount), 1);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    UT_ContextTask = CFE_ES_ResourceID_ToOSAL(CFE_ES_TaskRecordGetID(UtTaskRecPtr));
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Exception_GetSummary), &UT_ContextTask, sizeof(UT_ContextTask), false);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->StartParams.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    CFE_ES_RunExceptionScan(0, NULL);
    /* should have changed AppControlRequest from RUN to SYS_RESTART,
     * and the call to CFE_PSP_Restart should NOT increment */
    UT_Report(__FILE__, __LINE__,
            UtAppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_SYS_RESTART,
              "CFE_ES_RunExceptionScan",
              "Scan for exceptions; app restart request pending");

    UT_Report(__FILE__, __LINE__,
            UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 0,
              "CFE_ES_RunExceptionScan",
              "Scan for exceptions; no psp restart");

    /* repeat, but for a CORE app, which cannot be restarted */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_Exception_GetCount), 1);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    UT_ContextTask = CFE_ES_ResourceID_ToOSAL(CFE_ES_TaskRecordGetID(UtTaskRecPtr));
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Exception_GetSummary), &UT_ContextTask, sizeof(UT_ContextTask), false);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->StartParams.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    CFE_ES_RunExceptionScan(0, NULL);
    UT_Report(__FILE__, __LINE__,
            UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1,
              "CFE_ES_RunExceptionScan",
              "Scan for exceptions; core app, psp restart");

    /* check failure of getting summary data */
    UT_SetForceFail(UT_KEY(CFE_PSP_Exception_GetSummary), CFE_PSP_NO_EXCEPTION_DATA);
    CFE_ES_RunExceptionScan(0, NULL);
    UT_Report(__FILE__, __LINE__,
            UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 2,
              "CFE_ES_RunExceptionScan",
              "Scan for exceptions; fail to get context");

    /* Test clearing the log with a bad size in the verify command
     * length call
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0,
            UT_TPID_CFE_ES_CMD_CLEAR_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_ClearERLogCmd",
              "Packet length error");

    /* Test resetting and setting the max for the processor reset count */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_ResetPRCount_t),
            UT_TPID_CFE_ES_CMD_RESET_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_RESET_PR_COUNT_EID),
              "CFE_ES_ResetPRCountCmd",
              "Set processor reset count to zero");

    /* Test setting the maximum processor reset count */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.SetMaxPRCountCmd.Payload.MaxPRCount = 3;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_SetMaxPRCount_t),
            UT_TPID_CFE_ES_CMD_SET_MAX_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_SET_MAX_PR_COUNT_EID),
              "CFE_ES_SetMaxPRCountCmd",
              "Set maximum processor reset count");

    /* Test failed deletion of specified CDS */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", false, NULL);
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.DeleteCDSCmd.Payload.CdsName,
            "CFE_ES.CDS_NAME",
            sizeof(CmdBuf.DeleteCDSCmd.Payload.CdsName));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DeleteCDS_t),
            UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CDS_DELETE_ERR_EID),
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; error");

    /* Test failed deletion of specified critical table CDS */
    /* NOTE - reuse command from previous test */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", true, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DeleteCDS_t),
            UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CDS_DELETE_TBL_ERR_EID),
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; wrong type");

    /* Test successful deletion of a specified CDS */
    ES_ResetUnitTest();
    UT_SetCDSSize(0); /* defeats the "ReadFromCDS" and causes it to use the value here */
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", false, NULL);

    /* Set up the block to read what we need to from the CDS */
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    CFE_ES_CDSBlockDesc.SizeUsed =  512;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DeleteCDS_t),
            UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CDS_DELETED_INFO_EID),
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; success");

    /* Test deletion of a specified CDS with the owning app being active */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", false, NULL);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DeleteCDS_t),
            UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CDS_OWNER_ACTIVE_EID),
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; owner active");

    /* Test deletion of a specified CDS with the name not found */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", false, &UtCDSRegRecPtr);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_BAD", NULL, NULL);
    UtCDSRegRecPtr->Taken = false;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DeleteCDS_t),
            UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CDS_NAME_ERR_EID),
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; not found");

    /* Test successful dump of CDS to file using the default dump file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DumpCDSRegistry_t),
            UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CDS_REG_DUMP_INF_EID),
              "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; success (default dump file)");

    /* Test dumping of the CDS to a file with a bad FS write header */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, -1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DumpCDSRegistry_t),
            UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_WRITE_CFE_HDR_ERR_EID),
              "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; write header");

    /* Test dumping of the CDS to a file with an OS create failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DumpCDSRegistry_t),
            UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CREATING_CDS_DUMP_ERR_EID),
              "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; OS create");

    /* Test dumping of the CDS to a file with an OS write failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetForceFail(UT_KEY(OS_write), OS_ERROR);
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", false, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DumpCDSRegistry_t),
            UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CDS_DUMP_ERR_EID),
              "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; OS write");

    /* Test telemetry pool statistics retrieval with an invalid handle */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    memset(&UT_TestPool, 0, sizeof(UT_TestPool));
    CFE_SB_SET_MEMADDR(CmdBuf.TlmPoolStatsCmd.Payload.PoolHandle, &UT_TestPool);
    UT_TestPool.PoolHandle = (cpuaddr)&UT_TestPool;
    UT_TestPool.Size = 64;
    UT_TestPool.End = UT_TestPool.PoolHandle;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_SendMemPoolStats_t),
            UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_INVALID_POOL_HANDLE_ERR_EID),
              "CFE_ES_SendMemPoolStatsCmd",
              "Telemetry pool; bad handle");

    /* Test successful telemetry pool statistics retrieval */
    ES_ResetUnitTest();
    memset(&UT_TestPool, 0, sizeof(UT_TestPool));
    UT_TestPool.PoolHandle = (cpuaddr)&UT_TestPool;
    UT_TestPool.Size = 64;
    UT_TestPool.End = UT_TestPool.PoolHandle + UT_TestPool.Size;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_SendMemPoolStats_t),
            UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_TLM_POOL_STATS_INFO_EID),
              "CFE_ES_SendMemPoolStatsCmd",
              "Telemetry pool; success");

    /* Test the command pipe message process with an invalid command */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd),
            UT_TPID_CFE_ES_CMD_INVALID_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CC1_ERR_EID),
              "CFE_ES_TaskPipe",
              "Invalid ground command");

    /* Test locking the CDS registry with a mutex take failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_LockCDSRegistry() == OS_ERROR,
              "CFE_ES_LockCDSRegistry",
              "Mutex take failed");

    /* Test unlocking the CDS registry with a mutex give failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_UnlockCDSRegistry() == OS_ERROR,
              "CFE_ES_UnlockCDSRegistry",
              "Mutex give failed");

    /* Test sending a no-op command with an invalid command length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_NOOP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_NoopCmd",
              "No-op; invalid command length");

    /* Test sending a reset counters command with an invalid command length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_RESET_COUNTERS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_ResetCountersCmd",
              "Reset counters; invalid command length");

    /* Test sending a cFE restart command with an invalid command length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_RestartCmd",
              "Restart cFE; invalid command length");

    /* Test cFE restart with a power on reset */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.RestartCmd.Payload.RestartType = CFE_PSP_RST_TYPE_POWERON;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_Restart_t),
            UT_TPID_CFE_ES_CMD_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              !UT_EventIsInHistory(CFE_ES_BOOT_ERR_EID),
              "CFE_ES_RestartCmd",
              "Power on reset restart type");

    /* Test sending a start application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_StartAppCmd",
              "Start application command; invalid command length");

    /* Test start application command with a processor restart on application
     * exception
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppFileName, "filename",
            sizeof(CmdBuf.StartAppCmd.Payload.AppFileName));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint));
    strncpy((char *) CmdBuf.StartAppCmd.Payload.Application, "appName",
            sizeof(CmdBuf.StartAppCmd.Payload.Application));
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_PROC_RESTART;
    CmdBuf.StartAppCmd.Payload.Priority = 160;
    CmdBuf.StartAppCmd.Payload.StackSize =  CFE_PLATFORM_ES_DEFAULT_STACK_SIZE;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_StartApp_t),
            UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_START_INF_EID),
              "CFE_ES_StartAppCmd",
              "Processor restart on application exception");

    /* Test sending a stop application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_StopAppCmd",
              "Stop application command; invalid command length");

    /* Test sending a restart application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_RestartAppCmd",
              "Restart application command; invalid command length");

    /* Test sending a reload application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_ReloadAppCmd",
              "Reload application command; invalid command length");

    /* Test sending a write request for a single application with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_QueryOneAppCmd",
              "Query one application command; invalid command length");

    /* Test sending a write request for all applications with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_QueryAllAppCmd",
              "Query all applications command; invalid command length");

    /* Test write of all app data to file with a file open failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAll_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_ALL_APPS_EID),
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; file open");

    /* Test sending a write request for all tasks with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_QueryAllAppCmd",
              "Query all tasks command; invalid command length");

    /* Test write of all task data to file with a file open failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_QueryAllTasks_t),
            UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_TASKINFO_EID),
              "CFE_ES_QueryAllCmd",
              "Write task information file fail; file open");

    /* Test sending a request to clear the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_CLEAR_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_ClearSyslogCmd",
              "Clear system log command; invalid command length");

    /* Test sending a request to overwrite the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_OverwriteSyslogCmd",
              "Overwrite system log command; invalid command length");

    /* Test sending a request to write the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_WriteSyslogCmd",
              "Write system log command; invalid command length");

    /* Test successful overwriting of the system log using overwrite mode */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.OverwriteSysLogCmd.Payload.Mode = CFE_ES_LogMode_OVERWRITE;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_OverWriteSyslog_t),
            UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_SYSLOGMODE_EID),
              "CFE_ES_OverWriteSyslogCmd",
              "Overwrite system log received (overwrite mode)");

    /* Test sending a request to write the error log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_WriteErrlogCmd",
              "Write error log command; invalid command length");

    /* Test sending a request to reset the processor reset count with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_RESET_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_ResetPRCountCmd",
              "Reset processor reset count command; invalid command length");

    /* Test sending a request to set the maximum processor reset count with
     * an invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_SET_MAX_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_SetMaxPRCountCmd",
              "Set maximum processor reset count command; invalid "
                "command length");

    /* Test sending a request to delete the CDS with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_DeleteCDSCmd",
              "Delete CDS command; invalid command length");

    /* Test sending a telemetry pool statistics retrieval command with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_LEN_ERR_EID),
              "CFE_ES_DeleteCDSCmd",
              "Telemetry pool command; invalid command length");

    /* Test successful dump of CDS to file using a specified dump file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", false, NULL);
    strncpy(CmdBuf.DumpCDSRegCmd.Payload.DumpFilename, "DumpFile",
            sizeof(CmdBuf.DumpCDSRegCmd.Payload.DumpFilename));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_DumpCDSRegistry_t),
            UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_CDS_REG_DUMP_INF_EID),
              "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; success (dump file specified)");
} /* end TestTask */

void TestPerf(void)
{
    union
    {
        CFE_SB_Msg_t                Msg;
        CFE_ES_StartPerfData_t      PerfStartCmd;
        CFE_ES_StopPerfData_t       PerfStopCmd;
        CFE_ES_SetPerfFilterMask_t  PerfSetFilterMaskCmd;
        CFE_ES_SetPerfTriggerMask_t PerfSetTrigMaskCmd;
    } CmdBuf;

    UtPrintf("Begin Test Performance Log");

    /* Test successful performance mask and value initialization */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    Perf->MetaData.State = CFE_ES_PERF_MAX_STATES;
    CFE_ES_SetupPerfVariables(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_SetupPerfVariables",
              "Idle data collection");

    /* Test successful performance data collection start in START
     * trigger mode
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd),
            UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID),
              "CFE_ES_StartPerfDataCmd",
              "Collect performance data; mode START");

    /* Test successful performance data collection start in CENTER
     * trigger mode
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_CENTER;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd),
            UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID),
              "CFE_ES_StartPerfDataCmd",
              "Collect performance data; mode CENTER");

    /* Test successful performance data collection start in END
     * trigger mode
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_END;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd),
            UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID),
              "CFE_ES_StartPerfDataCmd",
              "Collect performance data; mode END");

    /* Test performance data collection start with an invalid trigger mode
     * (too high)
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = (CFE_ES_PERF_TRIGGER_END + 1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd),
            UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_TRIG_ERR_EID),
              "CFE_ES_StartPerfDataCmd",
              "Trigger mode out of range (high)");

    /* Test performance data collection start with an invalid trigger mode
     * (too low)
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = 0xffffffff;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd),
            UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_TRIG_ERR_EID),
              "CFE_ES_StartPerfDataCmd",
              "Trigger mode out of range (low)");

    /* Test performance data collection start with a file write in progress */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    /* clearing the BackgroundPerfDumpState will fully reset to initial state */
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
            sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_INIT;
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd),
            UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_ERR_EID),
              "CFE_ES_StartPerfDataCmd",
              "Cannot collect performance data; write in progress");

    /* Test performance data collection by sending another valid
     * start command
     */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
            sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd),
            UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID),
              "CFE_ES_StartPerfDataCmd",
              "Start collecting performance data");

    /* Test successful performance data collection stop */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
            sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStopCmd),
            UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STOPCMD_EID),
              "CFE_ES_StopPerfDataCmd",
              "Stop collecting performance data");

    /* Test successful performance data collection stop with a non-default
         file name */
    ES_ResetUnitTest();

    /* clearing the BackgroundPerfDumpState will fully reset to initial state */
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
            sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    strncpy(CmdBuf.PerfStopCmd.Payload.DataFileName, "filename",
        sizeof(CmdBuf.PerfStopCmd.Payload.DataFileName));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStopCmd),
            UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STOPCMD_EID),
              "CFE_ES_StopPerfDataCmd",
              "Stop collecting performance data (non-default file name)");

    /* Test performance data collection stop with a file write in progress */
    ES_ResetUnitTest();
    CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_INIT;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStopCmd),
            UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_STOPCMD_ERR2_EID),
              "CFE_ES_StopPerfDataCmd",
              "Stop performance data command ignored");

    /* Test performance filter mask command with out of range filter
         mask value */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetFilterMaskCmd.Payload.FilterMaskNum =
      CFE_ES_PERF_32BIT_WORDS_IN_MASK;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_SetPerfFilterMask_t),
            UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_FILTMSKERR_EID),
              "CFE_ES_SetPerfFilterMaskCmd",
              "Performance filter mask command error; index out of range");

    /* Test successful performance filter mask command */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetFilterMaskCmd.Payload.FilterMaskNum =
      CFE_ES_PERF_32BIT_WORDS_IN_MASK / 2;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_SetPerfFilterMask_t),
            UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_FILTMSKCMD_EID),
              "CFE_ES_SetPerfFilterMaskCmd",
              "Set performance filter mask command received");

    /* Test successful performance filter mask command with minimum filter
         mask value */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetTrigMaskCmd.Payload.TriggerMaskNum = 0;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_SetPerfTriggerMask_t),
            UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_TRIGMSKCMD_EID),
              "CFE_ES_SetPerfTriggerMaskCmd",
              "Set performance trigger mask command received; minimum index");

    /* Test successful performance filter mask command with maximum filter
     * mask value
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetTrigMaskCmd.Payload.TriggerMaskNum =
      CFE_ES_PERF_32BIT_WORDS_IN_MASK - 1;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_SetPerfTriggerMask_t),
            UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_TRIGMSKCMD_EID),
              "CFE_ES_SetPerfTriggerMaskCmd",
              "Set performance trigger mask command received; maximum index");

    /* Test successful performance filter mask command with a greater than the
     * maximum filter mask value
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetTrigMaskCmd.Payload.TriggerMaskNum =
      CFE_ES_PERF_32BIT_WORDS_IN_MASK + 1;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CFE_ES_SetPerfTriggerMask_t),
            UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_ES_PERF_TRIGMSKERR_EID),
              "CFE_ES_SetPerfTriggerMaskCmd",
              "Performance trigger mask command error; index out of range");

    /* Test successful addition of a new entry to the performance log */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.TriggerCount = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE +1;
    Perf->MetaData.InvalidMarkerReported = false;
    Perf->MetaData.DataEnd = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE +1 ;
    CFE_ES_PerfLogAdd(CFE_PLATFORM_ES_PERF_MAX_IDS, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.InvalidMarkerReported == true,
              "CFE_ES_PerfLogAdd",
              "Invalid performance marker");

    /* Test addition of a new entry to the performance log with START
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.InvalidMarkerReported = true;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_START;
    Perf->MetaData.DataCount = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE + 1;
    Perf->MetaData.TriggerMask[0] = 0xFFFF;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_START &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; START");

    /* Test addition of a new entry to the performance log with CENTER
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_CENTER;
    Perf->MetaData.TriggerCount = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE / 2 + 1 ;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_CENTER &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; CENTER");

    /* Test addition of a new entry to the performance log with END
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_END;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_END &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; END");

    /* Test addition of a new entry to the performance log with an invalid
     * marker after an invalid marker has already been reported
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.InvalidMarkerReported = 2;
    CFE_ES_PerfLogAdd(CFE_PLATFORM_ES_PERF_MAX_IDS + 1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.InvalidMarkerReported == 2,
              "CFE_ES_PerfLogAdd",
              "Invalid marker after previous invalid marker");

    /* Test addition of a new entry to the performance log with a marker that
     * is not in the filter mask
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.FilterMask[0] = 0x0;
    Perf->MetaData.DataEnd = 0;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.DataEnd == 0,
              "CFE_ES_PerfLogAdd",
              "Marker not in filter mask");

    /* Test addition of a new entry to the performance log with the data count
     * below the maximum allowed
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.DataCount = 0;
    Perf->MetaData.FilterMask[0] = 0xffff;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.DataCount == 1,
              "CFE_ES_PerfLogAdd",
              "Data count below maximum");

    /* Test addition of a new entry to the performance log with a marker that
     * is not in the trigger mask
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.TriggerMask[0] = 0x0;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.State != CFE_ES_PERF_TRIGGERED,
              "CFE_ES_PerfLogAdd",
              "Marker not in trigger mask");

    /* Test addition of a new entry to the performance log with a start
     * trigger mode and the trigger count is less the buffer size
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount = 0;
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_START;
    Perf->MetaData.TriggerMask[0] = 0xffff;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.TriggerCount == 1,
              "CFE_ES_PerfLogAdd",
              "Start trigger mode; trigger count less than the buffer size");

    /* Test addition of a new entry to the performance log with a center
     * trigger mode and the trigger count is less than half the buffer size
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE / 2 - 2;
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_CENTER;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.State != CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Center trigger mode; trigger count less than half the "
                "buffer size");

    /* Test addition of a new entry to the performance log with an invalid
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount = 0;
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = -1;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.State != CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Invalid trigger mode");

    /* Test performance data collection start with an invalid message length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              !UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID),
              "CFE_ES_StartPerfDataCmd",
              "Invalid message length");

    /* Test performance data collection stop with an invalid message length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              !UT_EventIsInHistory(CFE_ES_PERF_STOPCMD_EID),
              "CFE_ES_StopPerfDataCmd",
              "Invalid message length");

    /* Test performance data filer mask with an invalid message length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC);
    UT_Report(__FILE__, __LINE__,
              !UT_EventIsInHistory(CFE_ES_PERF_FILTMSKCMD_EID),
              "CFE_ES_SetPerfFilterMaskCmd",
              "Invalid message length");

    /* Test performance data trigger mask with an invalid message length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, 
        UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC);
    UT_Report(__FILE__, __LINE__,
              !UT_EventIsInHistory(CFE_ES_PERF_TRIGMSKCMD_EID),
              "CFE_ES_SetPerfTriggerMaskCmd",
              "Invalid message length");

    /* Test perf log dump state machine */
    /* Nominal call 1 - should go through up to the DELAY state */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
                sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    CFE_ES_TaskData.BackgroundPerfDumpState.PendingState = CFE_ES_PerfDumpState_INIT;
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_TaskData.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_DELAY,
            "CFE_ES_RunPerfLogDump - CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState (%d) == DELAY (%d)",
            (int)CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_DELAY);
    UtAssert_True(UT_GetStubCount(UT_KEY(OS_OpenCreate)) == 1, "CFE_ES_RunPerfLogDump - OS_OpenCreate() called");

    /* Nominal call 2 - should go through up to the remainder of states, back to IDLE */
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_TaskData.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_IDLE,
            "CFE_ES_RunPerfLogDump - CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState (%d) == IDLE (%d)",
            (int)CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_IDLE);
    UtAssert_True(UT_GetStubCount(UT_KEY(OS_close)) == 1, "CFE_ES_RunPerfLogDump - OS_close() called");

    /* Test a failure to open the output file */
    /* This should go immediately back to idle, and generate CFE_ES_PERF_LOG_ERR_EID */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
                sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    CFE_ES_TaskData.BackgroundPerfDumpState.PendingState = CFE_ES_PerfDumpState_INIT;
    UT_SetForceFail(UT_KEY(OS_OpenCreate), -10);
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_TaskData.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_IDLE,
            "CFE_ES_RunPerfLogDump - OS create fail, CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState (%d) == IDLE (%d)",
            (int)CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_IDLE);
    UtAssert_True(UT_EventIsInHistory(CFE_ES_PERF_LOG_ERR_EID),
              "CFE_ES_RunPerfLogDump - OS create fail, generated CFE_ES_PERF_LOG_ERR_EID");

    /* Test a failure to write to the output file */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
            sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    UT_SetForceFail(UT_KEY(OS_write), -10);
    CFE_ES_TaskData.BackgroundPerfDumpState.PendingState = CFE_ES_PerfDumpState_INIT;
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_TaskData.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_DELAY,
            "CFE_ES_RunPerfLogDump - OS_write fail, CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState (%d) == DELAY (%d)",
            (int)CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_DELAY);

    /* This will trigger the OS_write() failure, which should go through up to the remainder of states, back to IDLE */
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_TaskData.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_IDLE,
            "CFE_ES_RunPerfLogDump - OS_write fail, CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState (%d) == IDLE (%d)",
            (int)CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_IDLE);
    UtAssert_True(UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID),
              "CFE_ES_RunPerfLogDump - OS_write fail, generated CFE_ES_FILEWRITE_ERR_EID");


    /* Test the ability of the file writer to handle the "wrap around" from the end of
     * the perflog buffer back to the beginning.  Just need to set up the metadata
     * so that the writing position is toward the end of the buffer.
     */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
            sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    OS_OpenCreate(&CFE_ES_TaskData.BackgroundPerfDumpState.FileDesc, "UT", 0, OS_WRITE_ONLY);
    CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES;
    CFE_ES_TaskData.BackgroundPerfDumpState.PendingState = CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES;
    CFE_ES_TaskData.BackgroundPerfDumpState.DataPos = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE - 2;
    CFE_ES_TaskData.BackgroundPerfDumpState.StateCounter = 4;
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_TaskData.BackgroundPerfDumpState);
    /* check that the wraparound occurred */
    UtAssert_True(CFE_ES_TaskData.BackgroundPerfDumpState.DataPos == 2,
            "CFE_ES_RunPerfLogDump - wraparound, DataPos (%u) == 2",
            (unsigned int)CFE_ES_TaskData.BackgroundPerfDumpState.DataPos);
    /* should have written 4 entries to the log */
    UtAssert_True(CFE_ES_TaskData.BackgroundPerfDumpState.FileSize == sizeof(CFE_ES_PerfDataEntry_t) * 4,
            "CFE_ES_RunPerfLogDump - wraparound, FileSize (%u) == sizeof(CFE_ES_PerfDataEntry_t) * 4",
            (unsigned int)CFE_ES_TaskData.BackgroundPerfDumpState.FileSize);

    /* Confirm that the "CFE_ES_GetPerfLogDumpRemaining" function works.
     * This requires that the state is not idle, in order to get nonzero results.
     */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
            sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    OS_OpenCreate(&CFE_ES_TaskData.BackgroundPerfDumpState.FileDesc, "UT", 0, OS_WRITE_ONLY);
    CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_WRITE_PERF_METADATA;
    CFE_ES_TaskData.BackgroundPerfDumpState.StateCounter = 10;
    Perf->MetaData.DataCount = 100;
    /* in states other than WRITE_PERF_ENTRIES, it should report the full size of the log */
    UtAssert_True(CFE_ES_GetPerfLogDumpRemaining() == 100, " CFE_ES_GetPerfLogDumpRemaining - Setup Phase");
    /* in WRITE_PERF_ENTRIES, it should report the StateCounter */
    CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES;
    UtAssert_True(CFE_ES_GetPerfLogDumpRemaining() == 10, " CFE_ES_GetPerfLogDumpRemaining - Active Phase");
}

void TestAPI(void)
{
    osal_id_t TestObjId;
    char AppName[32];
    uint32 StackBuf[8];
    int32  Return;
    uint8  Data[12];
    uint32 ResetType;
    uint32 *ResetTypePtr;
    CFE_ES_ResourceID_t AppId;
    CFE_ES_ResourceID_t TaskId;
    uint32 RunStatus;
    CFE_ES_TaskInfo_t TaskInfo;
    CFE_ES_AppInfo_t AppInfo;
    CFE_ES_AppRecord_t *UtAppRecPtr;
    CFE_ES_TaskRecord_t *UtTaskRecPtr;

    UtPrintf("Begin Test API");

    /* Test resetting the cFE with a processor reset */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount =
        CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount - 1;
    CFE_ES_ResetCFE(ResetType);
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount =
        CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetCFE(ResetType) == CFE_ES_NOT_IMPLEMENTED &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_MAX_PROC_RESETS]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_PROC_RESET_COMMANDED]) && 
                  UT_GetStubCount(UT_KEY(OS_printf)) == 2,
              "CFE_ES_ResetCFE",
              "Processor reset");

    /* Test getting the reset type using a valid pointer and a null pointer */
    ES_ResetUnitTest();
    Return = CFE_ES_GetResetType(&ResetType);
    ResetTypePtr = NULL;
    CFE_ES_GetResetType(ResetTypePtr);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_PSP_RST_TYPE_PROCESSOR &&
              ResetTypePtr == NULL,
              "CFE_ES_GetResetType",
              "Get reset type successful");

    /* Test resetting the cFE with a power on reset */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_POWERON;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetCFE(ResetType) == CFE_ES_NOT_IMPLEMENTED &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_COMMANDED]) &&
                  UT_GetStubCount(UT_KEY(OS_printf)) == 1,
              "CFE_ES_ResetCFE",
              "Power on reset");

    /* Test resetting the cFE with an invalid argument */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_POWERON + 3;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetCFE(ResetType) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ResetCFE",
              "Bad reset type");

    /* Test restarting an app that doesn't exist */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    AppId = ES_UT_MakeAppIdForIndex(ES_UT_NumApps); /* Should be within range, but not used */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RestartApp(AppId) == CFE_ES_ERR_APPID,
              "CFE_ES_RestartApp",
              "Bad application ID");

    /* Test restarting an app with an ID out of range (high) */
    ES_ResetUnitTest();
    AppId = ES_UT_MakeAppIdForIndex(99999);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RestartApp(AppId) == CFE_ES_ERR_APPID,
              "CFE_ES_RestartApp",
              "Application ID too large");

    /* Test reloading an app that doesn't exist */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ReloadApp(AppId, "filename") == CFE_ES_ERR_APPID,
              "CFE_ES_ReloadApp",
              "Bad application ID");

    /* Test deleting an app that doesn't exist */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteApp(AppId) == CFE_ES_ERR_APPID,
              "CFE_ES_DeleteApp",
              "Bad application ID");

    /* Test exiting an app with an init error */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_INIT_ERROR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CORE_INIT]) &&
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_MAX_PROC_RESETS]) && 
                  UT_GetStubCount(UT_KEY(OS_printf)) == 3,
              "CFE_ES_ExitApp",
              "Application initialization error");

    /* Test exiting an app with a runtime error */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CORE_RUNTIME]) &&
                  UT_GetStubCount(UT_KEY(OS_printf)) == 1,
              "CFE_ES_ExitApp",
              "Application runtime error");

    /* Test exiting an app with an exit error */
    /* Note - this exit code of 1000 is invalid, which causes
     * an extra message to be logged in syslog about this.  This
     * should also be stored in the AppControlRequest as APP_ERROR. */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_STOPPED, "UT", &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    CFE_ES_ExitApp(1000);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CORE_APP_EXIT]) &&
                  UT_GetStubCount(UT_KEY(OS_printf)) == 2,
              "CFE_ES_ExitApp",
              "Application exit error");
    UtAssert_True(UtAppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_APP_ERROR,
            "CFE_ES_ExitApp - AppControlRequest (%u) == CFE_ES_RunStatus_APP_ERROR (%u)",
            (unsigned int)UtAppRecPtr->ControlReq.AppControlRequest,
            (unsigned int)CFE_ES_RunStatus_APP_ERROR);

#if 0
    /* Can't cover this path since it contains a while(1) (i.e.,
     * infinite) loop
     */
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_TaskRecordSetUsed(TaskRecPtr);
    TaskRecPtr->AppId = Id;
    AppRecPtr->Type = CFE_ES_AppType_EXTERNAL;
    AppRecPtr->AppState = CFE_ES_AppState_RUNNING;
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);
#endif

    /* Test successful run loop app run request */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    RunStatus = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_APP_RUN;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == true,
              "CFE_ES_RunLoop",
              "Request to run application");

    /* Test successful run loop app stop request */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    RunStatus = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_APP_EXIT;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == false,
              "CFE_ES_RunLoop",
              "Request to stop running application");

    /* Test successful run loop app exit request */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    RunStatus = CFE_ES_RunStatus_APP_EXIT;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_APP_EXIT;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == false,
              "CFE_ES_RunLoop",
              "Request to exit application");

    /* Test run loop with bad app ID */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    RunStatus = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_APP_RUN;
    CFE_ES_TaskRecordSetFree(UtTaskRecPtr); /* make it so task ID is bad */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == false,
              "CFE_ES_RunLoop",
              "Bad internal application ID");

    /* Test run loop with an invalid run status */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    RunStatus = 1000;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_APP_EXIT;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == false,
              "CFE_ES_RunLoop",
              "Invalid run status");

    /* Test run loop with a NULL run status */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest =
            CFE_ES_RunStatus_APP_RUN;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(NULL),
              "CFE_ES_RunLoop",
              "Nominal, NULL output pointer");

    /* Test run loop with startup sync code */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_LATE_INIT, NULL, &UtAppRecPtr, NULL);
    RunStatus = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppControlRequest =
        CFE_ES_RunStatus_APP_RUN;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == true &&
              UtAppRecPtr->AppState ==
                  CFE_ES_AppState_RUNNING,
              "CFE_ES_RunLoop",
              "Status change from initializing to run");

    /* Test successful CFE application registration */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterApp() == CFE_SUCCESS,
              "CFE_ES_RegisterApp",
              "Application registration successful");

    /* Test getting the cFE application ID by its name */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppID(&AppId) == CFE_SUCCESS,
              "CFE_ES_GetAppID",
              "Get application ID by name successful");

    /* Test getting the app name with a bad app ID */
    ES_ResetUnitTest();
    AppId = ES_UT_MakeAppIdForIndex(99999);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppName(AppName, AppId, 32) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppName",
              "Get application name by ID; bad application ID");

    /* Test getting the app name with that app ID out of range */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    AppId = ES_UT_MakeAppIdForIndex(99999);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppName(AppName,
                                AppId,
                                32) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppName",
              "Get application name by ID; ID out of range");

    /* Test successfully getting the app name using the app ID */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetAppName(AppName, AppId, 32) == CFE_SUCCESS,
              "CFE_ES_GetAppName",
              "Get application name by ID successful");

    /* Test getting task information using the task ID - NULL buffer */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(NULL, TaskId) == CFE_ES_ERR_BUFFER,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; NULL buffer");

    /* Test getting task information using the task ID - bad task ID  */
    UT_SetForceFail(UT_KEY(OS_ObjectIdToArrayIndex), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; bad task ID");

    /* Test getting task information using the task ID */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UtAppRecPtr->AppState = CFE_ES_AppState_RUNNING;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_SUCCESS,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID successful");

    /* Test getting task information using the task ID with parent inactive */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    CFE_ES_AppRecordSetFree(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_ES_ERR_APPID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; parent application not active");

    /* Test getting task information using the task ID with task inactive */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", NULL, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    CFE_ES_TaskRecordSetFree(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; task not active");

    /* Test getting task information using the task ID with invalid task ID */
    ES_ResetUnitTest();
    TaskId = CFE_ES_RESOURCEID_UNDEFINED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; invalid task ID");

    /* Test creating a child task with a bad app ID */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    StackBuf,
                                    sizeof(StackBuf),
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_APPID,
              "CFE_ES_ChildTaskCreate",
              "Bad application ID");

    /* Test creating a child task with an OS task create failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    UT_SetForceFail(UT_KEY(OS_TaskCreate), OS_ERROR);
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    StackBuf,
                                    sizeof(StackBuf),
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_CHILD_TASK_CREATE,
              "CFE_ES_ChildTaskCreate",
              "OS task create failed");

    /* Test creating a child task with a null task ID */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(NULL,
                                    "TaskName",
                                    TestAPI,
                                    StackBuf,
                                    sizeof(StackBuf),
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task ID null");

    /* Test creating a child task with a null task name */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    NULL,
                                    TestAPI,
                                    StackBuf,
                                    sizeof(StackBuf),
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task name null");

    /* Test creating a child task with a null task ID and name */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(NULL,
                                    NULL,
                                    TestAPI,
                                    StackBuf,
                                    sizeof(StackBuf),
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task name and ID null");

    /* Test creating a child task with a null function pointer */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    NULL,
                                    StackBuf,
                                    sizeof(StackBuf),
                                    2,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Function pointer null");

    /* Test creating a child task within a child task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    TestObjId = CFE_ES_ResourceID_ToOSAL(CFE_ES_TaskRecordGetID(UtTaskRecPtr));
    UT_SetForceFail(UT_KEY(OS_TaskGetId), OS_ObjectIdToInteger(TestObjId)); /* Set context to that of child */
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    StackBuf,
                                    sizeof(StackBuf),
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_ERR_CHILD_TASK_CREATE,
              "CFE_ES_CreateChildTask",
              "Cannot call from a child task");

    /* Test successfully creating a child task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    StackBuf,
                                    sizeof(StackBuf),
                                    400,
                                    0);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS, "CFE_ES_CreateChildTask",
              "Create child task successful");

    /* Test deleting a child task using a main task's ID */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", NULL, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(TaskId) ==
                  CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK,
              "CFE_ES_DeleteChildTask",
              "Task ID belongs to a main task");

    /* Test deleting a child task with an invalid task ID */
    UT_SetForceFail(UT_KEY(OS_ObjectIdToArrayIndex), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(TaskId) == CFE_ES_ERR_TASKID,
              "CFE_ES_DeleteChildTask",
              "Task ID invalid");

    /* Test successfully deleting a child task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr); /* the app ID */
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr); /* the child task ID */
    Return = CFE_ES_GetAppInfo(&AppInfo, AppId);
    UtAssert_True(Return == CFE_SUCCESS, 
         "CFE_ES_GetAppInfo() return=%x", (unsigned int)Return); 
    UtAssert_True(AppInfo.NumOfChildTasks == 1, 
         "AppInfo.NumOfChildTaskss == %u", (unsigned int)AppInfo.NumOfChildTasks); 
    Return = CFE_ES_DeleteChildTask(TaskId);
    UtAssert_True(Return == CFE_SUCCESS, 
         "DeleteChildResult() return=%x", (unsigned int)Return);
    Return = CFE_ES_GetAppInfo(&AppInfo,AppId);
    UtAssert_True(Return == CFE_SUCCESS, 
         "CFE_ES_GetAppInfo() return=%x", (unsigned int)Return); 
    UtAssert_True(AppInfo.NumOfChildTasks == 0, 
         "AppInfo.NumOfChildTaskss == %u", (unsigned int)AppInfo.NumOfChildTasks);   

    /* Test deleting a child task with an OS task delete failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr); /* the app ID */
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr); /* the child task ID */
    UT_SetForceFail(UT_KEY(OS_TaskDelete), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(TaskId) <= 0,
              "CFE_ES_DeleteChildTask",
              "OS task delete failure");

    /* Test deleting a child task with the task ID out of range */
    ES_ResetUnitTest();
    TaskId = CFE_ES_RESOURCEID_UNDEFINED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteChildTask(TaskId) == CFE_ES_ERR_TASKID,
              "CFE_ES_DeleteChildTask",
              "Task ID too large");

    /* Test successfully exiting a child task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    TestObjId = CFE_ES_ResourceID_ToOSAL(CFE_ES_TaskRecordGetID(UtTaskRecPtr));
    UT_SetForceFail(UT_KEY(OS_TaskGetId), OS_ObjectIdToInteger(TestObjId)); /* Set context to that of child */
    CFE_ES_ExitChildTask();
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(OS_TaskExit)) == 1,
              "CFE_ES_ExitChildTask",
              "Exit child task successful");

    /* Test exiting a child task within an app main task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    CFE_ES_ExitChildTask();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_CALL_APP_MAIN]) &&
                  UT_GetStubCount(UT_KEY(OS_printf)) == 1,
              "CFE_ES_ExitChildTask",
              "Cannot call from a cFE application main task");

    /* Test exiting a child task with an error retrieving the app ID */
    ES_ResetUnitTest();
    CFE_ES_ExitChildTask();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_TASKEXIT_BAD_CONTEXT]) &&
                  UT_GetStubCount(UT_KEY(OS_printf)) == 1,
              "CFE_ES_ExitChildTask",
              "Invalid context");

    /* Test registering a child task with an OS task register failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(OS_TaskRegister), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterChildTask() == CFE_ES_ERR_CHILD_TASK_REGISTER,
              "CFE_ES_RegisterChildTask",
              "OS task register failed");

    /* Test successfully registering a child task */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterChildTask() == CFE_SUCCESS,
              "CFE_ES_RegisterChildTask",
              "Register child task successful");

    /* Test successfully adding a time-stamped message to the system log that
     * must be truncated
     */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->SystemLogWriteIdx = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE - CFE_TIME_PRINTED_STRING_SIZE - 4;
    CFE_ES_ResetDataPtr->SystemLogEndIdx = CFE_ES_ResetDataPtr->SystemLogWriteIdx;
    CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LogMode_DISCARD;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_SysLogWrite_Unsync("SysLogText This message should be truncated") == CFE_ES_ERR_SYS_LOG_TRUNCATED,
              "CFE_ES_SysLogWrite_Internal",
              "Add message to log that must be truncated");

    /* Reset the system log index to prevent an overflow in later tests */
    CFE_ES_ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_ResetDataPtr->SystemLogEndIdx = 0;

    /* Test calculating a CRC on a range of memory using CRC type 8
     * NOTE: This capability is not currently implemented in cFE
     */
    memset(Data, 1, sizeof(Data));
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_MISSION_ES_CRC_8) == 0,
              "CFE_ES_CalculateCRC",
              "*Not implemented* CRC-8 algorithm");

    /* Test calculating a CRC on a range of memory using CRC type 16 */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_MISSION_ES_CRC_16) == 2688,
              "CFE_ES_CalculateCRC",
              "CRC-16 algorithm - memory read successful");

    /*
     * CRC memory read failure test case removed in #322 - 
     * deprecated CFE_PSP_MemRead8, now the FSW code does a direct read
     * which has no failure path.
     */

    /* Test calculating a CRC on a range of memory using CRC type 32
     * NOTE: This capability is not currently implemented in cFE
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_MISSION_ES_CRC_32) == 0,
              "CFE_ES_CalculateCRC",
              "*Not implemented* CRC-32 algorithm");

    /* Test calculating a CRC on a range of memory using an invalid CRC type
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
             CFE_ES_CalculateCRC(&Data, 12, 345353, -1) == 0,
              "CFE_ES_CalculateCRC",
              "Invalid CRC type");

    /* Test shared mutex take with a take error */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, -1);
    CFE_ES_LockSharedData(__func__, 12345);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MUTEX_TAKE]) && UT_GetStubCount(UT_KEY(OS_printf)) == 1,
              "CFE_ES_LockSharedData",
              "Mutex take error");

    /* Test shared mutex release with a release error */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, -1);
    CFE_ES_UnlockSharedData(__func__, 98765);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MUTEX_GIVE]) && UT_GetStubCount(UT_KEY(OS_printf)) == 1,
              "CFE_ES_UnlockSharedData",
              "Mutex release error");


    /* Test waiting for apps to initialize before continuing; transition from
     * initializing to running
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_EARLY_INIT, "UT", &UtAppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_OPERATIONAL;
    CFE_ES_WaitForStartupSync(0);
    UT_Report(__FILE__, __LINE__,
            UtAppRecPtr->AppState ==
                  CFE_ES_AppState_RUNNING,
              "CFE_ES_WaitForStartupSync",
              "Transition from initializing to running");

    /* Test waiting for apps to initialize before continuing with the semaphore
     * already released
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    CFE_ES_WaitForStartupSync(99);

    /* Note - CFE_ES_WaitForStartupSync() returns void, nothing to check for
     * here.  This is for code coverage
     */
    UT_Report(__FILE__, __LINE__,
              1,
              "CFE_ES_WaitForStartupSync",
              "System state core ready");

    /* Test waiting for apps to initialize as an external app
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_EARLY_INIT, "UT", &UtAppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    CFE_ES_WaitForStartupSync(99);

    /* Note - CFE_ES_WaitForStartupSync() returns void, nothing to check for
     * here.  This is for code coverage
     */
    UT_Report(__FILE__, __LINE__,
              1,
              "CFE_ES_WaitForStartupSync",
              "System state operational");

     /* Test adding a time-stamped message to the system log using an invalid
      * log mode
      *
      * TEST CASE REMOVED as the invalid log mode follow the same path as Discard,
      * this test case added nothing new
      */


     /* Test successfully adding a time-stamped message to the system log that
      * causes the log index to be reset
      */
     ES_ResetUnitTest();
     CFE_ES_ResetDataPtr->SystemLogWriteIdx = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE;
     CFE_ES_ResetDataPtr->SystemLogEndIdx = CFE_ES_ResetDataPtr->SystemLogWriteIdx;
     CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LogMode_DISCARD;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_WriteToSysLog("SysLogText") == CFE_ES_ERR_SYS_LOG_FULL,
               "CFE_ES_WriteToSysLog",
               "Add message to log that resets the log index");

     /* Test successfully adding a time-stamped message to the system log that
      * causes the log index to be reset
      */
     ES_ResetUnitTest();
     CFE_ES_ResetDataPtr->SystemLogWriteIdx = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE;
     CFE_ES_ResetDataPtr->SystemLogEndIdx = CFE_ES_ResetDataPtr->SystemLogWriteIdx;
     CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LogMode_OVERWRITE;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_WriteToSysLog("SysLogText") == CFE_SUCCESS &&
             CFE_ES_ResetDataPtr->SystemLogWriteIdx < CFE_PLATFORM_ES_SYSTEM_LOG_SIZE,
               "CFE_ES_WriteToSysLog",
               "Add message to log that resets the log index");

     /* Test run loop with an application error status */
     ES_ResetUnitTest();
     ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, NULL);
     RunStatus = CFE_ES_RunStatus_APP_ERROR;
     UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_ERROR;
     UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == false,
               "CFE_ES_RunLoop",
               "Application error run status");

}

void TestGenericCounterAPI(void)
{
    char CounterName[11];
    CFE_ES_ResourceID_t CounterId;
    uint32 CounterCount;
    int i;

    /* Test successfully registering a generic counter */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId, "Counter1") == CFE_SUCCESS,
              "CFE_ES_RegisterGenCounter",
              "Register counter successful");

    /* Test registering a generic counter that is already registered */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId,
                                        "Counter1") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter",
              "Attempt to register an existing counter");

    /* Test registering the maximum number of generic counters */
    for (i = 1; i < CFE_PLATFORM_ES_MAX_GEN_COUNTERS; i++)
    {
        snprintf(CounterName, sizeof(CounterName), "Counter%d", i + 1);

        if (CFE_ES_RegisterGenCounter(&CounterId, CounterName) != CFE_SUCCESS)
        {
            break;
        }
    }

    UT_Report(__FILE__, __LINE__,
              i == CFE_PLATFORM_ES_MAX_GEN_COUNTERS,
              "CFE_ES_RegisterGenCounter",
              "Register maximum number of counters");

    /* Test registering a generic counter after the maximum are registered */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId,
                                        "Counter999") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter",
              "Maximum number of counters exceeded");

    /* Test getting a registered generic counter that doesn't exist */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCounterIDByName(&CounterId,
                                           "Counter999") ==
                  CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCounterIDByName",
              "Cannot get counter that does not exist");

    /* Test successfully getting a registered generic counter ID by name */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCounterIDByName(&CounterId,
                                           "Counter5") == CFE_SUCCESS,
              "CFE_ES_GetGenCounterIDByName",
              "Get generic counter ID successful");

    /* Test deleting a registered generic counter that doesn't exist */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteGenCounter(CFE_ES_RESOURCEID_UNDEFINED) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_DeleteGenCounter",
              "Cannot delete counter that does not exist");

    /* Test successfully deleting a registered generic counter by ID */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteGenCounter(CounterId) == CFE_SUCCESS,
              "CFE_ES_DeleteGenCounter",
              "Successful");

    /* Test successfully registering a generic counter to verify a place for
     * it is now available and to provide an ID for subsequent tests
     */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId, "CounterX") == CFE_SUCCESS,
              "CFE_ES_RegisterGenCounter",
              "Register counter; back to maximum number");

    /* Test incrementing a generic counter that doesn't exist */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_IncrementGenCounter(CFE_ES_RESOURCEID_UNDEFINED)
                == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_IncrementGenCounter",
              "Bad counter ID");

    /* Test successfully incrementing a generic counter */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_IncrementGenCounter(CounterId) == CFE_SUCCESS,
              "CFE_ES_IncrementGenCounter",
              "Increment counter successful");

    /* Test getting a generic counter value for a counter that doesn't exist */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCount(CFE_ES_RESOURCEID_UNDEFINED, &CounterCount) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCount",
              "Bad counter ID");

    /* Test successfully getting a generic counter value */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetGenCount(CounterId, &CounterCount) == CFE_SUCCESS &&
              CounterCount == 1,
              "CFE_ES_GetGenCount",
              "Get counter value successful");

    /* Test setting a generic counter value for a counter that doesn't exist */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_SetGenCount(CFE_ES_RESOURCEID_UNDEFINED, 5) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_SetGenCount",
              "Bad counter ID");

    /* Test successfully setting a generic counter value */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_SetGenCount(CounterId, 5) == CFE_SUCCESS,
              "CFE_ES_SetGenCount",
              "Set counter value successful");

    /* Test value retrieved from a generic counter value */
    CFE_ES_GetGenCount(CounterId, &CounterCount);
    UT_Report(__FILE__, __LINE__,
              (CounterCount == 5), "CFE_ES_SetGenCount",
              "Check value for counter set");

    /* Test registering a generic counter with a null counter ID pointer */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
             CFE_ES_RegisterGenCounter(NULL,
                                        "Counter1") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter",
              "Attempt to register using a null counter ID pointer");

    /* Test registering a generic counter with a null counter name */
    ES_ResetUnitTest();

    UT_Report(__FILE__, __LINE__,
             CFE_ES_RegisterGenCounter(&CounterId,
                                        NULL) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter",
              "Attempt to register using a null counter name");

    /* Test incrementing a generic counter where the record is not in use */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
             CFE_ES_IncrementGenCounter(CounterId) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_IncrementGenCounter",
              "Record not in use");

    /* Test setting a generic counter where the record is not in use */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
             CFE_ES_SetGenCount(CounterId, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_SetGenCount",
              "Record not in use");

    /* Test getting a generic counter where the record is not in use */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
             CFE_ES_GetGenCount(CounterId, &CounterCount)
                == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCount",
              "Record not in use");

    /* Test getting a generic counter where the count is null */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
             CFE_ES_GetGenCount(CounterId, NULL)
                == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCount",
              "Null count");

    /* Test getting a registered generic counter ID using a null counter
     * pointer
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
             CFE_ES_GetGenCounterIDByName(NULL,
                                           "CounterX") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCounterIDByName",
              "Null name");
}

void TestCDS()
{
    uint32 CdsSize;
    uint8 *CdsPtr;
    char CDSName[CFE_ES_CDS_MAX_FULL_NAME_LEN + 4];
    CFE_ES_CDSHandle_t CDSHandle;
    uint32 i;
    uint32 TempSize;

    UtPrintf("Begin Test CDS");

    /* Set up the PSP stubs for CDS testing */
    UT_SetCDSSize(128 * 1024);

    /* Test CDS registering with a write CDS failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetCDSSize(50000);
    CFE_ES_InitializeCDS(50000);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "Name3") == OS_ERROR,
              "CFE_ES_RegisterCDS",
              "Writing to BSP CDS failure");

    /* Test successful CDS registering */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetCDSSize(50000);
    CFE_ES_InitializeCDS(50000);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Register CDS successful");

    /* Test CDS registering using an already registered name */
    /* No reset here -- just attempt to register the same name again */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name") == CFE_ES_CDS_ALREADY_EXISTS,
              "CFE_ES_RegisterCDS",
              "Retrieve existing CDS");

    /* Test CDS registering using the same name, but a different size */
    /* No reset here -- just attempt to register the same name again */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 6, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Get CDS of same name, but new size");

    /* Test CDS registering using a null name */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "") == CFE_ES_CDS_INVALID_NAME,
              "CFE_ES_RegisterCDS",
              "Invalid name size");

    /* Test CDS registering with a block size of zero */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                 0,
                                 "Name") == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_RegisterCDS",
              "Block size zero");

    /* Test CDS registering with no memory pool available */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name") == CFE_ES_NOT_IMPLEMENTED,
              "CFE_ES_RegisterCDS",
              "No memory pool available");

    /* Test CDS registering with all the CDS registries taken */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetCDSSize(50000);
    CFE_ES_InitializeCDS(50000);

    /* Set all the CDS registries to 'taken' */
    for (i = 0; i < CFE_ES_Global.CDSVars.MaxNumRegEntries; i++)
    {
        CFE_ES_Global.CDSVars.Registry[i].Taken = true;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name2") == CFE_ES_CDS_REGISTRY_FULL,
              "CFE_ES_RegisterCDS",
              "No available entries");

    /* Test CDS registering using a bad app ID */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "Name2") == CFE_ES_ERR_APPID,
              "CFE_ES_RegisterCDS",
              "Bad application ID");

    /* Register CDS to set up for the copy test */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetCDSSize(50000);
    CFE_ES_InitializeCDS(50000);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Register CDS successful (set up for copy test)");

    /* Test successfully copying to CDS */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_SUCCESS);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CopyToCDS(CDSHandle, &TempSize) == CFE_SUCCESS,
              "CFE_ES_CopyToCDS",
              "Copy to CDS successful");

    /* Test successfully restoring from a CDS */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RestoreFromCDS(&TempSize, CDSHandle) == CFE_SUCCESS,
              "CFE_ES_RestoreFromCDS",
              "Restore from CDS successful");

    /* Test CDS registering using a name longer than the maximum allowed */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetCDSSize(50000);
    CFE_ES_InitializeCDS(50000);

    for (i = 0; i < CFE_MISSION_ES_CDS_MAX_NAME_LENGTH + 1; i++)
    {
        CDSName[i] = 'a';
    }

    CDSName[i] = '\0';

    UT_Report(__FILE__, __LINE__,
             CFE_ES_RegisterCDS(&CDSHandle, 4, CDSName)
                == CFE_ES_CDS_INVALID_NAME,
              "CFE_ES_RegisterCDS",
              "Invalid name size");

    /* Test unsuccessful CDS registering */
    UT_Report(__FILE__, __LINE__,
             CFE_ES_RegisterCDS(&CDSHandle,
                                 0xffffffff,
                                 "Name") == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_RegisterCDS",
              "Register CDS unsuccessful");

    /* Test memory pool rebuild and registry recovery with an
     * unreadable registry
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "First read from CDS bad");

    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "Second read from CDS bad");

    /* Test CDS registry initialization with a CDS write failure */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitCDSRegistry() == -1,
              "CFE_ES_InitCDSRegistry",
              "Failed to write registry size");

    /* Test successful CDS initialization */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == CFE_SUCCESS,
              "CFE_ES_CDS_EarlyInit",
              "Initialization successful");

    /* Test CDS initialization with a read error */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == -1,
              "CFE_ES_CDS_EarlyInit",
              "Unrecoverable read error");

    /* Test CDS initialization with size below the minimum */
    ES_ResetUnitTest();
    UT_SetCDSSize(1024);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == OS_SUCCESS &&
              UT_GetStubCount(UT_KEY(CFE_PSP_GetCDSSize)) == 1,
              "CFE_ES_CDS_EarlyInit",
              "CDS size less than minimum");

    /* Test CDS initialization with size not obtainable */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_GetCDSSize), -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == OS_ERROR,
              "CFE_ES_CDS_EarlyInit",
              "Unable to obtain CDS size");

    /* Reset back to a sufficient CDS size */
    UT_SetCDSSize(128 * 1024);

    /* Test CDS initialization with rebuilding not possible */
    ES_ResetUnitTest();
    UT_GetDataBuffer(UT_KEY(CFE_PSP_ReadFromCDS), (void**)&CdsPtr, &CdsSize, NULL);
    memcpy(CdsPtr, "_CDSBeg_", 8);
    memcpy(CdsPtr + CdsSize - 8, "_CDSEnd_", 8);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 3, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == OS_SUCCESS,
              "CFE_ES_CDS_EarlyInit",
              "Rebuilding not possible; create new CDS");

    /* Test CDS validation with second CDS read call failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateCDS() == OS_ERROR,
              "CFE_ES_ValidateCDS",
              "CDS read (second call) failed");

    /* Test CDS validation with CDS read end check failure */
    ES_ResetUnitTest();
    UT_GetDataBuffer(UT_KEY(CFE_PSP_ReadFromCDS), (void**)&CdsPtr, &CdsSize, NULL);
    CFE_ES_Global.CDSVars.CDSSize = CdsSize;
    memcpy(CdsPtr + CdsSize - 8, "gibberish", 8);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_ValidateCDS",
              "Reading from CDS failed end check");

    /* Test CDS validation with first CDS read call failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateCDS() == -1,
              "CFE_ES_ValidateCDS",
              "CDS read (first call) failed");

    /* Test CDS initialization where first write call to the CDS fails */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(128 * 1024) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "Clear CDS failed");

    /* Test CDS initialization where second write call to the CDS fails */
    /* Note - the PSP is zeroed by writing 4x uint32 values.  The 2nd
     * call in the source code will be the 5th call to the PSP function. */
    ES_ResetUnitTest();
    UT_SetCDSSize(16);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 5, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(16) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "CDS write (second call) failed");

    /* Test CDS initialization where third write call to the CDS fails */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 6, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(16) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "CDS write (third call) failed");

    /* Test rebuilding the CDS where the registry is too large */
    ES_ResetUnitTest();
    UT_SetCDSSize(0);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES + 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "Registry too large to recover");

    /* Test successfully rebuilding the CDS */
    ES_ResetUnitTest();
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_SUCCESS,
              "CFE_ES_RebuildCDS",
              "CDS rebuild successful");

    /* Test rebuilding the CDS with the registry unreadable */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "CDS registry unreadable");

    /* Test deleting the CDS from the registry with a registry write failure */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    ES_UT_SetupSingleCDSRegistry("NO_APP.CDS_NAME", true, NULL);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteCDS("NO_APP.CDS_NAME", true) == -1,
              "CFE_ES_DeleteCDS",
              "CDS registry write failed");

    /* Test deleting the CDS from the registry with the owner application
     * still active
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", true, NULL);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteCDS("CFE_ES.CDS_NAME", true) ==
                  CFE_ES_CDS_OWNER_ACTIVE_ERR,
              "CFE_ES_DeleteCDS",
              "Owner application still active");

    /* Test CDS initialization where rebuilding the CDS is successful */
    ES_ResetUnitTest();
    CdsSize = 128 * 1024;
    CdsPtr = UT_SetCDSSize(128 * 1024);
    memcpy(CdsPtr, "_CDSBeg_", 8);
    memcpy(CdsPtr + CdsSize - 8, "_CDSEnd_", 8);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == CFE_SUCCESS,
              "CFE_ES_CDS_EarlyInit",
              "Initialization with successful rebuild");

    /* Test CDS initialization where rebuilding the CDS is unsuccessful */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 3, OS_ERROR);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == CFE_SUCCESS,
              "CFE_ES_CDS_EarlyInit",
              "Initialization with unsuccessful rebuild");

    /* Test CDS initialization where write call to the CDS fails while filling
     * in extra uint32 space
     */
    ES_ResetUnitTest();
    UT_SetCDSSize(0);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 5, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(32 * 4 - 4) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "CDS write failed while filling extra space");

    /* Test CDS initialization where creating the CDS pool fails */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_InitializeCDS(sizeof(CFE_ES_Global.CDSVars.ValidityField)) ==
                                     CFE_ES_BAD_ARGUMENT,
              "CFE_ES_InitializeCDS",
              "CDS pool create failed");

    /* Test deleting the CDS from the registry with a CDS name longer than the
     * maximum allowed
     */
    ES_ResetUnitTest();
    memset(CDSName, 'a', CFE_ES_CDS_MAX_FULL_NAME_LEN - 1);
    CDSName[CFE_ES_CDS_MAX_FULL_NAME_LEN - 1] = '\0';
    ES_UT_SetupSingleCDSRegistry(CDSName, true, NULL);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_DeleteCDS(CDSName,
                               true) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_DeleteCDS",
              "CDS name too long");
} /* End TestCDS */

void TestCDSMempool(void)
{
    uint32                  MinCDSSize = CFE_ES_CDS_MIN_BLOCK_SIZE +
                                         sizeof(CFE_ES_CDSBlockDesc_t);
    CFE_ES_CDSBlockHandle_t BlockHandle;
    int                     Data;
    uint32                  i;

    extern uint32 CFE_ES_CDSMemPoolDefSize[];

    UtPrintf("Begin Test CDS memory pool");

    UT_SetCDSSize(0);

    /* Set up the CDS block to read in the following tests */
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    CFE_ES_CDSBlockDesc.SizeUsed =  512;

    /* Test creating the CDS pool with the pool size too small */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CreateCDSPool(0, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_CreateCDSPool",
              "CDS pool size too small");

    /* Test rebuilding the CDS pool with the pool size too small */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(0, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RebuildCDSPool",
              "CDS pool size too small");

    /* Test rebuilding the CDS pool with the CDS block unused */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, OS_ERROR);
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_SUCCESS,
              "CFE_ES_RebuildCDSPool",
              "CDS block unused");

    /* Test rebuilding the CDS pool with a CDS read failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_ReadFromCDS), -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Error reading CDS");

    /* Test rebuilding the CDS pool with a CDS write failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, OS_ERROR);
    UT_SetForceFail(UT_KEY(CFE_PSP_WriteToCDS), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Error writing CDS");

    /* Test rebuilding the CDS pool with a block not previously used */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = 1;

    /* Set flags so as to fail on second CDS read */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_SUCCESS);
    UT_SetForceFail(UT_KEY(CFE_PSP_ReadFromCDS), -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 1) == OS_SUCCESS,
              "CFE_ES_RebuildCDSPool",
              "CDS block not used before");
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;

    /* Test rebuilding the CDS pool with an invalid block descriptor */
    ES_ResetUnitTest();
    CFE_ES_CDSMemPoolDefSize[0] = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Invalid block descriptor");
    CFE_ES_CDSMemPoolDefSize[0] = CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE;

    /* Test successfully creating a pool where the offset = 0 */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CreateCDSPool(1000000, 0) == CFE_SUCCESS,
              "CFE_ES_CreateCDSPool",
              "Create with zero offset; successful");

    /* Test allocating a CDS block with a block size error */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle,
                                 800) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetCDSBlock",
              "Block size error");

    /* Test returning a CDS block to the memory pool using an invalid
         block descriptor */
    ES_ResetUnitTest();
    BlockHandle = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid block descriptor");

    /* Test creating a new pool and set up for getting a pre-made block */
    ES_ResetUnitTest();
    BlockHandle = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CreateCDSPool(1000000, 8) == CFE_SUCCESS,
              "CFE_ES_CreateCDSPool",
              "Create with non-zero offset; successful");

    /* Test successfully allocating a pre-made CDS block */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle, 800) == OS_SUCCESS,
              "CFE_ES_GetCDSBlock",
              "Get a CDS block; successful");

    /* Test successfully returning a CDS block back to the memory pool */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == OS_SUCCESS,
              "CFE_ES_PutCDSBlock",
              "Return a CDS block; successful");

    /* Test allocating a CDS block with a CDS read failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle, 800) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_GetCDSBlock",
              "Error reading CDS");

    /* Test allocating a CDS block with a CDS write failure */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_WriteToCDS), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle, 800) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_GetCDSBlock",
              "Error writing CDS");

    /* Test allocating a CDS block using a block size that's too large */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle,
                                 CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE + 1) ==
                  CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetCDSBlock",
              "Block size too large");

    /* Test returning a CDS block to the memory pool with an
     * invalid CDS handle
     */
    ES_ResetUnitTest();
    BlockHandle = sizeof(CFE_ES_Global.CDSVars.ValidityField) - 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid CDS handle");

    /* Test returning a CDS block to the memory pool with a CDS read error */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_ReadFromCDS), -1);
    BlockHandle = 10;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_PutCDSBlock",
              "Error reading CDS");

    /* Test returning a CDS block to the memory pool with an invalid
     * block descriptor
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid block descriptor");

    /* Test returning a CDS block to the memory pool with the block size
     * too large
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid memory handle");

    /* Test returning a CDS block to the memory pool with a CDS write error */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.ActualSize  = 452;
    UT_SetForceFail(UT_KEY(CFE_PSP_WriteToCDS), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_PutCDSBlock",
              "Error writing CDS");

    /* Test CDS block write using an invalid memory handle */
    ES_ResetUnitTest();
    BlockHandle = 7;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Invalid memory handle");
    BlockHandle = 10;

    /* Test CDS block write with the block size too large */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Actual size too large");
    CFE_ES_CDSBlockDesc.ActualSize  = 452;

    /* Test CDS block write using an invalid (unused) block */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Invalid CDS block");
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;

    /* Test CDS block write with a CDS write error (block descriptor) */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error writing block descriptor to CDS");

    /* Test CDS block write with a CDS write error (new data) */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error writing new data to CDS");

    /* Test CDS block write with a CDS read error */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_ReadFromCDS), -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error reading CDS");

    /* Test CDS block read with an invalid memory handle */
    ES_ResetUnitTest();
    BlockHandle = 7;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Invalid memory handle");
    BlockHandle = 10;

    /* Test CDS block read with the block size too large */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Actual size too large");
    CFE_ES_CDSBlockDesc.ActualSize = 452;

    /* Test CDS block read using an invalid (unused) block */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Invalid CDS block");
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;

    /* Test CDS block read with a CRC mismatch */
    ES_ResetUnitTest();
    Data = CFE_ES_CDSBlockDesc.CRC;
    CFE_ES_CDSBlockDesc.CRC = 56456464;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data,
                                  BlockHandle) == CFE_ES_CDS_BLOCK_CRC_ERR,
              "CFE_ES_CDSBlockRead",
              "CRC doesn't match");
    CFE_ES_CDSBlockDesc.CRC = Data;

    /* Test CDS block read with a CDS read error (block descriptor) */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_ReadFromCDS), -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == OS_ERROR,
              "CFE_ES_CDSBlockRead",
              "Error reading block descriptor from CDS");

    /* Test CDS block read with a CDS read error (block data) */
    ES_ResetUnitTest();
    UT_SetForceFail(UT_KEY(CFE_PSP_ReadFromCDS), -1);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_SUCCESS);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) == OS_ERROR,
              "CFE_ES_CDSBlockRead",
              "Error reading block data from CDS");

    /* Test allocating a CDS block with a block size error (path 2)*/
    ES_ResetUnitTest();
    CFE_ES_CDSMemPool.Current = CFE_ES_CDSMemPool.End;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetCDSBlock(&BlockHandle,
                                 800) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetCDSBlock",
              "Block size error (second path)");

    /* Test rebuilding the CDS pool with an invalid block descriptor */
    ES_ResetUnitTest();
    CFE_ES_CDSMemPoolDefSize[0] = 0;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RebuildCDSPool(MinCDSSize, 0) == OS_SUCCESS,
              "CFE_ES_RebuildCDSPool",
              "Invalid block descriptor");

    /* Test returning a CDS block to the memory pool with an
     * invalid check bit pattern
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = 0x1111;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid check bit pattern");

    /* Test returning a CDS block to the memory pool with an
     * invalid check bit pattern
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = 0x1111;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Invalid check bit pattern");

    /* Test returning a CDS block to the memory pool with an
     * invalid check bit pattern
     */
    ES_ResetUnitTest();
    CFE_ES_CDSBlockDesc.CheckBits = 0x1111;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Invalid check bit pattern");

    /* Test returning a CDS block to the memory pool with an
     * invalid CDS handle (path 2)
     */
    ES_ResetUnitTest();
    BlockHandle = CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) -
            CFE_ES_CDSMemPool.MinBlockSize -
            sizeof(CFE_ES_Global.CDSVars.ValidityField) + 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid CDS handle (second path)");

    /* Test CDS block write with a CDS read error (path 2) */
    ES_ResetUnitTest();
    BlockHandle = CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) -
            CFE_ES_CDSMemPool.MinBlockSize -
            sizeof(CFE_ES_Global.CDSVars.ValidityField) + 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockWrite(BlockHandle, &Data) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Error writing CDS (second path)");

    /* Test CDS block read with a CDS read error (path 2) */
    ES_ResetUnitTest();
    BlockHandle = CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) -
            CFE_ES_CDSMemPool.MinBlockSize -
            sizeof(CFE_ES_Global.CDSVars.ValidityField) + 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSBlockRead(&Data, BlockHandle) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Error reading CDS (second path)");

    /* Test CDS minimum memory pool size with no non-zero blocks defined */
    ES_ResetUnitTest();

    for (i = 0; i < CFE_ES_CDS_NUM_BLOCK_SIZES; i++)
    {
        CFE_ES_CDSMemPoolDefSize[i] = 0;
    }

    CFE_ES_CDSMemPool.MinBlockSize = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDSReqdMinSize(1) == sizeof(CFE_ES_CDSBlockDesc_t),
              "CFE_ES_CDSReqdMinSize",
              "No non-zero blocks");
}

void TestESMempool(void)
{
    CFE_ES_MemHandle_t    HandlePtr;
    uint8                 Buffer[CFE_PLATFORM_ES_MAX_BLOCK_SIZE];
    uint32                *address = NULL;
    uint32                *address2 = NULL;
    Pool_t                *PoolPtr;
    CFE_ES_MemPoolStats_t Stats;
    uint32                BlockSizes[4];
    BD_t                  *BdPtr;
    CFE_ES_MemHandle_t    HandlePtr2;
    CFE_ES_MemHandle_t    HandlePtrSave;
    uint32                i;

    UtPrintf("Begin Test ES memory pool");

    /* Test creating memory pool without using a mutex with the pool size
      * too small
      */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateNoSem(&HandlePtr,
                                     Buffer,
                                     0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateNoSem",
              "Pool size too small");

    /* Test successfully creating memory pool without using a mutex */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateNoSem(&HandlePtr,
                                     Buffer,
                                     CFE_PLATFORM_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreateNoSem",
              "Memory pool create; successful");

    /* Test creating memory pool using a mutex with the pool size too small */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&HandlePtr, Buffer, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreate",
              "Pool size too small");

    /* Test successfully creating memory pool using a mutex */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_PLATFORM_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [1]; successful");

    /* Test successfully allocating a pool buffer */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [1]; successful");

    /* Test successfully getting the size of an existing pool buffer */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, address) > 0,
              "CFE_ES_GetPoolBufInfo",
              "Get pool buffer size; successful");

    /* Test successfully getting the size of an existing pool buffer.  Use no
     * mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, address) > 0,
              "CFE_ES_GetPoolBufInfo",
              "Get pool buffer size; successful (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test successfully returning a pool buffer to the memory pool */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, address) > 0,
              "CFE_ES_PutPoolBuf",
              "Return buffer to the memory pool; successful");

    /* Test successfully allocating an additional pool buffer */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf(&address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [2]; successful");

    /* Test successfully returning a pool buffer to the second memory pool.
     * Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, address) > 0,
              "CFE_ES_PutPoolBuf",
              "Return buffer to the second memory pool; successful");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test handle validation using a handle with an invalid memory address */
    ES_ResetUnitTest();
    PoolPtr = (Pool_t *) &HandlePtr2;
    PoolPtr->PoolHandle = (cpuaddr)&HandlePtrSave;
    PoolPtr->Size = 64;
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateHandle(HandlePtr2) == false,
              "CFE_ES_ValidateHandle",
              "Invalid handle; bad memory address");

    /* Test handle validation using a handle where the first pool structure
     * field is not the pool start address
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateHandle(HandlePtr2) == false,
              "CFE_ES_ValidateHandle",
              "Invalid handle; not pool start address");

    /* Test allocating a pool buffer where the memory handle is not the pool
     * start address
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf(&address,
                                HandlePtr2,
                                256) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBuf",
              "Invalid handle; not pool start address");

    /* Test getting memory pool statistics where the memory handle is not
     * the pool start address
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetMemPoolStats(&Stats,
                                     HandlePtr2) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetMemPoolStats",
              "Invalid handle; not pool start address");

    /* Test allocating a pool buffer where the memory block doesn't fit within
     * the remaining memory
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf(&address,
                                HandlePtr,
                                75000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Requested pool size too large");

    /* Test getting the size of an existing pool buffer using an
     * invalid handle
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, address) ==
                  CFE_ES_BUFFER_NOT_IN_POOL,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle");

    /* Test returning a pool buffer using an invalid memory block */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid memory block");

    /* Test initializing a pre-allocated pool specifying a number of block
     * sizes greater than the maximum
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  Buffer,
                                  sizeof(Buffer),
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES + 2,
                                  BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Number of block sizes exceeds maximum");

    /* Test initializing a pre-allocated pool specifying a pool size that
     * is too small and using the default block size
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  Buffer,
                                  sizeof(Pool_t) / 2,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES - 2,
                                  NULL,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool size too small (default block size)");


    /* 
     * Test to use default block sizes if none are given
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  Buffer,
                                  sizeof(Buffer),
                                  0,
                                  NULL,
                                  CFE_ES_USE_MUTEX) == CFE_SUCCESS,
              "CFE_ES_PoolCreateEx",
              "Use default block sizes when none are given");

    /* Test initializing a pre-allocated pool using an invalid mutex option */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  Buffer,
                                  sizeof(Buffer),
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES - 2,
                                  BlockSizes,
                                  2) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Invalid mutex option");

    /* Test initializing a pre-allocated pool specifying a pool size that
     * is too small and specifying the block size with one block size set
     * to zero
     */
    ES_ResetUnitTest();
    BlockSizes[0] = 10;
    BlockSizes[1] = 50;
    BlockSizes[2] = 100;
    BlockSizes[3] = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                 Buffer,
                                 sizeof(Pool_t) + sizeof(BD_t),
                                 4,
                                 BlockSizes,
                                 CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool size too small (block size specified)");

    ES_ResetUnitTest();
    BlockSizes[0] = 10;
    BlockSizes[1] = 50;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                 Buffer,
                                 sizeof(Pool_t) + sizeof(BD_t) + sizeof(CFE_ES_STATIC_POOL_TYPE(CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN)),
                                 2,
                                 BlockSizes,
                                 CFE_ES_USE_MUTEX) == CFE_SUCCESS,
              "CFE_ES_PoolCreateEx",
              "Make space for new size");
    
    /* Test successfully creating memory pool using a mutex for
     * subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_PLATFORM_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [2]; successful");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [3]; successful");

    /* Test getting the size of an existing pool buffer using an
     * unallocated block
     */
    ES_ResetUnitTest();
    BdPtr = ((BD_t *)address) - 1;
    BdPtr->Allocated = 717;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr,
                                    address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; unallocated block");

    /* Test getting the size of an existing pool buffer using an
     * unallocated block.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    BdPtr = ((BD_t *)address) - 1;
    BdPtr->Allocated = 717;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; unallocated block (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test returning a pool buffer using an unallocated block */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Deallocate an unallocated block");

    /* Test returning a pool buffer using an unallocated block.  Use no mutex
     * in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Deallocate an unallocated block (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test getting the size of an existing pool buffer using an
     * invalid check bit pattern
     */
    ES_ResetUnitTest();
    BdPtr->Allocated = 0xaaaa;
    BdPtr->CheckBits = 717;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; check bit pattern");

    /* Test getting the size of an existing pool buffer using an
     * invalid check bit pattern.  Use no mutex in order to get branch path
     * coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    BdPtr->Allocated = 0xaaaa;
    BdPtr->CheckBits = 717;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr, address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; check bit pattern (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test returning a pool buffer using an invalid or corrupted
     * memory descriptor
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid/corrupted memory descriptor");

    /* Test returning a pool buffer using an invalid or corrupted
     * memory descriptor.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid/corrupted memory descriptor (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test successfully creating memory pool using a mutex for
     * subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_PLATFORM_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [3]; successful");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf(&address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [4]; successful");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test returning a pool buffer using a buffer size larger than
     * the maximum
     */
    ES_ResetUnitTest();
    BdPtr->CheckBits = 0x5a5a;
    BdPtr->Size =CFE_PLATFORM_ES_MAX_BLOCK_SIZE +1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Pool buffer size exceeds maximum");

    /* Test returning a pool buffer using a buffer size larger than
     * the maximum.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    BdPtr->CheckBits = 0x5a5a;
    BdPtr->Size =CFE_PLATFORM_ES_MAX_BLOCK_SIZE +1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr, address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Pool buffer size exceeds maximum (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /* Test allocating an additional pool buffer using a buffer size larger
     * than the maximum
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf(&address2,
                                HandlePtr,
                                99000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Pool buffer size exceeds maximum");

    /* Test handle validation using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ValidateHandle(0) == false,
              "CFE_ES_ValidateHandle",
              "NULL handle");

    /* Test returning a pool buffer using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(0, address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "NULL memory handle");

    /* Test allocating a pool buffer using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf(&address,
                                0,
                                256) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBuf",
              "NULL memory handle");

    /* Test getting the size of an existing pool buffer using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(0, address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "NULL memory handle");

    /* Test initializing a pre-allocated pool specifying a small block size */
    ES_ResetUnitTest();
    BlockSizes[0] = 16;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&HandlePtr,
                                  Buffer,
                                  sizeof(Pool_t) + sizeof(BD_t) + 16 +
                                  sizeof(CFE_ES_STATIC_POOL_TYPE(CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN)),
                                  1,
                                  BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_SUCCESS,
              "CFE_ES_PoolCreateEx",
              "Allocate small memory pool");

    /* Test allocating an additional pool buffer using a buffer size larger
     * than the maximum.  Use no mutex in order to get branch path coverage
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf(&address2,
                                HandlePtr,
                                32) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Pool buffer size exceeds maximum (no mutex)");
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    /*
     * Test allocating a pool buffer where the memory block doesn't fit within
     * the remaining memory.  Use no mutex in order to get branch path coverage
     *
     * NOTE: Theoretically with a 128 byte pool this should fail after ~4 allocations.
     * (16 byte block plus 12 byte BD = 28 bytes each)
     *
     * However due to alignment requirements of the local CPU padding might be added
     * and the sizeof(BD_t) might be bigger too, resulting in fewer allocations.
     *
     * There should always be at least 1 successful allocation, but the number of
     * successful ones is dependent on the CPU architecture and the setting of
     * CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN.  Expect a failure within 20 allocations.
     */
    ES_ResetUnitTest();
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_NO_MUTEX;
    for (i=0; i < 25; ++i)
    {
        if (CFE_ES_GetPoolBuf(&address, HandlePtr,
                          12) == CFE_ES_ERR_MEM_BLOCK_SIZE)
        {
            break;
        }
    }
    ((Pool_t *) HandlePtr)->UseMutex = CFE_ES_USE_MUTEX;

    UT_Report(__FILE__, __LINE__,
              i >= 1 && i <= 20,
              "CFE_ES_GetPoolBuf",
              "Pool fully allocated (no mutex)");

    /* Test getting the size of a pool buffer that is not in the pool */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(HandlePtr,
                                    (uint32 *) HandlePtr + 1) ==
                                        CFE_ES_BUFFER_NOT_IN_POOL,
              "CFE_ES_GetPoolBufInfo",
              "Invalid pool buffer");

    /* Test getting the size of a pool buffer with an invalid memory handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) HandlePtr + 1) ==
                                        CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid memory handle");
}

/* Tests to fill gaps in coverage in SysLog */
void TestSysLog(void)
{
    CFE_ES_SysLogReadBuffer_t SysLogBuffer;
    char                      LogString[(CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2) + 2];

    char          TmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE + 1];

    UtPrintf("Begin Test Sys Log");

    /* Test loop in CFE_ES_SysLogReadStart_Unsync that ensures
     * reading at the start of a message */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_ResetDataPtr->SystemLogEndIdx = sizeof(CFE_ES_ResetDataPtr->SystemLog) - 1;
    
    memset(CFE_ES_ResetDataPtr->SystemLog, 'a', CFE_ES_ResetDataPtr->SystemLogEndIdx);
    CFE_ES_ResetDataPtr->SystemLog[CFE_ES_ResetDataPtr->SystemLogEndIdx-1] = '\n';

    CFE_ES_SysLogReadStart_Unsync(&SysLogBuffer);

    UT_Report(__FILE__, __LINE__,
              SysLogBuffer.EndIdx == sizeof(CFE_ES_ResetDataPtr->SystemLog) - 1 &&
              SysLogBuffer.LastOffset == sizeof(CFE_ES_ResetDataPtr->SystemLog) - 1 &&
              SysLogBuffer.BlockSize == 0 &&
              SysLogBuffer.SizeLeft == 0,
              "CFE_ES_SysLogReadStart_Unsync(SysLogBuffer)",
              "ResetDataPtr pointing to an old fragment of a message");
    
    /* Test truncation of a sys log message that is over half
     * the size of the total log */
    ES_ResetUnitTest();
    memset(LogString, 'a', (CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2) + 1);
    LogString[(CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2) + 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_ES_SysLogAppend_Unsync(LogString) == CFE_ES_ERR_SYS_LOG_TRUNCATED,
              "CFE_ES_SysLogAppend_Unsync",
              "Truncated sys log message");
    
    /* Test code that skips writing an empty string to the sys log */
    ES_ResetUnitTest();
    memset(LogString, 'a', (CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2) + 1);
    LogString[0] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_ES_SysLogAppend_Unsync(LogString) == CFE_SUCCESS,
              "CFE_ES_SysLogAppend_Unsync",
              "Don't log an empty string");
    
    /* Test Reading space between the current read offset and end of the log buffer */
    ES_ResetUnitTest();
    SysLogBuffer.EndIdx = 3;
    SysLogBuffer.LastOffset = 0;
    SysLogBuffer.BlockSize = 3;
    SysLogBuffer.SizeLeft = 1;
    
    CFE_ES_SysLogReadData(&SysLogBuffer);
    
    UT_Report(__FILE__, __LINE__,
              SysLogBuffer.EndIdx == 3 &&
              SysLogBuffer.LastOffset == 1 &&
              SysLogBuffer.BlockSize == 1 &&
              SysLogBuffer.SizeLeft == 0,
              "CFE_ES_SysLogReadData",
              "Read space between current offset and end of log buffer");
   
    /* Test nominal flow through CFE_ES_SysLogDump
     * with multiple reads and writes  */
    ES_ResetUnitTest();
    CFE_ES_ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_ResetDataPtr->SystemLogEndIdx = sizeof(CFE_ES_ResetDataPtr->SystemLog) - 1;
    
    CFE_ES_SysLogDump("fakefilename");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_ES_SysLogDump",
              "Multiple reads and writes to sys log");
   
    /* Test "message got truncated"  */
    ES_ResetUnitTest();
    memset(TmpString, 'a', CFE_ES_MAX_SYSLOG_MSG_SIZE);
    TmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE] = '\0';
    
    CFE_ES_WriteToSysLog("%s",TmpString);
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_ES_WriteToSysLog",
              "Truncate message");
    
}

void TestBackground(void)
{
    int32 status;

    /* CFE_ES_BackgroundInit() with default setup
     * causes  CFE_ES_CreateChildTask to fail.
     */
    ES_ResetUnitTest();
    status = CFE_ES_BackgroundInit();
    UtAssert_True(status == CFE_ES_ERR_APPID, "CFE_ES_BackgroundInit - CFE_ES_CreateChildTask failure (%08x)", (unsigned int)status);

    /* The CFE_ES_BackgroundCleanup() function has no conditionals -
     * it just needs to be executed as part of this routine,
     * and confirm that it deleted the semaphore.
     */
    ES_ResetUnitTest();
    OS_BinSemCreate(&CFE_ES_Global.BackgroundTask.WorkSem, "UT", 0, 0);
    CFE_ES_BackgroundCleanup();
    UtAssert_True(UT_GetStubCount(UT_KEY(OS_BinSemDelete)) == 1, "CFE_ES_BackgroundCleanup - OS_BinSemDelete called");

    /*
     * Test background task loop function
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskRegister), 1, -1);
    CFE_ES_BackgroundTask();
    /* this has no return value, but this can ensure that a syslog/printf was generated */
    UtAssert_True(UT_GetStubCount(UT_KEY(OS_printf)) == 1, "CFE_ES_BackgroundTask - CFE_ES_RegisterChildTask failure");

    /*
     * When testing the background task loop, it is normally an infinite loop,
     * so this is needed to set a condition for the loop to exit.
     *
     * This also sets a state so the background perf log dump will be "Active" to
     * execute the code which counts the number of active jobs.
     */
    ES_ResetUnitTest();
    memset(&CFE_ES_TaskData.BackgroundPerfDumpState, 0,
            sizeof(CFE_ES_TaskData.BackgroundPerfDumpState));
    UT_SetForceFail(UT_KEY(OS_write), -10);
    CFE_ES_TaskData.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_INIT;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTimedWait), 3, -4);
    CFE_ES_BackgroundTask();
    /* this has no return value, but this can ensure that a syslog/printf was generated */
    UtAssert_True(UT_GetStubCount(UT_KEY(OS_printf)) == 1, "CFE_ES_BackgroundTask - Nominal");
    /* The number of jobs running should be 1 (perf log dump) */
    UtAssert_True(CFE_ES_Global.BackgroundTask.NumJobsRunning == 1,
            "CFE_ES_BackgroundTask - Nominal, CFE_ES_Global.BackgroundTask.NumJobsRunning (%u) == 1",
            (unsigned int)CFE_ES_Global.BackgroundTask.NumJobsRunning);
}
