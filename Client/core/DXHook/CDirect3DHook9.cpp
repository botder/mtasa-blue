/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDirect3DHook9.cpp
 *  PURPOSE:     Function hooker for Direct3D 9
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <SharedUtil.Detours.h>

#if __has_include(<d3d9on12.h>)
    #include <d3d9on12.h>
#else
    #define MAX_D3D9ON12_QUEUES 2

    typedef struct _D3D9ON12_ARGS
    {
        BOOL      Enable9On12;
        IUnknown* pD3D12Device;
        IUnknown* ppD3D12Queues[MAX_D3D9ON12_QUEUES];
        UINT      NumQueues;
        UINT      NodeMask;
    } D3D9ON12_ARGS;
#endif

template <>
CDirect3DHook9* CSingleton<CDirect3DHook9>::m_pSingleton = NULL;

CDirect3DHook9::CDirect3DHook9()
{
    WriteDebugEvent("CDirect3DHook9::CDirect3DHook9");

    m_pfnDirect3DCreate9 = NULL;
    m_bDirect3DCreate9Suspended = false;
}

CDirect3DHook9::~CDirect3DHook9()
{
    WriteDebugEvent("CDirect3DHook9::~CDirect3DHook9");

    m_pfnDirect3DCreate9 = NULL;
}

bool CDirect3DHook9::ApplyHook()
{
    if (UsingAltD3DSetup())
        return true;

    // Hook Direct3DCreate9.
    if (!m_pfnDirect3DCreate9)
    {
        bool success = DetourLibraryFunction("d3d9.dll", "Direct3DCreate9", m_pfnDirect3DCreate9, API_Direct3DCreate9);

        if (m_pfnDirect3DCreate9 == nullptr || !success)
        {
            BrowseToSolution("d3dapplyhook-fail", EXIT_GAME_FIRST | ASK_GO_ONLINE, "There was a problem hooking Direct3DCreate9");
        }

        WriteDebugEvent(SString("Direct3D9 hook applied %08x", m_pfnDirect3DCreate9));
    }
    else
    {
        WriteDebugEvent("Direct3D9 hook resumed.");
        m_bDirect3DCreate9Suspended = false;
    }
    return true;
}

bool CDirect3DHook9::RemoveHook()
{
    if (UsingAltD3DSetup())
        return true;

    m_bDirect3DCreate9Suspended = true;
    WriteDebugEvent("Direct3D9 hook suspended.");
    return true;
}

