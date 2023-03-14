#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"

#include <iostream>
#include <fstream>
#include <exception>
#include <sstream>

namespace tr_cat::interface {
        class JsonReader : public RequestInterface {
        public:
            explicit JsonReader(aggregations::TransportCatalogue& catalog) 
            :RequestInterface (catalog) {}
            JsonReader(aggregations::TransportCatalogue& catalog, std::istream& input) 
            :RequestInterface (catalog, input) {}
            JsonReader(aggregations::TransportCatalogue& catalog, std::ostream& output) 
            :RequestInterface (catalog, output) {}
            JsonReader(aggregations::TransportCatalogue& catalog, std::istream& input, std::ostream& output) 
            :RequestInterface (catalog, input, output) {}

            void ReadDocument () override;
            void ParseDocument () override;
            void PrintAnswers () override;
            bool TestingFilesOutput(std::string filename_lhs, std::string filename_rhs) override;
            const render::RenderSettings& GetRenderSettings() const;
        private:
            struct CreateNode {
                explicit CreateNode(render::RenderSettings& settings)
                :settings_(settings){}
                json::Node operator() (int value);
                json::Node operator() (StopOutput& value);
                json::Node operator() (BusOutput& value); 
                json::Node operator() (MapOutput& value);   
            private:
                render::RenderSettings settings_;
            };    
            json::Document document_ = {};
            json::Document document_answers_ = {};
            render::RenderSettings render_settings_;
            void ParseBase (json::Node& base);
            void ParseStats (json::Node& stats);
            void ParseRenderSettings(json::Node& render_settings);
            void PrepareToPrint ();   
        };
    }//tr_cat