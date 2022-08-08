#pragma once

#include <istream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <set>
#include "transport_catalogue.h"

// напишите решение с нуля
// код сохраните в свой git-репозиторий
namespace inputReader {


    void LoadLine(std::istream &input);

    using Attributes = std::unordered_map<std::string, std::vector<std::string>>;


    std::string Lstrip(std::string line);

    std::string Rstrip(std::string line);

    class ListQueryIn {
    public:

        ListQueryIn() = default;


        using AttributesOut = std::vector<std::pair<std::string, std::string>>;

        //const std::vector<Node>& Children() const;
        static void AddQueryItem(std::string &line, Attributes *atr_container, AttributesOut *atrout_container);

        //void AddQueryItemOut(std::string& line, AttributesOut& atr_container);
        Attributes &AsMapIn();

        AttributesOut &AsMapOut();

        std::vector<std::tuple<const void *, std::string, double>> &AsMapLength();

        size_t GetSizeMapIn() const;

        size_t GetSizeMapOut() const;
        //std::string_view Name() const;

        //template <typename T>
        //T AttributeValue(const std::string& name) const;

    private:
        Attributes attrs_;
        std::vector<std::tuple<const void *, std::string, double>> attrs_length;
        AttributesOut attrs_out;
        //Attributes attrs_out;

    };


}