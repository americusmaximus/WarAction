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

#include "Activation.hxx"
#include "Assets.hxx"
#include "Audio.hxx"
#include "Computer.hxx"
#include "Cursor.hxx"
#include "Input.hxx"
#include "Lock.hxx"
#include "MessageControl.hxx"
#include "Module.hxx"
#include "Network.hxx"
#include "Shortcuts.hxx"
#include "Sound.hxx"
#include "StarControl.hxx"
#include "Statistics.hxx"
#include "Window.hxx"

#include <stdlib.h>

// 0x1003f000
// NOTE: This is a workaround for static initialization of objects, variables, states, etc.
VOID Activate(VOID)
{
    ActivateMessageControlState();
    ActivateComputerState();
    ActivateShortcutsState();
    ActivateSoundModuleState();
    ActivateStatisticsState();
    ActivateAudioPlayerState();
    ActivateStarControlState();
    ActivateNetworkState();
    ActivateBinAssetMainBKState();
    ActivateBinAssetMainBTState();
    ActivateBinAssetSingle0BKState();
    ActivateBinAssetSingle0BTState();
    ActivateBinAssetSingle2BKState();
    ActivateBinAssetSingle2BTState();
    ActivateBinAssetSinOld2BKState();
    ActivateBinAssetSinOld2BTState();
    ActivateBinAssetSingle3BKState();
    ActivateBinAssetSingle3BTState();
    ActivateBinAssetSingle4BKState();
    ActivateBinAssetSingle4BTState();
    ActivateBinAssetSingle5BKState();
    ActivateBinAssetSingle5BTState();
    ActivateBinAssetDialBKState();
    ActivateBinAssetDialBTState();
    ActivateBinAssetMulti1BKState();
    ActivateBinAssetMulti1BTState();
    ActivateBinAssetMulti2BKState();
    ActivateBinAssetMulti2BTState();
    ActivateBinAssetMulti3BKState();
    ActivateBinAssetMulti3BTState();
    ActivateBinAssetMulti4BKState();
    ActivateBinAssetMulti4BTState();
    ActivateBinAssetJMulti1BKState();
    ActivateBinAssetJMulti1BTState();
    ActivateBinAssetJMulti2BKState();
    ActivateBinAssetJMulti2BTState();
    ActivateBinAssetBriefBKState();
    ActivateBinAssetBriefBTState();
    ActivateBinAssetMStatBKState();
    ActivateBinAssetMStatBTState();
    ActivateBinAssetRatingBKState();
    ActivateBinAssetRatingBTState();
    ActivateBinAssetMsg0State();
    ActivateBinAssetMsg1BKState();
    ActivateBinAssetMsg1BT0State();
    ActivateBinAssetMsg1BT1State();
    ActivateBinAssetMsg1CBT0State();
    ActivateBinAssetMsg1CBT1State();
    ActivateBinAssetMsg2BKState();
    ActivateBinAssetMsg2BT10State();
    ActivateBinAssetMsg2BT11State();
    ActivateBinAssetMsg2BT20State();
    ActivateBinAssetMsg2BT21State();
    ActivateBinAssetGreetingsBKState();
    ActivateBinAssetRombikState();
    ActivateBinAssetMenuGState();
    ActivateBinAssetScrollBarState();
    ActivateBinAssetSingle3SCState();
    ActivateBinAssetSingle4SCState();
    ActivateBinAssetSingle5TXState();
    ActivateBinAssetDialSCState();
    ActivateBinAssetJMulti2SCState();
    ActivateBinAssetSingle0SCState();
    ActivateBinAssetSingle0DifState();
    ActivateBinAssetRatingSCState();
    ActivateBinAssetMulti2SCState();
    ActivateBinAssetMulti3SCState();
    ActivateBinAssetMulti3OPState();
    ActivateBinAssetMulti4ParmBKState();
    ActivateBinAssetMulti4ParmNMState();
    ActivateBinAssetMulti4ParmOPState();
    ActivateBinAssetMsgBoxState();
    ActivateBinAssetCreditsState();
    ActivateBinAssetZvezdyState();
    ActivateBinAssetZVMState();
    ActivateBinAssetZVGoldState();
    ActivateBinAssetMulti4COState();
    ActivateBinAssetMulti4LOPState();
    ActivateBinAssetMulti4NOPState();
    ActivateBinAssetMulti4ROPState();
    ActivateBinAssetMulti4TOPState();
    ActivateBinAssetMulti4SCState();
    ActivateBinAssetMulti4SPState();
    ActivateBinAssetMainRESState();
    ActivateBinAssetMainOPState();
    ActivateBinAssetKartaBKState();
    ActivateFontAssetComicState();
    ActivateFontAssetMainState();
    ActivateBinAssetMN2TextsState();
    ActivateCursorState();
    ActivateWindowState();
    ActivateInputInitializeState();
    ActivateInputExecuteState();
    ActivateInputReleaseState();
    ActivateInputMessageState();
    ActivateBinFileState();
    ActivateBinArchiveState();
}

// 0x10004d20
VOID ActivateMessageControlState(VOID)
{
    ActivateMessageControl();
    ActivateReleaseMessageControl();
}

// 0x10004d30
VOID ActivateMessageControl(VOID)
{
    ActivateMessageControl(&MessageControlState);
}

// 0x10004d40
VOID ActivateReleaseMessageControl(VOID)
{
    atexit(ReleaseMessageControlAction);
}

// 0x10004d50
VOID ReleaseMessageControlAction(VOID)
{
    DisposeMessageControl(&MessageControlState);
}

// 0x100055d0
// 0x100055e0
VOID ActivateComputerState(VOID)
{
    ComputerState.Capacity = 0;
    ComputerState.Players = NULL;
}

// 0x10006fe0
VOID ActivateShortcutsState(VOID)
{
    ActivateShortcuts();
    ActivateReleaseShortcuts();
}

// 0x10006ff0
VOID ActivateShortcuts(VOID)
{
    ActivateShortcuts(&ShortcutsState.State);
}

// 0x10007000
VOID ActivateReleaseShortcuts(VOID)
{
    atexit(ReleaseShortcutsAction);
}

// 0x10007010
VOID ReleaseShortcutsAction(VOID)
{
    // TODO Make this method make sense
    if (ShortcutsState.State.Unk09 == 0) // TODO
    {
        ShortcutsState.State.Unk09 = 0; // TODO

        SHORTCUTPTR current = ShortcutsState.State.Items->Unk00;
        SHORTCUTPTR next = current;

        // TODO
        /*while (current = next, next != ShortcutsState.State.Items)
        {
            FUN_1000bce0(current);
            FUN_1000a730(&ShortcutsState.State, &local_4, next);
            next = current;
        }*/
    }
    else
    {
        SHORTCUTPTR pSVar1 = NULL; // TODO
        SHORTCUTPTR pSVar2 = ShortcutsState.State.Items->Unk01;

        // TODO
        /*if (ShortcutsState.State.Items->Unk01 != ShortcutsState.Items)
        {
            do
            {
                FUN_1000ab10(pSVar2->Unk02);
                pSVar1 = pSVar2->Unk00;
                free(pSVar2);
                pSVar2 = pSVar1;
            } while (pSVar1 != ShortcutsState.Items);
        }*/

        ShortcutsState.State.Unk09 = 0;

        ShortcutsState.State.Items->Unk01 = ShortcutsState.Items;
        ShortcutsState.State.Items->Unk00 = ShortcutsState.State.Items;
        ShortcutsState.State.Items->Unk02 = ShortcutsState.State.Items;

        //FUN_1000a240(&ShortcutsState.State, (Shortcut*)&stack0xfffffff8); // TODO
    }

    free(ShortcutsState.State.Items);

    ShortcutsState.State.Items = NULL;
    ShortcutsState.State.Unk09 = 0; // TODO

    {
        LOCK lock;
        ActivateLock(&lock);

        SHORTCUTPTR current = ShortcutsState.Items;
        ShortcutsState.Count = ShortcutsState.Count - 1;

        SHORTCUTPTR shortcut = NULL;
        if (ShortcutsState.Count == 0)
        {
            ShortcutsState.Items = NULL;
            shortcut = current;
        }

        ReleaseLock(&lock);

        if (shortcut != NULL) { free(shortcut); }
    }
}

