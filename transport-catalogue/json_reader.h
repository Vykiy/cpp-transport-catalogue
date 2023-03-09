#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <iostream>
#include <fstream>
#include <exception>
#include <sstream>
#include <utility>

namespace tr_cat::interface {
    class JsonReader : public RequestInterface {
    public:
        using RequestInterface::RequestInterface;

        void ReadDocument() override;

        void ParseDocument() override;

        void PrintAnswers() override;

        bool TestingFilesOutput(std::string filename_lhs, std::string filename_rhs) override;

        const render::RenderSettings &GetRenderSettings() const;

    private:
        struct CreateNode {
            explicit CreateNode(render::RenderSettings settings)
                    : settings_(std::move(settings)) {} //С++ не разобрался

            json::Node operator()(int value);

            json::Node operator()(StopOutput &value);

            json::Node operator()(BusOutput &value);

            json::Node operator()(MapOutput &value);

        private:
            render::RenderSettings settings_;
        };

        json::Document document_ = {};
        json::Document document_answers_ = {};
        render::RenderSettings render_settings_;

        void ParseBase(json::Node &base);

        void ParseStats(json::Node &stats);

        void ParseRenderSettings(json::Node &render_settings);

        void PrepareToPrint();
    };
}//tr_cat