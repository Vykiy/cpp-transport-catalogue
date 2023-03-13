#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace tr_cat {
    namespace aggregations {

        class InputReader {

        public:

            explicit InputReader(TransportCatalogue& catalog)
            :catalog_(catalog){}

            explicit InputReader(TransportCatalogue& catalog, std::istream& input)
            :input_(input), catalog_(catalog){}

            void ParseBase() {
                ParseBase(input_);
            }

            void ParseBase(std::istream& input);

            void AddBuses() {
                AddBuses(catalog_);
            }

            void AddBuses(TransportCatalogue& catalog);

            void AddStops() {
                AddStops(catalog_);
            }

            void AddStops(TransportCatalogue& catalog);

            void AddDistances() {
                AddDistances(catalog_);
            }

            void AddDistances(TransportCatalogue& catalog);

        private:

            struct BusInput {
                std::string name;
                std::vector <std::string> stops;
                bool is_ring;
            };

            struct StopInput {
                std::string name;
                geo::Coordinates coordinates;
                std::vector<std::pair<std::string, int>> distances;
            };

            std::istream& input_ = std::cin;
            std::vector<StopInput> stops_;
            std::vector<BusInput> buses_;

            TransportCatalogue& catalog_;

            void ParseBus (std::string line);

            void ParseStop(std::string line);

        };
    }//aggregations



    void ReadBase(aggregations::TransportCatalogue& catalog, std::istream& input);

    void ReadBase(aggregations::TransportCatalogue& catalog);
}//tr_cat