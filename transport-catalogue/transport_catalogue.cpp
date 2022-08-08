// напишите решение с нуля
// код сохраните в свой git-репозиторий
//transport_catalogue.cpp
#include "transport_catalogue.h"


namespace TransportCatalogue {

    using namespace std;

    //ф-я добавления остановки
    void Stops::AddStop(const std::string &stopName, const Coordinates &coor) {
        //вставка остановки

        if (stopname_to_stop.find(stopName) == stopname_to_stop.end()) {
            Stop stop_;
            stop_.Name = stopName;
            stop_.coordinates.lat = coor.lat;
            stop_.coordinates.lng = coor.lng;
            stops_.push_back(stop_);
            size_t item_ = stops_.size() - 1;
            //вставка ссылки на структуру Stop
            stopname_to_stop[stops_[item_].Name] = &stops_[item_];
            //cout<<  stops_[item_].Name<< endl;
            //вставка в контейнер для быстрого поиска остановок с перечением маршрутов
            AddStopAccrossingBus(stops_[item_].Name, "");
            //cout << FindStop(std::get<0>(tup))->Name << endl;
            //cout << stop_.Name << " " << stop_.coordinates.lat<< " " << stop_.coordinates.lng << endl;
        }

    }

    //ф-я добавления ссылки на автобус
    void Stops::AddStopAccrossingBus(std::string_view name_stop, string_view name_bus) {
        if (name_bus == "") {
            stop_accross_to_bus[name_stop];
        } else {
            stop_accross_to_bus[name_stop].insert(name_bus);
        }

    }

    //ф-я поиска остановки
    const Stop *Stops::FindStop(const std::string &name) const {
        /* cout<<  stopname_to_stop.size()<< endl;
          for(auto eee:stopname_to_stop){
                cout<< eee.first << endl;
            }*/
        if (stopname_to_stop.find(name) != stopname_to_stop.end()) {
            //std::cout << "true" << endl;
            return stopname_to_stop.at(name);
        }

        return nullptr;
    }

    Buses::Buses(Stops *_stop) : stops_(_stop) {
    }

    //ф-я поиска автобусного маршрута
    Bus *Buses::FindBus(const std::string_view name) const {

        if (busname_to_bus.find(name) != busname_to_bus.end()) {
            //std::cout << "true" << endl;
            return busname_to_bus.at(name);
        }
        //std::cout << "false" << endl;
        return nullptr;
    }

    void Buses::AddBus(const std::string &name, const std::vector<std::string> &stopsInBus) {
        //вставка остановки
        if (FindBus(name) == nullptr) {
            Bus bus_;
            bus_.Name = name;
            buses_.push_back(bus_);
            //вставка ссылки на структуру Bus
            size_t item_ = buses_.size() - 1;
            busname_to_bus[buses_[item_].Name] = &buses_[item_];
            //контейнер для быстрого поиска остановок с перечением маршрутов
            //std::unordered_map<std::string_view, StopAcrossToBuses*> stopname_to_bus;

        }
        for (const auto &name_stop: stopsInBus) {
            const Stop *stp = stops_->FindStop(name_stop);
            Bus *bus = FindBus(name);
            //Вставка остановок в контейнер Stops структуры Bus
            bus->Stops.push_back(stp);
            //вставка в контейнер для быстрого поиска остановок с перечением маршрутов
            stops_->AddStopAccrossingBus(stp->Name, bus->Name);
        }
    }

    //ф-я вставки расстояний между соседними остановками
    void Buses::SetStopDistance(const std::string_view bus_name) { //для чего передавать все три параметра, когда в функцию уже передается автобус который итерируется по контейнеру
        auto bus_ = FindBus(bus_name);

        for (auto stops = (bus_->Stops).begin(); stops != (bus_->Stops).end() - 1;) {
            //cout<< "----"<< endl;
            //cout<< (*stops)->Name << "|" << (*(stops + 1))->Name << endl;
            StopToStop s_t_s;
            pair<double, double> lenght_;
            double distance = 0;
            Coordinates from = (*stops)->coordinates;
            Coordinates to = (*(stops + 1))->coordinates;
            //прямой порядок
            distance = ComputeDistance(from, to);
            s_t_s.Stop_to_Stop = make_pair((*stops), (*(stops + 1)));
            lenght_ = make_pair(distance, 0.0);
            stop_to_stop_distance[s_t_s] = lenght_;
            //обратный порядок
            s_t_s.Stop_to_Stop = make_pair((*(stops + 1)), (*stops));
            distance = ComputeDistance(to, from);
            lenght_ = make_pair(distance, 0.0);
            stop_to_stop_distance[s_t_s] = lenght_;

            //cout<< distance<< endl;
            //cout<< "----"<< endl;
            stops += 1;
        }
    }

