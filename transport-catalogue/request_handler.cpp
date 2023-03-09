#include "request_handler.h"

namespace tr_cat::interface {
        using namespace std;

    void RequestInterface::AddStops () {
        std::ranges::for_each(stops_, [&](StopInput const& stop) {catalog_.AddStop(stop.name, stop.coordinates);});
    }

    void RequestInterface::AddDistances () {
        for (auto const& [lhs, stops] : distances_) {
            for (auto const& [rhs, value] : stops) {
                catalog_.AddDistance(lhs, rhs, value);
            }
        }
    }

    void RequestInterface::AddBuses () {
        std::ranges::for_each(buses_, [&](BusInput& bus) {catalog_.AddBus(bus.name, bus.stops, bus.is_ring);});
    }

        void RequestInterface::GetAnswers() {

            for (const Stat& stat : stats_) {
                if (stat.type == "Bus"s) {
                    optional<const Bus> bus = catalog_.GetBusInfo(stat.name);
                    if (!bus) {
                        answers_.emplace_back(stat.id);
                        continue;
                    }
                    answers_.emplace_back(BusOutput{stat.id, bus});

                } else if (stat.type == "Stop"s) {
                    optional<const Stop> stop = catalog_.GetStopInfo(stat.name);
                    if (!stop) {
                        answers_.emplace_back(stat.id);
                        continue;
                    }
                    answers_.emplace_back(StopOutput{stat.id, stop});

                } else if (stat.type == "Map"s) {
                    answers_.emplace_back(MapOutput(stat.id, catalog_));

                } else {
                    throw invalid_argument ("Invalid Stat"s);
                }
            }
        }
        void Process(interface::RequestInterface& reader) {
            reader.ReadDocument();
            reader.ParseDocument();
            reader.AddStops();
            reader.AddDistances();
            reader.AddBuses();
            reader.GetAnswers();
            reader.PrintAnswers();
        }

    }//tr_cat