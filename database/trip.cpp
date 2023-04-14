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
#include "./user.h"
#include "../utils/json.h"

#define TABLE_NAME "trip"
#define TABLE_NAME_2 "passengers"

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
            if (!rs.moveFirst())
                return Trip::empty();

            trip.author() = database::User::get_by_id(trip._author_id);
            trip.route() = database::Route::get_by_id(trip._route_id);

            Poco::Data::Statement select2(session);
            long passenger_id;

            select2 << "select passenger_id from " << TABLE_NAME_2 << " where trip_id = ?",
                into(passenger_id),
                use(id),
                range(0, 1);
        
            while (!select2.done())
                if (select2.execute())
                    trip.passengers().push_back(database::User::get_by_id(passenger_id));

            return trip;
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
            trip.creation_date() = null;
            std::string points_str;

            select << "select t.id, t.author_id, r.points, t.date, t.name, t.description from " << TABLE_NAME << " t join route r on r.id = t.id where t.deleted = false",
                into(trip._id),
                into(trip._author_id),
                into(points_str),
                into(trip._date),
                into(trip._description),
                range(0, 1);
        
            while (!select.done())
                if (select.execute()) {
                    trip.points() = database::Route::pointsFromJson(points_str);
                    result.push_back(trip);
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

    void Trip::add_passenger(long passenger_id) {
        Poco::Data::Session session = database::Database::get().create_session();
        session.begin();
        try {
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO " << TABLE_NAME_2 << " (passenger_id, trip_id) VALUES(?, ?)",
                use(passenger_id),
                use(_id);

            insert.execute();
            long id;

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(id),
                range(0, 1);

            if (!select.done())
                select.execute();
            
            session.commit();
            
            std::cout << "New entity id:" << id << std::endl;
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

    void Trip::remove_passenger(long passenger_id) {
        Poco::Data::Session session = database::Database::get().create_session();
        session.begin();
        try {
            Poco::Data::Statement insert(session);

            insert << "DELETE FROM " << TABLE_NAME_2 << " WHERE id = ?",
                use(passenger_id);

            insert.execute();
            long id;

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(id),
                range(0, 1);

            if (!select.done())
                select.execute();
            
            session.commit();
            
            std::cout << "Delete entity id:" << id << std::endl;
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
    
    std::string get_date_formatted(Poco::DateTime date) {
        return Poco::DateTimeFormatter::format(date, "%Y-%m-%d %H:%M:%S");
    }

    Poco::JSON::Object::Ptr Trip::toJSON() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        Poco::JSON::Array::Ptr root2 = new Poco::JSON::Array();
        for (database::User passenger : _passengers) {
            root2->add(passenger.toJSON());
        }

        Poco::JSON::Array::Ptr root3 = new Poco::JSON::Array();
        for (std::string point : _points) {
            root3->add(point);
        }

        root->set("id", _id);
        if (_author.get_login() != "")
            root->set("author", _author.toJSON());
        else
            root->set("authorId", _author_id);
        if (_name != "")
            root->set("name", _name);
        if (_passengers.size() > 0)
            root->set("passengers", root2);
        if (_points.size() > 0)
            root->set("route", root3);
        else if (_route.get_name() != "")
            root->set("route", _route.toJSON());
        if (_description != "")
            root->set("description", _description);
        root->set("date", _date);
        if (_creation_date != null)
            root->set("creationDate", get_date_formatted(_creation_date));
        if (!_deleted)
            root->set("deleted", get_date_formatted(_deleted));

        return root;
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

    std::ostream& operator<<(std::ostream& os, const Trip& trip)
    {
        std::ostringstream passengers;
        passengers << "[";
        for (database::User user : trip.get_passengers())
            passengers << user << ", ";
        passengers << "]";

        std::string points_str;
        for (std::string point : trip.get_points()) {
            points_str += point;
        }

        return os << "("
                << trip.get_id() << ", "
                << trip.get_author() << ", "
                << trip.get_author_id() << ", "
                << trip.get_name() << ", "
                << trip.get_description() << ", "
                << get_date_formatted(trip.get_date()) << ", "
                << get_date_formatted(trip.get_creation_date()) << ", "
                << passengers.str() << ", "
                << trip.get_route() << ", "
                << trip.get_route_id() << ", "
                << points_str << ", "
                << ")";
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

    const database::User &Trip::get_author() const {
        return _author;
    }

    const std::vector<database::User> &Trip::get_passengers() const {
        return _passengers;
    }

    const database::Route &Trip::get_route() const {
        return _route;
    }

    const std::vector<std::string> &Trip::get_points() const {
        return _points;
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

    database::User &Trip::author() {
        return _author;
    }

    std::vector<database::User> &Trip::passengers() {
        return _passengers;
    }

    database::Route &Trip::route() {
        return _route;
    }

    std::vector<std::string> &Trip::points() {
        return _points;
    }

    bool Trip::is_deleted() const {
        return _deleted;
    }

    bool &Trip::deleted() {
        return _deleted;
    }
}