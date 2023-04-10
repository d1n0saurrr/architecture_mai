#include "user.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

#define TABLE_NAME "user"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database {
    long User::auth(std::string &login, std::string &password) {
        try {
            std::cout << "Trying to auth " << login << "::" << password << std::endl;
            Poco::Data::Session session = database::Database::get().create_session();
            std::cout << "Session created" << std::endl;
            Poco::Data::Statement select(session);
            long id;
            select << "SELECT id FROM " << TABLE_NAME << " where login = ? and password = ? and deleted = false",
                into(id),
                use(login),
                use(password),
                range(0, 1);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return id;
        } catch (Poco::Data::DataException &e) {
            std::cout << "Exception: " << e.what() << " :: " << e.message() << std::endl;
            return -1;
        }
        return 0;
    }

    User User::get_by_id(long id) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            User user;

            select << "select id, login, role, tel, first_name, last_name, email, deleted from "  << TABLE_NAME << " where id = ?",
                into(user._id),
                into(user._login),
                into(user._role),
                into(user._first_name),
                into(user._last_name),
                into(user._email),
                into(user._tel),
                into(user._deleted),
                use(id),
                range(0, 1);
        
            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return user;

            return User::empty();
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void User::save_to_db() {
        Poco::Data::Session session = database::Database::get().create_session();
        session.begin();
        try {
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO " << TABLE_NAME << " (login, password, role, tel, first_name, last_name, email) VALUES(?, ?, ?, ?, ?, ?, ?)",
                use(_login),
                use(_password),
                use(_role),
                use(_tel),
                use(_first_name),
                use(_last_name),
                use(_email);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1);

            if (!select.done())
                select.execute();
            
            session.commit();
            
            std::cout << "New entity id:" << _id << std::endl;
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            session.rollback();
            std::cout << "connection:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            session.rollback();
            std::cout << "statement:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        }
    }

    std::vector<User> User::search(User like_user) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            
            std::vector<User> result;
            User user;

            std::vector<std::string> conditions;

            if (like_user.get_login().length() > 0) {
                std::replace(like_user.login().begin(), like_user.login().end(), ' ', '%');
                conditions.push_back("lower(login) like '%" + like_user.get_login() + "%'");
            }

            if (like_user.get_email().length() > 0) {
                conditions.push_back("lower(email) like '%" + like_user.get_email() + "%'");
            }

            if (like_user.get_first_name().length() > 0) {
                std::replace(like_user.first_name().begin(), like_user.first_name().end(), ' ', '%');
                conditions.push_back("lower(first_name) like '%" + like_user.get_first_name() + "%'");
            }

            if (like_user.get_last_name().length() > 0) {
                std::replace(like_user.last_name().begin(), like_user.last_name().end(), ' ', '%');
                conditions.push_back("lower(first_name) like '%" + like_user.get_last_name() + "%'");
            }

            select << "select id, login, email, tel, first_name, last_name, role from "  << TABLE_NAME << " where deleted = false",
                into(user._id),
                into(user._login),
                into(user._email),
                into(user._tel),
                into(user._first_name),
                into(user._last_name),
                into(user._role),
                range(0, 1);

            if (conditions.size() > 0) {
                std::string cond_str;
                for (std::string cond: conditions) {
                    cond_str += " and " + cond;
                }
                std::cout << "Search condition: " << cond_str << std::endl;
                
                select << cond_str;
            }
        
            while (!select.done())
                if (select.execute())
                    result.push_back(user);

            return result;
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr User::toJSON() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("login", _login);
        root->set("email", _email);
        root->set("tel", _tel);
        root->set("firstName", _first_name);
        root->set("lastName", _last_name);
        root->set("deleted", _deleted);

        return root;
    }

    template<typename T>
    T getOrDefault(Poco::JSON::Object::Ptr object, std::string field, T defaultValue) {
        if (object->has(field)) {
            return object->getValue<T>(field);
        }
        return defaultValue;
    }

    User User::fromJson(const std::string &str) {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        
        user.id() = getOrDefault<long>(object, "id", 0);
        user.login() = getOrDefault<std::string>(object, "login", "");
        user.password() = getOrDefault<std::string>(object, "password", "");
        user.email() = getOrDefault<std::string>(object, "email", "");
        user.tel() = getOrDefault<std::string>(object, "tel", "");
        user.first_name() = getOrDefault<std::string>(object, "firstName", "");
        user.last_name() = getOrDefault<std::string>(object, "lastName", "");
        user.role() = getOrDefault<std::string>(object, "role", "");
        user.deleted() = getOrDefault<bool>(object, "deleted", false);

        return user;
    }

    std::ostream& operator<<(std::ostream& os, const User& user)
    {
        return os << "("
                << user.get_id() << ", "
                << user.get_login() << ", "
                << user.get_password() << ", "
                << user.get_email() << ", "
                << user.get_first_name() << ", "
                << user.get_last_name() << ", "
                << user.get_tel()
                << ")";
    }

    const std::string &User::get_login() const {
        return _login;
    }

    const std::string &User::get_password() const {
        return _password;
    }

    std::string &User::login() {
        return _login;
    }

    std::string &User::password() {
        return _password;
    }

    const std::string &User::get_role() const {
        return _role;
    }

    const std::string &User::get_tel() const {
        return _tel;
    }

    long User::get_id() const {
        return _id;
    }

    const std::string &User::get_first_name() const {
        return _first_name;
    }

    const std::string &User::get_last_name() const {
        return _last_name;
    }

    const std::string &User::get_email() const {
        return _email;
    }

    long &User::id() {
        return _id;
    }

    std::string &User::first_name() {
        return _first_name;
    }

    std::string &User::last_name() {
        return _last_name;
    }

    std::string &User::email() {
        return _email;
    }

    std::string &User::role() {
        return _role;
    }

    std::string &User::tel() {
        return _tel;
    }

    bool User::is_deleted() const {
        return _deleted;
    }

    bool &User::deleted() {
        return _deleted;
    }
}