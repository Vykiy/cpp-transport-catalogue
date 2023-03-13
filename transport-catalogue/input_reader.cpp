#include "input_reader.h"

namespace tr_cat {
    namespace aggregations {

        using namespace std::string_literals;

        void InputReader::ParseBase(std::istream& input) {
            int n;
            input >> n;

            {
                std::string _;
                getline(input, _);
            }

            for (int i = 0; i < n; ++i) {
                std::string line;
                getline(input, line);

                std::string word = line.substr(0, line.find_first_of(' '));
                line.erase(0, word.size());
                line.erase(0, line.find_first_not_of(' '));

                if (word == "Bus"s) {
                    ParseBus(std::move(line));
                    continue;
                }

                if (word == "Stop"s) {
                    ParseStop(std::move(line));
                }
            }
        }

        void InputReader::AddBuses(TransportCatalogue& catalog) {
            for (BusInput& bus : buses_) {
                catalog.AddBus(bus.name, bus.stops, bus.is_ring);
            }
        }

        void InputReader::AddStops(TransportCatalogue& catalog) {
            for (StopInput& stop : stops_) {
                catalog.AddStop(stop.name, stop.coordinates);
            }
        }

        void InputReader::AddDistances(TransportCatalogue& catalog) {
            for (StopInput& stop_lhs : stops_) {
                for (auto& stop_rhs : stop_lhs.distances) {
                    catalog.AddDistance(stop_lhs.name, stop_rhs.first, stop_rhs.second);
                }
            }
        }


        void InputReader::ParseBus (std::string line) {

            buses_.push_back({});
            BusInput& bus = buses_.back();

            bus.name = line.substr(0, line.find_first_of(':'));
            line.erase(0, line.find_first_not_of(' ', 1 + bus.name.size()));

            while (true) {
                bus.stops.push_back(line.substr(0, 
                    std::adjacent_find(line.begin(), line.end(), [](const auto& lhs, const auto& rhs) {
                        return (rhs == ' ') && (lhs == '-' || lhs == '>');})
                    -line.begin()));

                line.erase(0, bus.stops.back().size());

                if (line.empty()) {
                    return;
                }

                bus.is_ring = line[0] != '-';

                size_t pos = std::min(bus.stops.back().size(), bus.stops.back().find_last_not_of(' '));

                if (pos == bus.stops.back().size()) {
                    return;
                }

                bus.stops.back().erase(pos + 1);
                line.erase(0, line.find_first_not_of(' ', 1));
            }
        }

        void InputReader::ParseStop (std::string line) {

            stops_.push_back({});
            StopInput& stop = stops_.back();

            stop.name = line.substr(0, line.find_first_of(':'));
            line.erase(0, line.find_first_not_of(' ', stop.name.size() + 1));

            int pos = line.find_first_of(',');
            stop.coordinates.lat = std::stod(line.substr(0, pos));
            line.erase(0, line.find_first_not_of(' ', pos + 1));

            pos = line.find_first_of(',');
            stop.coordinates.lng = std::stod(line.substr(0, pos));
            line.erase(0, line.find_first_not_of(' ', pos + 1));

            while (!line.empty()) {
                size_t pos = line.find_first_of('m');
                int distance = std::stoi(line.substr(0, pos));
                line.erase(0, line.find_first_not_of(' ', line.find_first_not_of(' ', pos + 1) + 2));

                pos = std::min(line.size(), line.find_first_of(','));
                stop.distances.emplace_back(line.substr(0, pos), distance);

                if (pos == line.size()) {
                    return;
                }

                line.erase(0, line.find_first_not_of(' ', pos + 1));
            }
        }
    }//aggregations


    void ReadBase(aggregations::TransportCatalogue& catalog, std::istream& input) {
        aggregations::InputReader reader(catalog, input);
        reader.ParseBase();
        reader.AddStops();
        reader.AddDistances();
        reader.AddBuses();
    }

    void ReadBase(aggregations::TransportCatalogue& catalog) {
        ReadBase(catalog, std::cin);
    }
}//tr_cat