// 0x1000c070
// 0x1000c080
VOID ActivateSoundModuleState(VOID)
{
    SoundState.State.State = NULL;
    SoundState.State.File.Value = INVALID_BINFILE_VALUE;
}

// 0x1000c7b0
// 0x1000c7c0
VOID ActivateStatisticsState(VOID)
{
    ActivateStatistics(&StatisticsState.State);
}

// 0x1000db00
VOID ActivateAudioPlayerState(VOID)
{
    ActivateAudioPlayer();
    ActivateReleaseAudioPlayer();
}

// 0x1000db10
VOID ActivateAudioPlayer(VOID)
{
    ActivateAudioPlayer(&AudioPlayerState);
}

// 0x1000db20
VOID ActivateReleaseAudioPlayer(VOID)
{
    atexit(ReleaseAudioPlayerAction);
}

// 0x1000db30
VOID ReleaseAudioPlayerAction(VOID)
{
    DisposeAudioPlayer(&AudioPlayerState);
}

// 0x10016c60
VOID ActivateStarControlState(VOID)
{
    ActivateStarControl();
    ActivateReleaseStarControl();
}

// 0x10016c70
VOID ActivateStarControl(VOID)
{
    ActivateStarControl(&StarControlState);
}

// 0x10016c80
VOID ActivateReleaseStarControl(VOID)
{
    atexit(ReleaseStarControlAction);
}

// 0x10016c90
VOID ReleaseStarControlAction(VOID)
{
    DisposeControl((CONTROLPTR)&StarControlState);
}

// 0x1001e020
VOID ActivateNetworkState(VOID)
{
    ActivateNetwork();
    ActivateReleaseNetwork();
}

// 0x1001e030
VOID ActivateNetwork(VOID)
{
    ActivateNetworkModuleState(&NetworkState.State);
}

// 0x1001e040
VOID ActivateReleaseNetwork(VOID)
{
    atexit(ReleaseNetworkAction);
}

// 0x1001e050
VOID ReleaseNetworkAction(VOID)
{
    DisposeNetworkModuleState(&NetworkState.State);
}

// 0x1001f7c0
VOID ActivateBinAssetMainBKState(VOID)
{
    ActivateBinAssetMainBK();
    ActivateReleaseBinAssetMainBK();
}

// 0x1001f7d0
VOID ActivateBinAssetMainBK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MainBK);
}

// 0x1001f7e0
VOID ActivateReleaseBinAssetMainBK(VOID)
{
    atexit(ReleaseBinAssetMainBKAction);
}

// 0x1001f7f0
VOID ReleaseBinAssetMainBKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MainBK);
}

// 0x1001f800
VOID ActivateBinAssetMainBTState(VOID)
{
    ActivateBinAssetMainBT();
    ActivateReleaseBinAssetMainBT();
}

// 0x1001f810
VOID ActivateBinAssetMainBT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MainBT);
}

// 0x1001f820
VOID ActivateReleaseBinAssetMainBT(VOID)
{
    atexit(ReleaseBinAssetMainBTAction);
}

// 0x1001f830
VOID ReleaseBinAssetMainBTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MainBT);
}

// 0x1001f840
VOID ActivateBinAssetSingle0BKState(VOID)
{
    ActivateBinAssetSingle0BK();
    ActivateReleaseBinAssetSingle0BK();
}

// 0x1001f850
VOID ActivateBinAssetSingle0BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single0BK);
}

// 0x1001f860
VOID ActivateReleaseBinAssetSingle0BK(VOID)
{
    atexit(ReleaseBinAssetSingle0BKAction);
}

// 0x1001f870
VOID ReleaseBinAssetSingle0BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single0BK);
}

// 0x1001f880
VOID ActivateBinAssetSingle0BTState(VOID)
{
    ActivateBinAssetSingle0BT();
    ActivateReleaseBinAssetSingle0BT();
}

// 0x1001f890
VOID ActivateBinAssetSingle0BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single0BT);
}

// 0x1001f8a0
VOID ActivateReleaseBinAssetSingle0BT(VOID)
{
    atexit(ReleaseBinAssetSingle0BTAction);
}

// 0x1001f8b0
VOID ReleaseBinAssetSingle0BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single0BT);
}

// 0x1001f8c0
VOID ActivateBinAssetSingle2BKState(VOID)
{
    ActivateBinAssetSingle2BK();
    ActivateReleaseBinAssetSingle2BK();
}

// 0x1001f8d0
VOID ActivateBinAssetSingle2BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single2BK);
}

// 0x1001f8e0
VOID ActivateReleaseBinAssetSingle2BK(VOID)
{
    atexit(ReleaseBinAssetSingle2BKAction);
}

// 0x1001f8f0
VOID ReleaseBinAssetSingle2BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single2BK);
}

// 0x1001f910
VOID ActivateBinAssetSingle2BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single2BT);
}

// 0x1001f920
VOID ActivateReleaseBinAssetSingle2BT(VOID)
{
    atexit(ReleaseBinAssetSingle2BTAction);
}

// 0x1001f900
VOID ActivateBinAssetSingle2BTState(VOID)
{
    ActivateBinAssetSingle2BT();
    ActivateReleaseBinAssetSingle2BT();
}

// 0x1001f930
VOID ReleaseBinAssetSingle2BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single2BT);
}

// 0x1001f940
VOID ActivateBinAssetSinOld2BKState(VOID)
{
    ActivateBinAssetSinOld2BK();
    ActivateReleaseBinAssetSinOld2BK();
}

// 0x1001f950
VOID ActivateBinAssetSinOld2BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.SinOld2BK);
}

// 0x1001f960
VOID ActivateReleaseBinAssetSinOld2BK(VOID)
{
    atexit(ReleaseBinAssetSinOld2BKAction);
}

// 0x1001f970
VOID ReleaseBinAssetSinOld2BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.SinOld2BK);
}

// 0x1001f980
VOID ActivateBinAssetSinOld2BTState(VOID)
{
    ActivateBinAssetSinOld2BT();
    ActivateReleaseBinAssetSinOld2BT();
}

// 0x1001f990
VOID ActivateBinAssetSinOld2BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.SinOld2BT);
}

// 0x1001f9a0
VOID ActivateReleaseBinAssetSinOld2BT(VOID)
{
    atexit(ReleaseBinAssetSinOld2BTAcrion);
}

// 0x1001f9b0
VOID ReleaseBinAssetSinOld2BTAcrion(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.SinOld2BT);
}

