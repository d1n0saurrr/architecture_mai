#include "trip_service/trip_rest.h"

int main(int argc, char*argv[]) {
    HTTPTripWebServer app;
    return app.run(argc, argv);
}