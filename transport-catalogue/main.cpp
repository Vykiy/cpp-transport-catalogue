#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"

#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;
using namespace tr_cat;

int main() {
    aggregations::TransportCatalogue catalog;
    interface::JsonReader reader(catalog);
    interface::Process(reader);
}