// 0x1001f9c0
VOID ActivateBinAssetSingle3BKState(VOID)
{
    ActivateBinAssetSingle3BK();
    ActivateReleaseBinAssetSingle3BK();
}

// 0x1001f9d0
VOID ActivateBinAssetSingle3BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single3BK);
}

// 0x1001f9e0
VOID ActivateReleaseBinAssetSingle3BK(VOID)
{
    atexit(ReleaseBinAssetSingle3BKAction);
}

// 0x1001f9f0
VOID ReleaseBinAssetSingle3BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single3BK);
}

// 0x1001fa00
VOID ActivateBinAssetSingle3BTState(VOID)
{
    ActivateBinAssetSingle3BT();
    ActivateReleaseBinAssetSingle3BT();
}

// 0x1001fa10
VOID ActivateBinAssetSingle3BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single3BT);
}

// 0x1001fa20
VOID ActivateReleaseBinAssetSingle3BT(VOID)
{
    atexit(ReleaseBinAssetSingle3BTAction);
}

// 0x1001fa30
VOID ReleaseBinAssetSingle3BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single3BT);
}

// 0x1001fa40
VOID ActivateBinAssetSingle4BKState(VOID)
{
    ActivateBinAssetSingle4BK();
    ActivateReleaseBinAssetSingle4BK();
}

// 0x1001fa50
VOID ActivateBinAssetSingle4BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single4BK);
}

// 0x1001fa60
VOID ActivateReleaseBinAssetSingle4BK(VOID)
{
    atexit(ReleaseBinAssetSingle4BKAction);
}

// 0x1001fa70
VOID ReleaseBinAssetSingle4BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single4BK);
}

// 0x1001fa80
VOID ActivateBinAssetSingle4BTState(VOID)
{
    ActivateBinAssetSingle4BT();
    ActivateReleaseBinAssetSingle4BT();
}

// 0x1001fa90
VOID ActivateBinAssetSingle4BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single4BT);
}

// 0x1001faa0
VOID ActivateReleaseBinAssetSingle4BT(VOID)
{
    atexit(ReleaseBinAssetSingle4BTAction);
}

// 0x1001fab0
VOID ReleaseBinAssetSingle4BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single4BT);
}

// 0x1001fac0
VOID ActivateBinAssetSingle5BKState(VOID)
{
    ActivateBinAssetSingle5BK();
    ActivateReleaseBinAssetSingle5BK();
}

// 0x1001fad0
VOID ActivateBinAssetSingle5BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single5BK);
}

// 0x1001faf0
VOID ReleaseBinAssetSingle5BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single5BK);
}

// 0x1001fae0
VOID ActivateReleaseBinAssetSingle5BK(VOID)

{
    atexit(ReleaseBinAssetSingle5BKAction);
}

// 0x1001fb10
VOID ActivateBinAssetSingle5BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single5BT);
}

// 0x1001fb20
VOID ActivateReleaseBinAssetSingle5BT(VOID)
{
    atexit(ReleaseBinAssetSingle5BTAction);
}

// 0x1001fb00
VOID ActivateBinAssetSingle5BTState(VOID)
{
    ActivateBinAssetSingle5BT();
    ActivateReleaseBinAssetSingle5BT();
}

// 0x1001fb30
VOID ReleaseBinAssetSingle5BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single5BT);
}

// 0x1001fb40
VOID ActivateBinAssetDialBKState(VOID)
{
    ActivateBinAssetDialBK();
    ActivateReleaseBinAssetDialBK();
}

// 0x1001fb50
VOID ActivateBinAssetDialBK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.DialBK);
}

// 0x1001fb60
VOID ActivateReleaseBinAssetDialBK(VOID)
{
    atexit(ReleaseBinAssetDialBKAction);
}

// 0x1001fb70
VOID ReleaseBinAssetDialBKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.DialBK);
}

// 0x1001fb80
VOID ActivateBinAssetDialBTState(VOID)
{
    ActivateBinAssetDialBT();
    ActivateReleaseBinAssetDialBT();
}

// 0x1001fb90
VOID ActivateBinAssetDialBT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.DialBT);
}

// 0x1001fba0
VOID ActivateReleaseBinAssetDialBT(VOID)
{
    atexit(ReleaseBinAssetDialBTAction);
}

// 0x1001fbb0
VOID ReleaseBinAssetDialBTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.DialBT);
}

// 0x1001fbd0
VOID ActivateBinAssetMulti1BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi1BK);
}

// 0x1001fbe0
VOID ActivateReleaseBinAssetMulti1BK(VOID)
{
    atexit(ReleaseBinAssetMulti1BKAction);
}

// 0x1001fbf0
VOID ReleaseBinAssetMulti1BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi1BK);
}

// 0x1001fbc0
VOID ActivateBinAssetMulti1BKState(VOID)
{
    ActivateBinAssetMulti1BK();
    ActivateReleaseBinAssetMulti1BK();
}

// 0x1001fc00
VOID ActivateBinAssetMulti1BTState(VOID)
{
    ActivateBinAssetMulti1BT();
    ActivateReleaseBinAssetMulti1BT();
}

// 0x1001fc10
VOID ActivateBinAssetMulti1BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi1BT);
}

// 0x1001fc20
VOID ActivateReleaseBinAssetMulti1BT(VOID)
{
    atexit(ReleaseBinAssetMulti1BTAction);
}

// 0x1001fc30
VOID ReleaseBinAssetMulti1BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi1BT);
}

// 0x1001fc40
VOID ActivateBinAssetMulti2BKState(VOID)
{
    ActivateBinAssetMulti2BK();
    ActivateReleaseBinAssetMulti2BK();
}

// 0x1001fc50
VOID ActivateBinAssetMulti2BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi2BK);
}

// 0x1001fc60
VOID ActivateReleaseBinAssetMulti2BK(VOID)
{
    atexit(ReleaseBinAssetMulti2BKAction);
}

// 0x1001fc70
VOID ReleaseBinAssetMulti2BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi2BK);
}

// 0x1001fc80
VOID ActivateBinAssetMulti2BTState(VOID)
{
    ActivateBinAssetMulti2BT();
    ActivateReleaseBinAssetMulti2BT();
}

// 0x1001fc90
VOID ActivateBinAssetMulti2BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi2BT);
}

// 0x1001fca0
VOID ActivateReleaseBinAssetMulti2BT(VOID)
{
    atexit(ReleaseBinAssetMulti2BTAction);
}

// 0x1001fcb0
VOID ReleaseBinAssetMulti2BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi2BT);
}

// 0x1001fcc0
VOID ActivateBinAssetMulti3BKState(VOID)
{
    ActivateBinAssetMulti3BK();
    ActivateReleaseBinAssetMulti3BK();
}

// 0x1001fcd0
VOID ActivateBinAssetMulti3BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi3BK);
}

// 0x1001fce0
VOID ActivateReleaseBinAssetMulti3BK(VOID)
{
    atexit(ReleaseBinAssetMulti3BKAction);
}

// 0x1001fcf0
VOID ReleaseBinAssetMulti3BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi3BK);
}

// 0x1001fd00
VOID ActivateBinAssetMulti3BTState(VOID)
{
    ActivateBinAssetMulti3BT();
    ActivateReleaseBinAssetMulti3BT();
}

// 0x1001fd10
VOID ActivateBinAssetMulti3BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi3BT);
}

