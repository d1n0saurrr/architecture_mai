#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include "./user.h"
#include "./route.h"

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
            database::User _author;
            std::vector<database::User> _passengers;
            database::Route _route;
            std::vector<std::string> _points;
        public:
            static Trip fromJson(const std::string &json);
            Poco::JSON::Object::Ptr toJSON() const;
            static Trip empty() {
                Trip trip;
                trip._id = -1;
                return trip;
            }

            friend std::ostream& operator<<(std::ostream& os, const Trip& trip);
            long get_id() const;
            long get_author_id() const;
            long get_route_id() const;
            const std::string &get_name() const;
            const std::string &get_description() const;
            const Poco::DateTime &get_date() const;
            const Poco::DateTime &get_creation_date() const;
            bool is_deleted() const;
            const database::User &get_author() const;
            const std::vector<database::User> &get_passengers() const;
            const database::Route &get_route() const;
            const std::vector<std::string> &get_points() const;

            long& id();
            long& author_id();
            long& route_id();
            std::string &name();
            std::string &description();
            Poco::DateTime &date();
            Poco::DateTime &creation_date();
            bool &deleted();
            database::User &author();
            std::vector<database::User> &passengers();
            database::Route &route();
            std::vector<std::string> &points();
            void add_passenger(long passenger_id);
            void remove_passenger(long passenger_id);
            
            static Trip get_by_id(long id);
            static std::vector<Trip> get_trips();
            void save_to_db();
    };
}

#endif