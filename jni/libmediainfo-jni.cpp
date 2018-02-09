#include <assert.h>
#include <jni.h>

#include <stdlib.h>
#include <wchar.h>

#include <stdio.h>
#include <MediaInfo/MediaInfo_Events.h>

#include <unistd.h>

//#include <iostream>
//#include <sstream>
//#include <string>

// if need to debug messages, comment out below undef lines. Or use comment out lines in Android.mk
//#define _DEBUG
//#define DEBUG
//#undef _DEBUG
//#undef DEBUG

#if defined(WIN32) && defined(_DEBUG)
#   include <vld.h>
#endif

////////////////////////////////////////////////////////////////////////////
//  Platform specific parts
//
////////////////////////////////////////////////////////////////////////////

#if defined(ANDROID)

#   include "MediaInfo/MediaInfo.h"
    using namespace MediaInfoLib;

#   include <android/log.h>

#   define LOGW(...)  __android_log_print(ANDROID_LOG_WARN, "libmediainfo-jni", __VA_ARGS__)
#   define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, "libmediainfo-jni", __VA_ARGS__)
#   define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, "libmediainfo-jni", __VA_ARGS__)

#   if defined(_DEBUG) || defined(DEBUG)
#       define LOG(...)   __android_log_print(ANDROID_LOG_DEBUG, "libmediainfo-jni", __VA_ARGS__)
#   else
#       define LOG(...)   ((void)0)
#   endif

#   define FUNC        __func__

#elif defined(_WIN32)

#   include "MediaInfoDLL/MediaInfoDLL.h"
    using namespace MediaInfoDLL;

#   include <stdio.h>

#   define LOGW(...)  printf("[W|libmediainfo-jni] " __VA_ARGS__)
#   define LOGE(...)  printf("[E|libmediainfo-jni] " __VA_ARGS__)

#   if defined(_DEBUG) || defined(DEBUG)
#       define LOG(...)   printf("[D|libmediainfo-jni] " __VA_ARGS__)
#   else
#       define LOG(...)   ((void)0)
#   endif

#   define FUNC        __FUNCTION__

#else

#   error not support target!

#endif

#if defined(_DEBUG) || defined(DEBUG)
static inline const char*
_PrintableChars(const Char* chars, char* buf, size_t buflen)
{
    assert(buf);
    assert(buflen > 0);

    const char* pend = buf + buflen - 1; // 1 for '\0'
    char* p = buf;

    while (*chars && p < pend)
        *p++ = (char) *chars++;

    *p = '\0';

    return buf;
}

static inline const char*
PrintableChars(const Char* chars)
{
//#if defined(_DEBUG) || defined(DEBUG)
    static char buf[128];
    return _PrintableChars(chars, &buf[0], sizeof(buf)/sizeof(char));
//#else
//    return "";
//#endif
}

static inline const char*
PrintableChars2(const Char* chars)
{
//#if defined(_DEBUG) || defined(DEBUG)
    static char buf[128];
    return _PrintableChars(chars, &buf[0], sizeof(buf)/sizeof(char));
//#else
//    return "";
//#endif
}
#endif

class FuncCallLog {

public:
    FuncCallLog(const char* pszFuncName) {

#if defined(_DEBUG) || defined(DEBUG)
        _pszFuncName = pszFuncName;
        LOG("%s() is called.\n", pszFuncName);
    }

    ~FuncCallLog() {
        LOG("%s() will be returned.\n", _pszFuncName);
    }

private:
    const char* _pszFuncName;

#else // defined(_DEBUG) || defined(DEBUG)

    } // end of constructor

#endif // defined(_DEBUG) || defined(DEBUG)
};


////////////////////////////////////////////////////////////////////////////
//  Declarations of exported functions
//
////////////////////////////////////////////////////////////////////////////

extern "C" {
    JNIEXPORT jstring      JNICALL MediaInfo_getById(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jint parameter);
    JNIEXPORT jstring      JNICALL MediaInfo_getByName(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jstring parameter);
    JNIEXPORT jobjectArray JNICALL MediaInfo_getByNames(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jobjectArray parameters);
    JNIEXPORT jstring      JNICALL MediaInfo_getByIdDetail(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jint parameter, jint kindOfInfo);
    JNIEXPORT jstring      JNICALL MediaInfo_getByNameDetail(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jstring parameter, jint kindOfInfo, jint kindOfSearch);
    JNIEXPORT jint         JNICALL MediaInfo_countGet(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum);

    JNIEXPORT jstring      JNICALL MediaInfo_getMediaInfo(JNIEnv* pEnv, jobject self, jstring filename);
    JNIEXPORT jstring      JNICALL MediaInfo_getMediaInfoOption(JNIEnv* pEnv, jobject self, jstring param);
}