// 0x1001fd20
VOID ActivateReleaseBinAssetMulti3BT(VOID)
{
    atexit(ReleaseBinAssetMulti3BTAction);
}

// 0x1001fd30
VOID ReleaseBinAssetMulti3BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi3BT);
}

// 0x1001fd40
VOID ActivateBinAssetMulti4BKState(VOID)
{
    ActivateBinAssetMulti4BK();
    ActivateReleaseBinAssetMulti4BK();
}

// 0x1001fd50
VOID ActivateBinAssetMulti4BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4BK);
}

// 0x1001fd60
VOID ActivateReleaseBinAssetMulti4BK(VOID)
{
    atexit(ReleaseBinAssetMulti4BKAction);
}

// 0x1001fd70
VOID ReleaseBinAssetMulti4BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4BK);
}

// 0x1001fd80
VOID ActivateBinAssetMulti4BTState(VOID)
{
    ActivateBinAssetMulti4BT();
    ActivateReleaseBinAssetMulti4BT();
}

// 0x1001fd90
VOID ActivateBinAssetMulti4BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4BT);
}

// 0x1001fda0
VOID ActivateReleaseBinAssetMulti4BT(VOID)
{
    atexit(ReleaseBinAssetMulti4BTAction);
}

// 0x1001fdb0
VOID ReleaseBinAssetMulti4BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4BT);
}

// 0x1001fdc0
VOID ActivateBinAssetJMulti1BKState(VOID)
{
    ActivateBinAssetJMulti1BK();
    ActivateReleaseBinAssetJMulti1BK();
}

// 0x1001fdd0
VOID ActivateBinAssetJMulti1BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.JMulti1BK);
}

// 0x1001fde0
VOID ActivateReleaseBinAssetJMulti1BK(VOID)
{
    atexit(ReleaseBinAssetJMulti1BKAction);
}

// 0x1001fdf0
VOID ReleaseBinAssetJMulti1BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.JMulti1BK);
}

// 0x1001fe00
VOID ActivateBinAssetJMulti1BTState(VOID)

{
    ActivateBinAssetJMulti1BT();
    ActivateReleaseBinAssetJMulti1BT();
}

// 0x1001fe10
VOID ActivateBinAssetJMulti1BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.JMulti1BT);
}

// 0x1001fe20
VOID ActivateReleaseBinAssetJMulti1BT(VOID)
{
    atexit(ReleaseBinAssetJMulti1BTAction);
}

// 0x1001fe30
VOID ReleaseBinAssetJMulti1BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.JMulti1BT);
}

// 0x1001fe40
VOID ActivateBinAssetJMulti2BKState(VOID)
{
    ActivateBinAssetJMulti2BK();
    ActivateReleaseBinAssetJMulti2BK();
}

// 0x1001fe50
VOID ActivateBinAssetJMulti2BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.JMulti2BK);
}

// 0x1001fe60
VOID ActivateReleaseBinAssetJMulti2BK(VOID)
{
    atexit(ReleaseBinAssetJMulti2BKAction);
}

// 0x1001fe70
VOID ReleaseBinAssetJMulti2BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.JMulti2BK);
}

// 0x1001fe80
VOID ActivateBinAssetJMulti2BTState(VOID)
{
    ActivateBinAssetJMulti2BT();
    ActivateReleaseBinAssetJMulti2BT();
}

// 0x1001fe90
VOID ActivateBinAssetJMulti2BT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.JMulti2BT);
}

// 0x1001fea0
VOID ActivateReleaseBinAssetJMulti2BT(VOID)
{
    atexit(ReleaseBinAssetJMulti2BTAction);
}

// 0x1001feb0
VOID ReleaseBinAssetJMulti2BTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.JMulti2BT);
}

// 0x1001fec0
VOID ActivateBinAssetBriefBKState(VOID)
{
    ActivateBinAssetBriefBK();
    ActivateReleaseBinAssetBriefBK();
}

// 0x1001fed0
VOID ActivateBinAssetBriefBK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.BriefBK);
}

// 0x1001fee0
VOID ActivateReleaseBinAssetBriefBK(VOID)
{
    atexit(ReleaseBinAssetBriefBKAction);
}

// 0x1001fef0
VOID ReleaseBinAssetBriefBKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.BriefBK);
}

// 0x1001ff00
VOID ActivateBinAssetBriefBTState(VOID)
{
    ActivateBinAssetBriefBT();
    ActivateReleaseBinAssetBriefBT();
}

// 0x1001ff10
VOID ActivateBinAssetBriefBT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.BriefBT);
}

// 0x1001ff20
VOID ActivateReleaseBinAssetBriefBT(VOID)
{
    atexit(ReleaseBinAssetBriefBTAction);
}

// 0x1001ff30
VOID ReleaseBinAssetBriefBTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.BriefBT);
}

// 0x1001ff40
VOID ActivateBinAssetMStatBKState(VOID)
{
    ActivateBinAssetMStatBK();
    ActivateReleaseBinAssetMStatBK();
}

// 0x1001ff50
VOID ActivateBinAssetMStatBK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MStatBK);
}

// 0x1001ff60
VOID ActivateReleaseBinAssetMStatBK(VOID)
{
    atexit(ReleaseBinAssetMStatBKAction);
}

// 0x1001ff70
VOID ReleaseBinAssetMStatBKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MStatBK);
}

// 0x1001ff80
VOID ActivateBinAssetMStatBTState(VOID)
{
    ActivateBinAssetMStatBT();
    ActivateReleaseBinAssetMStatBT();
}

// 0x1001ff90
VOID ActivateBinAssetMStatBT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MStatBT);
}

// 0x1001ffa0
VOID ActivateReleaseBinAssetMStatBT(VOID)
{
    atexit(ReleaseBinAssetMStatBTAction);
}

// 0x1001ffb0
VOID ReleaseBinAssetMStatBTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MStatBT);
}

// 0x1001ffc0
VOID ActivateBinAssetRatingBKState(VOID)
{
    ActivateBinAssetRatingBK();
    ActivateReleaseBinAssetRatingBK();
}

// 0x1001ffd0
VOID ActivateBinAssetRatingBK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.RatingBK);
}

// 0x1001ffe0
VOID ActivateReleaseBinAssetRatingBK(VOID)
{
    atexit(ReleaseBinAssetRatingBKAction);
}

// 0x1001fff0
VOID ReleaseBinAssetRatingBKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.RatingBK);
}

// 0x10020000
VOID ActivateBinAssetRatingBTState(VOID)
{
    ActivateBinAssetRatingBT();
    ActivateReleaseBinAssetRatingBT();
}

// 0x10020010
VOID ActivateBinAssetRatingBT(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.RatingBT);
}

// 0x10020020
VOID ActivateReleaseBinAssetRatingBT(VOID)
{
    atexit(ReleaseBinAssetRatingBTAction);
}

// 0x10020030
VOID ReleaseBinAssetRatingBTAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.RatingBT);
}

// 0x10020050
VOID ActivateBinAssetMsg0(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg0);
}

// 0x10020060
VOID ActivateReleaseBinAssetMsg0(VOID)
{
    atexit(ReleaseBinAssetMsg0Action);
}

// 0x10020040
VOID ActivateBinAssetMsg0State(VOID)
{
    ActivateBinAssetMsg0();
    ActivateReleaseBinAssetMsg0();
}