    void Buses::SetStopLength(const void *container) {

        auto *ptr = (vector<tuple<void *, string, double>> *) container;
        if (!ptr->empty() && ptr->size() > 0) {
            for (auto &tuple_val: (*ptr)) {
                StopToStop s_t_s;
                s_t_s.Stop_to_Stop = make_pair(((Stop *) (std::get<0>(tuple_val))),
                                               (stops_->FindStop(std::get<1>(tuple_val))));
                auto itr = stop_to_stop_distance.find(s_t_s);
                //cout << s_t_s.Stop_to_Stop.first->Name << "|" << s_t_s.Stop_to_Stop.second->Name <<endl;
                if (itr != stop_to_stop_distance.end()) {
                    stop_to_stop_distance[s_t_s].second = std::get<2>(tuple_val);
                }
            }
        }
    }

    //получение ссылки на контейнер хранения вычисленных расстояний
    const std::unordered_map<StopToStop, pair<double, double>, StoptoStopHash> &Buses::AsMapDistance() const {
        return stop_to_stop_distance;
    }

    //количество остановок на маршруте
    int Buses::GetAmountStops(const std::string_view bus_name) const {
        auto bus = FindBus(bus_name);
        if (!bus->cicle_route) {
            return ((bus->Stops).size()) * 2 - 1;
        }

        return (bus->Stops).size();
    }

    //количество уникальных остановок на маршруте
    int Buses::GetAmountUniqueStops(string_view numb_bus) const {
        std::unordered_set<const Stop *> stop_counting;
        for (auto stops: FindBus(numb_bus)->Stops) {

            stop_counting.insert(stops);
        }
        return stop_counting.size();
    }

    //длина маршрута
    pair<double, double> Buses::GetLengthRoute(std::string_view bus_name) const {
        auto bus = FindBus(bus_name);
        double factLen = 0;
        double geoLen = 0;
        const auto &dist = AsMapDistance();

        for (auto stp = bus->Stops.begin(); stp != (bus->Stops).end() - 1;) {
            StopToStop stopTo;
            StopToStop stopFrom;
            stopTo.Stop_to_Stop = make_pair(*stp,
                                            *(stp + 1)); //прямой порядок от stop_from(*stp) до stop_to(*(stp + 1))
            stopFrom.Stop_to_Stop = make_pair(*(stp + 1),
                                              *stp); //обратный порядок от stop_to(*(stp + 1)) до  stop_from(*stp)
            double geoLenTo = dist.at(stopTo).first; //географическое расстояние прямого порядка
            double factLenTo = dist.at(stopTo).second; //фактическое расстояние прямого порядка
            double geoLenFrom = dist.at(stopFrom).first; //географическое расстояние обратного порядка
            double factLenFrom = dist.at(stopFrom).second; //фактическое расстояние обратного порядка

            factLenTo = factLenTo == 0 && factLenFrom > 0 ? factLenFrom : factLenTo;

            factLenFrom = factLenFrom == 0 && factLenTo > 0 ? factLenTo : factLenFrom;

            factLen += factLenTo > 0 ? factLenTo : geoLenTo;
            geoLen += geoLenTo;

            if (bus->cicle_route == 0) {
                factLen += factLenFrom > 0 ? factLenFrom : geoLenFrom;
                geoLen += geoLenFrom;
            }
            stp += 1;
        }
        return make_pair(factLen, (factLen / geoLen));
    }

    string Stops::ToString(string_view name_stop) const {
        ostringstream out;
        bool first = false;
        if (stop_accross_to_bus.find(name_stop) == stop_accross_to_bus.end()) {
            out << " not found";
            return out.str();
        } else if (stop_accross_to_bus.at(name_stop).empty()) {
            out << " no buses";
            return out.str();
        }
        out << " buses ";
        for (auto &str: stop_accross_to_bus.at(name_stop)) {

            if (!first) {
                out << str;
                first = true;
            } else {
                out << " " << str;
            }


        }
        out << " ";
        return out.str();
    }

    string Buses::ToString(string_view numb_bus) const {
        ostringstream out;
        auto resault = FindBus(numb_bus);
        if (resault == nullptr) {
            out << "Bus " << numb_bus << ": not found";
        } else {
            auto rout_l_c = GetLengthRoute(numb_bus);
            out << "Bus " << numb_bus << ": " << GetAmountStops(numb_bus) << " stops on route, " <<
                GetAmountUniqueStops(numb_bus) << " unique stops, "
                << setprecision(6) << rout_l_c.first << " route length, "
                << rout_l_c.second << " curvature";
        }
        //out << " ";
        return out.str();
    }

}
