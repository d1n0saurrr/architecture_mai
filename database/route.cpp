#include "route.h"
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

#define TABLE_NAME "route"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database {
    Route Route::get_by_id(long id) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Route route;

            select << "select id, author_id, name, points, deleted from "  << TABLE_NAME << " where id = ?",
                into(route._id),
                into(route._author_id),
                into(route._name),
                into(route._points),
                into(route._deleted),
                use(id),
                range(0, 1);
        
            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return route;

            return Route::empty();
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Route::save_to_db() {
        Poco::Data::Session session = database::Database::get().create_session();
        session.begin();
        try {
            Poco::Data::Statement insert(session);
            std::string points_string = get_points_string();

            insert << "INSERT INTO " << TABLE_NAME << " (author_id, name, points) VALUES(?, ?, ?)",
                use(_author_id),
                use(_name),
                use(points_string);

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

    std::vector<Route> Route::get_routes(long author_id) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            
            std::vector<Route> result;
            Route route;
            std::string points_str;

            select << "select id, author_id, name, points from "  << TABLE_NAME << " where deleted = false and author_id = ?",
                into(route._id),
                into(route._author_id),
                into(route._name),
                into(points_str),
                use(author_id),
                range(0, 1);
        
            while (!select.done())
                if (select.execute()) {
                    route._points = pointsFromJson(points_str);
                    result.push_back(route);
                }

            return result;
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Route::toJSON() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("author_id", _author_id);
        root->set("name", _name);
        root->set("points", _points);
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

    Route Route::fromJson(const std::string &str) {
        Route route;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        
        route.id() = getOrDefault<long>(object, "id", 0);
        route.author_id() = getOrDefault<long>(object, "authorId", 0);
        route.name() = getOrDefault<std::string>(object, "name", "");
        route.deleted() = getOrDefault<bool>(object, "deleted", false);
        std::vector<std::string> points_vector = std::vector<std::string>{};
        Poco::JSON::Array::Ptr points = object->getArray("route");
        Poco::JSON::Array::ConstIterator it;
        for (it = points->begin(); it != points->end(); ++it) {
            points_vector.push_back(it->toString());
        }
        route.points() = points_vector;

        return route;
    }

    std::vector<std::string> Route::pointsFromJson(const std::string &json) {
        std::cout << json << std::endl;
        std::vector<std::string> points_vector = std::vector<std::string>{};
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(json);
        Poco::JSON::Array::Ptr object = result.extract<Poco::JSON::Array::Ptr>();
        Poco::JSON::Array::ConstIterator it;
        for (it = object->begin(); it != object->end(); ++it) {
            points_vector.push_back(it->toString());
        }
        
        return points_vector;
    }

    const std::string Route::get_points_string() const {
        Poco::JSON::Array::Ptr root = new Poco::JSON::Array();
        for (std::string point : _points) {
            root->add(point);
        }
        
        std::ostringstream os;
        Poco::JSON::Stringifier::stringify(root, os);
        std::string result = "";
        result = os.str();
        return result;
    }

    std::ostream& operator<<(std::ostream& os, const Route& route)
    {
        std::string points_str;
        for (std::string point : route.get_points()) {
            points_str += point;
        }

        return os << "("
                << route.get_id() << ", "
                << route.get_author_id() << ", "
                << route.get_name() << ", ["
                << points_str << "], "
                << ")";
    }

    long Route::get_id() const {
        return _id;
    }

    long Route::get_author_id() const {
        return _author_id;
    }

    const std::string &Route::get_name() const {
        return _name;
    }

    const std::vector<std::string> &Route::get_points() const {
        return _points;
    }

    long &Route::id() {
        return _id;
    }

    long &Route::author_id() {
        return _author_id;
    }

    std::string &Route::name() {
        return _name;
    }

    std::vector<std::string> &Route::points() {
        return _points;
    }

    bool Route::is_deleted() const {
        return _deleted;
    }

    bool &Route::deleted() {
        return _deleted;
    }
}