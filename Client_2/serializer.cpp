#include <QDebug>

#include "serializer.h"
#include "../ProtoFiles/IAuthRequest.pb.h"
#include "../ProtoFiles/IAuthResponse.pb.h"
#include "../ProtoFiles/IGetRequest.pb.h"
#include "../ProtoFiles/IGetResponse.pb.h"
#include "../ProtoFiles/ISearchRequest.pb.h"
#include "../ProtoFiles/ISearchResponse.pb.h"
#include "../ProtoFiles/IGetChatHistoryRequest.pb.h"

QByteArray Serializer::serializeAuthReq(const QString &login, const QString &password)
{
    IAuthRequest request;

    request.set_login(login.toStdString());
    request.set_password(password.toStdString());

    std::string serialized_data;
    if (!request.SerializeToString(&serialized_data)){
        qDebug() << "Error while serialize";
    }

    QByteArray data(serialized_data.c_str(), serialized_data.size());
    return data;
}

QByteArray Serializer::serializeGetReq(const int userId)
{
    IGetRequest request;

    request.set_userid(userId);
    request.set_chats_limit(20); /// получаем максимум 20 чатов
    request.set_messages_in_chat_limit(20); /// максимум 20 сообщений в каждом чате

    std::string serialized_data;
    if (!request.SerializeToString(&serialized_data)){
        qDebug() << "Error while serialize get request";
    }

    QByteArray data(serialized_data.c_str(), serialized_data.size());
    return data;
}

QByteArray Serializer::serializeSearchUsersReq(const QString &loginToSearch)
{
    ISearchRequest request;

    request.set_login(loginToSearch.toStdString());

    std::string serialized_data;
    if (!request.SerializeToString(&serialized_data)){
        qDebug() << "Error while serialize search users request";
    }

    QByteArray data(serialized_data.c_str(), serialized_data.size());
    return data;
}

QByteArray Serializer::serializeGetChatHistoryReq(const int senderId, const int interlocutorId)
{
    IGetChatHistoryRequest request;

    request.set_sender_id(senderId);
    request.set_interlocutor_id(interlocutorId);

    std::string serialized_data;
    if (!request.SerializeToString(&serialized_data)){
        qDebug() << "Error while serialize get chat history request";
    }

    QByteArray data(serialized_data.c_str(), serialized_data.size());
    return data;
}

QPair<int, int> Serializer::deserializeAuthResponse(const QByteArray &data)
{
    IAuthResponse response;
    if (!response.ParseFromString(data.toStdString())) {
        qDebug() << "Error while deserialize auth response";
        return QPair<int, int>();
    }

    int serverResponseCode = response.code();
    int userId = response.userid();

    return qMakePair(serverResponseCode, userId);
}

QVector<Chat> Serializer::deserializeGetDefaultDataResponse(const QByteArray &data)
{
    QVector<Chat> chats;

    IGetResponse response;
    if (!response.ParseFromArray(data.data(), data.size())) {
        qDebug() << "Error while deserialize get default data response";
        return QVector<Chat>();
    }

    if (response.is_empty()) {
        qDebug() << "Флаг isEmpty == true, возвращаю пустой объект";
        return QVector<Chat>();
    }

    /// дальнейшая обработка непустого ответа здесь
    return QVector<Chat>(); /// чтобы не было warning
}

QList<QPair<int, QString>> Serializer::deserializeFoundUsersResponse(const QByteArray &data)
{
    QList<QPair<int, QString>> foundUsers;

    ISearchResponse response;
    if (!response.ParseFromArray(data.constData(), data.size())) {
        qDebug() << "Error while deserializing found users response";
        return QList<QPair<int, QString>>();
    }

    if (response.is_empty()) {
        qDebug() << "No users found (empty flag set)";
        return QList<QPair<int, QString>>();
    }

    // Извлекаем пользователей из ответа
    const auto& users = response.found_users();
    for (const auto& user : users) {
        foundUsers.append(qMakePair(user.user_id(), QString::fromStdString(user.login())));
    }

    return foundUsers;
}
