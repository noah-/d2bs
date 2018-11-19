#include "JSSocket.h"
#include "D2BS.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

struct SocketData {
    int mode;
    SOCKET socket;
};
EMPTY_CTOR(socket)

JSAPI_PROP(socket_getProperty) {
    SocketData* sdata = (SocketData*)JS_GetInstancePrivate(cx, obj, &socket_class, NULL);

    if (sdata) {
        jsval ID;
        JS_IdToValue(cx, id, &ID);
        JS_BeginRequest(cx);
        switch (JSVAL_TO_INT(ID)) {
            struct timeval timeout;
            int result;
        case SOCKET_READABLE:
            fd_set read_set;
            timeout.tv_sec = 0;
            timeout.tv_usec = 100; // 100 ms
            FD_ZERO(&read_set);
            FD_SET(sdata->socket, &read_set);

            result = select(1, &read_set, NULL, NULL, &timeout);
            vp.setInt32(result);
            break;

        case SOCKET_WRITEABLE:
            fd_set write_set;

            timeout.tv_sec = 0;
            timeout.tv_usec = 100; // 100 ms
            FD_ZERO(&write_set);
            FD_SET(sdata->socket, &write_set);

            result = select(1, NULL, &write_set, NULL, &timeout);

            vp.setInt32(result);
            break;
        }
    }

    return JS_TRUE;
}

JSAPI_STRICT_PROP(socket_setProperty) {
    return JS_TRUE;
}

JSAPI_FUNC(socket_open) {

    if (argc < 2) {
        JS_SET_RVAL(cx, vp, JSVAL_FALSE);
        return JS_TRUE;
    }
    char* hostName = NULL;
    int32 port = 0;
    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
        hostName = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

    if (JS_ValueToInt32(cx, JS_ARGV(cx, vp)[1], &port) == JS_FALSE)
        THROW_ERROR(cx, "Could not convert parameter 2");

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return JS_TRUE;
    }

    SocketData* Sdata = new SocketData;

    Sdata->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct hostent* host;
    host = gethostbyname(hostName);
    if (host == NULL)
        THROW_ERROR(cx, "Cannot find host");
    SOCKADDR_IN SockAddr;
    SockAddr.sin_port = htons(port);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    Sdata->mode = Sdata->socket;

    if (connect(Sdata->socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
        THROW_ERROR(cx, "Failed to connect");
    }

    JSObject* res = BuildObject(cx, &socket_class, socket_methods, socket_props, Sdata);
    if (!res) {
        closesocket(Sdata->socket);
        WSACleanup();
        THROW_ERROR(cx, "Failed to define the socket object");
    }
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res));
    return JS_TRUE;
}

JSAPI_FUNC(socket_close) {
    SocketData* sData = (SocketData*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &socket_class, NULL);

    closesocket(sData->socket);
    WSACleanup();

    return JS_TRUE;
}

JSAPI_FUNC(socket_send) {
    SocketData* sData = (SocketData*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &socket_class, NULL);

    char* msg = NULL;
    int32 port = 0;
    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
        msg = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

    send(sData->socket, msg, strlen(msg), 0);

    return JS_TRUE;
}

JSAPI_FUNC(socket_read) {
    SocketData* sData = (SocketData*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &socket_class, NULL);

    char buffer[10000] = {0};
    std::string returnVal;
    int nDataLength;

    int iResult = 0;
    do {

        iResult = recv(sData->socket, buffer, 10000, 0);
        if (iResult > 0)
            returnVal.append(buffer);
        if (iResult == -1) {
            THROW_ERROR(cx, "Socket Error");
        }

    } while (iResult > 10000);

    JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, returnVal.c_str())));
    return JS_TRUE;
}

void socket_finalize(JSFreeOp* fop, JSObject* obj) {

    SocketData* sData = (SocketData*)JS_GetPrivate(obj);
    if (sData) {
        closesocket(sData->socket);
        WSACleanup();
    }
}