// 0x10020070
VOID ReleaseBinAssetMsg0Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg0);
}

// 0x10020080
VOID ActivateBinAssetMsg1BKState(VOID)
{
    ActivateBinAssetMsg1BK();
    ActivateReleaseBinAssetMsg1BK();
}

// 0x10020090
VOID ActivateBinAssetMsg1BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg1BK);
}

// 0x100200a0
VOID ActivateReleaseBinAssetMsg1BK(VOID)
{
    atexit(ReleaseBinAssetMsg1BKAction);
}

// 0x100200b0
VOID ReleaseBinAssetMsg1BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg1BK);
}

// 0x100200c0
VOID ActivateBinAssetMsg1BT0State(VOID)
{
    ActivateBinAssetMsg1BT0();
    ActivateReleaseBinAssetMsg1BT0();
}

// 0x100200d0
VOID ActivateBinAssetMsg1BT0(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg1BT0);
}

// 0x100200e0
VOID ActivateReleaseBinAssetMsg1BT0(VOID)
{
    atexit(ReleaseBinAssetMsg1BT0Action);
}

// 0x100200f0
VOID ReleaseBinAssetMsg1BT0Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg1BT0);
}

// 0x10020100
VOID ActivateBinAssetMsg1BT1State(VOID)
{
    ActivateBinAssetMsg1BT1();
    ActivateReleaseBinAssetMsg1BT1();
}

// 0x10020110
VOID ActivateBinAssetMsg1BT1(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg1BT1);
}

// 0x10020120
VOID ActivateReleaseBinAssetMsg1BT1(VOID)
{
    atexit(ReleaseBinAssetMsg1BT1Action);
}

// 0x10020130
VOID ReleaseBinAssetMsg1BT1Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg1BT1);
}

// 0x10020140
VOID ActivateBinAssetMsg1CBT0State(VOID)
{
    ActivateBinAssetMsg1CBT0();
    ActivateReleaseBinAssetMsg1CBT0();
}

// 0x10020150
VOID ActivateBinAssetMsg1CBT0(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg1CBT0);
}

// 0x10020160
VOID ActivateReleaseBinAssetMsg1CBT0(VOID)
{
    atexit(ReleaseBinAssetMsg1CBT0Action);
}

// 0x10020170
VOID ReleaseBinAssetMsg1CBT0Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg1CBT0);
}

// 0x10020180
VOID ActivateBinAssetMsg1CBT1State(VOID)
{
    ActivateBinAssetMsg1CBT1();
    ActivateReleaseBinAssetMsg1CBT1();
}

// 0x10020190
VOID ActivateBinAssetMsg1CBT1(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg1CBT1);
}

// 0x100201a0
VOID ActivateReleaseBinAssetMsg1CBT1(VOID)
{
    atexit(ReleaseBinAssetMsg1CBT1Action);
}

// 0x100201b0
VOID ReleaseBinAssetMsg1CBT1Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg1CBT1);
}

// 0x100201c0
VOID ActivateBinAssetMsg2BKState(VOID)
{
    ActivateBinAssetMsg2BK();
    ActivateReleaseBinAssetMsg2BK();
}

// 0x100201d0
VOID ActivateBinAssetMsg2BK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg2BK);
}

// 0x100201e0
VOID ActivateReleaseBinAssetMsg2BK(VOID)
{
    atexit(ReleaseBinAssetMsg2BKAction);
}

// 0x100201f0
VOID ReleaseBinAssetMsg2BKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg2BK);
}

// 0x10020200
VOID ActivateBinAssetMsg2BT10State(VOID)
{
    ActivateBinAssetMsg2BT10();
    ActivateReleaseBinAssetMsg2BT10();
}

// 0x10020210
VOID ActivateBinAssetMsg2BT10(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg2BT10);
}

// 0x10020220
VOID ActivateReleaseBinAssetMsg2BT10(VOID)
{
    atexit(ReleaseBinAssetMsg2BT10Action);
}

// 0x10020230
VOID ReleaseBinAssetMsg2BT10Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg2BT10);
}

// 0x10020240
VOID ActivateBinAssetMsg2BT11State(VOID)
{
    ActivateBinAssetMsg2BT11();
    ActivateReleaseBinAssetMsg2BT11();
}

// 0x10020250
VOID ActivateBinAssetMsg2BT11(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg2BT11);
}

// 0x10020260
VOID ActivateReleaseBinAssetMsg2BT11(VOID)
{
    atexit(ReleaseBinAssetMsg2BT11Action);
}

// 0x10020270
VOID ReleaseBinAssetMsg2BT11Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg2BT11);
}

// 0x10020280
VOID ActivateBinAssetMsg2BT20State(VOID)
{
    ActivateBinAssetMsg2BT20();
    ActivateReleaseBinAssetMsg2BT20();
}

// 0x10020290
VOID ActivateBinAssetMsg2BT20(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg2BT20);
}

// 0x100202a0
VOID ActivateReleaseBinAssetMsg2BT20(VOID)
{
    atexit(ReleaseBinAssetMsg2BT20Action);
}

// 0x100202b0
VOID ReleaseBinAssetMsg2BT20Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg2BT20);
}

// 0x100202c0
VOID ActivateBinAssetMsg2BT21State(VOID)
{
    ActivateBinAssetMsg2BT21();
    ActivateReleaseBinAssetMsg2BT21();
}

// 0x100202d0
VOID ActivateBinAssetMsg2BT21(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Msg2BT21);
}

// 0x100202e0
VOID ActivateReleaseBinAssetMsg2BT21(VOID)
{
    atexit(ReleaseBinAssetMsg2BT21Action);
}

// 0x100202f0
VOID ReleaseBinAssetMsg2BT21Action(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Msg2BT21);
}

// 0x10020300
VOID ActivateBinAssetGreetingsBKState(VOID)
{
    ActivateBinAssetGreetingsBK();
    ActivateReleaseBinAssetGreetingsBK();
}

// 0x10020310
VOID ActivateBinAssetGreetingsBK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.GreetingsBK);
}

// 0x10020320
VOID ActivateReleaseBinAssetGreetingsBK(VOID)
{
    atexit(ReleaseBinAssetGreetingsBKAction);
}

// 0x10020330
VOID ReleaseBinAssetGreetingsBKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.GreetingsBK);
}

// 0x10020340
VOID ActivateBinAssetRombikState(VOID)
{
    ActivateBinAssetRombik();
    ActivateReleaseBinAssetRombik();
}

// 0x10020350
VOID ActivateBinAssetRombik(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Rombik);
}

// 0x10020360
VOID ActivateReleaseBinAssetRombik(VOID)
{
    atexit(ReleaseBinAssetRombikAction);
}

// 0x10020370
VOID ReleaseBinAssetRombikAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Rombik);
}

// 0x10020380
VOID ActivateBinAssetMenuGState(VOID)
{
    ActivateBinAssetMenuG();
    ActivateReleaseBinAssetMenuG();
}

// 0x10020390
VOID ActivateBinAssetMenuG(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MenuG);
}

// 0x100203a0
VOID ActivateReleaseBinAssetMenuG(VOID)
{
    atexit(ReleaseBinAssetMenuGAction);
}

// 0x100203b0
VOID ReleaseBinAssetMenuGAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MenuG);
}

// 0x100203c0
VOID ActivateBinAssetScrollBarState(VOID)
{
    ActivateBinAssetScrollBar();
    ActivateReleaseBinAssetScrollBar();
}

