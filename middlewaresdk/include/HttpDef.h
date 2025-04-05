#ifndef _HTTP_DEF_h
#define _HTTP_DEF_h

#include <map>
#include <vector>
#include <string>
//#include "dataAssist.h"
extern const int kSignalServerEngineId;
extern const int kMediaServerEngineId;
extern const int kTcpSignalServerEngineId;
extern const char* kXMIPTag;
extern const int kMaxMsgLen;
extern const int kMaxFrameLen;

typedef enum _XMDataType 
{
	XM_DATA_STRING = 0,
	XM_DATA_STREAM,
	XM_DATA_UPGRADER,
	XM_DATA_PICTURE
}XMDataType;

typedef enum _XMIPEventType
{
    XM_EVENT_DISCONNECT = 0,
}XMIPEventType;

typedef struct _XMMsgInfo 
{
	int engineId;
	int connId;
	uint8_t type;
	int len;
	std::string msg;
    std::string connType;
}XMMsgInfo;

typedef struct _XMMediaInfo
{
	//视频
	int video_codec;
	int width;
	int height;
	int frame_rate; //帧率，一般25
	//音频
	int audio_codec;
	int bit_per_sample;
	int samples_per_sec;
	int audio_channel;
}XMMediaInfo;

typedef void (*XM_Data_CallBack)(int engineId, int connId, uint8_t type, char *data, char *connType, int len);
typedef void (*XM_Event_CallBack)(int engineId, int connId, XMIPEventType type, char* data, int len);
typedef void (*XM_CloseThread_CallBack)(int connId);

//接口
class ICmdListener
{
public:
	virtual ~ICmdListener() {}
public:
	virtual int OnClose(int engineId, int connId, std::string clientUuid) = 0;
	virtual int OnParseData(int engineId, int connId, const char* data, int len) = 0;
};

//http相关消息
typedef std::map<std::string, std::string> string_map_t;
typedef std::vector<std::string> string_vector_t;

typedef struct
{
	std::string type;
	std::string method;
	std::string url;
	int status_code;
	string_map_t headers;
	std::string body;
	int msglen;
}http_msg_t;

typedef enum _XMStatusCode
{
    Continue = 100,
    SwitchingProtocol = 101,
    Processing = 102,
    EarlyHints = 103,

    Ok = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultiStatus = 207,
    AlreadyReported = 208,
    ImUsed = 226,

    MultipleChoice = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    UseProxy = 305,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,

    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    PayloadTooLarge = 413,
    UriTooLong = 414,
    UnsupportedMediaType = 415,
    RangeNotSatisfiable = 416,
    ExpectationFailed = 417,
    MisdirectedRequest = 421,
    UnprocessableEntity = 422,
    Locked = 423,
    FailedDependency = 424,
    TooEarly = 425,
    UpgradeRequired = 426,
    PreconditionRequired = 428,
    TooManyRequests = 429,
    RequestHeaderFieldsTooLarge = 431,
    UnavailableForLegalReasons = 451,

    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HttpVersionNotSupported = 505,
    VariantAlsoNegotiates = 506,
    InsufficientStorage = 507,
    LoopDetected = 508,
    NotExtended = 510,
    NetworkAuthenticationRequired = 511
}StatusCode;

#endif