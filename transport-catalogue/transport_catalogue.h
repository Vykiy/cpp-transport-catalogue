#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <vector>
#include <set>
#include <deque>
#include <iomanip>
#include "geo.h"
// напишите решение с нуля
// код сохраните в свой git-репозиторий
//transport_catalogue.h

namespace TransportCatalogue {
    //остановка
    struct Stop {
        std::string Name;
        Coordinates coordinates;
        std::string RestQuery;
    };
    //автобусный маршрут
    struct Bus {
        std::string Name;
        std::vector<const Stop *> Stops;
        bool cicle_route = false;//1-маршрут кольцевой, 0-линейный
    };
    //пересечение с автобусными маршрутами
    struct StopAcrossToBuses {
        std::string_view Name;
        std::unordered_set<const Bus *> Buses;
    };

    //переменная ссылок на координаты соседних остановок для хранения в контейнере
    struct StopToStop {

        bool operator==(const StopToStop &other) const {
            return Stop_to_Stop.first == other.Stop_to_Stop.first && Stop_to_Stop.second == other.Stop_to_Stop.second;
        }

        std::pair<const Stop *, const Stop *> Stop_to_Stop;
    };

    struct StoptoStopHash {

        size_t operator()(const StopToStop &value) const {

            size_t h_stop1 = d_hasher_(value.Stop_to_Stop.first);
            size_t h_stop2 = d_hasher_(value.Stop_to_Stop.second);

            return h_stop1 * (37u) + h_stop2 * (37u * 37u);
        }

    private:
        std::hash<const void *> d_hasher_;
    };


    class Stops {
    public:
        //конструктор по умолчанию
        explicit Stops() = default;

        //ф-я добавления остановки
        void AddStop(const std::string &stopName, const Coordinates &coor);

        //ф-я поиска остановки
        const Stop *FindStop(const std::string &name) const;

        //ф-я добавления ссылки на автобус
        void AddStopAccrossingBus(std::string_view name_stop, std::string_view name_bus);

        //формирование списка автобусов
        std::string ToString(std::string_view name_stop) const;

    private:
        // контейнер остановок
        std::deque<Stop> stops_;
        //контейнер для быстрого поиска остановок
        std::unordered_map<std::string_view, Stop *> stopname_to_stop;
        //контейнер для быстрого поиска остановок с перечением маршрутов
        std::unordered_map<std::string_view, std::set<std::string_view>> stop_accross_to_bus;
    };

    class Buses {
    public:
        //конструктор по умолчанию
        explicit Buses() = default;

        explicit Buses(Stops *_stop);

        //ф-я добавления маршрута
        void AddBus(const std::string& name, const std::vector<std::string>& stopsInBus);

        //ф-я поиска автобусного маршрута
        Bus *FindBus(std::string_view name) const;

        //ф-я вставки расстояний между соседними остановками (географические)
        void SetStopDistance(std::string_view bus_name);

        //ф-я вставки расстояний между соседними остановками (фактические)
        void SetStopLength(const void *container);

        //количество остановок на маршруте
        int GetAmountStops(std::string_view name) const;

        //количество уникальных остановок на маршруте
        int GetAmountUniqueStops(std::string_view name) const;

        //длина маршрута
        std::pair<double, double> GetLengthRoute(std::string_view bus_name) const;

        //получение ссылки на контейнер хранения вычисленных расстояний
        const std::unordered_map<StopToStop, std::pair<double, double>, StoptoStopHash> &AsMapDistance() const;

        std::string ToString(std::string_view numb_bus) const;

    private:
        //ссылка на класс остановка
        Stops *stops_{};
        // контейнер маршрутов
        std::deque<Bus> buses_;
        //контейнер для быстрого поиска маршрутов
        std::unordered_map<std::string_view, Bus *> busname_to_bus;
        //контейнер для быстрого поиска расстояний между остановками
        std::unordered_map<StopToStop, std::pair<double, double>, StoptoStopHash> stop_to_stop_distance;
    };


}