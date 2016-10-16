/**
 *  @file RemoteSaveWorker.h
 *
 *  @license: GNU Public License
 *      Copyright (c) 2012 Rory Strawther
 *      All Rights Reserved.
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Library General Public License for more details.
 *
 *      You should have received a copy of the GNU Library General Public
 *      License along with this library; if not, write to the
 *      Free Software Foundation, Inc.
 *      675 Mass Ave
 *      Cambridge, MA 02139
 *      U.S.A.
 *
 *  @author Rory Strawther
 *  @version 1.0
 *
 *  @brief This is the worker class to attempt to save the provided Document
 *      to the remote server. This class will execute in a separate thread.
 *      Very rudimentary authentication is used! It should be replaced with a
 *      more robust method.
 */

#ifndef REMOTE_SAVE_THREAD_H
#define REMOTE_SAVE_THREAD_H

#include <QObject>
class Document;

#define AES_BLOCK_SIZE 16
#define AES_KEY_SCHEDULE_SIZE 60
#define AES_KEY_SIZE 32
#define SHA256_BLOCK_SIZE 32
//#define USE_AES_DECRYPT

class RemoteSaveWorker :
    public QObject
{
    Q_OBJECT
public:
    RemoteSaveWorker(Document* doc, QObject* parent = 0);
    ~RemoteSaveWorker();

public slots:
    void process();

signals:
    void finished();
    void error(const QString& errorMessage);

protected:
    unsigned char* sha256(
        unsigned char* input, 
        unsigned int inputLength) const;
    void convertHexTextToKeyBlock(
        unsigned char output[SHA256_BLOCK_SIZE],
        const QByteArray& text) const;
    void aesAddRoundKey(
        unsigned char state[4][4],
        const unsigned int keySchedule[4]);
    void aesSubByte(unsigned char state[4][4]);
#ifdef USE_AES_DECRYPT
    void aesInvSubByte(unsigned char state[4][4]);
#endif // USE_AES_DECRYPT
    void aesShiftRows(unsigned char state[4][4]);
#ifdef USE_AES_DECRYPT
    void aesInvShiftRows(unsigned char state[4][4]);
#endif // USE_AES_DECRYPT
    void aesMixColumns(unsigned char state[4][4]);
#ifdef USE_AES_DECRYPT
    void aesInvMixColumns(unsigned char state[4][4]);
#endif // USE_AES_DECRYPT
    unsigned int aesSubWord(unsigned int word);
    void aesKeySetup(
        unsigned int output[AES_KEY_SCHEDULE_SIZE],
        const unsigned char key[AES_KEY_SIZE]);
    void aesEncrypt(
        unsigned char output[AES_BLOCK_SIZE],
        const unsigned char input[AES_BLOCK_SIZE],
        const unsigned int keySchedule[AES_KEY_SCHEDULE_SIZE]);

#ifdef USE_AES_DECRYPT
    void aesDecrypt(
        unsigned char output[AES_BLOCK_SIZE],
        const unsigned char input[AES_BLOCK_SIZE],
        const unsigned int keySchedule[AES_KEY_SCHEDULE_SIZE]);
#endif // USE_AES_DECRYPT

    /** Creates a CRC32 checksum of the provided string to allow verification
     *      of the file transfer.
     *  @param[in] text - The text for which the checksum is created.
     *  @return A 32 bit value representing the checksum.
     */
    unsigned int crc32Checksum(const QString& text) const;

private:
    Document *document_;

    static unsigned char aesGfMul[256][6];
    static unsigned char aesSBox[AES_BLOCK_SIZE][AES_BLOCK_SIZE];
#ifdef USE_AES_DECRYPT
    static unsigned char aesInvSBox[AES_BLOCK_SIZE][AES_BLOCK_SIZE];
#endif // USE_AES_DECRYPT
};

#endif // REMOTE_SAVE_THREAD_H