// 0x100203d0
VOID ActivateBinAssetScrollBar(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.ScrollBar);
}

// 0x100203e0
VOID ActivateReleaseBinAssetScrollBar(VOID)
{
    atexit(ReleaseBinAssetScrollBarAction);
}

// 0x100203f0
VOID ReleaseBinAssetScrollBarAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.ScrollBar);
}

// 0x10020400
VOID ActivateBinAssetSingle3SCState(VOID)
{
    ActivateBinAssetSingle3SC();
    ActivateReleaseBinAssetSingle3SC();
}

// 0x10020410
VOID ActivateBinAssetSingle3SC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single3SC);
}

// 0x10020420
VOID ActivateReleaseBinAssetSingle3SC(VOID)
{
    atexit(ReleaseBinAssetSingle3SCAction);
}

// 0x10020430
VOID ReleaseBinAssetSingle3SCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single3SC);
}

// 0x10020440
VOID ActivateBinAssetSingle4SCState(VOID)
{
    ActivateBinAssetSingle4SC();
    ActivateReleaseBinAssetSingle4SC();
}

// 0x10020450
VOID ActivateBinAssetSingle4SC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single4SC);
}

// 0x10020460
VOID ActivateReleaseBinAssetSingle4SC(VOID)
{
    atexit(ReleaseBinAssetSingle4SCAction);
}

// 0x10020470
VOID ReleaseBinAssetSingle4SCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single4SC);
}

// 0x10020480
VOID ActivateBinAssetSingle5TXState(VOID)
{
    ActivateBinAssetSingle5TX();
    ActivateReleaseBinAssetSingle5TX();
}

// 0x10020490
VOID ActivateBinAssetSingle5TX(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single5TX);
}

// 0x100204a0
VOID ActivateReleaseBinAssetSingle5TX(VOID)
{
    atexit(ReleaseBinAssetSingle5TXAction);
}

// 0x100204b0
VOID ReleaseBinAssetSingle5TXAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single5TX);
}

// 0x100204c0
VOID ActivateBinAssetDialSCState(VOID)
{
    ActivateBinAssetDialSC();
    ActivateReleaseBinAssetDialSC();
}

// 0x100204d0
VOID ActivateBinAssetDialSC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.DialSC);
}

// 0x100204e0
VOID ActivateReleaseBinAssetDialSC(VOID)
{
    atexit(ReleaseBinAssetDialSCAction);
}

// 0x100204f0
VOID ReleaseBinAssetDialSCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.DialSC);
}

// 0x10020500
VOID ActivateBinAssetJMulti2SCState(VOID)
{
    ActivateBinAssetJMulti2SC();
    ActivateReleaseBinAssetJMulti2SC();
}

// 0x10020510
VOID ActivateBinAssetJMulti2SC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.JMulti2SC);
}

// 0x10020520
VOID ActivateReleaseBinAssetJMulti2SC(VOID)
{
    atexit(ReleaseBinAssetJMulti2SCAction);
}

// 0x10020530
VOID ReleaseBinAssetJMulti2SCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.JMulti2SC);
}

// 0x10020540
VOID ActivateBinAssetSingle0SCState(VOID)
{
    ActivateBinAssetSingle0SC();
    ActivateReleaseBinAssetSingle0SC();
}

// 0x10020550
VOID ActivateBinAssetSingle0SC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single0SC);
}

// 0x10020560
VOID ActivateReleaseBinAssetSingle0SC(VOID)
{
    atexit(ReleaseBinAssetSingle0SCAction);
}

// 0x10020570
VOID ReleaseBinAssetSingle0SCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single0SC);
}

// 0x10020580
VOID ActivateBinAssetSingle0DifState(VOID)
{
    ActivateBinAssetSingle0Dif();
    ActivateReleaseBinAssetSingle0Dif();
}

// 0x10020590
VOID ActivateBinAssetSingle0Dif(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Single0Dif);
}

// 0x100205a0
VOID ActivateReleaseBinAssetSingle0Dif(VOID)
{
    atexit(ReleaseBinAssetSingle0DifAction);
}

// 0x100205b0
VOID ReleaseBinAssetSingle0DifAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Single0Dif);
}

// 0x100205c0
VOID ActivateBinAssetRatingSCState(VOID)
{
    ActivateBinAssetRatingSC();
    ActivateReleaseBinAssetRatingSC();
}

// 0x100205d0
VOID ActivateBinAssetRatingSC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.RatingSC);
}

// 0x100205e0
VOID ActivateReleaseBinAssetRatingSC(VOID)
{
    atexit(ReleaseBinAssetRatingSCAction);
}

// 0x100205f0
VOID ReleaseBinAssetRatingSCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.RatingSC);
}

// 0x10020600
VOID ActivateBinAssetMulti2SCState(VOID)
{
    ActivateBinAssetMulti2SC();
    ActivateReleaseBinAssetMulti2SC();
}

// 0x10020610
VOID ActivateBinAssetMulti2SC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi2SC);
}

// 0x10020620
VOID ActivateReleaseBinAssetMulti2SC(VOID)
{
    atexit(ReleaseBinAssetMulti2SCAction);
}

// 0x10020630
VOID ReleaseBinAssetMulti2SCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi2SC);
}

// 0x10020640
VOID ActivateBinAssetMulti3SCState(VOID)
{
    ActivateBinAssetMulti3SC();
    ActivateReleaseBinAssetMulti3SC();
}

// 0x10020650
VOID ActivateBinAssetMulti3SC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi3SC);
}

// 0x10020660
VOID ActivateReleaseBinAssetMulti3SC(VOID)
{
    atexit(ReleaseBinAssetMulti3SCAction);
}

// 0x10020670
VOID ReleaseBinAssetMulti3SCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi3SC);
}

// 0x10020680
VOID ActivateBinAssetMulti3OPState(VOID)
{
    ActivateBinAssetMulti3OP();
    ActivateReleaseBinAssetMulti3OP();
}

// 0x10020690
VOID ActivateBinAssetMulti3OP(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi3OP);
}

// 0x100206a0
VOID ActivateReleaseBinAssetMulti3OP(VOID)
{
    atexit(ReleaseBinAssetMulti3OPAction);
}

// 0x100206b0
VOID ReleaseBinAssetMulti3OPAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi3OP);
}

// 0x100206c0
VOID ActivateBinAssetMulti4ParmBKState(VOID)
{
    ActivateBinAssetMulti4ParmBK();
    ActivateReleaseBinAssetMulti4ParmBK();
}

// 0x100206d0
VOID ActivateBinAssetMulti4ParmBK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4ParmBK);
}

// 0x100206e0
VOID ActivateReleaseBinAssetMulti4ParmBK(VOID)
{
    atexit(ReleaseBinAssetMulti4ParmBKAction);
}

// 0x100206f0
VOID ReleaseBinAssetMulti4ParmBKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4ParmBK);
}

// 0x10020700
VOID ActivateBinAssetMulti4ParmNMState(VOID)
{
    ActivateBinAssetMulti4ParmNM();
    ActivateReleaseBinAssetMulti4ParmNM();
}

// 0x10020710
VOID ActivateBinAssetMulti4ParmNM(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4ParmNM);
}

// 0x10020720
VOID ActivateReleaseBinAssetMulti4ParmNM(VOID)
{
    atexit(ReleaseBinAssetMulti4ParmNMAction);
}

