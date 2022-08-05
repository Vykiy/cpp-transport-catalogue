#include "input_reader.h"

// напишите решение с нуля
// код сохраните в свой git-репозиторий
using namespace std;
namespace loaddata {

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

    pair <string, string> Split(string line, string by) {
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
        string str_key = "";
        string str_value = "";
        for (auto c = line.begin(); c != line.end(); c++) {

            if (str_key != "Stop" && str_key != "Bus") {
                str_key += *c;
            } else {
                str_value += *c;
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

    AttributesOut &ListQueryIn::AsMapOut() {
        return attrs_out;
    }

    size_t ListQueryIn::GetSizeMapOut() const {
        return attrs_out.size();
    }

    vector <std::tuple<const void *, std::string, double>> &ListQueryIn::AsMapLength() {
        return attrs_length;
    }

    /*ListQueryIn::ListQueryIn(std::string line){
        
    }*/

    std::tuple<std::string, double, double, std::string> get_stops(string &line) {
        auto [val1, val2] = Split(line, ":");
        auto [lat, val3] = Split(val2, ",");
        auto [lng, val4] = Split(val3, ",");

        //throw std::invalid_argument("id");

        ///std::cout.precision(20);
        return std::make_tuple(Rstrip(Lstrip(val1)), std::stod(lat), std::stod(lng), val4);
    }

    void LoadLine(std::istream &input) {
        int count_;
        TransportCatalogue::Stops stops;
        TransportCatalogue::Buses buses(&stops);
        ListQueryIn list_queryIn;
        StatReader::Print Print(&buses, &stops);

        while (!input.eof()) {

            input >> count_;
            string line = "";
            getline(input, line);

            for (int i = 0; i < count_; i++) {
                //cout << i << endl;
                getline(input, line);
                //cout << line << endl;
                line = Lstrip(line);

                if (line.size() > 0 && line.find_first_of(':') != std::string::npos) {

                    list_queryIn.AddQueryItem(line, &list_queryIn.AsMapIn(), nullptr);

                } else {
                    list_queryIn.AddQueryItem(line, nullptr, &list_queryIn.AsMapOut());
                }
            }

        }//while(!input.eof())


        //проверка на условие того, что контейнер запросов не пуст
        if (!list_queryIn.AsMapIn().empty()) {
            //проверка на наличие запросов на добавление остановок
            if (list_queryIn.AsMapIn().count("Stop") > 0) {
                //цикл запросов добавления остановок
                for (auto query: list_queryIn.AsMapIn().at("Stop")) {

                    //cout.precision(8);
                    auto bb = get_stops(query);
                    /*cout<< &std::get<0>(bb) <<endl;
                    cout<< std::get<1>(bb) <<endl;
                    cout<< std::get<2>(bb) <<endl;*/
                    stops.AddStop(std::make_tuple((std::get<0>(bb)), std::get<1>(bb), std::get<2>(bb)));
                    const auto *reff = stops.FindStop(std::get<0>(bb));
                    if (std::get<3>(bb).size()) {
                        //cout<< "reff = " << reff << endl;
                        pair<string, string> value;
                        string next_line = std::get<3>(bb);
                        while (true) {
                            //cout << "next_line = " << next_line << endl;
                            value = Split(next_line, ",");
                            auto [length_, name_stop] = Split(value.first, "m to ");
                            list_queryIn.AsMapLength().push_back(make_tuple(reff, name_stop, std::stod(length_)));

                            if (value.second == string()) {
                                break;
                            }
                            next_line = value.second;
                        }
                    }
                    //cout<< grade2  <<" " <<name2 <<endl;
                }
            }
            if (list_queryIn.AsMapIn().count("Stop") > 0) {
                //цикл запросов добавления остановок в маршрут
                string line_stops = "";
                for (auto query: list_queryIn.AsMapIn().at("Bus")) {
                    bool cicle_route = false;
                    pair<string, string> value;
                    auto [bus_number, line_stops] = Split(query, ":");
                    //cout << bus_number << "=" << endl;

                    while (true) {

                        if (line_stops.find_first_of(">") != std::string::npos) {
                            cicle_route = true;
                            value = Split(line_stops, ">");

                            //cout << value.first<< endl;
                        } else if (line_stops.find_last_not_of('-') != std::string::npos) {
                            value = Split(line_stops, "-");
                            //cout << value.first<< endl;
                        } else {
                            break;
                        }
                        buses.AddBus(bus_number, value.first);
                        line_stops = value.second;
                        //cout << value.first << "|" << value.second << endl;
                        if (value.second == string()) {
                            break;
                        }
                    }
                    //ф-я вставки расстояний между соседними остановками
                    buses.StopDistanceAdd(bus_number);
                    buses.FindBus(bus_number)->cicle_route = cicle_route;
                    //stops.AddStop(get_stops(query));
                    //cout<< "buses:"<< bus_number  <<endl;
                }
            }
        }
        //проверка на условие того, что контейнер расстояний между соседними остановками не пуст
        if (!list_queryIn.AsMapLength().empty() && list_queryIn.AsMapLength().size() > 0) {
            buses.StopLengthAdd(&list_queryIn.AsMapLength());
        }
        //проверка на условие того, что контейнер запросов не пуст
        if (!list_queryIn.AsMapOut().empty() && list_queryIn.AsMapOut().size() > 0) {
            string _name = "";
            for (auto &query: list_queryIn.AsMapOut()) {
                _name = Lstrip(Rstrip(query.second));

                if (query.first == "Bus") {
                    Print.PrintValueBus(_name);
                } else if (query.first == "Stop") {
                    Print.PrintValueStop(_name);
                }
            }


            /*
            if(list_queryIn.AsMapOut().find("Bus") != list_queryIn.AsMapOut().end()){
              for(auto query:list_queryIn.AsMapOut().at("Bus")){
                    string _name = Lstrip(Rstrip(query));
                    Print.PrintValueBus(_name);
               }
            }
            if(list_queryIn.AsMapOut().find("Stop") != list_queryIn.AsMapOut().end()){
              for(auto query:list_queryIn.AsMapOut().at("Stop")){
                    string _name = Lstrip(Rstrip(query));
                    Print.PrintValueStop(_name);
               }
            }
          */

        }
    }
}