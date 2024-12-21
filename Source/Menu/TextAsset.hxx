/*
Copyright (c) 2024 Americus Maximus

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

#include "Basic.hxx"

#define MAP_MISSION_TEXT_ASSET_PARAM    0
#define START_VIDEO_TEXT_ASSET_PARAM    1
#define VICTORY_VIDEO_TEXT_ASSET_PARAM  2
#define DEFEAT_VIDEO_TEXT_ASSET_PARAM   3

#define VIDEO_NAME_ALIAS_TEXT_ASSET_PARAM       0
#define VIDEO_NAME_ACTUAL_TEXT_ASSET_PARAM      1

#define VIDEO_RECORD_SIZE_TEXT_ASSET_PARAM      0
#define VIDEO_RECORD_NAME_TEXT_ASSET_PARAM      1
#define VIDEO_RECORD_UNKNOWN1_TEXT_ASSET_PARAM  2 /* TODO */
#define VIDEO_RECORD_UNKNOWN2_TEXT_ASSET_PARAM  3 /* TODO */

typedef struct TextAsset
{
    LPSTR       Content;
    U32         Count;
    U32         Length;
} TEXTASSET, * TEXTASSETPTR;

TEXTASSETPTR CLASSCALL ActivateTextAsset(TEXTASSETPTR self);
U32 CLASSCALL AcquireTextAssetHash(TEXTASSETPTR self);
VOID CLASSCALL AcquireTextAssetString(TEXTASSETPTR self, CONST U32 line, LPSTR result);
VOID CLASSCALL AcquireTextAssetStringValue(TEXTASSETPTR self, CONST U32 line, CONST U32 param, LPSTR result);
VOID CLASSCALL InitializeTextAsset(TEXTASSETPTR self, LPCSTR name);
VOID CLASSCALL DisposeTextAsset(TEXTASSETPTR self);