#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"


#include <cassert>
#include <fstream>
#include <iostream>

int main() {
    using namespace std;
    using namespace tr_cat;
    aggregations::TransportCatalogue catalog;
    interface::JsonReader reader(catalog);
    Process(reader);
}