// 0x10020730
VOID ReleaseBinAssetMulti4ParmNMAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4ParmNM);
}

// 0x10020740
VOID ActivateBinAssetMulti4ParmOPState(VOID)
{
    ActivateBinAssetMulti4ParmOP();
    ActivateReleaseBinAssetMulti4ParmOP();
}

// 0x10020750
VOID ActivateBinAssetMulti4ParmOP(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4ParmOP);
}

// 0x10020760
VOID ActivateReleaseBinAssetMulti4ParmOP(VOID)
{
    atexit(ReleaseBinAssetMulti4ParmOPAction);
}

// 0x10020770
VOID ReleaseBinAssetMulti4ParmOPAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4ParmOP);
}

// 0x10020780
VOID ActivateBinAssetMsgBoxState(VOID)
{
    ActivateBinAssetMsgBox();
    ActivateReleaseBinAssetMsgBox();
}

// 0x10020790
VOID ActivateBinAssetMsgBox(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MsgBox);
}

// 0x100207a0
VOID ActivateReleaseBinAssetMsgBox(VOID)
{
    atexit(ReleaseBinAssetMsgBoxAction);
}

// 0x100207b0
VOID ReleaseBinAssetMsgBoxAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MsgBox);
}

// 0x100207c0
VOID ActivateBinAssetCreditsState(VOID)
{
    ActivateBinAssetCredits();
    ActivateReleaseBinAssetCredits();
}

// 0x100207d0
VOID ActivateBinAssetCredits(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Credits);
}

// 0x100207e0
VOID ActivateReleaseBinAssetCredits(VOID)
{
    atexit(ActivateBinAssetCreditsAction);
}

// 0x100207f0
VOID ActivateBinAssetCreditsAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Credits);
}

// 0x10020800
VOID ActivateBinAssetZvezdyState(VOID)
{
    ActivateBinAssetZvezdy();
    ActivateReleaseBinAssetZvezdy();
}

// 0x10020810
VOID ActivateBinAssetZvezdy(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Zvezdy);
}

// 0x10020820
VOID ActivateReleaseBinAssetZvezdy(VOID)
{
    atexit(ReleaseBinAssetZvezdyAction);
}

// 0x10020830
VOID ReleaseBinAssetZvezdyAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Zvezdy);
}

// 0x10020840
VOID ActivateBinAssetZVMState(VOID)
{
    ActivateBinAssetZVM();
    ActivateReleaseBinAssetZVM();
}

// 0x10020850
VOID ActivateBinAssetZVM(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.ZVM);
}

// 0x10020860
VOID ActivateReleaseBinAssetZVM(VOID)
{
    atexit(ReleaseBinAssetZVMAction);
}

// 0x10020870
VOID ReleaseBinAssetZVMAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.ZVM);
}

// 0x10020880
VOID ActivateBinAssetZVGoldState(VOID)
{
    ActivateBinAssetZVGold();
    ActivateReleaseBinAssetZVGold();
}

// 0x10020890
VOID ActivateBinAssetZVGold(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.ZVGold);
}

// 0x100208a0
VOID ActivateReleaseBinAssetZVGold(VOID)
{
    atexit(ReleaseBinAssetZVGoldAction);
}

// 0x100208b0
VOID ReleaseBinAssetZVGoldAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.ZVGold);
}

// 0x100208c0
VOID ActivateBinAssetMulti4COState(VOID)
{
    ActivateBinAssetMulti4CO();
    ActivateReleaseBinAssetMulti4CO();
}

// 0x100208d0
VOID ActivateBinAssetMulti4CO(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4CO);
}

// 0x100208e0
VOID ActivateReleaseBinAssetMulti4CO(VOID)
{
    atexit(ReleaseBinAssetMulti4COAction);
}

// 0x100208f0
VOID ReleaseBinAssetMulti4COAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4CO);
}

// 0x10020900
VOID ActivateBinAssetMulti4LOPState(VOID)
{
    ActivateBinAssetMulti4LOP();
    ActivateReleaseBinAssetMulti4LOP();
}

// 0x10020910
VOID ActivateBinAssetMulti4LOP(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4LOP);
}

// 0x10020920
VOID ActivateReleaseBinAssetMulti4LOP(VOID)
{
    atexit(ReleaseBinAssetMulti4LOPAction);
}

// 0x10020930
VOID ReleaseBinAssetMulti4LOPAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4LOP);
}

// 0x10020940
VOID ActivateBinAssetMulti4NOPState(VOID)
{
    ActivateBinAssetMulti4NOP();
    ActivateReleaseBinAssetMulti4NOP();
}

// 0x10020950
VOID ActivateBinAssetMulti4NOP(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4NOP);
}

// 0x10020960
VOID ActivateReleaseBinAssetMulti4NOP(VOID)
{
    atexit(ReleaseBinAssetMulti4NOPAction);
}

// 0x10020970
VOID ReleaseBinAssetMulti4NOPAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4NOP);
}

// 0x10020980
VOID ActivateBinAssetMulti4ROPState(VOID)
{
    ActivateBinAssetMulti4ROP();
    ActivateReleaseBinAssetMulti4ROP();
}

// 0x10020990
VOID ActivateBinAssetMulti4ROP(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4ROP);
}

// 0x100209a0
VOID ActivateReleaseBinAssetMulti4ROP(VOID)
{
    atexit(ReleaseBinAssetMulti4ROPAction);
}

// 0x100209b0
VOID ReleaseBinAssetMulti4ROPAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4ROP);
}

// 0x100209c0
VOID ActivateBinAssetMulti4TOPState(VOID)
{
    ActivateBinAssetMulti4TOP();
    ActivateReleaseBinAssetMulti4TOP();
}

// 0x100209d0
VOID ActivateBinAssetMulti4TOP(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4TOP);
}

// 0x100209e0
VOID ActivateReleaseBinAssetMulti4TOP(VOID)
{
    atexit(ReleaseBinAssetMulti4TOPAction);
}

// 0x100209f0
VOID ReleaseBinAssetMulti4TOPAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4TOP);
}

// 0x10020a00
VOID ActivateBinAssetMulti4SCState(VOID)
{
    ActivateBinAssetMulti4SC();
    ActivateReleaseBinAssetMulti4SC();
}

// 0x10020a10
VOID ActivateBinAssetMulti4SC(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4SC);
}

// 0x10020a20
VOID ActivateReleaseBinAssetMulti4SC(VOID)
{
    atexit(ReleaseBinAssetMulti4SCAction);
}

// 0x10020a30
VOID ReleaseBinAssetMulti4SCAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4SC);
}

// 0x10020a40
VOID ActivateBinAssetMulti4SPState(VOID)
{
    ActivateBinAssetMulti4SP();
    ActivateReleaseBinAssetMulti4SP();
}

// 0x10020a50
VOID ActivateBinAssetMulti4SP(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.Multi4SP);
}

// 0x10020a60
VOID ActivateReleaseBinAssetMulti4SP(VOID)
{
    atexit(ReleaseBinAssetMulti4SPAction);
}

// 0x10020a70
VOID ReleaseBinAssetMulti4SPAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.Multi4SP);
}

// 0x10020a80
VOID ActivateBinAssetMainRESState(VOID)
{
    ActivateBinAssetMainRES();
    ActivateReleaseBinAssetMainRES();
}

// 0x10020a90
VOID ActivateBinAssetMainRES(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MainRES);
}

