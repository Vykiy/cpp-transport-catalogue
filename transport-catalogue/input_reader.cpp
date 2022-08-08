#include "input_reader.h"
#include "stat_reader.h"

// напишите решение с нуля
// код сохраните в свой git-репозиторий

namespace inputReader {
    using namespace std;

    string Lstrip(string line) {
        while (!line.empty() && isspace(line[0])) {
            //cout << line << endl;
            line.erase(0, 1);
            //cout << line << endl;
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

    pair<string, string> Split(const string &line, const string &by) {
        size_t pos = line.find(by);
        string left = Lstrip(Rstrip(line.substr(0, pos)));

        if (pos < line.size() && pos + by.size() < line.size()) {
            return {left, Rstrip(Lstrip(line.substr(pos + by.size())))};
        } else {
            return {left, string()};
        }
    }

    void ListQueryIn::AddQueryItem(std::string &line, Attributes *atr_container, AttributesOut *atrout_container) {
        //cout << line << endl;
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

        //cout << "key = " << str_key << " value = " << str_value << endl;

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

    /*ListQueryIn::ListQueryIn(std::string line){
        
    }*/

    TransportCatalogue::Stop get_stops(string &line) {
        TransportCatalogue::Stop stop;
        auto [stopName, stopQuery] = Split(line, ":");
        auto [lat, stopQueryWithoutLat] = Split(stopQuery, ",");
        auto [lng, stopQueryLength] = Split(stopQueryWithoutLat, ",");

        //throw std::invalid_argument("id");

        ///std::cout.precision(20);
        stop.Name = Rstrip(Lstrip(stopName));
        stop.coordinates.lat = std::stod(lat);
        stop.coordinates.lng = std::stod(lng);
        stop.RestQuery = stopQueryLength;
        return stop;
    }

    struct refForPar {
        inputReader::ListQueryIn *list_queryIn;
        TransportCatalogue::Buses *buses;
        TransportCatalogue::Stops *stopS;
    };

    void AddStopBus(refForPar *ref) {
            //проверка на условие того, что контейнер запросов не пуст

            if (!ref->list_queryIn->AsMapIn().empty()) {
                //проверка на наличие запросов на добавление остановок
                if (ref->list_queryIn->AsMapIn().count("Stop") > 0) {
                    //цикл запросов добавления остановок
                    for (auto query: ref->list_queryIn->AsMapIn().at("Stop")) {
                        auto stName = get_stops(query).Name;
                        auto coor = get_stops(query).coordinates;
                        auto restQuery = get_stops(query).RestQuery;
                        //cout.precision(8);
                        //auto bb = get_stops(query);
                        /*cout<< &std::get<0>(bb) <<endl;
                        cout<< std::get<1>(bb) <<endl;
                        cout<< std::get<2>(bb) <<endl;*/
                        ref->stopS->AddStop(stName, coor);
                        const auto *reff = ref->stopS->FindStop(stName);
                        if (!restQuery.empty()) {
                            //cout<< "reff = " << reff << endl;
                            pair<string, string> value;
                            string next_line = restQuery;
                            while (true) {
                                //cout << "next_line = " << next_line << endl;
                                value = Split(next_line, ",");
                                auto [length_, name_stop] = Split(value.first, "m to ");
                                ref->list_queryIn->AsMapLength().emplace_back(reff, name_stop, std::stod(length_));

                                if (value.second == string()) {
                                    break;
                                }
                                next_line = value.second;
                            }
                        }
                        //cout<< grade2  <<" " <<name2 <<endl;
                    }
                }
                if (ref->list_queryIn->AsMapIn().count("Stop") > 0) {
                    //цикл запросов добавления остановок в маршрут
                    string line_stops;
                    for (const auto &query: ref->list_queryIn->AsMapIn().at("Bus")) {
                        bool cicle_route = false;
                        pair<string, string> value;
                        auto [bus_number, line_stops] = Split(query, ":");
                        //cout << bus_number << "=" << endl;
                        std::vector<std::string> stopsInBus;
                        while (true) {

                            if (line_stops.find_first_of('>') != std::string::npos) {
                                cicle_route = true;
                                value = Split(line_stops, ">");

                                //cout << value.first<< endl;
                            } else if (line_stops.find_last_not_of('-') != std::string::npos) {
                                value = Split(line_stops, "-");
                                //cout << value.first<< endl;
                            } else {
                                break;
                            }
                            stopsInBus.push_back(value.first);
                            //buses.AddBus(bus_number, value.first);
                            line_stops = value.second;
                            //cout << value.first << "|" << value.second << endl;
                            if (value.second.empty()) {
                                break;
                            }
                        }
                        ref->buses->AddBus(bus_number, stopsInBus);
                        //ф-я вставки расстояний между соседними остановками
                        ref->buses->SetStopDistance(bus_number);
                        ref->buses->FindBus(bus_number)->cicle_route = cicle_route;
                        //stops.AddStop(get_stops(query));
                        //cout<< "buses:"<< bus_number  <<endl;
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
        references.stopS = &stops;
        references.buses = &buses;
        StatReader::Print Print(&buses, &stops);

        while (!input.eof()) {

            input >> count_;
            string line;
            getline(input, line);

            for (int i = 0; i < count_; i++) {
                //cout << i << endl;
                getline(input, line);
                //cout << line << endl;
                line = Lstrip(line);

                if (!line.empty() && line.find_first_of(':') != std::string::npos) {

                    list_queryIn.AddQueryItem(line, &list_queryIn.AsMapIn(), nullptr);

                } else {
                    list_queryIn.AddQueryItem(line, nullptr, &list_queryIn.AsMapOut());
                }
            }
        }//while(!input.eof())

        AddStopBus(&references);

        //проверка на условие того, что контейнер расстояний между соседними остановками не пуст
        if (!list_queryIn.AsMapLength().empty() && !list_queryIn.AsMapLength().empty()) {
            buses.SetStopLength(&list_queryIn.AsMapLength());
        }
        //проверка на условие того, что контейнер запросов не пуст
        if(!list_queryIn.AsMapOut().empty() && list_queryIn.AsMapOut().size() > 0) {
            Print.PrintValues(&list_queryIn.AsMapOut());
        }
    }
}