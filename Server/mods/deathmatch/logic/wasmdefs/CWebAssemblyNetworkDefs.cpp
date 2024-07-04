/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/wasmdefs/CWebAssemblyNetworkDefs.cpp
 *  PURPOSE:     Web assembly definitions base class
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWebAssemblyDefs.h"
#include <SharedUtil.Memory.h>
#include "CDummy.h"
#include "CStaticFunctionDefinitions.h"
#include "Utils.h"
#include "CScriptDebugging.h"

#include "CRemoteCalls.h"
#include "CAccessControlListManager.h"

#include "../wasm/CWebAssemblyContext.h"
#include "../wasm/CWebAssemblyVariable.h"
#include "../wasm/CWebAssemblyArgReader.h"

#include "CWebAssemblyNetworkDefs.h"

struct RemoteRequestInfo
{
    bool                             isFetch;
    CWebAssemblyMemoryPointerAddress url;
    CWebAssemblyMemoryPointerAddress queue;
    CWebAssemblyUserData             resource;
    uint64_t                         start;
    CWebAssemblyMemoryPointerAddress postData;
    uint32_t                         postDataSize;
    CWebAssemblyMemoryPointerAddress headers;
    uint32_t                         headersCount;
    CWebAssemblyMemoryPointerAddress method;
    uint32_t                         connectionAttempts;
    uint32_t                         connectionTimeout;
    uint32_t                         bytesReceived;
    uint32_t                         bytesTotal;
    uint32_t                         currentAttempt;
};

void CWebAssemblyNetworkDefs::RegisterFunctionTypes()
{
    SetFunctionType("call_remote", "ussii*u");
    SetFunctionType("fetch_remote", "us**u");
    SetFunctionType("get_remote_requests", "iu*i");
    SetFunctionType("get_remote_request_info", "bu*ib");
    SetFunctionType("cleanup_remote_request_info", "b*");
    SetFunctionType("abort_remote_request", "bu");
}

void CWebAssemblyNetworkDefs::RegisterFunctions(CWebAssemblyScript* script)
{
    CWebAssemblyCFunctionMap functions = {
        { "call_remote", CallRemote },
        { "fetch_remote", FetchRemote },
        { "get_remote_requests", GetRemoteRequests },
        { "get_remote_request_info", GetRemoteRequestInfo },
        { "cleanup_remote_request_info", CleanupRemoteRequestInfo },
        { "abort_remote_request", AbortRemoteRequest }
    };

    WASM_REGISTER_API(script, functions);
}

WebAssemblyApi(CWebAssemblyNetworkDefs::CallRemote, env, args, results)
{
    SString         url;
    SString         queueName;
    uint32_t        connectionAttempts;
    uint32_t        connectionTimeout;
    CLuaFunctionRef callbackFunction;
    CLuaArguments   arguments;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(url);
    argStream.ReadString(queueName, CALL_REMOTE_DEFAULT_QUEUE_NAME);
    argStream.ReadUInt32(connectionAttempts, 10);
    argStream.ReadUInt32(connectionTimeout, 10000);
    argStream.ReadLuaFunctionRef(callbackFunction);
    argStream.ReadLuaArguments(arguments);

    CLuaMain* luaMain = GetWebAssemblyLuaMain(env);

    if (!luaMain)
    {
        return argStream.ReturnNull();
    }

    CRemoteCall* remoteRequest = g_pGame->GetRemoteCalls()->Call(url, &arguments, luaMain, callbackFunction, queueName, connectionAttempts, connectionTimeout);

    if (!remoteRequest)
    {
        return argStream.ReturnNull();
    }

    return argStream.ReturnSystemPointer(remoteRequest);
}

