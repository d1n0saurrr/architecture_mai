#include "cache.h"
#include "../config/config.h"

#include <exception>
#include <mutex>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

namespace database
{
    std::mutex _mtx;
    Cache::Cache() {
        std::string host = Config::get().get_cache_servers().substr(0, Config::get().get_cache_servers().find(':'));
        std::string port = Config::get().get_cache_servers().substr(Config::get().get_cache_servers().find(':') + 1);
        _stream = rediscpp::make_stream(host, port);
    }

    Cache Cache::get() {
        static Cache instance;
        return instance;
    }

    void Cache::put([[maybe_unused]] long id, [[maybe_unused]] const std::string &val) {
        std::lock_guard<std::mutex> lck(_mtx);
        rediscpp::value response = rediscpp::execute(*_stream, "set",
                                                     std::to_string(id),
                                                     val, "ex", "60");
    }

    bool Cache::get([[maybe_unused]] long id, [[maybe_unused]] std::string &val) {
        std::lock_guard<std::mutex> lck(_mtx);
        rediscpp::value response = rediscpp::execute(*_stream, "get", std::to_string(id));

        if (response.is_error_message())
            return false;
        if (response.empty())
            return false;

        val = response.as<std::string>();
        return true;
    }
}