// 0x10020aa0
VOID ActivateReleaseBinAssetMainRES(VOID)
{
    atexit(ReleaseBinAssetMainRESAction);
}

// 0x10020ab0
VOID ReleaseBinAssetMainRESAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MainRES);
}

// 0x10020ac0
VOID ActivateBinAssetMainOPState(VOID)
{
    ActivateBinAssetMainOP();
    ActivateReleaseBinAssetMainOP();
}

// 0x10020ad0
VOID ActivateBinAssetMainOP(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MainOP);
}

// 0x10020ae0
VOID ActivateReleaseBinAssetMainOP(VOID)
{
    atexit(ReleaseBinAssetMainOPAction);
}

// 0x10020af0
VOID ReleaseBinAssetMainOPAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MainOP);
}

// 0x10020b00
VOID ActivateBinAssetKartaBKState(VOID)
{
    ActivateBinAssetKartaBK();
    ActivateReleaseBinAssetKartaBK();
}

// 0x10020b10
VOID ActivateBinAssetKartaBK(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.KartaBK);
}

// 0x10020b20
VOID ActivateReleaseBinAssetKartaBK(VOID)
{
    atexit(ReleaseBinAssetKartaBKAction);
}

// 0x10020b30
VOID ReleaseBinAssetKartaBKAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.KartaBK);
}

// 0x10020b40
VOID ActivateFontAssetComicState(VOID)
{
    ActivateFontAssetComic();
    ActivateReleaseFontAssetComic();
}

// 0x10020b50
VOID ActivateFontAssetComic(VOID)
{
    ActivateFontAsset(&AssetsState.Fonts.Comic);
}

// 0x10020b80
VOID ActivateReleaseFontAssetComic(VOID)
{
    atexit(ActivateReleaseFontAssetComicAction);
}

// 0x10020b90
VOID ActivateReleaseFontAssetComicAction(VOID)
{
    DisposeFontAsset(&AssetsState.Fonts.Comic);
}

// 0x10020ba0
VOID ActivateFontAssetMainState(VOID)
{
    ActivateFontAssetMain();
    ActivateReleaseFontAssetMain();
}

// 0x10020bb0
VOID ActivateFontAssetMain(VOID)
{
    ActivateFontAsset(&AssetsState.Fonts.Main);
}

// 0x10020bc0
VOID ActivateReleaseFontAssetMain(VOID)
{
    atexit(ReleaseFontAssetMainAction);
}

// 0x10020bd0
VOID ReleaseFontAssetMainAction(VOID)
{
    DisposeFontAsset(&AssetsState.Fonts.Main);
}

// 0x10020be0
VOID ActivateBinAssetMN2TextsState(VOID)
{
    ActivateBinAssetMN2Texts();
    ActivateReleaseBinAssetMN2Texts();
}

// 0x10020bf0
VOID ActivateBinAssetMN2Texts(VOID)
{
    ActivateBinAsset(&AssetsState.Assets.MN2Texts);
}

// 0x10020c00
VOID ActivateReleaseBinAssetMN2Texts(VOID)
{
    atexit(ReleaseBinAssetMN2TextsAction);
}

// 0x10020c10
VOID ReleaseBinAssetMN2TextsAction(VOID)
{
    DisposeBinAsset(&AssetsState.Assets.MN2Texts);
}

// 0x10020de0
VOID ActivateCursorState(VOID)
{
    ActivateCursor();
    ActivateReleaseCursor();
}

// 0x10020df0
VOID ActivateCursor(VOID)
{
    ActivateCursor(&CursorState.Cursor);
}

// 0x10020e00
VOID ActivateReleaseCursor(VOID)
{
    atexit(ReleaseCursorAction);
}

// 0x10020e10
VOID ReleaseCursorAction(VOID)
{
    DisposeCursor(&CursorState.Cursor);
}

// 0x10021ee0
// 0x10021ef0
VOID ActivateWindowState(VOID)
{
    ActivateWindowState(&WindowState.Window, InitializeModuleAction, ExecuteModuleAction, ReleaseModuleAction);
}

// 0x10023a30
VOID ActivateInputInitializeState(VOID)
{
    ActivateInputInitialize();
    ActivateReleaseInputInitialize();
}

// 0x10023a40
VOID ActivateInputInitialize(VOID)
{
    InitializeActionHandler(&InputState.Initialize, &ActionState.Initialize, INPUT_ACTION_HANDLER_PRIORITY, InitializeDirectInputAction);
}

// 0x10023a60
VOID ActivateReleaseInputInitialize(VOID)
{
    atexit(ReleaseInputInitializeAction);
}

// 0x10023a70
VOID ReleaseInputInitializeAction(VOID)
{
    ReleaseActionHandler(&InputState.Initialize);
}

// 0x10023a80
VOID ActivateInputExecuteState(VOID)
{
    ActivateInputExecute();
    ActivateReleaseInputExecute();
}

// 0x10023a90
VOID ActivateInputExecute(VOID)
{
    InitializeActionHandler(&InputState.Execute, &ActionState.Execute, INPUT_ACTION_HANDLER_PRIORITY, AcquireDirectInputAction);
}

// 0x10023ab0
VOID ActivateReleaseInputExecute(VOID)
{
    atexit(ReleaseInputExecuteAction);
}

// 0x10023ac0
VOID ReleaseInputExecuteAction(VOID)
{
    ReleaseActionHandler(&InputState.Execute);
}

// 0x10023ad0
VOID ActivateInputReleaseState(VOID)
{
    ActivateInputRelease();
    ActivateReleaseInputRelease();
}

// 0x10023ae0
VOID ActivateInputRelease(VOID)
{
    InitializeActionHandler(&InputState.Release, &ActionState.Release, INPUT_ACTION_HANDLER_PRIORITY, ReleaseDirectInputAction);
}

// 0x10023b00
VOID ActivateReleaseInputRelease(VOID)
{
    atexit(ReleaseInputReleaseAction);
}

// 0x10023b10
VOID ReleaseInputReleaseAction(VOID)
{
    ReleaseActionHandler(&InputState.Release);
}

// 0x10023b20
VOID ActivateInputMessageState(VOID)
{
    ActivateInputMessage();
    ActivateReleaseInputMessage();
}

// 0x10023b30
VOID ActivateInputMessage(VOID)
{
    InitializeActionHandler(&InputState.Message, &ActionState.Message, INPUT_ACTION_HANDLER_PRIORITY, (ACTIONHANDLERLAMBDA)InputMessageAction);
}

// 0x10023b50
VOID ActivateReleaseInputMessage(VOID)
{
    atexit(ReleaseInputMessageAction);
}

// 0x10023b60
VOID ReleaseInputMessageAction(VOID)
{
    ReleaseActionHandler(&InputState.Message);
}

// 0x10024610
// 0x10024620
VOID ActivateBinFileState(VOID)
{
    for (U32 x = 0; x < MAX_BINARY_FILE_COUNT; x++)
    {
        AssetsState.Files[x].File.Value = INVALID_BINFILE_VALUE;
    }
}

// 0x10024640
// 0x10024650
VOID ActivateBinArchiveState(VOID)
{
    for (U32 x = 0; x < MAX_BINARY_ARCHIVE_COUNT; x++)
    {
        AssetsState.Archives[x].File.Value = INVALID_BINFILE_VALUE;
    }
}