const size_t buffsize = 7 * 188 * 1024; //1024 * 1024; //7 * 188; //7 * 188 - magic
static const char* const kClassName = "org/mediainfo/android/MediaInfo";

static const JNINativeMethod gMethods[] = {
    { "getById", "(Ljava/lang/String;III)Ljava/lang/String;", (void*)MediaInfo_getById },
    { "getByName", "(Ljava/lang/String;IILjava/lang/String;)Ljava/lang/String;", (void*)MediaInfo_getByName },
    { "getByNames", "(Ljava/lang/String;II[Ljava/lang/String;)[Ljava/lang/String;", (void*)MediaInfo_getByNames },
    { "getByIdDetail", "(Ljava/lang/String;IIII)Ljava/lang/String;", (void*)MediaInfo_getByIdDetail },
    { "getByNameDetail", "(Ljava/lang/String;IILjava/lang/String;II)Ljava/lang/String;", (void*)MediaInfo_getByNameDetail },
    { "countGet", "(Ljava/lang/String;II)I", (void*)MediaInfo_countGet },
    { "getMediaInfo", "(Ljava/lang/String;)Ljava/lang/String;", (void*)MediaInfo_getMediaInfo },
    { "getMediaInfoOption", "(Ljava/lang/String;)Ljava/lang/String;", (void*)MediaInfo_getMediaInfoOption },
};

JNIEXPORT jint JNICALL
JNI_OnLoad (JavaVM * vm, void * reserved)
{
    FuncCallLog funclog(FUNC);

    JNIEnv *env;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) == JNI_OK) {
        LOG("JNI_VERSION_1_6 OK!\n");
    } else if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK) {
        LOGW("JNI_VERSION_1_4 OK!\n");
    } else if (vm->GetEnv((void**) &env, JNI_VERSION_1_2) == JNI_OK) {
        LOGW("JNI_VERSION_1_2 OK!\n");
    } else {
        LOGE("JNI_VERSION_?_? FAIL!\n");
        return -1;
    }

    LOG("Registering %s natives\n", kClassName);

    jclass clazz = env->FindClass(kClassName);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'!\n", kClassName);
        return -1;
    }

    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods)/sizeof(gMethods[0])) < 0) {
        LOGE("env->RegisterNatives)() fails for '%s'!\n", kClassName);
        env->DeleteLocalRef(clazz);
        return -1;
    }

    env->DeleteLocalRef(clazz);

    return JNI_VERSION_1_6;
}


////////////////////////////////////////////////////////////////////////////
//  Internal implementations
//
////////////////////////////////////////////////////////////////////////////

static inline stream_t
CastStreamKind(jint streamKind)
{
    assert(streamKind < Stream_Max);

    return (stream_t) streamKind;
}

static inline info_t
CastInfoKind(jint infoKind)
{
    assert(infoKind < Info_Max);

    return (info_t) infoKind;
}

static inline jstring
NewJString(JNIEnv *pEnv, String str)
{
    // 1.
    /*//size_t len = str.max_size(); //5242880; //str.max_size(); //str.size();
    //int chcount = 1048576; //1048576; //str.length();

    char* cstr = new char[1048576];
    wcstombs(cstr, str.c_str(), str.max_size());
    jstring jstr = pEnv->NewStringUTF(cstr);
    jchar* jchars = (jchar*) pEnv->GetStringChars(jstr, NULL);

    pEnv->ReleaseStringChars(jstr, jchars);

    return jstr;*/


    // 2.
    /*char* cstr = new char[str.size() + 1];
    int i = 0;
    while(str.c_str()[i] != '\0')
    {
        cstr[i] = (char)str.c_str()[i];
        ++i;
    }
    jstring jstr = pEnv->NewStringUTF(cstr);
    return jstr;*/


    // 3.
    jstring result = NULL;
    size_t len = wcslen(str.c_str()) * 4 + 1;
    size_t sz = wcstombs(0, str.c_str(), len);

    char* ch = new char[sz + 1];

    wcstombs(ch, str.c_str(), len);

    ch[sz] = '\0';
    result = pEnv->NewStringUTF(ch);
    delete[] ch;

    #if defined(_DEBUG) || defined(DEBUG)
        const char *nativeString = (pEnv)->GetStringUTFChars(result, NULL);
        LOG("NewJString(JNIEnv *pEnv, String str) -> jstring = \n'%s'\n", nativeString);
        //pEnv->ReleaseStringUTFChars(jstr, nativeString);
    #endif

    return result;
}

