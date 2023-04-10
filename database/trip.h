#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database {
    class Trip {
        private:
            long _id;
            long _author_id;
            long _route_id;
            std::string _name;
            std::string _description;
            Poco::DateTime _date;
            Poco::DateTime _creation_date;
            bool _deleted;
        public:
            static Trip fromJson(const std::string &json);
            Poco::JSON::Object::Ptr toJSON() const;
            static Trip empty() {
                Trip trip;
                trip._id = -1;
                return trip;
            }

            long get_id() const;
            long get_author_id() const;
            long get_route_id() const;
            const std::string &get_name() const;
            const std::string &get_description() const;
            const Poco::DateTime &get_date() const;
            const Poco::DateTime &get_creation_date() const;
            bool is_deleted() const;

            long& id();
            long& author_id();
            long& route_id();
            std::string &name();
            std::string &description();
            Poco::DateTime &date();
            Poco::DateTime &creation_date();
            bool &deleted();
            
            static Trip get_by_id(long id);
            static std::vector<Trip> get_trips();
            void save_to_db();
    };
}

#endif