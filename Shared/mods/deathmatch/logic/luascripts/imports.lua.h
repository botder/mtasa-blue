namespace EmbeddedLuaCode
{
    const char* const imports = R"~LUA~(
--[[
    SERVER AND CLIENT.

    Code for allowing this syntax:
        imports "resourceName"                  -- Adds all exports to _G table
        imports ("resourceName")                -- Adds all exports to _G table
        imports ("resourceName", tableName)     -- Adds all exports to tableName table
        imports ("resourceName", "tableName")   -- Adds all exports to _G["tableName"] table
        imports (resourcePointer)               -- Adds all exports to _G table
        imports (resourcePointer, tableName)    -- Adds all exports to tableName table
        imports (resourcePointer, "tableName")  -- Adds all exports to _G["tableName"] table
--]]

local _G = _G
local type = type
local error = error
local getUserdataType = getUserdataType
local getResourceFromName = getResourceFromName
local getResourceExportedFunctions = getResourceExportedFunctions
local call = call

function imports(resource, into)
    if type(resource) == "userdata" then
        local userdataType = getUserdataType(resource)
        if userdataType ~= "resource-data" then
            error("imports: expected resource or non-empty string at argument 1, got ".. userdataType, 2)
        end
    elseif type(resource) == "string" and resource ~= "" then
        local resourceName = resource
        resource = getResourceFromName(resourceName)
        if not resource then
            error("imports: resource is not running or does not exist (".. resourceName ..")", 2)
        end
    else
        error("imports: expected resource or non-empty string at argument 1, got ".. type(resource), 2)
    end

    if into == nil then
        into = _G
    elseif type(into) == "string" and into ~= "" then
        local target = _G[into]

        if type(target) ~= "table" then
            target = {}
            _G[into] = target
        end

        into = target
    elseif type(into) ~= "table" then
        error("imports: expected table or non-empty string or nil at argument 2, got ".. type(into), 2)
    end

    local exports = getResourceExportedFunctions(resource) or {}

    for i = 1, #exports do
        local name = exports[i]
        into[name] = function (...)
            return call(resource, name, ...)
        end
    end
end
    )~LUA~";
}