static inline String
NewString(JNIEnv *pEnv, jstring str)
{
    // 1.
    /*const char *CStr = pEnv->GetStringUTFChars(str, NULL);
    if (NULL == CStr)
        return NULL;

    jsize len = pEnv->GetStringUTFLength(str);
    wchar_t *wcstr = new wchar_t[1048576];
    mbstowcs(wcstr, CStr, len);

    pEnv->ReleaseStringUTFChars(str, CStr);

    LOG("NewString('%s') returns '%s'.\n", CStr, PrintableChars(wcstr));

    return wcstr;*/


    // 2.
    std::wstring value;

    const jchar *raw = pEnv->GetStringChars(str, 0);
    jsize len = pEnv->GetStringLength(str);

    value.assign(raw, raw + len);

    pEnv->ReleaseStringChars(str, raw);

    #if defined(_DEBUG) || defined(DEBUG)
        const char *CStrParam = pEnv->GetStringUTFChars(str, NULL);
        LOG("NewString('%s') returns '%s'.\n", CStrParam, PrintableChars(value.c_str()));
    #endif

    return value;
}

static inline bool
IsCanceled(JNIEnv *pEnv, jobject self)
{
    jclass clazz = pEnv->FindClass(kClassName);
    jmethodID getIsCanceled = pEnv->GetMethodID(clazz, "getIsCanceled", "()I");
    int result = pEnv->CallIntMethod(self, getIsCanceled);

    return (result != 0);
}

long long fseek_64(FILE *stream, int64_t offset, int origin) {
    long long fd = fileno(stream);
    return lseek64(fd, offset, origin);
}

////////////////////////////////////////////////////////////////////////////
//  Implementations of exported functions
//
////////////////////////////////////////////////////////////////////////////

JNIEXPORT jstring JNICALL
MediaInfo_getById(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jint parameter)
{
    FuncCallLog funclog(FUNC);

    const char * cfilename = (pEnv)->GetStringUTFChars(filename, NULL);
    String strInfo;

    //From: preparing an example file for reading
    FILE* F = fopen(cfilename, "rb"); //You can use something else than a file
    if (F == 0)
        return NewJString(pEnv, __T("Error opening file..."));

    //From: preparing a memory buffer for reading
    unsigned char* From_Buffer = new unsigned char[buffsize]; //Note: you can do your own buffer
    size_t From_Buffer_Size; //The size of the read file buffer

    //From: retrieving file size
    long long F_Size = fseek_64(F, 0, SEEK_END);
    fseek_64(F, 0, SEEK_SET);

    //Initializing MediaInfo
    MediaInfo MI;
    //Preparing to fill MediaInfo with a buffer
    MI.Open_Buffer_Init(F_Size, 0);
    //The parsing loop

    bool isCanceled = false;

    do {
        /*if (IsCanceled(pEnv, self)) {
            LOGD("MediaInfo_getById() IsCanceled = '%s'\n", "True");
            MI.Open_Buffer_Finalize();
            fclose(F);
            return pEnv->NewStringUTF("getById is canceled");
        }*/

        //Reading data somewhere, do what you want for this.
        From_Buffer_Size = fread(From_Buffer, 1, buffsize, F);
        //Sending the buffer to MediaInfo
        size_t Status = MI.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);

        if (Status & 0x08) //Bit3=Finished
            break;

        isCanceled = IsCanceled(pEnv, self);
        if (isCanceled) {
            LOGD("MediaInfo_getById() IsCanceled = '%s'\n", "True");
            break;
        }

        //Testing if there is a MediaInfo request to go elsewhere
        if (MI.Open_Buffer_Continue_GoTo_Get() != (MediaInfo_int64u) -1) {
            //fseek(F, (long) MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET); //Position the file
            //MI.Open_Buffer_Init(F_Size, ftell(F)); //Informing MediaInfo we have seek
            MI.Open_Buffer_Init(F_Size, fseek_64(F, MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET));
        }
    } while (From_Buffer_Size > 0);

    //Finalizing
    MI.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finnish some work

    strInfo = MI.Get(CastStreamKind(streamKind), streamNum, parameter);

    fclose(F);

    delete [] From_Buffer;

    //MediaInfo MI;

    //String strInfo;

    //String cfilename = NewString(pEnv, filename);

    //MI.Open(cfilename);

    //strInfo = MI.Get(CastStreamKind(streamKind), streamNum, parameter);

    //MI.Close();

    LOG("MediaInfo->Get(%d,%d,%d) returns '%s'\n",
        CastStreamKind(streamKind), streamNum, parameter, PrintableChars(strInfo.c_str()));

    return NewJString(pEnv, strInfo);
}