WebAssemblyApi(CWebAssemblyNetworkDefs::FetchRemote, env, args, results)
{
    SString         url;
    struct
    {
        uint32_t                         connectionAttempts;
        uint32_t                         connectionTimeout;
        CWebAssemblyMemoryPointerAddress method;
        CWebAssemblyMemoryPointerAddress queueName;
        CWebAssemblyMemoryPointerAddress postData;
        bool                             postIsBinary;
        uint32_t                         maxRedirects;
        CWebAssemblyMemoryPointerAddress username;
        CWebAssemblyMemoryPointerAddress password;
        CWebAssemblyMemoryPointerAddress headers;
        uint32_t                         headersCount;
        CWebAssemblyMemoryPointerAddress formFields;
        uint32_t                         formFieldsCount;
    }*              options;
    CLuaFunctionRef callbackFunction;
    CLuaArguments   arguments;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadString(url);
    argStream.ReadPointer(options);
    argStream.ReadLuaFunctionRef(callbackFunction);
    argStream.ReadLuaArguments(arguments);

    CLuaMain*           luaMain = GetWebAssemblyLuaMain(env);
    CWebAssemblyMemory* memory = GetWebAssemblyEnvMemory(env);

    if (!luaMain || !memory || !options)
    {
        return argStream.ReturnNull();
    }

    SString queueName = memory->UTF8ToString(options->queueName);

    SHttpRequestOptions httpOptions;

    httpOptions.uiConnectionAttempts = options->connectionAttempts;
    httpOptions.uiConnectTimeoutMs = options->connectionTimeout;
    httpOptions.strRequestMethod = memory->UTF8ToString(options->method);
    httpOptions.strPostData = memory->UTF8ToString(options->method);
    httpOptions.bPostBinary = options->postIsBinary;
    httpOptions.uiMaxRedirects = options->maxRedirects;
    httpOptions.strUsername = memory->UTF8ToString(options->username);
    httpOptions.strPassword = memory->UTF8ToString(options->password);

    if (options->headers != WEB_ASSEMBLY_NULL_PTR && options->headersCount > 0)
    {
        CWebAssemblyMemoryPointerAddress* headers = (CWebAssemblyMemoryPointerAddress*)memory->GetMemoryPhysicalPointer(options->headers);

        for (uint32_t i = 0; i < (options->headersCount * 2); i += 2)
        {
            SString headerName = memory->UTF8ToString(headers[i]);
            SString headerValue = memory->UTF8ToString(headers[i + 1]);

            httpOptions.requestHeaders[headerName] = headerValue;
        }
    }

    if (options->formFields != WEB_ASSEMBLY_NULL_PTR && options->formFieldsCount > 0)
    {
        CWebAssemblyMemoryPointerAddress* formFields = (CWebAssemblyMemoryPointerAddress*)memory->GetMemoryPhysicalPointer(options->formFields);

        for (uint32_t i = 0; i < (options->formFieldsCount * 2); i += 2)
        {
            SString fieldName = memory->UTF8ToString(formFields[i]);
            SString fieldValue = memory->UTF8ToString(formFields[i + 1]);

            httpOptions.formFields[fieldName] = fieldValue;
        }
    }

    CRemoteCall* remoteRequest = g_pGame->GetRemoteCalls()->Call(url, &arguments, luaMain, callbackFunction, queueName, httpOptions);

    if (!remoteRequest)
    {
        return argStream.ReturnNull();
    }

    return argStream.ReturnSystemPointer(remoteRequest);
}

WebAssemblyApi(CWebAssemblyNetworkDefs::GetRemoteRequests, env, args, results)
{
    CResource*                       resource;
    CWebAssemblyMemoryPointerAddress outRemoteRequests;
    uint32_t                         maxItemSize;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadUserData(resource);
    argStream.ReadPointerAddress(outRemoteRequests);
    argStream.ReadUInt32(maxItemSize, 0xffffffff);

    CLuaMain* luaMain = nullptr;

    if (resource)
    {
        luaMain = resource->GetVirtualMachine();
    }

    uint32_t count = 0;

    for (const auto& remoteRequest : g_pGame->GetRemoteCalls()->GetCalls())
    {
        if (maxItemSize <= 0)
        {
            break;
            continue;
        }

        if (!resource || remoteRequest->GetVM() == luaMain)
        {
            CWebAssemblyUserData udata = POINTER_TO_WASM_SYSTEM_POINTER(remoteRequest);

            argStream.WritePointer(outRemoteRequests + (count * sizeof(udata)), &udata);

            count++;
            maxItemSize--;
        }
    }

    return argStream.Return(count);
}

