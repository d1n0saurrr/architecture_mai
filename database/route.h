#ifndef ROUTE_H
#define ROUTE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database {
    class Route {
        private:
            long _id;
            long _author_id;
            std::vector<std::string> _points;
            std::string _name;
            bool _deleted;
        public:
            static Route fromJson(const std::string &json);
            Poco::JSON::Object::Ptr toJSON() const;
            static Route empty() {
                Route route;
                route._id = -1;
                return route;
            }

            friend std::ostream& operator<<(std::ostream& os, const Route& route);
            long get_id() const;
            long get_author_id() const;
            const std::string &get_name() const;
            const std::vector<std::string> &get_points() const;
            const std::string get_points_string() const;
            bool is_deleted() const;

            long& id();
            long& author_id();
            std::string &name();
            std::vector<std::string> &points();
            bool &deleted();
            
            static std::vector<std::string> pointsFromJson(const std::string &json);
            static Route get_by_id(long id);
            static std::vector<Route> get_routes(long author_id);
            void save_to_db();
    };
}

#endif