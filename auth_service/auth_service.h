#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"
#include <string>
#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Poco/JWT/Token.h"
#include "Poco/JWT/Signer.h"
#include <istream>
#include <ostream>
#include <sstream>

#include "../database/user.h"
#include "../utils/request.h"
#include "../utils/validation.h"
#include "../utils/exceptions.h"

bool decode_info(const std::string info, std::string &login, std::string &password) {
    std::istringstream istr(info);
    std::ostringstream ostr;
    Poco::Base64Decoder b64in(istr);
    copy(std::istreambuf_iterator<char>(b64in),
         std::istreambuf_iterator<char>(),
         std::ostreambuf_iterator<char>(ostr));
    std::string decoded = ostr.str();

    size_t pos = decoded.find(':');
    login = decoded.substr(0, pos);
    password = decoded.substr(pos + 1);
    return true;
}

std::string getJWTKey() {
    if (std::getenv("JWT_KEY") != nullptr) {
        return std::getenv("JWT_KEY");
    }
    return "0123456789ABCDEF0123456789ABCDEF";
}

std::string generate_token(long &id, std::string &login) {
    Poco::JWT::Token token;
    token.setType("JWT");
    token.setSubject("login");
    token.payload().set("login", login);
    token.payload().set("id", id);
    token.setIssuedAt(Poco::Timestamp());

    Poco::JWT::Signer signer(getJWTKey());
    return signer.sign(token, Poco::JWT::Signer::ALGO_HS256);
}

bool extract_payload(std::string &jwt_token, long &id, std::string &login) {
    if (jwt_token.length() == 0) {
        return false;
    }

    Poco::JWT::Signer signer(getJWTKey());
    try {
        Poco::JWT::Token token = signer.verify(jwt_token);
        if (token.payload().has("login") && token.payload().has("id")) {
            login = token.payload().getValue<std::string>("login");
            id = token.payload().getValue<long>("id");
            return true;
        }
        std::cout << "Not enough fields in token" << std::endl;

    } catch (...) {
        std::cout << "Token verification failed" << std::endl;
    }
    return false;
}

std::string auth_user(std::string info) {
    std::string login;
    std::string password;

    if (!decode_info(info, login, password)) {
        throw access_denied_exception("Can't decode info");
    }

    long id = database::User::auth(login, password);
    if (id <= 0) {
        throw access_denied_exception("Wrong login/password");
    }
    std::cout << "Found user with id " << id << std::endl;
    return generate_token(id, login);
}

database::User validate(std::string token) {
    std::string login;
    long id;
    if (extract_payload(token, id, login)) {
        database::User user = database::User::get_by_id(id);
        if (user.get_id() <= 0) {
            throw access_denied_exception("User not found");
        }
        if (user.is_deleted()) {
            throw access_denied_exception("User deleted");
        }
        return user;
    } else {
        throw access_denied_exception("Failed to deserialize token");
    }
}

long create_user(std::string body) {
    if (body.length() == 0) {
        throw validation_exception("Body is missing");
    }
    std::string validation_message;
    database::User user = database::User::fromJson(body);
    user.role() = "user";

    if (validate_user(user, validation_message)) {
        std::cout << "Creating new user: " << body << std::endl;
        user.save_to_db();
        return user.get_id();
    } else {
        throw validation_exception(validation_message);
    }
}

#endif