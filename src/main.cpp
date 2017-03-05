/*
* Copyright (c) 2017 Jani Niemelä.
* License: https://github.com/siquel/irkki/blob/master/LICENSE
*/

#include <WinSock2.h>
#include <libircclient.h>
#include <libirc_events.h>
#include <stdio.h>
#include <stdint.h>

static void event_connect(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    printf("Connected\n");
}

static void event_numeric(irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count)
{
}

static void event_join(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    
}

int main(int _argc, char* _argv[])
{
    (void)_argc;
    (void)_argv;

#ifdef _WIN32
    WSADATA wsaData;
    int err;
    if ((err = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
    {
        printf("Could not find a usable version of Winsock\n");
        WSACleanup();
        return 1;
    }

#endif
    irc_callbacks_t callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.event_connect = event_connect;
    callbacks.event_numeric = event_numeric;
    callbacks.event_join = event_join;

    irc_session_t* session =  irc_create_session(&callbacks);
    irc_option_set(session, LIBIRC_OPTION_SSL_NO_VERIFY);

    uint16_t port = 2486;

    if (irc_connect(session, "#irc.kamu.li", port, NULL, "irkki", "irkki", "irkki"))
    {
        printf("Could not connect: %s\n", irc_strerror(irc_errno(session)));
        printf("Failed to connect %d\n", WSAGetLastError());
        return 1;
    }

    while (1)
    {
        if (!irc_is_connected(session))
            break;

        struct timeval tv;
        fd_set in_set, out_set;
        int maxfd;

        tv.tv_usec = 250000;
        tv.tv_sec = 0;

        FD_ZERO(&in_set);
        FD_ZERO(&out_set);

        irc_add_select_descriptors(session, &in_set, &out_set, &maxfd);

        if (select(maxfd + 1, &in_set, &out_set, 0, &tv) < 0)
        {
            // error
            break;
        }

        irc_process_select_descriptors(session, &in_set, &out_set);
    }

    irc_destroy_session(session);
    
#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}