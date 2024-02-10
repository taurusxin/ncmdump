#include "ncmcrypt.h"
#include "aes.h"
#include "base64.h"
#include "cJSON.h"

#define TAGLIB_STATIC
#include "taglib/toolkit/tfile.h"
#include "taglib/mpeg/mpegfile.h"
#include "taglib/flac/flacfile.h"
#include "taglib/mpeg/id3v2/frames/attachedpictureframe.h"
#include "taglib/mpeg/id3v2/id3v2tag.h"
#include "taglib/tag.h"

#include <stdexcept>
#include <string>
#include <fstream>

#include <filesystem>

//定义变量
#define DEFINE_VALUE(type, valueName, defaultValue) \
    public: \
    type valueName() const { return m_##valueName; } \
    void set_##valueName(type valueName) { \
        if(valueName == m_##valueName) { \
            return; \
    } \
        m_##valueName = valueName; \
} \
    private: \
    type m_##valueName = defaultValue; \
    public: \

class NeteaseMusicMetadata {

    DEFINE_VALUE(std::string, album, "")
    DEFINE_VALUE(std::string, artist, "")
    DEFINE_VALUE(std::string, format, "")
    DEFINE_VALUE(std::string, name, "")
    DEFINE_VALUE(int, duration, 0)
    DEFINE_VALUE(int, bitrate, 0)
private:
    cJSON* mRaw;

public:
    NeteaseMusicMetadata(cJSON*);
    ~NeteaseMusicMetadata();

};

class NeteaseCryptPrivateData {

public:
    bool isNcmFile();
    bool openFile(std::filesystem::path const&);
    int read(char *s, std::streamsize n);
    void buildKeyBox(unsigned char *key, int keyLen);
    std::string mimeType(std::string& data);

public:
    static const unsigned char sCoreKey[17];
    static const unsigned char sModifyKey[17];
    static const unsigned char mPng[8];
    enum NcmFormat { MP3, FLAC };

public:
    std::filesystem::path mFilepath;
    std::filesystem::path mDumpFilepath;
    NcmFormat mFormat;
    std::string mImageData;
    std::ifstream mFile;
    unsigned char mKeyBox[256];
    NeteaseMusicMetadata* mMetaData;
};

const unsigned char NeteaseCryptPrivateData::sCoreKey[17]   = {0x68, 0x7A, 0x48, 0x52, 0x41, 0x6D, 0x73, 0x6F, 0x35, 0x6B, 0x49, 0x6E, 0x62, 0x61, 0x78, 0x57, 0};
const unsigned char NeteaseCryptPrivateData::sModifyKey[17] = {0x23, 0x31, 0x34, 0x6C, 0x6A, 0x6B, 0x5F, 0x21, 0x5C, 0x5D, 0x26, 0x30, 0x55, 0x3C, 0x27, 0x28, 0};

const unsigned char NeteaseCryptPrivateData::mPng[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

static void aesEcbDecrypt(const unsigned char *key, std::string& src, std::string& dst) {
	int n, i;

	unsigned char out[16];

	n = src.length() >> 4;

	dst.clear();

	AES aes(key);

	for (i = 0; i < n-1; i++) {
		aes.decrypt((unsigned char*)src.c_str() + (i << 4), out);
		dst += std::string((char*)out, 16);
	}

	aes.decrypt((unsigned char*)src.c_str() + (i << 4), out);
	char pad = out[15];
	if (pad > 16) {
		pad = 0;
	}
	dst += std::string((char*)out, 16-pad);
}

static void replace(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

NeteaseMusicMetadata::~NeteaseMusicMetadata() {
	cJSON_Delete(mRaw);
}

NeteaseMusicMetadata::NeteaseMusicMetadata(cJSON* raw) {
	if (!raw) {
		return;
	}

	cJSON *swap;
	int artistLen, i;

	mRaw = raw;

	swap = cJSON_GetObjectItem(raw, "musicName");
	if (swap) {
        set_name(std::string(cJSON_GetStringValue(swap)));
	}

	swap = cJSON_GetObjectItem(raw, "album");
	if (swap) {
        set_album(std::string(cJSON_GetStringValue(swap)));
	}

	swap = cJSON_GetObjectItem(raw, "artist");
	if (swap) {
		artistLen = cJSON_GetArraySize(swap);

		i = 0;
		for (i = 0; i < artistLen-1; i++) {
            set_artist(artist() + std::string(cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetArrayItem(swap, i), 0))) + "/");
		}
        set_artist(artist() + std::string(cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetArrayItem(swap, i), 0))));
	}

	swap = cJSON_GetObjectItem(raw, "bitrate");
	if (swap) {
        set_bitrate(swap->valueint);
	}

	swap = cJSON_GetObjectItem(raw, "duration");
    if (swap) {
        set_duration(swap->valueint);
	}

	swap = cJSON_GetObjectItem(raw, "format");
	if (swap) {
        set_format(std::string(cJSON_GetStringValue(swap)));
	}
}