JNIEXPORT jstring JNICALL
MediaInfo_getByName(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jstring parameter)
{
    FuncCallLog funclog(FUNC);

    const char * cfilename = (pEnv)->GetStringUTFChars(filename, NULL);
    String strInfo;

    //From: preparing an example file for reading
    FILE* F = fopen(cfilename, "rb"); //You can use something else than a file
    if (F == 0)
        return NewJString(pEnv, __T("Error opening file..."));

    //From: preparing a memory buffer for reading
    unsigned char* From_Buffer = new unsigned char[buffsize]; //Note: you can do your own buffer
    size_t From_Buffer_Size; //The size of the read file buffer

    //From: retrieving file size
    long long F_Size = fseek_64(F, 0, SEEK_END);
    fseek_64(F, 0, SEEK_SET);

    //Initializing MediaInfo
    MediaInfo MI;
    //Preparing to fill MediaInfo with a buffer
    MI.Open_Buffer_Init(F_Size, 0);
    //The parsing loop

    bool isCanceled = false;

    do {
        /*if (IsCanceled(pEnv, self)) {
            LOGD("MediaInfo_getByName() IsCanceled = '%s'\n", "True");
            MI.Open_Buffer_Finalize();
            fclose(F);
            return pEnv->NewStringUTF("getByName is canceled");
        }*/

        //Reading data somewhere, do what you want for this.
        From_Buffer_Size = fread(From_Buffer, 1, buffsize, F);
        //Sending the buffer to MediaInfo
        size_t Status = MI.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);

        if (Status & 0x08) //Bit3=Finished
            break;

        isCanceled = IsCanceled(pEnv, self);
        if (isCanceled) {
            LOGD("MediaInfo_getByName() IsCanceled = '%s'\n", "True");
            break;
        }

        //Testing if there is a MediaInfo request to go elsewhere
        if (MI.Open_Buffer_Continue_GoTo_Get() != (MediaInfo_int64u) -1) {
            //fseek(F, (long) MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET); //Position the file
            //MI.Open_Buffer_Init(F_Size, ftell(F)); //Informing MediaInfo we have seek
            MI.Open_Buffer_Init(F_Size, fseek_64(F, MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET));
        }
    } while (From_Buffer_Size > 0);

    //Finalizing
    MI.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finnish some work

    String cstr = NewString(pEnv, parameter);
    strInfo = MI.Get(CastStreamKind(streamKind), streamNum, cstr);

    fclose(F);

    delete [] From_Buffer;

    //MediaInfo MI;

    //String strInfo;

    //String cstr = NewString(pEnv, parameter);
    //String cfilename = NewString(pEnv, filename);

    //MI.Open(cfilename);

    //strInfo = MI.Get(CastStreamKind(streamKind), streamNum, cstr);

    //MI.Close();

    LOG("MediaInfo->Get(%d,%d,'%s') returns '%s'%d\n",
        CastStreamKind(streamKind), streamNum, PrintableChars(cstr.c_str()), PrintableChars2(strInfo.c_str()), strInfo.length());

    return NewJString(pEnv, strInfo);
}

JNIEXPORT jobjectArray JNICALL
MediaInfo_getByNames(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jobjectArray parameters)
{
    FuncCallLog funclog(FUNC);

    const char *cfilename = (pEnv)->GetStringUTFChars(filename, NULL);
    // From: preparing an example file for reading
    FILE *F = fopen(cfilename, "rb"); //You can use something else than a file
    if (F == 0) {
        LOGW("getByNames: Error opening file...");

        return NULL;
    }
    int stringCount = pEnv->GetArrayLength(parameters);

    if (stringCount == 0) {
        LOGW("getByNames: Invalid parameters array!");

        return NULL;
    }
    jstring jstringArray[stringCount];
    jclass stringClass = pEnv->FindClass("java/lang/String");
    jobjectArray valueArray = pEnv->NewObjectArray(stringCount, stringClass, NULL);

    for (int i = 0; i < stringCount; i++) {
        jstringArray[i] = (jstring)(pEnv->GetObjectArrayElement(parameters, i));
    }
    // From: preparing a memory buffer for reading
    unsigned char* From_Buffer = new unsigned char[buffsize]; //Note: you can do your own buffer
    size_t From_Buffer_Size; // The size of the read file buffer

    // From: retrieving file size
    long long F_Size = fseek_64(F, 0, SEEK_END);

    fseek_64(F, 0, SEEK_SET);
    // Initializing MediaInfo
    MediaInfo MI;
    //Preparing to fill MediaInfo with a buffer
    MI.Open_Buffer_Init(F_Size, 0);

    // The parsing loop
    bool isCanceled = false;

    do {
        // Reading data somewhere, do what you want for this.
        From_Buffer_Size = fread(From_Buffer, 1, buffsize, F);
        // Sending the buffer to MediaInfo
        size_t Status = MI.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);

        if (Status & 0x08) { // Bit3=Finished
            break;
        }
        isCanceled = IsCanceled(pEnv, self);
        if (isCanceled) {
            LOGD("MediaInfo_getByName() IsCanceled = '%s'\n", "True");
            break;
        }
        // Testing if there is a MediaInfo request to go elsewhere
        if (MI.Open_Buffer_Continue_GoTo_Get() != (MediaInfo_int64u) -1) {
            //fseek(F, (long) MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET); // Position the file
            //MI.Open_Buffer_Init(F_Size, ftell(F)); // Informing MediaInfo we have seek
            MI.Open_Buffer_Init(F_Size, fseek_64(F, MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET));
        }
    } while (From_Buffer_Size > 0);
    // Finalizing
    MI.Open_Buffer_Finalize(); // This is the end of the stream, MediaInfo must finnish some work

    for (int i = 0; i < stringCount; i++) {
        String cstr = NewString(pEnv, jstringArray[i]);
        String strInfo = MI.Get(CastStreamKind(streamKind), streamNum, cstr);
        jstring jstringInfo = NewJString(pEnv, strInfo);

        pEnv->SetObjectArrayElement(valueArray, i, jstringInfo);
        pEnv->DeleteLocalRef(jstringInfo);
    }
    pEnv->DeleteLocalRef(stringClass);

    fclose(F);

    delete [] From_Buffer;

    return valueArray;
}

