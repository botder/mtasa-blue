/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Enums.h
 *  PURPOSE:     Client/server shared enum definitions
 *
 *****************************************************************************/
#pragma once

namespace EEventPriority
{
    enum EEventPriorityType
    {
        LOW,
        NORMAL,
        HIGH,
    };
}
using EEventPriority::EEventPriorityType;

DECLARE_ENUM(EEventPriority::EEventPriorityType)

namespace EPlayerScreenShotResult
{
    enum EPlayerScreenShotResultType
    {
        NONE,
        SUCCESS,
        MINIMIZED,
        DISABLED,
        ERROR_,
    };
}
using EPlayerScreenShotResult::EPlayerScreenShotResultType;

DECLARE_ENUM(EPlayerScreenShotResult::EPlayerScreenShotResultType)

namespace EDebugHook
{
    enum EDebugHookType
    {
        PRE_EVENT,
        POST_EVENT,
        PRE_FUNCTION,
        POST_FUNCTION,
        PRE_EVENT_FUNCTION,
        POST_EVENT_FUNCTION,
        MAX_DEBUG_HOOK_TYPE
    };
}
using EDebugHook::EDebugHookType;

DECLARE_ENUM(EDebugHook::EDebugHookType);

enum eEulerRotationOrder
{
    EULER_DEFAULT,
    EULER_ZXY,
    EULER_ZYX,
    EULER_MINUS_ZYX,
};

DECLARE_ENUM(eEulerRotationOrder);

DECLARE_ENUM(EHashFunction::EHashFunctionType);
DECLARE_ENUM_CLASS(PasswordHashFunction);
DECLARE_ENUM_CLASS(StringEncryptFunction);

DECLARE_ENUM(ePacketID);

/* Describes the level, at which any resource can mutate an element. The owner of an element (the resource it belongs to) has always full access.
 *
 * PUBLIC:      Every resource can mutate the state (default)
 * SEMI_PUBLIC: Like PUBLIC, but only owner resource can destroy it
 * PROTECTED:   Like SEMI_PUBLIC, but only owner can mutate the state, others can only read
 * PRIVATE:     Element is only visible to the owner resource
 */
enum class eElementAccessLevel : unsigned char
{
    PUBLIC,
    SEMI_PUBLIC,
    PROTECTED,
    PRIVATE,
};