WebAssemblyApi(CWebAssemblyNetworkDefs::GetRemoteRequestInfo, env, args, results)
{
    CRemoteCall*                     remoteRequest;
    CWebAssemblyMemoryPointerAddress outInfo;
    int32_t                          postDataLength;
    bool                             includeHeaders;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(remoteRequest);
    argStream.ReadPointerAddress(outInfo);
    argStream.ReadInt32(postDataLength, 0);
    argStream.ReadBoolean(includeHeaders, false);

    if (!remoteRequest || outInfo == WEB_ASSEMBLY_NULL_PTR)
    {
        return argStream.Return(false);
    }

    CResource* thisResource = GetWebAssemblyEnvResource(env);
    CResource* requestResource = nullptr;

    CWebAssemblyMemory* memory = GetWebAssemblyEnvMemory(env);

    if (remoteRequest->GetVM())
    {
        requestResource = remoteRequest->GetVM()->GetResource();
    }

    bool extendedInfo = false;
    
    if (thisResource == requestResource || g_pGame->GetACLManager()->CanObjectUseRight(thisResource->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, "fullRemoteRequestInfo", CAccessControlListRight::RIGHT_TYPE_GENERAL, false))
    {
        extendedInfo = true;
    }

    RemoteRequestInfo info;
    memset(&info, 0, sizeof(info));

    SString url = remoteRequest->GetURL();

    if (extendedInfo)
    {
        url = url.ReplaceI("%3F", "?").Replace("#", "?").SplitLeft("?");
    }

    info.isFetch = remoteRequest->IsFetch();
    info.url = memory->StringToUTF8(url);
    info.queue = memory->StringToUTF8(remoteRequest->GetQueueName());
    info.resource = RESOURCE_TO_WASM_USERDATA(requestResource);
    info.start = remoteRequest->GetStartTime();

    if (extendedInfo)
    {
        if (postDataLength == -1 || postDataLength > 0)
        {
            const SString& postData = remoteRequest->GetOptions().strPostData;

            if (postDataLength > 0 && postDataLength < static_cast<int32_t>(postData.length()))
            {
                info.postData = memory->StringToUTF8(postData.SubStr(0, postDataLength));
                info.postDataSize = postDataLength;
            }
            else
            {
                info.postData = memory->StringToUTF8(postData);
                info.postDataSize = static_cast<uint32_t>(postData.length());
            }
        }

        if (includeHeaders)
        {
            std::map<SString, SString> requestHeaders = remoteRequest->GetOptions().requestHeaders;

            if (!requestHeaders.empty())
            {
                size_t count = requestHeaders.size();

                info.headers = memory->Malloc(count * (sizeof(CWebAssemblyMemoryPointerAddress) * 2));

                CWebAssemblyMemoryPointerAddress headersPtr = info.headers;

                if (info.headers != WEB_ASSEMBLY_NULL_PTR)
                {
                    size_t headersCount = 0;

                    for (const auto& header : requestHeaders)
                    {
                        CWebAssemblyMemoryPointerAddress headerName = memory->StringToUTF8(header.first);
                        CWebAssemblyMemoryPointerAddress headerValue = memory->StringToUTF8(header.second);

                        if (headerName == WEB_ASSEMBLY_NULL_PTR)
                        {
                            continue;
                        }

                        if (headerValue == WEB_ASSEMBLY_NULL_PTR)
                        {
                            memory->Free(headerValue);
                            continue;
                        }

                        argStream.WritePointer(headersPtr, &headerName);
                        headersPtr += sizeof(headerName);

                        argStream.WritePointer(headersPtr, &headerValue);
                        headersPtr += sizeof(headerValue);

                        headersCount++;
                    }

                    info.headersCount = headersCount;
                }
            }
        }
    }

    SString requestMethod = remoteRequest->GetOptions().strRequestMethod;

    info.method = memory->StringToUTF8(requestMethod.length() >= 1 ? requestMethod.ToUpper() : "POST");
    info.connectionAttempts = remoteRequest->GetOptions().uiConnectionAttempts;
    info.connectionTimeout = remoteRequest->GetOptions().uiConnectTimeoutMs;

    SDownloadStatus downloadStatus = remoteRequest->GetDownloadStatus();

    info.bytesReceived = downloadStatus.uiBytesReceived;
    info.bytesTotal = downloadStatus.uiContentLength;
    info.currentAttempt = downloadStatus.uiAttemptNumber;

    argStream.WritePointer(outInfo, &info);

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyNetworkDefs::CleanupRemoteRequestInfo, env, args, results)
{
    RemoteRequestInfo* info;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadPointer(info);

    CWebAssemblyMemory* memory = GetWebAssemblyEnvMemory(env);

    if (info->url != WEB_ASSEMBLY_NULL_PTR)
    {
        memory->Free(info->url);
    }

    if (info->queue != WEB_ASSEMBLY_NULL_PTR)
    {
        memory->Free(info->queue);
    }

    if (info->postData != WEB_ASSEMBLY_NULL_PTR)
    {
        memory->Free(info->postData);
    }

    if (info->headers != WEB_ASSEMBLY_NULL_PTR)
    {
        CWebAssemblyMemoryPointerAddress* headers = (CWebAssemblyMemoryPointerAddress*)memory->GetMemoryPhysicalPointer(info->headers);
        
        if (info->headersCount > 0)
        {
            for (uint32_t i = 0; i < info->headersCount; i++)
            {
                memory->Free(headers[i]);
            }
        }

        memory->Free(info->headers);
    }

    if (info->method != WEB_ASSEMBLY_NULL_PTR)
    {
        memory->Free(info->method);
    }

    memset(info, 0, sizeof(*info));

    return argStream.Return(true);
}

WebAssemblyApi(CWebAssemblyNetworkDefs::AbortRemoteRequest, env, args, results)
{
    CRemoteCall* remoteRequest;

    CWebAssemblyArgReader argStream(env, args, results);

    argStream.ReadSystemPointer(remoteRequest);

    if (!remoteRequest)
    {
        return argStream.Return(false);
    }

    bool result = remoteRequest->CancelDownload();

    g_pGame->GetRemoteCalls()->Remove(remoteRequest);

    return argStream.Return(result);
}
