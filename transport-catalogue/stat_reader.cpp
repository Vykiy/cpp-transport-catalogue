#include "stat_reader.h"
// напишите решение с нуля
// код сохраните в свой git-репозиторий

namespace StatReader {

    using namespace std;

    Print::Print(Buses *_buses, Stops *_stops) : buses_(_buses), stops_(_stops) {}

    void Print::PrintValues(ListQueryIn::AttributesOut *attr) {

        //auto *ptr=(std::vector<std::pair<std::string,std::string>>*)attr;

        string _name;

        for (auto &query: (*attr)) {
            _name = Lstrip(Rstrip(query.second));

            if (query.first == "Bus") {
                PrintValueBus(_name);
            } else if (query.first == "Stop") {
                PrintValueStop(_name);
            }
        }
    }

    void Print::PrintValueBus(std::string_view numb_bus) {
        cout << buses_->ToString(numb_bus) << endl;
    }

    void Print::PrintValueStop(std::string_view name_stop) {

        cout << "Stop " << name_stop << ":" << stops_->ToString(name_stop) << endl;
    }
}//namespace StatReader