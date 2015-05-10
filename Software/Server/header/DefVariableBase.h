#ifndef DEFVARIABLEBASE_H
#define DEFVARIABLEBASE_H
#include <string>

using namespace std;
enum MYSQL_TYPE {MYSQL_INT, MYSQL_DOUBLE, MYSQL_STRING, MYSQL_TEXT, MYSQL_DATETIME, MYSQL_BLOB, MYSQL_ENUM, MYSQL_BOOLEAN};

typedef struct _Col
{
	MYSQL_TYPE type;
	string value;

} Col;

typedef struct _User
{
	string name = "User";
	Col id {.type = MYSQL_INT,.value = "IdUser"};
	Col firstName {.type = MYSQL_STRING,.value = "FirstName"};
	Col lastName {.type = MYSQL_STRING,.value = "LastName"};
	Col password {.type = MYSQL_STRING,.value = "Password"};
	Col email {.type = MYSQL_STRING,.value = "Email"};

} User;

typedef struct _WebsiteUser
{
	string name = "WebsiteUser";
	Col id {.type = MYSQL_INT,.value = "IdUser"};
	Col registration {.type = MYSQL_DATETIME,.value = "DateRegistration"};

} WebsiteUser;

typedef struct _RecorderUser
{
	string name = "RecorderUser";
	Col id {.type = MYSQL_INT,.value = "IdUser"};
	Col dateBegin {.type = MYSQL_DATETIME,.value = "DateBegin"};
	Col dateEnd {.type = MYSQL_DATETIME,.value = "DateEnd"};

} RecorderUser;

typedef struct _Card
{
	string name = "Card";
	Col idcard {.type = MYSQL_INT,.value = "IdCard"};
	Col iduser {.type = MYSQL_INT,.value = "IdUser"};
	Col number {.type = MYSQL_STRING,.value = "NumberCard"};

} Card;

typedef struct _Historic
{
	string name = "Historic";
	Col dateTimeBegin {.type = MYSQL_DATETIME,.value = "DateTimeBegin"};
	Col dateTimeEnd {.type = MYSQL_DATETIME,.value = "DateTimeEnd"};
	Col authorization {.type = MYSQL_BOOLEAN,.value = "Authorization"};

} Historic;

typedef struct _RecordingModule
{
	string name = "RecordingModule";
	Col id {.type = MYSQL_INT,.value = "IdRecordingModule"};
	Col idNetwork {.type = MYSQL_INT,.value = "IdNetwork"};
	Col adressMAC {.type = MYSQL_STRING,.value = "AdressMAC"};
	Col recordingConfiguration {.type = MYSQL_BLOB,.value = "RecordingConfiguration"};

} RecordingModule;

typedef struct _Recorder
{
	string name = "Recorder";
	Col idRecorder {.type = MYSQL_INT,.value = "IdRecorder"};
	Col idRoom {.type = MYSQL_INT,.value = "IdRoom"};
	Col idConnectingModule {.type = MYSQL_INT,.value = "IdConnectingModule"};
	Col idRecordingModule {.type = MYSQL_INT,.value = "IdRecordingModule"};

} Recorder;

typedef struct _ConnectingModule
{
	string name = "ConnectingModule";
	Col id {.type = MYSQL_INT,.value = "IdConnectingModule"};
	Col idNetwork {.type = MYSQL_INT,.value = "IdNetworkRecording"};

} ConnectingModule;

typedef struct _Room
{
	string name = "Room";
	Col id {.type = MYSQL_INT,.value = "IdRoom"};
	Col roomName {.type = MYSQL_STRING,.value = "RoomName"};
	Col description {.type = MYSQL_TEXT,.value = "RoomDescription"};

} Room;

typedef struct _Chapter
{
	string name = "Chapter";
	Col id {.type = MYSQL_INT,.value = "IdChapter"};
	Col status {.type = MYSQL_ENUM,.value = "StatusChapter"};
	Col date {.type = MYSQL_DATETIME,.value = "DateChapter"};
	Col idLesson {.type = MYSQL_INT, .value ="IdLesson"};
	Col idUser { .type = MYSQL_INT, .value = "IdUser"};
	Col idRecorder { .type = MYSQL_INT, .value = "IdRecorder"};
} Chapter;

typedef struct _Category
{
	string name = "Category";
	Col id {.type = MYSQL_INT,.value = "IdCategory"};
	Col categoryName {.type = MYSQL_STRING,.value = "NameCategory"};

} Category;

typedef struct _Lesson
{
	string name = "Lesson";
	Col id {.type = MYSQL_INT,.value = "IdLesson"};
	Col lessonName {.type = MYSQL_TEXT,.value = "NameLesson"};
	Col date {.type = MYSQL_DATETIME,.value = "DateLesson"};
	Col status {.type = MYSQL_ENUM,.value = "StatusLesson"};

} Lesson;

typedef struct _Attachment
{
	string name = "Attachment";
	Col id {.type = MYSQL_INT,.value = "IdAttachment"};
	Col AttachmentName {.type = MYSQL_STRING,.value = "NameAttachment"};
	Col path {.type = MYSQL_STRING,.value = "PathAttachment"};
} Attachment;

typedef struct _Reference
{
	string name = "Reference";
	Col text {.type = MYSQL_TEXT,.value = "TextReference"};
	Col date {.type = MYSQL_DATETIME,.value = "TimeCode"};
	Col relevance {.type = MYSQL_INT,.value = "Relevance"};
} Reference;

typedef struct _FileLesson
{
	string name = "FileLesson";
	Col id {.type = MYSQL_INT,.value = "IdFileLesson"};
	Col fileLessonName {.type = MYSQL_STRING,.value = "NameFileLesson"};
	Col type {.type = MYSQL_ENUM,.value = "TypeFile"};
	Col status {.type = MYSQL_ENUM,.value = "StatusFileLesson"};
} FileLesson;







#endif // DEFVARIABLEBASE_H
