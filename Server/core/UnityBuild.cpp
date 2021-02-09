/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Unity build file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "CCrashHandler.cpp"
#include "CCrashHandlerAPI.cpp"
#include "CDynamicLibrary.cpp"
#include "CModManagerImpl.cpp"
#include "CServerImpl.cpp"
#include "CThreadCommandQueue.cpp"
#include "Server.cpp"

#if defined(WIN32)
    #include "CExceptionInformation_Impl.cpp"
#endif
