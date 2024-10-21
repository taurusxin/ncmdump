#include "ncmcrypt.h"
#include "aes.h"
#include "base64.h"
#include "cJSON.h"
#include "color.h"

#define TAGLIB_STATIC
#include "taglib/toolkit/tfile.h"
#include "taglib/mpeg/mpegfile.h"
#include "taglib/flac/flacfile.h"
#include "taglib/mpeg/id3v2/frames/attachedpictureframe.h"
#include "taglib/mpeg/id3v2/id3v2tag.h"
#include "taglib/tag.h"

#include <stdexcept>
#include <string>
#include <filesystem>

#pragma warning(disable:4267)
#pragma warning(disable:4244)

const unsigned char NeteaseCrypt::sCoreKey[17] = {0x68, 0x7A, 0x48, 0x52, 0x41, 0x6D, 0x73, 0x6F, 0x35, 0x6B, 0x49, 0x6E, 0x62, 0x61, 0x78, 0x57, 0};
const unsigned char NeteaseCrypt::sModifyKey[17] = {0x23, 0x31, 0x34, 0x6C, 0x6A, 0x6B, 0x5F, 0x21, 0x5C, 0x5D, 0x26, 0x30, 0x55, 0x3C, 0x27, 0x28, 0};

const unsigned char NeteaseCrypt::mPng[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

static void aesEcbDecrypt(const unsigned char *key, std::string &src, std::string &dst)
{
    int n, i;

    unsigned char out[16];

    n = src.length() >> 4;

    dst.clear();

    AES aes(key);

    for (i = 0; i < n - 1; i++)
    {
        aes.decrypt((unsigned char *)src.c_str() + (i << 4), out);
        dst += std::string((char *)out, 16);
    }

    aes.decrypt((unsigned char *)src.c_str() + (i << 4), out);
    char pad = out[15];
    if (pad > 16)
    {
        pad = 0;
    }
    dst += std::string((char *)out, 16 - pad);
}

static void replace(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

NeteaseMusicMetadata::~NeteaseMusicMetadata()
{
    cJSON_Delete(mRaw);
}

NeteaseMusicMetadata::NeteaseMusicMetadata(cJSON *raw)
{
    if (!raw)
    {
        return;
    }

    cJSON *swap;
    int artistLen, i;

    mRaw = raw;

    swap = cJSON_GetObjectItem(raw, "musicName");
    if (swap)
    {
        mName = std::string(cJSON_GetStringValue(swap));
    }

    swap = cJSON_GetObjectItem(raw, "album");
    if (swap)
    {
        mAlbum = std::string(cJSON_GetStringValue(swap));
    }

    swap = cJSON_GetObjectItem(raw, "artist");
    if (swap)
    {
        artistLen = cJSON_GetArraySize(swap);

        i = 0;
        for (i = 0; i < artistLen; i++)
        {
            auto artist = cJSON_GetArrayItem(swap, i);
            if (cJSON_GetArraySize(artist) > 0)
            {
                if (!mArtist.empty())
                {
                    mArtist += "/";
                }
                mArtist += std::string(cJSON_GetStringValue(cJSON_GetArrayItem(artist, 0)));
            }
        }
    }

    swap = cJSON_GetObjectItem(raw, "bitrate");
    if (swap)
    {
        mBitrate = swap->valueint;
    }

    swap = cJSON_GetObjectItem(raw, "duration");
    if (swap)
    {
        mDuration = swap->valueint;
    }

    swap = cJSON_GetObjectItem(raw, "format");
    if (swap)
    {
        mFormat = std::string(cJSON_GetStringValue(swap));
    }
}

bool NeteaseCrypt::openFile(std::string const &path)
{
    mFile.open(std::filesystem::u8path(path), std::ios::in | std::ios::binary);
    if (!mFile.is_open())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool NeteaseCrypt::isNcmFile()
{
    unsigned int header;

    mFile.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header != (unsigned int)0x4e455443)
    {
        return false;
    }

    mFile.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header != (unsigned int)0x4d414446)
    {
        return false;
    }

    return true;
}

int NeteaseCrypt::read(char *s, std::streamsize n)
{
    mFile.read(s, n);

    int gcount = mFile.gcount();

    if (gcount <= 0)
    {
        throw std::invalid_argument("Can't read file");
    }

    return gcount;
}

void NeteaseCrypt::buildKeyBox(unsigned char *key, int keyLen)
{
    int i;
    for (i = 0; i < 256; ++i)
    {
        mKeyBox[i] = (unsigned char)i;
    }

    unsigned char swap = 0;
    unsigned char c = 0;
    unsigned char last_byte = 0;
    unsigned char key_offset = 0;

    for (i = 0; i < 256; ++i)
    {
        swap = mKeyBox[i];
        c = ((swap + last_byte + key[key_offset++]) & 0xff);
        if (key_offset >= keyLen)
            key_offset = 0;
        mKeyBox[i] = mKeyBox[c];
        mKeyBox[c] = swap;
        last_byte = c;
    }
}

std::string NeteaseCrypt::mimeType(std::string &data)
{
    if (memcmp(data.c_str(), mPng, 8) == 0)
    {
        return std::string("image/png");
    }

    return std::string("image/jpeg");
}

void NeteaseCrypt::FixMetadata()
{

    TagLib::File *audioFile;
    TagLib::Tag *tag;
    TagLib::ByteVector vector(mImageData.c_str(), mImageData.length());

    if (mFormat == NeteaseCrypt::MP3)
    {
        audioFile = new TagLib::MPEG::File(mDumpFilepath.c_str());
        tag = dynamic_cast<TagLib::MPEG::File *>(audioFile)->ID3v2Tag(true);

        if (!mImageData.empty())
        {
            TagLib::ID3v2::AttachedPictureFrame *frame = new TagLib::ID3v2::AttachedPictureFrame;

            frame->setMimeType(mimeType(mImageData));
            frame->setPicture(vector);

            dynamic_cast<TagLib::ID3v2::Tag *>(tag)->addFrame(frame);
        }
    }
    else if (mFormat == NeteaseCrypt::FLAC)
    {
        audioFile = new TagLib::FLAC::File(mDumpFilepath.c_str());
        tag = audioFile->tag();

        if (!mImageData.empty())
        {
            TagLib::FLAC::Picture *cover = new TagLib::FLAC::Picture;
            cover->setMimeType(mimeType(mImageData));
            cover->setType(TagLib::FLAC::Picture::FrontCover);
            cover->setData(vector);

            dynamic_cast<TagLib::FLAC::File *>(audioFile)->addPicture(cover);
        }
    }

    if (mMetaData != NULL)
    {
        tag->setTitle(TagLib::String(mMetaData->name(), TagLib::String::UTF8));
        tag->setArtist(TagLib::String(mMetaData->artist(), TagLib::String::UTF8));
        tag->setAlbum(TagLib::String(mMetaData->album(), TagLib::String::UTF8));
    }

    // tag->setComment(TagLib::String("Create by taurusxin/ncmdump.", TagLib::String::UTF8));

    audioFile->save();
    audioFile->~File();
}

void NeteaseCrypt::Dump(std::string const &outputDir = "")
{
    if (outputDir.empty())
    {
        mDumpFilepath = std::filesystem::u8path(mFilepath);
    } else {
        mDumpFilepath = std::filesystem::u8path(outputDir) / std::filesystem::u8path(mFilepath).filename();
    }

    std::vector<unsigned char> buffer(0x8000);

    std::ofstream output;

    while (!mFile.eof())
    {
        int n = read((char *)buffer.data(), buffer.size());

        for (int i = 0; i < n; i++)
        {
            int j = (i + 1) & 0xff;
            buffer[i] ^= mKeyBox[(mKeyBox[j] + mKeyBox[(mKeyBox[j] + j) & 0xff]) & 0xff];
        }

        if (!output.is_open())
        {
            // identify format
            // ID3 format mp3
            if (buffer[0] == 0x49 && buffer[1] == 0x44 && buffer[2] == 0x33)
            {
                mDumpFilepath = mDumpFilepath.replace_extension("mp3");
                mFormat = NeteaseCrypt::MP3;
            }
            else
            {
                mDumpFilepath = mDumpFilepath.replace_extension("flac");
                mFormat = NeteaseCrypt::FLAC;
            }

            output.open(mDumpFilepath, std::ofstream::out | std::ofstream::binary);
        }

        output.write((char *)buffer.data(), n);
    }

    output.flush();
    output.close();
}

NeteaseCrypt::~NeteaseCrypt()
{
    if (mMetaData != NULL)
    {
        delete mMetaData;
    }

    mFile.close();
}

NeteaseCrypt::NeteaseCrypt(std::string const &path)
{
    if (!openFile(path))
    {
        throw std::invalid_argument("Can't open file");
    }

    if (!isNcmFile())
    {
        throw std::invalid_argument("Not netease protected file");
    }

    if (!mFile.seekg(2, mFile.cur))
    {
        throw std::invalid_argument("Can't seek file");
    }

    mFilepath = path;

    unsigned int n;
    read(reinterpret_cast<char *>(&n), sizeof(n));

    if (n <= 0)
    {
        throw std::invalid_argument("Broken NCM file");
    }

    std::vector<char> keydata(n);
    read(keydata.data(), n);

    for (size_t i = 0; i < n; i++)
    {
        keydata[i] ^= 0x64;
    }

    std::string rawKeyData(keydata.begin(), keydata.end());
    std::string mKeyData;

    aesEcbDecrypt(sCoreKey, rawKeyData, mKeyData);

    buildKeyBox((unsigned char *)mKeyData.c_str() + 17, mKeyData.length() - 17);

    read(reinterpret_cast<char *>(&n), sizeof(n));

    if (n <= 0)
    {
        std::cout << BOLDYELLOW << "[Warn] " << RESET << "'" << path << "' missing metadata infomation can't fix some infomation!" << std::endl;

        mMetaData = NULL;
    }
    else
    {
        std::vector<char> modifyData(n);
        read(modifyData.data(), n);

        for (size_t i = 0; i < n; i++)
        {
            modifyData[i] ^= 0x63;
        }

        std::string swapModifyData;
        std::string modifyOutData;
        std::string modifyDecryptData;

        swapModifyData = std::string(modifyData.begin() + 22, modifyData.end());

        // escape `163 key(Don't modify):`
        Base64::Decode(swapModifyData, modifyOutData);

        aesEcbDecrypt(sModifyKey, modifyOutData, modifyDecryptData);

        // escape `music:`
        modifyDecryptData = std::string(modifyDecryptData.begin() + 6, modifyDecryptData.end());

        // std::cout << modifyDecryptData << std::endl;

        mMetaData = new NeteaseMusicMetadata(cJSON_Parse(modifyDecryptData.c_str()));
    }

    // skip crc32 & image version
    if (!mFile.seekg(5, mFile.cur))
    {
        throw std::invalid_argument("can't seek file");
    }

    uint32_t cover_frame_len{0};
    read(reinterpret_cast<char *>(&cover_frame_len), 4);
    read(reinterpret_cast<char *>(&n), sizeof(n));

    if (n > 0)
    {
        mImageData = std::string(n, '\0');
        read(&mImageData[0], n);
    }
    else
    {
        std::cout << BOLDYELLOW << "[Warn] " << RESET << "'" << path << "' missing album can't fix album image!" << std::endl;
    }
    mFile.seekg(cover_frame_len - n, mFile.cur);
}
