/*
 *   
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

/*
 * NOTICE: Portions Copyright (c) 2007-2009 Davy Preuveneers.
 * This file has been modified by Davy Preuveneers on 2009/01/11. The
 * changes are licensed under the terms of the GNU General Public
 * License version 2. This notice was added to meet the conditions of
 * Section 3.a of the GNU General Public License version 2.
 */

#include <serial_port_export.h>
#include <midpServices.h>
#include <midp_thread.h>
#include <kni.h>
#include <sni.h>

#include <string.h>
#include <stdio.h>
#include <windows.h>

#define REPORT(msg) fprintf(stdout, msg)

/* COMM options */
#define SERIAL_STOP_BITS_2     0x01
#define SERIAL_ODD_PARITY      0x02
#define SERIAL_EVEN_PARITY     0x04
#define SERIAL_AUTO_RTS        0x10
#define SERIAL_AUTO_CTS        0x20
#define SERIAL_BITS_PER_CHAR_7 0x80
#define SERIAL_BITS_PER_CHAR_8 0xC0

/**
 * Configure a serial port optional parameters.
 *
 * @param hPort port number
 * @param baudRate baudRate rate to set the port at
 * @param options options for the serial port:
 * bit 0: 0 - 1 stop bit, 1 - 2 stop bits 
 * bit 2-1: 00 - no parity, 01 - odd parity, 10 - even parity 
 * bit 4: 0 - no auto RTS, 1 - set auto RTS 
 * bit 5: 0 - no auto CTS, 1 - set auto CTS 
 * bit 7-6: 01 - 7 bits per symbol, 11 - 8 bits per symbol 
 *
 * @return PCSL_NET_SUCCESS for successful configure operation;\n 
 *       PCSL_NET_IOERROR for any error
 */
int configurePort(int hPort, int baudRate, unsigned int options) {
    DCB deviceControlBlock;
    COMMTIMEOUTS commTimeouts;

    // fprintf(stdout, "configurePort(port=%d,baud=%d,options=%d)\n", hPort, baudRate, options);
    memset(&deviceControlBlock, 0, sizeof (deviceControlBlock));
    deviceControlBlock.DCBlength = sizeof (deviceControlBlock);
    deviceControlBlock.BaudRate = baudRate;

    /* binary mode, no EOF check */
    deviceControlBlock.fBinary = 1;

    /* 0-4=no,odd,even,mark,space */
    if (options & SERIAL_ODD_PARITY) {
        deviceControlBlock.fParity = 1;
        deviceControlBlock.Parity = 1;
    } else if (options & SERIAL_EVEN_PARITY) {
        deviceControlBlock.fParity = 1;
        deviceControlBlock.Parity = 2;
    }

    /* CTS output flow control */
    if (options & SERIAL_AUTO_CTS) {
        deviceControlBlock.fOutxCtsFlow = 1;
    }

    deviceControlBlock.fDtrControl = DTR_CONTROL_ENABLE;

    /* RTS flow control */
    if (options & SERIAL_AUTO_RTS) {
        deviceControlBlock.fRtsControl = RTS_CONTROL_HANDSHAKE;
    } else {
        deviceControlBlock.fRtsControl = RTS_CONTROL_ENABLE;
    }

    /* BITS_PER_CHAR_8 is 2 bits and includes BITS_PER_CHAR_7 */
    if ((options & SERIAL_BITS_PER_CHAR_8)
            == SERIAL_BITS_PER_CHAR_8) {
        deviceControlBlock.ByteSize = 8;
    } else {
        deviceControlBlock.ByteSize = 7;
    }

    /* StopBits 0,1,2 = 1, 1.5, 2 */
    if (options & SERIAL_STOP_BITS_2) {
        deviceControlBlock.StopBits = 2;
    }

    if (!SetCommState((HANDLE) hPort, &deviceControlBlock)) {
        REPORT("configurePort << ERROR SetCommState\n");
        return PCSL_NET_IOERROR;
    }

    if (!GetCommTimeouts((HANDLE) hPort, &commTimeouts)) {
        REPORT("configurePort << ERROR GetCommTime timeouts\n");
        return PCSL_NET_IOERROR;
    }

    /* set non blocking (async methods) */
    commTimeouts.ReadIntervalTimeout = MAXDWORD;
    commTimeouts.ReadTotalTimeoutMultiplier = 0;
    commTimeouts.ReadTotalTimeoutConstant = 0;

    commTimeouts.WriteTotalTimeoutMultiplier = 0;
    commTimeouts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts((HANDLE) hPort, &commTimeouts)) {
        REPORT("configurePort << ERROR cannot SetCommTimeouts timeouts\n");
        return PCSL_NET_IOERROR;
    }

    return PCSL_NET_SUCCESS;
}