JNIEXPORT jstring JNICALL
MediaInfo_getByIdDetail(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jint parameter, jint kindOfInfo)
{
    FuncCallLog funclog(FUNC);

    const char * cfilename = (pEnv)->GetStringUTFChars(filename, NULL);
    String strInfo;

    //From: preparing an example file for reading
    FILE* F = fopen(cfilename, "rb"); //You can use something else than a file
    if (F == 0)
        return NewJString(pEnv, __T("Error opening file..."));

    //From: preparing a memory buffer for reading
    unsigned char* From_Buffer = new unsigned char[buffsize]; //Note: you can do your own buffer
    size_t From_Buffer_Size; //The size of the read file buffer

    //From: retrieving file size
    long long F_Size = fseek_64(F, 0, SEEK_END);
    fseek_64(F, 0, SEEK_SET);

    //Initializing MediaInfo
    MediaInfo MI;
    //Preparing to fill MediaInfo with a buffer
    MI.Open_Buffer_Init(F_Size, 0);
    //The parsing loop

    bool isCanceled = false;

    do {
        /*if (IsCanceled(pEnv, self)) {
            LOGD("MediaInfo_getByIdDetail() IsCanceled = '%s'\n", "True");
            MI.Open_Buffer_Finalize();
            fclose(F);
            return pEnv->NewStringUTF("getByIdDetail is canceled");
        }*/

        //Reading data somewhere, do what you want for this.
        From_Buffer_Size = fread(From_Buffer, 1, buffsize, F);
        //Sending the buffer to MediaInfo
        size_t Status = MI.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);

        if (Status & 0x08) //Bit3=Finished
            break;

        isCanceled = IsCanceled(pEnv, self);
        if (isCanceled) {
            LOGD("MediaInfo_getByIdDetail() IsCanceled = '%s'\n", "True");
            break;
        }

        //Testing if there is a MediaInfo request to go elsewhere
        if (MI.Open_Buffer_Continue_GoTo_Get() != (MediaInfo_int64u) -1) {
            //fseek(F, (long) MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET); //Position the file
            //MI.Open_Buffer_Init(F_Size, ftell(F)); //Informing MediaInfo we have seek
            MI.Open_Buffer_Init(F_Size, fseek_64(F, MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET));
        }
    } while (From_Buffer_Size > 0);

    //Finalizing
    MI.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finnish some work

    strInfo = strInfo = MI.Get(CastStreamKind(streamKind), streamNum, parameter, CastInfoKind(kindOfInfo));

    fclose(F);

    delete [] From_Buffer;

    //MediaInfo MI;

    //String strInfo;

    //String cfilename = NewString(pEnv, filename);

    //MI.Open(cfilename);

    //strInfo = MI.Get(CastStreamKind(streamKind), streamNum, parameter,
    //                                 CastInfoKind(kindOfInfo));
    //MI.Close();

    LOG("MediaInfo->Get(%d,%d,%d,%d) returns '%s'\n",
        CastStreamKind(streamKind), streamNum, parameter, kindOfInfo, PrintableChars(strInfo.c_str()));

    return NewJString(pEnv, strInfo);
}

