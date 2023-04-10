#include "trip.h"
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

#define TABLE_NAME "trip"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database {
    Trip Trip::get_by_id(long id) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Trip trip;

            select << "select id, author_id, name, description, route_id, date, creation_date, deleted from "  << TABLE_NAME << " where id = ?",
                into(trip._id),
                into(trip._author_id),
                into(trip._name),
                into(trip._description),
                into(trip._route_id),
                into(trip._date),
                into(trip._creation_date),
                into(trip._deleted),
                use(id),
                range(0, 1);
        
            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return trip;

            return Trip::empty();
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Trip::save_to_db() {
        Poco::Data::Session session = database::Database::get().create_session();
        session.begin();
        try {
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO " << TABLE_NAME << " (author_id, name, description, route_id, date) VALUES(?, ?, ?, ?, ?)",
                use(_author_id),
                use(_name),
                use(_description),
                use(_route_id),
                use(_date);

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

    std::vector<Trip> Trip::get_trips() {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            
            std::vector<Trip> result;
            Trip trip;
            std::string points_str;

            select << "select id, author_id, name, description, route_id, date, creation_date from "  << TABLE_NAME << " where deleted = false",
                into(trip._id),
                into(trip._author_id),
                into(trip._name),
                into(trip._description),
                into(trip._route_id),
                into(trip._date),
                into(trip._creation_date),
                range(0, 1);
        
            while (!select.done())
                if (select.execute())
                    result.push_back(trip);

            return result;
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << " :: " << e.message() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Trip::toJSON() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("authorId", _author_id);
        root->set("name", _name);
        root->set("description", _description);
        root->set("routeId", _route_id);
        root->set("date", _date);
        root->set("creationDate", _creation_date);
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

    Trip Trip::fromJson(const std::string &str) {
        Trip trip;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        
        trip.id() = getOrDefault<long>(object, "id", 0);
        trip.author_id() = getOrDefault<long>(object, "authorId", -1);
        trip.name() = getOrDefault<std::string>(object, "name", "");
        trip.description() = getOrDefault<std::string>(object, "description", "");
        trip.route_id() = getOrDefault<long>(object, "routeId", -1);
        
        if (object->has("date"))
            trip.date() = object->getValue<Poco::DateTime>("date");

        trip.deleted() = getOrDefault<bool>(object, "deleted", false);

        return trip;
    }

    long Trip::get_id() const {
        return _id;
    }

    long Trip::get_author_id() const {
        return _author_id;
    }

    const std::string &Trip::get_name() const {
        return _name;
    }

    const std::string &Trip::get_description() const {
        return _description;
    }

    long Trip::get_route_id() const {
        return _route_id;
    }

    const Poco::DateTime &Trip::get_date() const {
        return _date;
    }

    const Poco::DateTime &Trip::get_creation_date() const {
        return _creation_date;
    }

    long &Trip::id() {
        return _id;
    }

    long &Trip::author_id() {
        return _author_id;
    }

    std::string &Trip::name() {
        return _name;
    }

    std::string &Trip::description() {
        return _description;
    }

    long &Trip::route_id() {
        return _route_id;
    }

    Poco::DateTime &Trip::creation_date() {
        return _creation_date;
    }

    Poco::DateTime &Trip::date() {
        return _date;
    }

    bool Trip::is_deleted() const {
        return _deleted;
    }

    bool &Trip::deleted() {
        return _deleted;
    }
}