/**
 * Open a serial port by logical device name.
 *
 * @param pszDeviceName logical name of the port (for example, COM1)
 * @param baudRate baud rate to set the port at
 * @param options options for the serial port
 * bit 0: 0 - 1 stop bit, 1 - 2 stop bits 
 * bit 2-1: 00 - no parity, 01 - odd parity, 10 - even parity 
 * bit 4: 0 - no auto RTS, 1 - set auto RTS 
 * bit 5: 0 - no auto CTS, 1 - set auto CTS 
 * bit 7-6: 01 - 7 bits per symbol, 11 - 8 bits per symbol 
 * @param pHandle returns the connection handle; it is
 *        set only when this function returns PCSL_NET_SUCCESS
 * @param pContext filled by ptr to data for reinvocations
 *
 * @return PCSL_NET_SUCCESS for successful read operation;\n 
 *       PCSL_NET_WOULDBLOCK if the operation would block,\n 
 *       PCSL_NET_INTERRUPTED for an Interrupted IO Exception,\n
 *       PCSL_NET_IOERROR for all other errors
 */
int
openPortByNameStart(char* pszDeviceName, int baudRate,
					int options, int *pHandle, void **pContext)
{
    wchar_t lpFileName[6];
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts={0};
    unsigned short lastError[1024];
    HANDLE fd;

    //fprintf(stdout, "openPortByNameStart(name=%s,baud=%d,options=%d)\n", pszDeviceName, baudRate, options);
    wsprintf(lpFileName, L"COM%d:", pszDeviceName[3] - '0');

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),lastError,1024,NULL);

    fd = CreateFileW(lpFileName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,0);

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),lastError,1024,NULL);

    if (fd == INVALID_HANDLE_VALUE) {
        REPORT("serial_open: couldn't open port!\n");
        return PCSL_NET_IOERROR;
    }

    if (configurePort(fd, baudRate, options) != PCSL_NET_SUCCESS) {
        closePort(fd);
        *pHandle = (int)INVALID_HANDLE;
        return PCSL_NET_IOERROR;
    }

    *pHandle = (int)fd;

    return PCSL_NET_SUCCESS;
}

/**
 * Open a serial port by system dependent device name.
 *
 * @param pszDeviceName device name of the port
 * @param baudRate baud rate to set the port at
 * @param options options for the serial port
 * bit 0: 0 - 1 stop bit, 1 - 2 stop bits 
 * bit 2-1: 00 - no parity, 01 - odd parity, 10 - even parity 
 * bit 4: 0 - no auto RTS, 1 - set auto RTS 
 * bit 5: 0 - no auto CTS, 1 - set auto CTS 
 * bit 7-6: 01 - 7 bits per symbol, 11 - 8 bits per symbol 
 * @param pHandle returns the connection handle; it is
 *        set only when this function returns PCSL_NET_SUCCESS
 * @param context ptr to data saved before sleeping
 *
 * @return PCSL_NET_SUCCESS for successful read operation;\n 
 *       PCSL_NET_WOULDBLOCK if the operation would block,\n 
 *       PCSL_NET_INTERRUPTED for an Interrupted IO Exception,\n
 *       PCSL_NET_IOERROR for all other errors
 */
int openPortByNameFinish(char* pszDeviceName, int baudRate,
    int options, int *pHandle, void *context) {

    int returnStatus = PCSL_NET_IOERROR;

    // fprintf(stdout, "openPortByNameFinish(name=%s,baud=%d,options=%d)\n", pszDeviceName, baudRate, options);
    return returnStatus;
}

/**
 * Close a serial port.
 *
 * @param hPort handle to a native serial port
 */
void closePort(int hPort)
{
    // fprintf(stdout, "closePort(port=%d)\n", hPort);
    CloseHandle((HANDLE)hPort);
}

/**
 * Write to a serial port without blocking.
 *
 * @param hPort handle to a native serial port
 * @param pBuffer I/O buffer
 * @param nNumberOfBytesToWrite length of data
 * @param pBytesWritten returns the number of bytes written after
 *        successful write operation; only set if this function returns
 *        PCSL_NET_SUCCESS
 * @param pContext filled by ptr to data for reinvocations
 *
 * @return PCSL_NET_SUCCESS for successful write operation;\n 
 *       PCSL_NET_WOULDBLOCK if the operation would block,\n 
 *       PCSL_NET_INTERRUPTED for an Interrupted IO Exception\n 
 *       PCSL_NET_IOERROR for all other errors
 */