JNIEXPORT jstring JNICALL
MediaInfo_getByNameDetail(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum, jstring parameter, jint kindOfInfo, jint kindOfSearch)
{
    FuncCallLog funclog(FUNC);

    const char * cfilename = (pEnv)->GetStringUTFChars(filename, NULL);
    String strInfo;

    //From: preparing an example file for reading
    FILE* F = fopen(cfilename, "rb"); //You can use something else than a file
    if (F == 0)
        return NewJString(pEnv, __T("Error opening file..."));

    //From: preparing a memory buffer for reading
    unsigned char* From_Buffer = new unsigned char[buffsize]; //Note: you can do your own buffer
    size_t From_Buffer_Size; //The size of the read file buffer

    //From: retrieving file size
    long long F_Size = fseek_64(F, 0, SEEK_END);
    fseek_64(F, 0, SEEK_SET);

    //Initializing MediaInfo
    MediaInfo MI;
    //Preparing to fill MediaInfo with a buffer
    MI.Open_Buffer_Init(F_Size, 0);
    //The parsing loop

    bool isCanceled = false;

    do {
        /*if (IsCanceled(pEnv, self)) {
            LOGD("MediaInfo_getByNameDetail() IsCanceled = '%s'\n", "True");
            MI.Open_Buffer_Finalize();
            fclose(F);
            return pEnv->NewStringUTF("getByNameDetail is canceled");
        }*/

        //Reading data somewhere, do what you want for this.
        From_Buffer_Size = fread(From_Buffer, 1, buffsize, F);
        //Sending the buffer to MediaInfo
        size_t Status = MI.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);

        if (Status & 0x08) //Bit3=Finished
            break;

        isCanceled = IsCanceled(pEnv, self);
        if (isCanceled) {
            LOGD("MediaInfo_getByNameDetail() IsCanceled = '%s'\n", "True");
            break;
        }

        //Testing if there is a MediaInfo request to go elsewhere
        if (MI.Open_Buffer_Continue_GoTo_Get() != (MediaInfo_int64u) -1) {
            //fseek(F, (long) MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET); //Position the file
            //MI.Open_Buffer_Init(F_Size, ftell(F)); //Informing MediaInfo we have seek
            MI.Open_Buffer_Init(F_Size, fseek_64(F, MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET));
        }
    } while (From_Buffer_Size > 0);

    //Finalizing
    MI.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finnish some work

    String cstr = NewString(pEnv, parameter);
    strInfo = strInfo = MI.Get(CastStreamKind(streamKind), streamNum, cstr, CastInfoKind(kindOfInfo), CastInfoKind(kindOfSearch));

    fclose(F);

    delete [] From_Buffer;

    //MediaInfo MI;

    //String strInfo;

    //String cstr = NewString(pEnv, parameter);

    //String cfilename = NewString(pEnv, filename);

    //MI.Open(cfilename);

    //strInfo = MI.Get(CastStreamKind(streamKind), streamNum, cstr,
    //                                 CastInfoKind(kindOfInfo), CastInfoKind(kindOfSearch));

    //MI.Close();

    LOG("MediaInfo->Get(%d,%d,'%s',%d,%d) returns '%s'\n",
        CastStreamKind(streamKind), streamNum, PrintableChars(cstr.c_str()),
        CastInfoKind(kindOfInfo), CastInfoKind(kindOfSearch), PrintableChars2(strInfo.c_str()));

    return NewJString(pEnv, strInfo);
}

