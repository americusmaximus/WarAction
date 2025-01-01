/*
Copyright (c) 2024 - 2025 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Objects.hxx"

#include <Control.hxx>

typedef enum ControlType
{
    CONTROLTYPE_CONTROL             = 0,
    CONTROLTYPE_BUTTON              = 1,
    CONTROLTYPE_TOGGLE              = 2,
    CONTROLTYPE_IMAGE               = 3,
    CONTROLTYPE_PANEL               = 4,
    CONTROLTYPE_SCROLL              = 6,
    CONTROLTYPE_LIST                = 7,
    CONTROLTYPE_LOAD_SAVE           = 8,
    CONTROLTYPE_DIAL                = 9,
    CONTROLTYPE_INPUT               = 10,
    CONTROLTYPE_MULTI_SELECT        = 11,
    CONTROLTYPE_MULTI_JOIN          = 12,
    CONTROLTYPE_MAIN                = 13,
    CONTROLTYPE_MULTI_START         = 14,
    CONTROLTYPE_MULTI_JOIN_RESULT   = 15,
    CONTROLTYPE_MESSAGE             = 17,
    BASECONTROLTYPE_FORCE_DWORD     = 0x7FFFFFF
} CONTROLTYPE, * CONTROLTYPEPTR;

struct Control;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPEACTION)(Control* self);
typedef VOID(CLASSCALL* CONTROLINITIALIZEACTION)(Control* self);
typedef VOID(CLASSCALL* CONTROLDISABLEACTION)(Control* self);
typedef VOID(CLASSCALL* CONTROLTICKACTION)(Control* self);
typedef U32(CLASSCALL* CONTROLACTIONACTION)(Control* self);
typedef Control* (CLASSCALL* CONTROLRELEASEACTION)(Control* self, CONST OBJECTRELEASETYPE mode);

typedef struct ControlSelf
{
    CONTROLTYPEACTION         Type;
    CONTROLINITIALIZEACTION   Initialize;
    CONTROLDISABLEACTION      Disable;
    CONTROLTICKACTION         Tick;
    CONTROLACTIONACTION       Action;
    CONTROLRELEASEACTION      Release;
} CONTROLSELF, * CONTROLSELFPTR;

EXTERN CONTROLSELF ControlSelfState;

#pragma pack(push, 1)
typedef struct Control
{
    CONTROLSELFPTR          Self;
    U8                      IsActive;
} CONTROL, * CONTROLPTR;
#pragma pack(pop)

typedef struct ControlState
{
    CONTROLPTR              Active; // 0x10046130
} CONTROLSTATE, * CONTROLSTATEPTR;

EXTERN CONTROLSTATE ControlState;

VOID CLASSCALL InitializeControl(CONTROLPTR self);
VOID CLASSCALL DisableControl(CONTROLPTR self);
VOID CLASSCALL TickControl(CONTROLPTR self);
U32 CLASSCALL ActionControl(CONTROLPTR self);
CONTROLPTR CLASSCALL ReleaseControl(CONTROLPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisposeControl(CONTROLPTR self);

CONTROLTYPE CLASSCALL AcquireControlTypeButton(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeControl(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeDial(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeImage(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeInput(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeList(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeLoadSave(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeMain(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeMessage(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeMultiJoin(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeMultiJoinResult(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeMultiSelect(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeMultiStart(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypePanel(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeScroll(LPVOID);
CONTROLTYPE CLASSCALL AcquireControlTypeToggle(LPVOID);

#define CONTROLACTION_MAIN_SINGLE                           1000
#define CONTROLACTION_MAIN_MULTI                            1001
#define CONTROLACTION_MAIN_INTRO                            1002
#define CONTROLACTION_MAIN_CREDITS                          1003
#define CONTROLACTION_MAIN_EXIT                             1004
#define CONTROLACTION_MAIN_RATING                           1005
#define CONTROLACTION_MAIN_GREETINGS                        1006
#define CONTROLACTION_MAIN_RESOLUTIONS                      1007
#define CONTROLACTION_MAIN_RESOLUTIONS2                     1008
#define CONTROLACTION_SINGLE0_NEW                           1009
#define CONTROLACTION_SINGLE0_SINGLEMISSIONS                1010
#define CONTROLACTION_SINGLE0_LOAD                          1011
#define CONTROLACTION_SINGLE0_CANCEL                        1012
#define CONTROLACTION_SINGLE0_DELETE                        1013
#define CONTROLACTION_SINGLE0_ADDON_CAMPAIGNS               1014
#define CONTROLACTION_SINGLE0_DIFFICULTY                    1015
#define CONTROLACTION_1016                                  1016 /* TODO Single1?*/
#define CONTROLACTION_SINGLE2_BRITISH                       1017 /* TODO Single2?*/
#define CONTROLACTION_SINGLE2_GERMAN                        1018
#define CONTROLACTION_SINGLE2_RUSSIAN                       1019
#define CONTROLACTION_SINGLE2_CANCEL                        1020
#define CONTROLACTION_SINGLE2_AMERICAN                      1021
#define CONTROLACTION_SINGLE3_LOAD                          1022
#define CONTROLACTION_SINGLE3_CANCEL                        1023
#define CONTROLACTION_SINGLE4_LOAD                          1024
#define CONTROLACTION_SINGLE4_CANCEL                        1025
#define CONTROLACTION_SINGLE5_CONTINUE                      1026
#define CONTROLACTION_SINGLE5_REPLAY                        1027
#define CONTROLACTION_SINGLE5_EXIT                          1028
#define CONTROLACTION_SINGLE5_VICTORY                       1029
#define CONTROLACTION_SCROLL                                1030
#define CONTROLACTION_SCROLL_CHANGE                         1031
#define CONTROLACTION_SCROLL_UP                             1032
#define CONTROLACTION_SCROLL_DOWN                           1033
#define CONTROLACTION_LIST_SELECT                           1034
#define CONTROLACTION_DIAL_DIAL                             1035
#define CONTROLACTION_DIAL_CANCEL                           1036
#define CONTROLACTION_DIAL_ADD                              1037
#define CONTROLACTION_DIAL_REMOVE                           1038
#define CONTROLACTION_JMULTI_INPUT1                         1039
#define CONTROLACTION_JMULTI_INPUT2                         1040
#define CONTROLACTION_MULTI1_INTERNET                       1041
#define CONTROLACTION_MULTI1_LAN                            1042
#define CONTROLACTION_MULTI1_MODEM                          1043
#define CONTROLACTION_MULTI1_CANCEL                         1044
#define CONTROLACTION_MULTI2_CREATE                         1045
#define CONTROLACTION_MULTI2_JOIN                           1046
#define CONTROLACTION_MULTI2_CANCEL                         1047
#define CONTROLACTION_MULTI2_DELETE                         1048
#define CONTROLACTION_JMULTI1_OK                            1049
#define CONTROLACTION_JMULTI1_CANCEL                        1050
#define CONTROLACTION_JMULTI2_JOIN                          1051
#define CONTROLACTION_JMULTI2_CANCEL                        1052
#define CONTROLACTION_MULTI3_OK                             1053
#define CONTROLACTION_MULTI3_CANCEL                         1054
#define CONTROLACTION_MULTI3_OPTIONS                        1055
#define CONTROLACTION_MULTI4_START                          1056
#define CONTROLACTION_MULTI4_CANCEL                         1057
#define CONTROLACTION_MULTI4_GREYSTART                      1058
#define CONTROLACTION_MULTI4_READY                          1059
#define CONTROLACTION_1060                                  1060 /* TODO */
#define CONTROLACTION_1061                                  1061 /* TODO */
#define CONTROLACTION_1062                                  1062 /* TODO */
#define CONTROLACTION_1063                                  1063 /* TODO */
#define CONTROLACTION_1064                                  1064 /* TODO */
#define CONTROLACTION_1065                                  1065 /* TODO */
#define CONTROLACTION_1066                                  1066 /* TODO */
#define CONTROLACTION_1067                                  1067 /* TODO */
#define CONTROLACTION_MULTI4_INACTIVITY_PERIOD              1068
#define CONTROLACTION_MULTI4_INACTIVITY_PERIOD_MINUTES      1069
#define CONTROLACTION_MULTI4_INACTIVITY_PERIOD_SECONDS      1070
#define CONTROLACTION_MULTI4_CAPTURE_DELAY                  1071
#define CONTROLACTION_MULTI4_CAPTURE_DELAY_MINUTES          1072
#define CONTROLACTION_MULTI4_CAPTURE_DELAY_SECONDS          1073
#define CONTROLACTION_MULTI4_TIMEOUT_DELAY                  1074
#define CONTROLACTION_MULTI4_TIMEOUT_DELAY_MINUTES          1075
#define CONTROLACTION_MULTI4_TIMEOUT_DELAY_SECONDS          1076
#define CONTROLACTION_GREETINGS                             1077
#define CONTROLACTION_GREETINGS_EXIT                        1078
#define CONTROLACTION_RATING_CAMPAIGN                       1079
#define CONTROLACTION_RATING_SINGLE                         1080
#define CONTROLACTION_RATING_CANCEL                         1081
#define CONTROLACTION_BRIEF                                 1082
#define CONTROLACTION_BRIEF_OK                              1083
#define CONTROLACTION_BRIEF_MENU                            1084
#define CONTROLACTION_BRIEF_THIRDBUTTON                     1085
#define CONTROLACTION_INITIALIZE_NETWORK                    1086
#define CONTROLACTION_1087                                  1087 /* TODO */
#define CONTROLACTION_1088                                  1088 /* TODO */
#define CONTROLACTION_1089                                  1089 /* TODO */
#define CONTROLACTION_1090                                  1090 /* TODO */
#define CONTROLACTION_1091                                  1091 /* TODO */
#define CONTROLACTION_1092                                  1092 /* TODO */
#define CONTROLACTION_1093                                  1093 /* TODO */
#define CONTROLACTION_MSGBOX_OK                             1094
#define CONTROLACTION_MSGBOX_CANCEL                         1095
#define CONTROLACTION_MSTAT_DETAIL                          1096
#define CONTROLACTION_MSTAT_TOTAL                           1097
#define CONTROLACTION_MSTAT_EXIT                            1098
#define CONTROLACTION_MSTAT_DESTROYED                       1099
#define CONTROLACTION_MSTAT_LOST                            1100
#define CONTROLACTION_STATISTICS_BASE                       1101
#define CONTROLACTION_STATISTICS_1                          (CONTROLACTION_STATISTICS_BASE + 0)
#define CONTROLACTION_STATISTICS_2                          (CONTROLACTION_STATISTICS_BASE + 1)
#define CONTROLACTION_STATISTICS_3                          (CONTROLACTION_STATISTICS_BASE + 2)
#define CONTROLACTION_STATISTICS_4                          (CONTROLACTION_STATISTICS_BASE + 3)
#define CONTROLACTION_STATISTICS_5                          (CONTROLACTION_STATISTICS_BASE + 4)
#define CONTROLACTION_STATISTICS_6                          (CONTROLACTION_STATISTICS_BASE + 5)
#define CONTROLACTION_STATISTICS_7                          (CONTROLACTION_STATISTICS_BASE + 6)
#define CONTROLACTION_STATISTICS_8                          (CONTROLACTION_STATISTICS_BASE + 7)
#define CONTROLACTION_STATISTICS_9                          (CONTROLACTION_STATISTICS_BASE + 8)
#define CONTROLACTION_STATISTICS_10                         (CONTROLACTION_STATISTICS_BASE + 9)
#define CONTROLACTION_STATISTICS_11                         (CONTROLACTION_STATISTICS_BASE + 10)
#define CONTROLACTION_STATISTICS_12                         (CONTROLACTION_STATISTICS_BASE + 11)
#define CONTROLACTION_STATISTICS_13                         (CONTROLACTION_STATISTICS_BASE + 12)
#define CONTROLACTION_MAP_HOVER                             1114
#define CONTROLACTION_PLAY_COMPLETED                        1115
#define CONTROLACTION_1116                                  1116 /* TODO */
#define CONTROLACTION_PLAY_SHORT_INTRO2                     1117
#define CONTROLACTION_PLAY_SHORT_INTRO3                     1118
#define CONTROLACTION_1119                                  1119 /* TODO Exit ? */

#define MAX_CONTROL_ACTION_QUEUE_ITEM_COUNT                 100

typedef struct ControlActionQueue
{
    U32 Items[MAX_CONTROL_ACTION_QUEUE_ITEM_COUNT];     // 0x10046cb8
    U32 Count;                                          // 0x10046f78
} CONTROLACTIONQUEUE, * CONTROLACTIONQUEUEPTR;

EXTERN CONTROLACTIONQUEUE ControlActionQueueState;

VOID EnqueueControlActionQueue(CONST U32 value);
U32 DequeueControlActionQueue(VOID);