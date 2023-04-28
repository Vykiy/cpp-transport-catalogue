#pragma once

#include "geo.h"
#include "domain.h"
#include "graph.h"

#include <string>
#include <list>
#include <unordered_set>
#include <vector>
#include <tuple>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <cmath>
#include <utility>
#include <optional>

namespace tr_cat::aggregations {
        using namespace std::string_literals;

        class TransportCatalogue {
        public:
            void AddStop (std::string_view name, geo::Coordinates coords);
            void AddBus (std::string_view name, const std::vector<std::string_view>& stops, bool is_ring);
            void AddDistance(std::string_view lhs, std::string_view rhs, double distance);
            std::optional<const Bus*>  GetBusInfo (std::string_view name) const;
            std::optional<const Stop*> GetStopInfo (std::string_view name) const;
            int GetDistance(const Stop* lhs, const Stop* rhs) const;
            auto begin() const {return buses_.begin();}
            auto end() const {return buses_.end();}
            size_t cat_size() const {return buses_.size();}

        private:
            class DistanceHasher {
            public:
                size_t operator() (const std::pair<const Stop*, const Stop*> element) const {
                    const size_t shift = (size_t)log2(1 + sizeof(Stop));
                    const size_t result = (size_t)(element.first) >> shift;
                    return result + ((size_t)(element.second) >> shift) * 37;
                }
            };
            class DistanceCompare {
            public:
                bool operator() (const std::pair<const Stop*, const Stop*> lhs, const std::pair<const Stop*, const Stop*> rhs) const {
                    return lhs.first == rhs.first && rhs.second == lhs.second;
                }
            };

            std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher, DistanceCompare> distances_;
            std::deque<Stop> stops_data_;
            std::deque<Bus> buses_data_;
            std::unordered_map<std::string_view, Stop*> stops_container_;
            std::unordered_map<std::string_view, Bus*> buses_container_;
            std::vector<std::string_view> buses_;

            int ComputeRouteDistance (std::string_view name) const;
            double ComputeGeoRouteDistance (std::string_view name) const;
            Stop* FindStop (std::string_view name) const;
            Bus* FindBus (std:: string_view name)const;
        };
} // namespace tr_cat::aggregations//tr_cat