IDirect3D9* CDirect3DHook9::API_Direct3DCreate9(UINT SDKVersion)
{
    // Get our self instance.
    CDirect3DHook9* pThis = CDirect3DHook9::GetSingletonPtr();
    assert(pThis && "API_Direct3DCreate9: No CDirect3DHook9");

    if (pThis->m_bDirect3DCreate9Suspended)
        return pThis->m_pfnDirect3DCreate9(SDKVersion);

    // A little hack to get past the loading time required to decrypt the gta
    // executable into memory...
    if (!CCore::GetSingleton().AreModulesLoaded())
    {
        CCore::GetSingleton().SetModulesLoaded(true);
        CCore::GetSingleton().CreateNetwork();
        CCore::GetSingleton().CreateGame();
        CCore::GetSingleton().CreateMultiplayer();
        CCore::GetSingleton().CreateXML();
        CCore::GetSingleton().CreateGUI();
        CCore::GetSingleton().ResetDiscordRichPresence();
    }

    // D3DX_SDK_VERSION checks
    // August 2009 SDK required for shaders to work properly
    #if D3DX_SDK_VERSION != 42
        WriteDebugEvent("D3DX_SDK_VERSION incorrect " QUOTE_DEFINE(D3DX_SDK_VERSION));
        #pragma message( "WARNING: Microsoft DirectX SDK (August 2009) includes missing" )
        #ifndef CI_BUILD
            #ifndef MTA_DEBUG
                #error "Microsoft DirectX SDK (August 2009) includes missing"
            #endif
        #endif
    #endif

    if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION))
    {
        SString strMessage("D3DXCheckVersion FAILED (D3D_SDK_VERSION: %d  D3DX_SDK_VERSION: %d  SDKVersion: %d)", D3D_SDK_VERSION, D3DX_SDK_VERSION,
                           SDKVersion);
        WriteDebugEvent(strMessage);
        AddReportLog(9640, strMessage);
    }
    else
    {
        SString strMessage("D3DXCheckVersion success (D3D_SDK_VERSION: %d  D3DX_SDK_VERSION: %d  SDKVersion: %d)", D3D_SDK_VERSION, D3DX_SDK_VERSION,
                           SDKVersion);
        WriteDebugEvent(strMessage);
    }

    Sleep(3000);
    IDirect3D9* pDirect3D9 = nullptr;

    // Try to create our interface with Direct3DCreate9On12
    using ProtoDirect3DCreate9On12 = IDirect3D9*(WINAPI*)(UINT, D3D9ON12_ARGS*, UINT);
    auto FnDirect3DCreate9On12 = reinterpret_cast<ProtoDirect3DCreate9On12>(GetProcAddress(GetModuleHandleA("d3d9.dll"), "Direct3DCreate9On12"));
    auto useDirect3D9on12 = CVARS_GET_VALUE<bool>("use_d3d9on12");

    SString strMessage("Direct3DCreate9On12 feature test (enabled: %s, function: %p)", (useDirect3D9on12 ? "yes" : "no"), FnDirect3DCreate9On12);
    WriteDebugEvent(strMessage);
    AddReportLog(9642, strMessage);

    if (useDirect3D9on12 && FnDirect3DCreate9On12)
    {
        WriteDebugEvent("Calling Direct3DCreate9On12");

        D3D9ON12_ARGS arg;
        ZeroMemory(&arg, sizeof(arg));
        arg.Enable9On12 = TRUE;

        pDirect3D9 = FnDirect3DCreate9On12(SDKVersion, &arg, 1);

        if (pDirect3D9)
            WriteDebugEvent("Direct3DCreate9On12 succeeded");
        else
            WriteDebugEvent("Direct3DCreate9On12 failed");
    }

    // Try to create our interface with Direct3DCreate9Ex
    auto FnDirect3DCreate9Ex = reinterpret_cast<decltype(&Direct3DCreate9Ex)>(GetProcAddress(GetModuleHandleA("d3d9.dll"), "Direct3DCreate9Ex"));
    auto useDirect3D9Ex = CVARS_GET_VALUE<bool>("use_d3d9ex");

    strMessage = SString("Direct3DCreate9Ex feature test (enabled: %s, function: %p)", (useDirect3D9Ex ? "yes" : "no"), FnDirect3DCreate9Ex);
    WriteDebugEvent(strMessage);
    AddReportLog(9643, strMessage);

    if (!pDirect3D9 && useDirect3D9Ex && FnDirect3DCreate9Ex)
    {
        WriteDebugEvent("Calling Direct3DCreate9Ex");

        IDirect3D9Ex* pDirect3D9Ex = nullptr;
        HRESULT       hr = FnDirect3DCreate9Ex(SDKVersion, &pDirect3D9Ex);

        if (SUCCEEDED(hr) && pDirect3D9Ex)
        {
            WriteDebugEvent("Direct3DCreate9Ex succeeded");
            pDirect3D9 = pDirect3D9Ex;
        }
        else
        {
            WriteDebugEvent("Direct3DCreate9Ex failed");
        }
    }

    // Create our interface with Direct3DCreate9 (default)
    if (!pDirect3D9)
    {
        WriteDebugEvent("Calling Direct3DCreate9");
        pDirect3D9 = pThis->m_pfnDirect3DCreate9(SDKVersion);

        if (pDirect3D9)
            WriteDebugEvent("Direct3DCreate9 succeeded");
        else
            WriteDebugEvent("Direct3DCreate9 failed");
    }

    if (!pDirect3D9)
    {
        MessageBoxUTF8(NULL,
                       _("Could not initialize Direct3D9.\n\n"
                         "Please ensure the DirectX End-User Runtime and\n"
                         "latest Windows Service Packs are installed correctly."),
                       _("Error") + _E("CC50"),
                       MB_OK | MB_ICONERROR | MB_TOPMOST);            // Could not initialize Direct3D9.  Please ensure the DirectX End-User Runtime and latest
                                                                      // Windows Service Packs are installed correctly.
        return NULL;
    }

    // Create a proxy device.
    return new CProxyDirect3D9(pDirect3D9);
}
