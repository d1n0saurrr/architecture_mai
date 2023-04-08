#ifndef AUTH_REST_H
#define AUTH_REST_H


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <Poco/URIStreamFactory.h>
#include <Poco/URIStreamOpener.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::NameValueCollection;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include <iostream>
#include <string>
#include <fstream>
#include "../database/user.h"
#include "../utils/exceptions.h"
#include "../utils/request.h"

class UserRequestHandler : public HTTPRequestHandler {
    public:
        UserRequestHandler(const std::string &format): _format(format){};

        void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
            response.add("Access-Control-Allow-Origin", "*");
            response.add("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            response.add("Access-Control-Allow-Headers", "Content-Type, api_key, Authorization");
            if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_OPTIONS) {
                response.setContentType("application/json");
                response.setKeepAlive(true);
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
                response.send();
                return;
            }

            try {
                std::string scheme;
                std::string token;
                request.getCredentials(scheme, token);

                std::string login;
                long id;
                if (!validateToken(scheme, token, id, login)) {
                    std::cout << "Failed to authorize user" << std::endl;
                    throw access_denied_exception("Failed to authorize user");
                }
                std::cout << "Authorized user " << login << std::endl;

                if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                    if (hasSubstr(request.getURI(), "/search")) {
                        const Poco::URI uri(request.getURI());
                        const Poco::URI::QueryParameters params = uri.getQueryParameters();
                        database::User likeUser;
                        for (std::pair<std::string, std::string> key_value: params) {
                            if (key_value.first == "login") {
                                likeUser.login() = key_value.second;
                            } else if (key_value.first == "first_name") {
                                likeUser.first_name() = key_value.second;
                            } else if (key_value.first == "last_name") {
                                likeUser.last_name() = key_value.second;
                            } else if (key_value.first == "email") {
                                likeUser.email() = key_value.second;
                            } else {
                                std::cout << "Param " << key_value.first << " :: " << key_value.second << " ignored" << std::endl;
                            }
                        }

                        std::vector<database::User> result = database::User::search(likeUser);
                        std::cout << "Found total " << result.size() << std::endl;

                        Poco::JSON::Array arr;
                        for (database::User user: result) {
                            arr.add(user.toJSON());
                        }
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        Poco::JSON::Stringifier::stringify(arr, ostr);
                    } else if (hasSubstr(request.getURI(), "/user")) {
                        const Poco::URI uri(request.getURI());
                        const Poco::URI::QueryParameters params = uri.getQueryParameters();
                        int id;
                        for (std::pair<std::string, std::string> key_value: params) {
                            if (key_value.first == "id") {
                                id = stoi(key_value.second);
                            }
                        }

                        database::User user = database::User::get_by_id(id);
                        if (user.get_id() == -1) {
                            throw not_found_exception("There is no user with id = " + id);
                        }

                        std::cout << "Found user: " << user << std::endl;
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        Poco::JSON::Stringifier::stringify(user.toJSON(), ostr);
                    }
                }
            } catch (validation_exception &ex) {
                std::cout << "Validation exception: " << ex.what() << std::endl;
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/validation");
                root->set("detail", ex.what());
                root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_BAD_REQUEST);
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
            } catch (not_found_exception &ex) {
                std::cout << "Not found exception: " << ex.what() << std::endl;
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/not_found");
                root->set("detail", ex.what());
                root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_NOT_FOUND);
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
            } catch (access_denied_exception &ex) {   
                std::cout << "Access denied exception: " << ex.what() << std::endl;
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/anauthorized");
                root->set("detail", ex.what());
                root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_UNAUTHORIZED);
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
            } catch (std::exception &ex) {
                std::cout << "Server exception: " << ex.what() << std::endl;
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/server_error");
                root->set("title", "Internal exception");
                root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_INTERNAL_SERVER_ERROR);
                root->set("detail", ex.what());
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
            }
            if (!response.sent()) {
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/not_found");
                root->set("detail", "Requested page is not available");
                root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_NOT_FOUND);
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
            }
        }
    private:
        std::string _format;
};

class HTTPUserRequestFactory : public HTTPRequestHandlerFactory {
    public:
        HTTPUserRequestFactory(const std::string &format) : _format(format){}
        HTTPRequestHandler *createRequestHandler([[maybe_unused]] const HTTPServerRequest &request){
            std::cout << "request [" << request.getMethod() << "] " << request.getURI() << std::endl;

            if (request.getURI().rfind("/user") == 0) {
                return new UserRequestHandler(_format);
            }
            return 0;
        }
    private:
        std::string _format;
};

class HTTPUserWebServer : public Poco::Util::ServerApplication {
    public:
        HTTPUserWebServer() : _helpRequested(false){}
        ~HTTPUserWebServer() {}
    protected:
        void initialize(Application &self) {
            loadConfiguration();
            ServerApplication::initialize(self);
        }
        void uninitialize() {
            ServerApplication::uninitialize();
        }
        int main([[maybe_unused]] const std::vector<std::string> &args) {
            const char * portValue = "8080";
            if (std::getenv("USER_SERVICE_PORT") != nullptr) {
                portValue = std::getenv("USER_SERVICE_PORT");
            }

            if (strlen(portValue) == 0) {
                std::cout << "Port value is missing" << std::endl;
                return Application::EXIT_DATAERR;
            }

            if (!_helpRequested) {
                ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", atoi(portValue)));
                HTTPServer srv(new HTTPUserRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
                srv.start();
                std::cout << "user server started on port " << portValue << std::endl;
                waitForTerminationRequest();
                srv.stop();
                std::cout << "user server stoped" << std::endl;
            }
            return Application::EXIT_OK;
        }
    private:
        bool _helpRequested;
};

#endif