#include "input_reader.h"
#include "stat_reader.h"

// напишите решение с нуля
// код сохраните в свой git-репозиторий

namespace inputReader {
    using namespace std;

    string Lstrip(string line) {
        while (!line.empty() && isspace(line[0])) {
            line.erase(0, 1);
        }
        return line;
    }

    string Rstrip(string line) {
        std::size_t found = line.find_last_not_of(' ');
        if (found != std::string::npos) {
            line.erase(found + 1, line.size() - found);
        }
        return line;
    }

    pair<string, string> Split(const string &line, const string &separator) {
        size_t pos = line.find(separator);
        string left = Lstrip(Rstrip(line.substr(0, pos)));

        if (pos < line.size() && pos + separator.size() < line.size()) {
            return {left, Rstrip(Lstrip(line.substr(pos + separator.size())))};
        } else {
            return {left, string()};
        }
    }

    void ListQueryIn::AddQueryItem(std::string &line, Attributes *atr_container, AttributesOut *atrout_container) {
        string str_key;
        string str_value;
        for (char &c: line) {

            if (str_key != "Stop" && str_key != "Bus") {
                str_key += c;
            } else {
                str_value += c;
            }

        }

        if (str_key == "Stop" || str_key == "Bus") {
            if (atr_container != nullptr) {
                (*atr_container)[str_key].push_back(str_value);
            } else if (atrout_container != nullptr) {
                (*atrout_container).push_back(make_pair(str_key, str_value));
            }
        }
    }

    Attributes &ListQueryIn::AsMapIn() {
        return attrs_;
    }

    size_t ListQueryIn::GetSizeMapIn() const {
        return attrs_.size();
    }

    ListQueryIn::AttributesOut &ListQueryIn::AsMapOut() {
        return attrs_out;
    }

    size_t ListQueryIn::GetSizeMapOut() const {
        return attrs_out.size();
    }

    vector<std::tuple<const void *, std::string, double>> &ListQueryIn::AsMapLength() {
        return attrs_length;
    }

    TransportCatalogue::Stop GetStops(string &line) {
        TransportCatalogue::Stop stop;
        auto [stop_name, stop_query] = Split(line, ":");
        auto [lat, stop_query_without_lat] = Split(stop_query, ",");
        auto [lng, stop_query_length] = Split(stop_query_without_lat, ",");

        stop.name = Rstrip(Lstrip(stop_name));
        stop.coordinates.lat = std::stod(lat);
        stop.coordinates.lng = std::stod(lng);
        stop.rest_query = stop_query_length;
        return stop;
    }

    struct refForPar {
        inputReader::ListQueryIn *list_queryIn;
        TransportCatalogue::Buses *buses;
        TransportCatalogue::Stops *stops;
    };

    void AddStopBus(refForPar *ref) {
        //проверка на условие того, что контейнер запросов не пуст
        if (!ref->list_queryIn->AsMapIn().empty()) {
            //проверка на наличие запросов на добавление остановок
            if (ref->list_queryIn->AsMapIn().count("Stop") > 0) {
                //цикл запросов добавления остановок
                for (auto query: ref->list_queryIn->AsMapIn().at("Stop")) {
                    TransportCatalogue::Stop stop = GetStops(query);
                    ref->stops->AddStop(stop.name, stop.coordinates);
                    const auto *reff = ref->stops->FindStop(stop.name);
                    if (!stop.rest_query.empty()) {
                        pair<string, string> temp_rest_query_for_parsing;
                        string next_line = stop.rest_query;
                        while (true) {
                            temp_rest_query_for_parsing = Split(next_line, ",");
                            auto [length_, name_stop] = Split(temp_rest_query_for_parsing.first, "m to ");
                            ref->list_queryIn->AsMapLength().emplace_back(reff, name_stop, std::stod(length_));
                            if (temp_rest_query_for_parsing.second.empty()) {
                                break;
                            }
                            next_line = temp_rest_query_for_parsing.second;
                        }
                    }
                }
            }
            if (ref->list_queryIn->AsMapIn().count("Stop") > 0) {
                //цикл запросов добавления остановок в маршрут
                string line_stops;
                for (const auto &query: ref->list_queryIn->AsMapIn().at("Bus")) {
                    bool cicle_route = false;
                    pair<string, string> temp_for_parsing_route;
                    auto [bus_number, line_stops] = Split(query, ":");
                    std::vector<std::string> stopsInBus;
                    while (true) {
                        if (line_stops.find_first_of('>') != std::string::npos) {
                            cicle_route = true;
                            temp_for_parsing_route = Split(line_stops, ">");
                        } else if (line_stops.find_last_not_of('-') != std::string::npos) {
                            temp_for_parsing_route = Split(line_stops, "-");
                        } else {
                            break;
                        }
                        stopsInBus.push_back(temp_for_parsing_route.first);
                        line_stops = temp_for_parsing_route.second;
                        if (temp_for_parsing_route.second.empty()) {
                            break;
                        }
                    }
                    ref->buses->AddBus(bus_number, stopsInBus);
                    //ф-я вставки расстояний между соседними остановками
                    ref->buses->SetStopDistance(bus_number);
                    ref->buses->FindBus(bus_number)->cicle_route = cicle_route;
                }
            }
        }
    };

    void LoadLine(std::istream &input) {
        int count_;
        TransportCatalogue::Stops stops;
        TransportCatalogue::Buses buses(&stops);
        ListQueryIn list_queryIn;
        refForPar references{};
        references.list_queryIn = &list_queryIn;
        references.stops = &stops;
        references.buses = &buses;
        StatReader::Print Print(&buses, &stops);

        while (!input.eof()) {

            input >> count_;
            string line;
            getline(input, line);

            for (int i = 0; i < count_; i++) {
                getline(input, line);
                line = Lstrip(line);

                if (!line.empty() && line.find_first_of(':') != std::string::npos) {

                    list_queryIn.AddQueryItem(line, &list_queryIn.AsMapIn(), nullptr);

                } else {
                    list_queryIn.AddQueryItem(line, nullptr, &list_queryIn.AsMapOut());
                }
            }
        }

        AddStopBus(&references);

        //проверка на условие того, что контейнер расстояний между соседними остановками не пуст
        if (!list_queryIn.AsMapLength().empty() && !list_queryIn.AsMapLength().empty()) {
            buses.SetStopLength(&list_queryIn.AsMapLength());
        }
        //проверка на условие того, что контейнер запросов не пуст
        if (!list_queryIn.AsMapOut().empty() && list_queryIn.AsMapOut().size() > 0) {
            Print.PrintValues(&list_queryIn.AsMapOut());
        }
    }
}