#pragma once
// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include <iostream>
#include <iomanip>
#include "transport_catalogue.h"

namespace StatReader {

    using Buses = TransportCatalogue::Buses;
    using Stops = TransportCatalogue::Stops;

    class Print {
    public:
        //конструктор по умолчанию
        explicit Print() = default;

        explicit Print(Buses *_buses, Stops *_stops);

        void PrintValueBus(std::string_view numb_bus);

        void PrintValueStop(std::string_view name_stop);

    private:
        //ссылка на класс остановка
        Buses *buses_;
        Stops *stops_;
    };

}//namespace StatReader