JNIEXPORT jint JNICALL
MediaInfo_countGet(JNIEnv* pEnv, jobject self, jstring filename, jint streamKind, jint streamNum)
{
    const char * cfilename = (pEnv)->GetStringUTFChars(filename, NULL);

    //From: preparing an example file for reading
    FILE* F = fopen(cfilename, "rb"); //You can use something else than a file
    if (F == 0)
        return -1;

    //From: preparing a memory buffer for reading
    unsigned char* From_Buffer = new unsigned char[buffsize]; //Note: you can do your own buffer
    size_t From_Buffer_Size; //The size of the read file buffer

    //From: retrieving file size
    long long F_Size = fseek_64(F, 0, SEEK_END);
    fseek_64(F, 0, SEEK_SET);

    //Initializing MediaInfo
    MediaInfo MI;
    //Preparing to fill MediaInfo with a buffer
    MI.Open_Buffer_Init(F_Size, 0);
    //The parsing loop

    bool isCanceled = false;

    do {
        /*if (IsCanceled(pEnv, self)) {
            LOGD("MediaInfo_countGet() IsCanceled = '%s'\n", "True");
            MI.Open_Buffer_Finalize();
            fclose(F);
            return pEnv->NewStringUTF("countGet is canceled");
        }*/

        //Reading data somewhere, do what you want for this.
        From_Buffer_Size = fread(From_Buffer, 1, buffsize, F);
        //Sending the buffer to MediaInfo
        size_t Status = MI.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);

        if (Status & 0x08) //Bit3=Finished
            break;

        isCanceled = IsCanceled(pEnv, self);
        if (isCanceled) {
            LOGD("MediaInfo_countGet() IsCanceled = '%s'\n", "True");
            break;
        }

        //Testing if there is a MediaInfo request to go elsewhere
        if (MI.Open_Buffer_Continue_GoTo_Get() != (MediaInfo_int64u) -1) {
            //fseek(F, (long) MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET); //Position the file
            //MI.Open_Buffer_Init(F_Size, ftell(F)); //Informing MediaInfo we have seek
            MI.Open_Buffer_Init(F_Size, fseek_64(F, MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET));
        }
    } while (From_Buffer_Size > 0);

    //Finalizing
    MI.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finnish some work

    int intInfo = MI.Count_Get(CastStreamKind(streamKind), (size_t) streamNum);;

    fclose(F);

    delete [] From_Buffer;

    //MediaInfo MI;

    //int intInfo;

    //String cfilename = NewString(pEnv, filename);

    //MI.Open(cfilename);

    //intInfo = MI.Count_Get(CastStreamKind(streamKind), (size_t) streamNum);
    //MI.Close();

    return intInfo;
}

JNIEXPORT jstring JNICALL
MediaInfo_getMediaInfo(JNIEnv* pEnv, jobject self, jstring filename)
{
    const char * cfilename = (pEnv)->GetStringUTFChars(filename, NULL);
    String strInfo;

    strInfo =  __T("File\r\n");
    strInfo += __T("Complete name                            : ");
    strInfo += NewString(pEnv, filename);
    strInfo += __T("\r\n\r\n");

    //From: preparing an example file for reading
    FILE* F = fopen(cfilename, "rb"); //You can use something else than a file
    if (F == 0)
        return NewJString(pEnv, __T("Error opening file..."));

    //From: preparing a memory buffer for reading
    unsigned char* From_Buffer = new unsigned char[buffsize]; //Note: you can do your own buffer
    size_t From_Buffer_Size; //The size of the read file buffer

    ////From: retrieving file size
    //fseek(F, 0, SEEK_END);
    //long F_Size = ftell(F);
    //fseek(F, 0, SEEK_SET);

    //char temp[128];
    //sprintf(temp, "%ld", F_Size);
    //LOGE(" F_Size() = '%s'\n", temp);

    //std::wstring value;
    //value.assign(temp, temp + 128);

    //strInfo += __T("File size                                : ");
    //strInfo += value;
    //strInfo += __T("\r\n\r\n");

    long long F_Size = fseek_64(F, 0, SEEK_END);
    fseek_64(F, 0, SEEK_SET);

    //Initializing MediaInfo
    MediaInfo MI;
    //Preparing to fill MediaInfo with a buffer
    MI.Open_Buffer_Init(F_Size, 0);
    //The parsing loop

    bool isCanceled = false;

    do {
        /*if (IsCanceled(pEnv, self)) {
            LOGD("MediaInfo_getMediaInfo() IsCanceled = '%s'\n", "True");
            MI.Open_Buffer_Finalize();
            fclose(F);
            return pEnv->NewStringUTF("getMediaInfo is canceled");
        }*/

        //Reading data somewhere, do what you want for this.
        From_Buffer_Size = fread(From_Buffer, 1, buffsize, F);
        //Sending the buffer to MediaInfo
        size_t Status = MI.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);

        if (Status & 0x08) //Bit3=Finished
            break;

        isCanceled = IsCanceled(pEnv, self);
        if (isCanceled) {
            LOGD("MediaInfo_getMediaInfo() IsCanceled = '%s'\n", "True");
            break;
        }

        //Testing if there is a MediaInfo request to go elsewhere
        if (MI.Open_Buffer_Continue_GoTo_Get() != (MediaInfo_int64u) -1) {
            //fseek(F, (long) MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET); //Position the file
            //MI.Open_Buffer_Init(F_Size, ftell(F)); //Informing MediaInfo we have seek
            MI.Open_Buffer_Init(F_Size, fseek_64(F, MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET));
        }
    } while (From_Buffer_Size > 0);

    //Finalizing
    MI.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finnish some work

    //strInfo += __T("\r\n\r\nInform with Complete=false\r\n");
    MI.Option(__T("Complete"));
    strInfo += MI.Inform().c_str();
    //strInfo += __T("\r\n\r\nClose\r\n");

    if (isCanceled) {
        strInfo +=  __T("Getting MediaInfo for '");
        strInfo += NewString(pEnv, filename);
        strInfo += __T("' has been terminated!");
        strInfo += __T("\r\nThe data obtained are not fully!");
    }

    fclose(F);

    delete [] From_Buffer;

    LOG("MediaInfo_getMediaInfo() returns '%s'\n", PrintableChars(strInfo.c_str()));

    return NewJString(pEnv, strInfo);

    //// this code don't work with files larger then 2GB (Use buffer code)
    //String cstr = NewString(pEnv, filename);

    //MediaInfo MI;

    //String strInfo;

    ////strInfo += __T("\r\n\r\nOpen\r\n");
    //MI.Open(cstr);
    ////strInfo += __T("\r\n\r\nInform with Complete=false\r\n");
    //MI.Option(__T("Complete"));
    //strInfo = MI.Inform().c_str();
    ////strInfo += __T("\r\n\r\nClose\r\n");
    //MI.Close();

    //LOG("MediaInfo_getMediaInfo() returns '%s'\n", PrintableChars(strInfo.c_str()));

    //return NewJString(pEnv, strInfo);
}