int writeToPortStart(int hPort, char* pBuffer,
    int nNumberOfBytesToWrite, int* pBytesWritten, void **pContext) {
    int status = PCSL_NET_IOERROR;
    int result = 0;

    // fprintf(stdout, "writeToPortStart(port=%d,len=%d)\n", hPort, nNumberOfBytesToWrite);
    if (nNumberOfBytesToWrite == 0) {
        *pBytesWritten = 0;
        status = PCSL_NET_SUCCESS;
    } else {
        result = WriteFile((HANDLE)hPort, pBuffer, nNumberOfBytesToWrite, pBytesWritten, NULL);
        // fprintf(stdout, "  result:%d\n", result);
        // fprintf(stdout, "  pBytesWritten:%d\n", *pBytesWritten);
        status = PCSL_NET_SUCCESS;
    }
    return status;
}

/**
 * Write to a serial port without blocking.
 *
 * @param hPort handle to a native serial port
 * @param pBuffer I/O buffer
 * @param nNumberOfBytesToWrite length of data
 * @param pBytesWritten returns the number of bytes written after
 *        successful write operation; only set if this function returns
 *        PCSL_NET_SUCCESS
 * @param context ptr to data saved before sleeping
 *
 * @return PCSL_NET_SUCCESS for successful write operation;\n 
 *       PCSL_NET_WOULDBLOCK if the operation would block,\n 
 *       PCSL_NET_INTERRUPTED for an Interrupted IO Exception\n 
 *       PCSL_NET_IOERROR for all other errors
 */
int writeToPortFinish(int hPort, char* pBuffer,
        int nNumberOfBytesToWrite, int* pBytesWritten, void *context) {
    int status = PCSL_NET_IOERROR;
    int result = 0;

    // fprintf(stdout, "writeToPortFinish(port=%d,len=%d)\n", hPort, nNumberOfBytesToWrite);
    if (nNumberOfBytesToWrite == 0) {
        *pBytesWritten = 0;
        status = PCSL_NET_SUCCESS;
    } else {
        return writeToPortStart(hPort, pBuffer, nNumberOfBytesToWrite, pBytesWritten, NULL);
    }
    return status;
}

/**
 * Read from a serial port.
 *
 * @param hPort handle to a native serial port
 * @param pBuffer I/O buffer
 * @param nNumberOfBytesToRead length of data
 * @param pBytesRead returns the number of bytes actually read; it is
 *        set only when this function returns PCSL_NET_SUCCESS
 * @param pContext filled by ptr to data for reinvocations
 *
 * @return PCSL_NET_SUCCESS for successful read operation;\n 
 *       PCSL_NET_WOULDBLOCK if the operation would block,\n 
 *       PCSL_NET_INTERRUPTED for an Interrupted IO Exception,\n
 *       PCSL_NET_IOERROR for all other errors
 */
int readFromPortStart(int hPort, char* pBuffer,
                 int nNumberOfBytesToRead, int* pBytesRead, void **pContext) {
    int result = 0;
    int attempt = 0;

    // fprintf(stdout, "readFromPortStart(port=%d,len=%d)\n", hPort, nNumberOfBytesToRead);
    if (nNumberOfBytesToRead == 0) {
        *pBytesRead = 0;
    } else {
        memset(pBuffer, 0, nNumberOfBytesToRead);
        result = ReadFile((HANDLE)hPort, pBuffer, nNumberOfBytesToRead, pBytesRead, NULL);
        // fprintf(stdout, "  result:%d\n", result);
        // fprintf(stdout, "  read:%d\n", *pBytesRead);
    }
    return PCSL_NET_SUCCESS;
}

/**
 * Read from a serial port.
 *
 * @param hPort handle to a native serial port
 * @param pBuffer I/O buffer
 * @param nNumberOfBytesToRead length of data
 * @param pBytesRead returns the number of bytes actually read; it is
 *        set only when this function returns PCSL_NET_SUCCESS
 * @param context ptr to data saved before sleeping
 *
 * @return PCSL_NET_SUCCESS for successful read operation;\n 
 *       PCSL_NET_WOULDBLOCK if the operation would block,\n 
 *       PCSL_NET_INTERRUPTED for an Interrupted IO Exception,\n
 *       PCSL_NET_IOERROR for all other errors
 */
int readFromPortFinish(int hPort, char* pBuffer, 
    int nNumberOfBytesToRead, int* pBytesRead, void *context) {
    int status = PCSL_NET_IOERROR;

    // fprintf(stdout, "readFromPortFinish(port=%d,len=%d)\n", hPort, nNumberOfBytesToRead);
    if (nNumberOfBytesToRead == 0) {
        *pBytesRead = 0;
        status = PCSL_NET_SUCCESS;
    } else {
        return readFromPortStart(hPort, pBuffer, nNumberOfBytesToRead, pBytesRead, NULL);
    }
    return status;
}
