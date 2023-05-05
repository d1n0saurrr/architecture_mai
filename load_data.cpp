#include <string>
#include <iostream>
#include <fstream>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include "database/database.h"

using namespace Poco::Data::Keywords;

auto main() -> int {
    try {
        std::cout << "starting" << std::endl;
        std::string json;
        std::ifstream is("data.json");
        std::istream_iterator<char> eos;
        std::istream_iterator<char> iit(is);
        while (iit != eos)
            json.push_back(*(iit++));
        is.close();
        std::cout << "read data" << std::endl;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(json);
        Poco::JSON::Array::Ptr arr = result.extract<Poco::JSON::Array::Ptr>();
        std::cout << "parsed data" << std::endl;
        Poco::Data::Session session = database::Database::get().create_session();
        std::cout << "created session" << std::endl;

        size_t i{0};
        for (i = 0; i < arr->size(); ++i) {
            Poco::JSON::Object::Ptr object = arr->getObject(i);
            std::string _first_name = object->getValue<std::string>("first_name");
            std::string _last_name = object->getValue<std::string>("last_name");
            std::string _tel = object->getValue<std::string>("tel");
            std::string _email = object->getValue<std::string>("email");
            std::string _login = object->getValue<std::string>("login");
            std::string _password = object->getValue<std::string>("password");

            Poco::Data::Statement insert(session);
            long _id = database::Database::generate_new_id();
            insert << "INSERT INTO user (id, login, password, role, tel, first_name, last_name, email) VALUES(?, ?, ?, 'user', ?, ?, ?, ?)" << database::Database::sharding_hint(_id),
                use(_id),
                use(_login),
                use(_password),
                use(_tel),
                use(_first_name),
                use(_last_name),
                use(_email);

            insert.execute();
            if(i % 100 == 0)
                std::cout << ".";
        }

        std::cout << "Inserted " << i << " records" << std::endl; 
       
    }
    catch (Poco::Data::MySQL::ConnectionException &e) {
        std::cout << "connection:" << e.what() << std::endl;
    }
    catch (Poco::Data::MySQL::StatementException &e) {

        std::cout << "statement:" << e.what() << std::endl;
    }
    catch (std::exception* ex) {
        std::cout << "exception:" << ex->what() << std::endl;
    }

    return 1;
}