JNIEXPORT jstring JNICALL
MediaInfo_getMediaInfoOption(JNIEnv* pEnv, jobject self, jstring param)
{
    String cstr = NewString(pEnv, param);

    MediaInfo MI;

    String strInfo;

    //String strInfo = MI.Option(__T("Info_Version"), __T("0.7.0.0;MediaInfoDLL_Example_MSVC;0.7.0.0")).c_str();
    strInfo = MI.Option(cstr).c_str();
    MI.Close();

    LOG("MediaInfo_getMediaInfoOption('%s', ..) returns '%s'.\n",
        PrintableChars(strInfo.c_str()), PrintableChars2(strInfo.c_str()));

    return NewJString(pEnv, strInfo);
}

//// HOW TO USE:
////Information about MediaInfo
//MediaInfo MI;
//ZenLib::Ztring To_Display=MI.Option(__T("Info_Version"), __T("0.7.0.0;MediaInfoDLL_Example_MSVC;0.7.0.0")).c_str();
//
//To_Display += __T("\r\n\r\nInfo_Parameters\r\n");
//To_Display += MI.Option(__T("Info_Parameters")).c_str();
//
//To_Display += __T("\r\n\r\nInfo_Capacities\r\n");
//To_Display += MI.Option(__T("Info_Capacities")).c_str();
//
//To_Display += __T("\r\n\r\nInfo_Codecs\r\n");
//To_Display += MI.Option(__T("Info_Codecs")).c_str();
//
////An example of how to use the library
//To_Display += __T("\r\n\r\nOpen\r\n");
//MI.Open(__T("Example.ogg"));
//
//To_Display += __T("\r\n\r\nInform with Complete=false\r\n");
//MI.Option(__T("Complete"));
//To_Display += MI.Inform().c_str();
//
//To_Display += __T("\r\n\r\nInform with Complete=true\r\n");
//MI.Option(__T("Complete"), __T("1"));
//To_Display += MI.Inform().c_str();
//
//To_Display += __T("\r\n\r\nCustom Inform\r\n");
//MI.Option(__T("Inform"), __T("General;Example : FileSize=%FileSize%"));
//To_Display += MI.Inform().c_str();
//
//To_Display += __T("\r\n\r\nGet with Stream=General and Parameter=\"FileSize\"\r\n");
//To_Display += MI.Get(Stream_General, 0, __T("FileSize"), Info_Text, Info_Name).c_str();
//
//To_Display += __T("\r\n\r\nGetI with Stream=General and Parameter=46\r\n");
//To_Display += MI.Get(Stream_General, 0, 46, Info_Text).c_str();
//
//To_Display += __T("\r\n\r\nCount_Get with StreamKind=Stream_Audio\r\n");
//To_Display += ZenLib::Ztring::ToZtring(MI.Count_Get(Stream_Audio, -1)); //Warning : this is an integer
//
//To_Display += __T("\r\n\r\nGet with Stream=General and Parameter=\"AudioCount\"\r\n");
//To_Display += MI.Get(Stream_General, 0, __T("AudioCount"), Info_Text, Info_Name).c_str();
//
//To_Display += __T("\r\n\r\nGet with Stream=Audio and Parameter=\"StreamCount\"\r\n");
//To_Display += MI.Get(Stream_Audio, 0, __T("StreamCount"), Info_Text, Info_Name).c_str();
//
//To_Display += __T("\r\n\r\nClose\r\n");
//MI.Close();
//
//std::cout<<To_Display.To_Local().c_str()<<std::endl;
