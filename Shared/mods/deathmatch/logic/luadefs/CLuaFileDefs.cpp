/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaFileDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <filesystem>

#ifdef MTA_CLIENT
    #include "CResource.h"
#else
    #include "Resource.h"
    #include "ResourceFilePath.h"
    #include "ScriptFile.h"
#endif

#define DEFAULT_MAX_FILESIZE 52428800

namespace fs = std::filesystem;

using namespace mtasa;

void CLuaFileDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"fileOpen", fileOpen},
        {"fileCreate", fileCreate},
        {"fileExists", fileExists},
        {"fileCopy", fileCopy},
        {"fileRename", fileRename},
        {"fileDelete", fileDelete},

        {"fileClose", fileClose},
        {"fileFlush", fileFlush},
        {"fileRead", fileRead},
        {"fileWrite", fileWrite},

        {"fileGetPos", fileGetPos},
        {"fileGetSize", fileGetSize},
        {"fileGetPath", fileGetPath},
        {"fileIsEOF", fileIsEOF},

        {"fileSetPos", fileSetPos},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaFileDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classmetamethod(luaVM, "__gc", fileCloseGC);

#ifdef MTA_CLIENT
    lua_classfunction(luaVM, "create", CLuaFileDefs::File);
#else
    lua_classfunction(luaVM, "create", "fileCreate", CLuaFileDefs::File);
#endif

    lua_classfunction(luaVM, "open", "fileOpen");
    lua_classfunction(luaVM, "new", "fileCreate");
    lua_classfunction(luaVM, "exists", "fileExists");
    lua_classfunction(luaVM, "copy", "fileCopy");
    lua_classfunction(luaVM, "rename", "fileRename");
    lua_classfunction(luaVM, "delete", "fileDelete");

    lua_classfunction(luaVM, "close", "fileClose");
    lua_classfunction(luaVM, "flush", "fileFlush");
    lua_classfunction(luaVM, "read", "fileRead");
    lua_classfunction(luaVM, "write", "fileWrite");

    lua_classfunction(luaVM, "getPos", "fileGetPos");
    lua_classfunction(luaVM, "getSize", "fileGetSize");
    lua_classfunction(luaVM, "getPath", "fileGetPath");
    lua_classfunction(luaVM, "isEOF", "fileIsEOF");

    lua_classfunction(luaVM, "setPos", "fileSetPos");

    lua_classvariable(luaVM, "pos", "fileSetPos", "fileGetPos");
    lua_classvariable(luaVM, "size", nullptr, "fileGetSize");
    lua_classvariable(luaVM, "eof", nullptr, "fileIsEOF");
    lua_classvariable(luaVM, "path", nullptr, "fileGetPath");

    lua_registerclass(luaVM, "File");
}

