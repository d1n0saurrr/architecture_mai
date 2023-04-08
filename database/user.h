#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database {
    class User {
        private:
            long _id;
            std::string _login;
            std::string _password;
            std::string _role;
            std::string _first_name;
            std::string _last_name;
            std::string _email;
            std::string _tel;
            bool _deleted;

            void insert_entity();
            void update_entity();
        public:
            static User fromJson(const std::string &json);
            Poco::JSON::Object::Ptr toJSON() const;
            static User empty() {
                User user;
                user._id = -1;
                return user;
            }

            friend std::ostream& operator<<(std::ostream& os, const User& user);
            long get_id() const;
            const std::string &get_login() const;
            const std::string &get_password() const;
            const std::string &get_role() const;
            const std::string &get_first_name() const;
            const std::string &get_last_name() const;
            const std::string &get_email() const;
            const std::string &get_tel() const;
            bool is_deleted() const;

            long& id();
            std::string &login();
            std::string &password();
            std::string &role();
            std::string &first_name();
            std::string &last_name();
            std::string &email();
            std::string &tel();
            bool &deleted();

            static long auth(std::string &login, std::string &password);
            static User get_by_id(long id);
            static std::vector<User> search(User like_user);
            void save_to_db();
    };
}

#endif