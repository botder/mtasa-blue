/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Controller pad input logic
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Set values to 128 unless otherwise specified
class CControllerState
{
public:
    signed short LeftStickX = 0;             // move/steer left (-128?)/right (+128)
    signed short LeftStickY = 0;            // move back(+128)/forwards(-128?)
    signed short RightStickX = 0;            // numpad 6(+128)/numpad 4(-128?)
    signed short RightStickY = 0;

    signed short LeftShoulder1 = 0;
    signed short LeftShoulder2 = 0;
    signed short RightShoulder1 = 0;            // target / hand brake
    signed short RightShoulder2 = 0;

    signed short DPadUp = 0;            // radio change up
    signed short DPadDown = 0;            // radio change down
    signed short DPadLeft = 0;
    signed short DPadRight = 0;

    signed short Start = 0;
    signed short Select = 0;

    signed short ButtonSquare = 0;            // jump / reverse
    signed short ButtonTriangle = 0;            // get in/out
    signed short ButtonCross = 0;               // sprint / accelerate
    signed short ButtonCircle = 0;              // fire

    signed short ShockButtonL = 0;
    signed short ShockButtonR = 0;            // look behind

    signed short m_bChatIndicated = 0;
    signed short m_bPedWalk = 0;
    signed short m_bVehicleMouseLook = 0;
    signed short m_bRadioTrackSkip = 0;
};

class CPad
{
public:
    virtual CControllerState* GetCurrentControllerState(CControllerState* ControllerState) = 0;
    virtual CControllerState* GetLastControllerState(CControllerState* ControllerState) = 0;
    virtual void              SetCurrentControllerState(CControllerState* ControllerState) = 0;
    virtual void              SetLastControllerState(CControllerState* ControllerState) = 0;
    virtual void              Restore() = 0;
    virtual void              Store() = 0;
    virtual bool              IsEnabled() = 0;
    virtual void              Disable(bool bDisable) = 0;
    virtual void              Clear() = 0;
    virtual void              SetHornHistoryValue(bool value) = 0;
    virtual long              GetAverageWeapon() = 0;
    virtual void              SetLastTimeTouched(uint32_t dwTime) = 0;
    virtual uint32_t          GetDrunkInputDelay() = 0;
    virtual void              SetDrunkInputDelay(uint32_t inputDelay) = 0;
};