bool NeteaseCryptPrivateData::openFile(std::filesystem::path const& path) {
    mFile.open(path, std::ios::in | std::ios::binary);
    if (!mFile.is_open()) {
		return false;
	} else {
		return true;
	}
}

bool NeteaseCryptPrivateData::isNcmFile() {
	unsigned int header;

    mFile.read(reinterpret_cast<char *>(&header), sizeof(header));
	if (header != (unsigned int)0x4e455443) {
		return false;
	}

    mFile.read(reinterpret_cast<char *>(&header), sizeof(header));
	if (header != (unsigned int)0x4d414446) {
		return false;
	}

	return true;
}

int NeteaseCryptPrivateData::read(char *s, std::streamsize n) {
    mFile.read(s, n);

    int gcount = mFile.gcount();

	if (gcount <= 0) {
		throw std::invalid_argument("Can't read file");
	}

	return gcount;
}

void NeteaseCryptPrivateData::buildKeyBox(unsigned char *key, int keyLen) {
	int i;
	for (i = 0; i < 256; ++i) {
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
		if (key_offset >= keyLen) key_offset = 0;
        mKeyBox[i] = mKeyBox[c]; mKeyBox[c] = swap;
		last_byte = c;
	}
}

std::string NeteaseCryptPrivateData::mimeType(std::string& data) {
    if (memcmp(data.c_str(), mPng, 8) == 0) {
		return std::string("image/png");
	}

	return std::string("image/jpeg");
}

void NeteaseCrypt::FixMetadata() {
    if (d->mDumpFilepath.string().length() <= 0) {
		throw std::invalid_argument("must dump before");
	}

	TagLib::File *audioFile;
	TagLib::Tag *tag;
    TagLib::ByteVector vector(d->mImageData.c_str(), d->mImageData.length());

    if (d->mFormat == NeteaseCryptPrivateData::MP3) {
        audioFile = new TagLib::MPEG::File(d->mDumpFilepath.c_str());
		tag = dynamic_cast<TagLib::MPEG::File*>(audioFile)->ID3v2Tag(true);

        if (d->mImageData.length() > 0) {
			TagLib::ID3v2::AttachedPictureFrame *frame = new TagLib::ID3v2::AttachedPictureFrame;

            frame->setMimeType(d->mimeType(d->mImageData));
			frame->setPicture(vector);

			dynamic_cast<TagLib::ID3v2::Tag*>(tag)->addFrame(frame);
		}
    } else if (d->mFormat == NeteaseCryptPrivateData::FLAC) {
        audioFile = new TagLib::FLAC::File(d->mDumpFilepath.c_str());
		tag = audioFile->tag();

        if (d->mImageData.length() > 0) {
			TagLib::FLAC::Picture *cover = new TagLib::FLAC::Picture;
            cover->setMimeType(d->mimeType(d->mImageData));
			cover->setType(TagLib::FLAC::Picture::FrontCover);
			cover->setData(vector);

			dynamic_cast<TagLib::FLAC::File*>(audioFile)->addPicture(cover);
		}
	}

    if (d->mMetaData != NULL) {
        tag->setTitle(TagLib::String(d->mMetaData->name(), TagLib::String::UTF8));
        tag->setArtist(TagLib::String(d->mMetaData->artist(), TagLib::String::UTF8));
        tag->setAlbum(TagLib::String(d->mMetaData->album(), TagLib::String::UTF8));
	}

	tag->setComment(TagLib::String("Create by netease copyright protected dump tool. author 5L", TagLib::String::UTF8));

	audioFile->save();
}