int CLuaFileDefs::File(lua_State* luaVM)
{
    std::string_view filePath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(filePath);

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> file = ParseResourceFilePath(filePath, self);

            if (file.has_value())
            {
                CheckCanModifyOtherResource(argStream, self, file->resource);
                CheckCanAccessOtherResourceFile(argStream, self, file->resource, file->absolutePath);

                if (!argStream.HasErrors())
                {
                    OpenScriptFileMode fileMode = OpenScriptFileMode::READ_WRITE;

                    if (std::error_code errorCode; !fs::exists(file->absolutePath, errorCode))
                        fileMode = OpenScriptFileMode::CREATE;

#ifdef MTA_CLIENT
                    if (fileMode == OpenScriptFileMode::CREATE)
                    {
                        if (!g_pNet->ValidateBinaryFileName(strInputPath))
                        {
                            argStream.SetCustomError(SString("Filename not allowed %.*s", filePath.size(), filePath.data()), "File error");
                            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }

                        g_pClientGame->GetResourceManager()->OnFileModifedByScript(strAbsPath, "File");
                    }

                    eAccessType  accessType = strInputPath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                    CScriptFile* pFile = new CScriptFile(pThisResource->GetScriptID(), strMetaPath, DEFAULT_MAX_FILESIZE, accessType);
#else
                    auto scriptFile = std::make_unique<ScriptFile>(file->absolutePath, file->relativePath);
                    scriptFile->SetOwnerResource(self);
                    scriptFile->SetRelativeToResource(file->resource);
                    scriptFile->SetMaxFileSize(DEFAULT_MAX_FILESIZE);
#endif

                    if (scriptFile->Load(fileMode))
                    {
#ifdef MTA_CLIENT
                        // Make it a child of the resource's file root
                        pFile->SetParent(pResource->GetResourceDynamicEntity());
#endif
                        if (CElementGroup* elementGroup = self->GetElementGroup(); elementGroup != nullptr)
                            elementGroup->Add(scriptFile.get());

                        lua_pushelement(luaVM, scriptFile.release());
                        return 1;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("unable to load file '%.*s'", filePath.size(), filePath.data()));
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileOpen(lua_State* luaVM)
{
    //  file fileOpen ( string filePath [, bool readOnly = false ] )
    std::string_view filePath;
    bool             isReadOnly;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(filePath);
    argStream.ReadBool(isReadOnly, false);

    if (argStream.NextIsUserData())
        m_pScriptDebugging->LogCustom(luaVM, "fileOpen may be using an outdated syntax. Please check and update.");

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> file = ParseResourceFilePath(filePath, self);

            if (file.has_value())
            {
                CheckCanModifyOtherResource(argStream, self, file->resource);
                CheckCanAccessOtherResourceFile(argStream, self, file->resource, file->absolutePath);

                if (!argStream.HasErrors())
                {
#ifdef MTA_CLIENT
                    // TODO: Add implementation here
#else
                    auto scriptFile = std::make_unique<ScriptFile>(file->absolutePath, file->relativePath);
                    scriptFile->SetOwnerResource(self);
                    scriptFile->SetRelativeToResource(file->resource);
                    scriptFile->SetMaxFileSize(DEFAULT_MAX_FILESIZE);
#endif

                    OpenScriptFileMode fileMode = (isReadOnly ? OpenScriptFileMode::READ : OpenScriptFileMode::READ_WRITE);

                    if (scriptFile->Load(fileMode))
                    {
#ifdef MTA_CLIENT
                        // TODO: Add implementation here
                        pFile->SetParent(pResource->GetResourceDynamicEntity());
                        pFile->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#else
                        scriptFile->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#endif

                        if (CElementGroup* elementGroup = self->GetElementGroup(); elementGroup != nullptr)
                            elementGroup->Add(scriptFile.get());

                        lua_pushelement(luaVM, scriptFile.release());
                        return 1;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("unable to load file '%.*s'", filePath.size(), filePath.data()));
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileCreate(lua_State* luaVM)
{
    //  file fileCreate ( string filePath )
    std::string_view filePath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(filePath);

    if (argStream.NextIsUserData())
        m_pScriptDebugging->LogCustom(luaVM, "fileCreate may be using an outdated syntax. Please check and update.");

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> file = ParseResourceFilePath(filePath, self);

            if (file.has_value())
            {
                CheckCanModifyOtherResource(argStream, self, file->resource);
                CheckCanAccessOtherResourceFile(argStream, self, file->resource, file->absolutePath);

                if (!argStream.HasErrors())
                {
#ifdef MTA_CLIENT
                    if (!g_pNet->ValidateBinaryFileName(strInputPath))
                    {
                        argStream.SetCustomError(SString("Filename not allowed %s", *strInputPath), "File error");
                        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                        lua_pushboolean(luaVM, false);
                        return 1;
                    }

                    g_pClientGame->GetResourceManager()->OnFileModifedByScript(strAbsPath, "fileCreate");
#else
                    auto scriptFile = std::make_unique<ScriptFile>(file->absolutePath, file->relativePath);
                    scriptFile->SetOwnerResource(self);
                    scriptFile->SetRelativeToResource(file->resource);
                    scriptFile->SetMaxFileSize(DEFAULT_MAX_FILESIZE);
#endif

                    if (scriptFile->Load(OpenScriptFileMode::CREATE))
                    {
#ifdef MTA_CLIENT
                        pFile->SetParent(pResource->GetResourceDynamicEntity());
                        pFile->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#else
                        scriptFile->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#endif
                        if (CElementGroup* elementGroup = self->GetElementGroup(); elementGroup != nullptr)
                            elementGroup->Add(scriptFile.get());

                        lua_pushelement(luaVM, scriptFile.release());
                        return 1;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Unable to create  %.*s", filePath.size(), filePath.data()));
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileExists(lua_State* luaVM)
{
    //  bool fileExists ( string filePath )
    std::string_view filePath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(filePath);

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> file = ParseResourceFilePath(filePath, self);

            if (file.has_value())
            {
                std::error_code errorCode;
                lua_pushboolean(luaVM, fs::is_regular_file(file->absolutePath, errorCode));
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileCopy(lua_State* luaVM)
{
    //  bool fileCopy ( string filePath, string newFilePath, bool overwrite = false )
    std::string_view sourceFilePath;
    std::string_view destinationFilePath;
    bool             overwriteExisting;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(sourceFilePath);
    argStream.ReadStringView(destinationFilePath);
    argStream.ReadBool(overwriteExisting, false);

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> sourceFile = ParseResourceFilePath(sourceFilePath, self);
            std::optional<ResourceFilePath> destinationFile = ParseResourceFilePath(destinationFilePath, self);

            if (sourceFile.has_value() && destinationFile.has_value())
            {
                CheckCanModifyOtherResources(argStream, self, {sourceFile->resource, destinationFile->resource});
                CheckCanAccessOtherResourceFile(argStream, self, sourceFile->resource, sourceFile->absolutePath);
                CheckCanAccessOtherResourceFile(argStream, self, destinationFile->resource, destinationFile->absolutePath);

                if (!argStream.HasErrors())
                {
#ifdef MTA_CLIENT
                    if (!g_pNet->ValidateBinaryFileName(strInputDestPath))
                    {
                        argStream.SetCustomError(SString("Filename not allowed %s", *strInputDestPath), "File error");
                        m_pScriptDebugging->LogError(luaVM, argStream.GetFullErrorMessage());

                        lua_pushboolean(luaVM, false);
                        return 1;
                    }
#endif
                    if (std::error_code errorCode; fs::is_regular_file(sourceFile->absolutePath, errorCode))
                    {
                        if (overwriteExisting || (!fs::exists(destinationFile->absolutePath, errorCode) && !errorCode))
                        {
#ifdef MTA_CLIENT
                            g_pClientGame->GetResourceManager()->OnFileModifedByScript(strDestAbsPath, "fileCopy");
#endif
                            fs::create_directories(destinationFile->absolutePath.parent_path(), errorCode);

                            if (!errorCode)
                                fs::copy_file(sourceFile->absolutePath, destinationFile->absolutePath, fs::copy_options::overwrite_existing, errorCode);

                            if (errorCode)
                            {
                                argStream.SetCustomError(SString("Unable to copy %.*s to %.*s", sourceFilePath.size(), sourceFilePath.data(),
                                                                 destinationFilePath.size(), destinationFilePath.data()),
                                                         "Operation failed");
                            }
                            else
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                        else
                        {
                            argStream.SetCustomError(SString("Destination file already exists (%.*s)", destinationFilePath.size(), destinationFilePath.data()),
                                                     "Operation failed");
                        }
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Source file doesn't exist (%.*s)", sourceFilePath.size(), sourceFilePath.data()), "Operation failed");
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileRename(lua_State* luaVM)
{
    //  bool fileRename ( string filePath, string newFilePath )
    std::string_view sourceFilePath;
    std::string_view destinationFilePath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(sourceFilePath);
    argStream.ReadStringView(destinationFilePath);

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> sourceFile = ParseResourceFilePath(sourceFilePath, self);
            std::optional<ResourceFilePath> destinationFile = ParseResourceFilePath(destinationFilePath, self);

            if (sourceFile.has_value() && destinationFile.has_value())
            {
                CheckCanModifyOtherResources(argStream, self, {sourceFile->resource, destinationFile->resource});
                CheckCanAccessOtherResourceFile(argStream, self, sourceFile->resource, sourceFile->absolutePath);
                CheckCanAccessOtherResourceFile(argStream, self, destinationFile->resource, destinationFile->absolutePath);

                if (!argStream.HasErrors())
                {
#ifdef MTA_CLIENT
                    if (!g_pNet->ValidateBinaryFileName(strInputDestPath))
                    {
                        argStream.SetCustomError(SString("Filename not allowed %s", *strInputDestPath), "File error");
                        m_pScriptDebugging->LogError(luaVM, argStream.GetFullErrorMessage());

                        lua_pushboolean(luaVM, false);
                        return 1;
                    }
#endif
                    if (std::error_code errorCode; fs::is_regular_file(sourceFile->absolutePath, errorCode))
                    {
                        if (!fs::exists(destinationFile->absolutePath, errorCode) && !errorCode)
                        {
#ifdef MTA_CLIENT
                            g_pClientGame->GetResourceManager()->OnFileModifedByScript(strDestAbsPath, "fileRename");
#endif
                            fs::create_directories(destinationFile->absolutePath.parent_path(), errorCode);

                            if (!errorCode)
                                fs::rename(sourceFile->absolutePath, destinationFile->absolutePath, errorCode);

                            if (errorCode)
                            {
                                m_pScriptDebugging->LogWarning(luaVM, SString("fileRename failed; unable to rename file (Error %d)", errorCode.value()));
                            }
                            else
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                        else
                        {
                            m_pScriptDebugging->LogWarning(luaVM, "fileRename failed; destination file already exists");
                        }
                    }
                    else
                    {
                        m_pScriptDebugging->LogWarning(luaVM, "fileRename failed; source file doesn't exist");
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileDelete(lua_State* luaVM)
{
    //  bool fileDelete ( string filePath )
    std::string_view filePath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(filePath);

    // This is only really necessary server side
    if (argStream.NextIsUserData())
        m_pScriptDebugging->LogCustom(luaVM, "fileDelete may be using an outdated syntax. Please check and update.");

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> file = ParseResourceFilePath(filePath, self);

            if (file.has_value())
            {
                CheckCanModifyOtherResource(argStream, self, file->resource);
                CheckCanAccessOtherResourceFile(argStream, self, file->resource, file->absolutePath);

                if (!argStream.HasErrors())
                {
#ifdef MTA_CLIENT
                    g_pClientGame->GetResourceManager()->OnFileModifedByScript(strAbsPath, "fileDelete");
#endif
                    if (std::error_code errorCode; fs::remove(file->absolutePath, errorCode))
                    {
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    else
                    {
                        argStream.SetCustomError(std::string{filePath}.c_str(), "Operation failed");
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileClose(lua_State* luaVM)
{
    //  bool fileClose ( file theFile )
    ScriptFile* scriptFile = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);

    if (!argStream.HasErrors())
    {
        scriptFile->Unload();
        m_pElementDeleter->Delete(scriptFile);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileFlush(lua_State* luaVM)
{
    //  bool fileFlush ( file theFile )
    ScriptFile*  scriptFile = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);

    if (!argStream.HasErrors())
    {
        scriptFile->Flush();
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileRead(lua_State* luaVM)
{
    //  string fileRead ( file theFile, int count )
    ScriptFile* scriptFile = nullptr;
    std::size_t byteCount = 0;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);
    argStream.ReadNumber(byteCount);

    if (!argStream.HasErrors())
    {
        if (byteCount > 10000)
        {
            std::size_t fileSize = 0;
            std::size_t currentPosition = 0;

            if (scriptFile->TryGetSize(fileSize) && scriptFile->TryGetPosition(currentPosition) && fileSize >= currentPosition)
            {
                std::size_t remainder = fileSize - currentPosition;

                if (remainder < byteCount)
                    byteCount = remainder;
            }
        }

         // Reading zero bytes from a file results in an empty string
        if (byteCount == 0)
        {
            lua_pushstring(luaVM, "");
            return 1;
        }

        // Allocate a buffer to read the stuff into and read some :~ into it
        auto        buffer = std::make_unique<char[]>(byteCount);
        std::size_t bytesRead = 0;

        if (scriptFile->TryRead(byteCount, buffer.get(), byteCount, bytesRead))
        {
            lua_pushlstring(luaVM, buffer.get(), bytesRead);
            return 1;
        }

        m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileWrite(lua_State* luaVM)
{
    //  int fileWrite ( file theFile, string string1 [, string string2, string string3 ...])
    ScriptFile* scriptFile = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);

    // Ensure we have at least one string
    if (!argStream.NextIsString())
        argStream.SetTypeError("string");

    if (!argStream.HasErrors())
    {
        std::size_t totalBytesWritten = 0;

        // While we're not out of string arguments
        // (we will always have at least one string because we validated it above)
        while (argStream.NextIsString())
        {
            std::string_view data;
            argStream.ReadStringView(data);

            if (argStream.HasErrors() || data.empty())
                continue;

            std::size_t bytesWritten = 0;

            if (!scriptFile->TryWrite(data, bytesWritten))
            {
                m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
                lua_pushnil(luaVM);
                return 1;
            }

            totalBytesWritten += bytesWritten;
        }

#ifdef MTA_CLIENT
        // Inform file verifier
        if (lBytesWritten != 0)
            g_pClientGame->GetResourceManager()->OnFileModifedByScript(pFile->GetAbsPath(), "fileWrite");
#endif

        lua_pushnumber(luaVM, totalBytesWritten);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileGetPos(lua_State* luaVM)
{
    //  int fileGetPos ( file theFile )
    ScriptFile* scriptFile = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);

    if (!argStream.HasErrors())
    {
        std::size_t position = 0;

        if (scriptFile->TryGetPosition(position))
        {
            lua_pushnumber(luaVM, position);
            return 1;
        }

        m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileGetSize(lua_State* luaVM)
{
    //  int fileGetSize ( file theFile )
    ScriptFile* scriptFile = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);

    if (!argStream.HasErrors())
    {
        std::size_t size = 0;

        if (scriptFile->TryGetSize(size))
        {
            lua_pushnumber(luaVM, size);
            return 1;
        }

        m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileGetPath(lua_State* luaVM)
{
    //  string fileGetPath ( file theFile )
    ScriptFile* scriptFile = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);

    if (!argStream.HasErrors())
    {
        if (Resource* relativeFrom = m_pLuaManager->GetResourceFromLuaState(luaVM); relativeFrom != nullptr)
        {
            std::string filePath = scriptFile->GetResourceRelativePath(relativeFrom);
            lua_pushlstring(luaVM, filePath.c_str(), filePath.size());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileIsEOF(lua_State* luaVM)
{
    //  bool fileIsEOF ( file theFile )
    ScriptFile* scriptFile = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, scriptFile->IsEOF());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileSetPos(lua_State* luaVM)
{
    //  int fileSetPos ( file theFile, int offset )
    ScriptFile* scriptFile = nullptr;
    std::size_t position = 0;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);
    argStream.ReadNumber(position);

    if (!argStream.HasErrors())
    {
        std::size_t afterSeekPosition = 0;

        if (scriptFile->TrySetPosition(position, afterSeekPosition))
        {
            lua_pushnumber(luaVM, afterSeekPosition);
            return 1;
        }

        m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

// Called by Lua when file userdatas are garbage collected
int CLuaFileDefs::fileCloseGC(lua_State* luaVM)
{
    ScriptFile* scriptFile = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(scriptFile);

    if (!argStream.HasErrors())
    {
        // This file wasn't closed, so we should warn
        // the scripter that they forgot to close it.
        std::string filePath = scriptFile->GetRelativePath();

        m_pScriptDebugging->LogWarning(scriptFile->GetLuaDebugInfo(), "Unclosed file (%.*s) was garbage collected. Check your resource for dereferenced files.",
                                       filePath.size(), filePath.c_str());

        // Close the file and delete it from elements
        scriptFile->Unload();
        m_pElementDeleter->Delete(scriptFile);

        lua_pushboolean(luaVM, true);
        return 1;
    }

    lua_pushnil(luaVM);
    return 1;
}
