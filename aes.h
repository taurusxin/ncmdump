#pragma once

#include <string.h>
#include <stdio.h>

class AES {

public:
    AES();
    AES(const unsigned char *key);
    virtual ~AES();
    void encrypt(const unsigned char data[16], unsigned char out[16]);
    void decrypt(const unsigned char data[16], unsigned char out[16]);

private:
    //
    int mNb;

    //word length of the secret key used in one turn 
    int mNk;

    //number of turns
    int mNr;

    //the secret key,which can be 16bytes，24bytes or 32bytes
    unsigned char mKey[32];

    //the extended key,which can be 176bytes,208bytes,240bytes
    unsigned char mW[60][4];

    static unsigned char sBox[];
    static unsigned char invSBox[];
    //constant 
    static unsigned char rcon[];
    void setKey(const unsigned char *key);

    void subBytes(unsigned char state[][4]);
    void shiftRows(unsigned char state[][4]);
    void mixColumns(unsigned char state[][4]);
    void addRoundKey(unsigned char state[][4], unsigned char w[][4]);

    void invSubBytes(unsigned char state[][4]);
    void invShiftRows(unsigned char state[][4]);
    void invMixColumns(unsigned char state[][4]);

    void keyExpansion();

    //
    unsigned char GF28Multi(unsigned char s, unsigned char a);

    void rotWord(unsigned char w[]);
    void subWord(unsigned char w[]);

    //get the secret key
    void getKeyAt(unsigned char key[][4],int i);

};