void NeteaseCrypt::Dump() {
    d->mDumpFilepath = d->mFilepath;

	std::vector<unsigned char> buffer(0x8000);

	std::ofstream output;

    while (!d->mFile.eof()) {
        int n = d->read((char*)buffer.data(), buffer.size());

		for (int i = 0; i < n; i++) {
			int j = (i + 1) & 0xff;
            buffer[i] ^= d->mKeyBox[(d->mKeyBox[j] + d->mKeyBox[(d->mKeyBox[j] + j) & 0xff]) & 0xff];
		}

		if (!output.is_open()) {
			// identify format
			// ID3 format mp3
			if (buffer[0] == 0x49 && buffer[1] == 0x44 && buffer[2] == 0x33) {
                d->mDumpFilepath.replace_extension(".mp3");
                d->mFormat = NeteaseCryptPrivateData::MP3;
			} else {
                d->mDumpFilepath.replace_extension(".flac");
                d->mFormat = NeteaseCryptPrivateData::FLAC;
			}

            output.open(d->mDumpFilepath, output.out | output.binary);
		}

		output.write((char*)buffer.data(), n);
	}

	output.flush();
	output.close();
}

NeteaseCrypt::~NeteaseCrypt() {
    if (d->mMetaData != NULL) {
        delete d->mMetaData;
	}

    d->mFile.close();
    delete d;
}

NeteaseCrypt::NeteaseCrypt(std::filesystem::path const& path) {
    d = new NeteaseCryptPrivateData();
    if (!d->openFile(path)) {
		throw std::invalid_argument("Can't open file");
	}

    if (!d->isNcmFile()) {
		throw std::invalid_argument("Not netease protected file");
	}

    if (!d->mFile.seekg(2, d->mFile.cur)) {
		throw std::invalid_argument("Can't seek file");
	}

    d->mFilepath = path;

	unsigned int n;
    d->read(reinterpret_cast<char *>(&n), sizeof(n));

	if (n <= 0) {
		throw std::invalid_argument("Broken NCM file");
	}

	std::vector<char> keydata(n);
    d->read(keydata.data(), n);

	for (size_t i = 0; i < n; i++) {
		keydata[i] ^= 0x64;
	}

	std::string rawKeyData(keydata.begin(), keydata.end());
	std::string mKeyData;

    aesEcbDecrypt(d->sCoreKey, rawKeyData, mKeyData);

    d->buildKeyBox((unsigned char*)mKeyData.c_str()+17, mKeyData.length()-17);

    d->read(reinterpret_cast<char *>(&n), sizeof(n));

	if (n <= 0) {
		printf("[Warn] `%s` missing metadata infomation can't fix some infomation!\n", path.c_str());

        d->mMetaData = NULL;
	} else {
		std::vector<char> modifyData(n);
        d->read(modifyData.data(), n);

		for (size_t i = 0; i < n; i++) {
			modifyData[i] ^= 0x63;
		}

		std::string swapModifyData;
		std::string modifyOutData;
		std::string modifyDecryptData;

		swapModifyData = std::string(modifyData.begin() + 22, modifyData.end());

		// escape `163 key(Don't modify):`
		Base64::Decode(swapModifyData, modifyOutData);

        aesEcbDecrypt(d->sModifyKey, modifyOutData, modifyDecryptData);

		// escape `music:`
		modifyDecryptData = std::string(modifyDecryptData.begin()+6, modifyDecryptData.end());

		// std::cout << modifyDecryptData << std::endl;

        d->mMetaData = new NeteaseMusicMetadata(cJSON_Parse(modifyDecryptData.c_str()));
	}

	// skip crc32 & unuse charset
    if (!d->mFile.seekg(9, d->mFile.cur)) {
		throw std::invalid_argument("can't seek file");
	}

    d->read(reinterpret_cast<char *>(&n), sizeof(n));

	if (n > 0) {
		char *imageData = (char*)malloc(n);
        d->read(imageData, n);

        d->mImageData = std::string(imageData, n);
	} else {
		printf("[Warn] `%s` missing album can't fix album image!\n", path.c_str());
	}
}

const std::filesystem::path& NeteaseCrypt::filepath() const {
    return d->mFilepath;
}

const std::filesystem::path& NeteaseCrypt::dumpFilepath() const {
    return d->mDumpFilepath;
}
