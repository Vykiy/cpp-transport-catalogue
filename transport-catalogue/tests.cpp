#include "tests.h"

using namespace std::string_literals;

namespace tr_cat::tests {
        namespace detail {

            void AssertImpl(const bool value, const std::string& expr, const std::string& file, const std::string& func, 
                            unsigned line, const std::string& hint) {
                if (!value) {
                    std::cerr << std::boolalpha;
                    std::cerr << file << "("s << line << "): "s << func << ": "s << "ASSERT("s << expr << ") failed."s;
                    if (!hint.empty()) {
                        std::cerr << " Hint: "s << hint << std::endl;
                    }
                    abort();
                }
            }
        }// detail



        void TestOutput() {
            std::ifstream inf {"s10_final_opentest_3.json"s};
            std::ofstream outf {"test_output.json"s};
            aggregations::TransportCatalogue catalog;
            interface::JsonReader reader(catalog, inf, outf);
            reader.ReadDocument();
            reader.ParseDocument();
            reader.AddStops();
            reader.AddDistances();
            reader.AddBuses();
            reader.GetAnswers();
            reader.PrintAnswers();
            inf.close();
            outf.close();
            ASSERT_HINT(reader.TestingFilesOutput("test_output.json"s, "s10_final_opentest_3_answer.json"), "Output files not equal"s);
        }

        void TestRenderSpeed() {
            LOG_DURATION("TOTAL"s);
            aggregations::TransportCatalogue catalog;
            std::ifstream inf {"s10_final_opentest_3.json"s};
            std::ofstream outf {"test_output.svg"s};
            interface::JsonReader reader(catalog, inf, outf);
            {
                LOG_DURATION("BASE FILLING"s);

                {
                    LOG_DURATION("    READING         "s);
                    reader.ReadDocument();
                }

                {
                    LOG_DURATION("    PARSING         "s);
                    reader.ParseDocument();
                }

                {
                    LOG_DURATION("    ADD STOPS       "s);
                    reader.AddStops();
                }

                {
                    LOG_DURATION("    ADD DISTANCES   ");
                    reader.AddDistances();
                }

                {
                    LOG_DURATION("    ADD BUSES       "s);
                    reader.AddBuses();
                }
            }
//            render::MapRenderer render(catalog, reader.GetRenderSettings(), outf);
//            {
//                LOG_DURATION("RENDERING"s);
//                {
//                    LOG_DURATION("    DRAWING     "s);
//                    render.Render();
//                }
//            }
            std::cerr << "-----------------------------------\n\n"s;
        }

        void TestCatalogSpeed() {
            LOG_DURATION("TOTAL"s);
            aggregations::TransportCatalogue catalog;
            std::ifstream inf {"s10_final_opentest_3.json"s};
            std::ofstream outf {"test_output.json"s};
            interface::JsonReader reader(catalog, inf, outf);
            {
                LOG_DURATION("BASE FILLING"s);

                {
                    LOG_DURATION("    READING         "s);
                    reader.ReadDocument();
                }

                {
                    LOG_DURATION("    PARSING         "s);
                    reader.ParseDocument();
                }

                {
                    LOG_DURATION("    ADD STOPS       "s);
                    reader.AddStops();
                }

                {
                    LOG_DURATION("    ADD DISTANCES   ");
                    reader.AddDistances();
                }

                {
                    LOG_DURATION("    ADD BUSES       "s);
                    reader.AddBuses();
                }
            }
            {
                LOG_DURATION("ANSWERS  "s);
                {
                    LOG_DURATION("    GET ANSWERS   "s);
                    reader.GetAnswers();
                }
                {
                    LOG_DURATION("    PRINT   "s);
                    reader.PrintAnswers();
                }
            }
            std::cerr << "-----------------------------------\n\n"s;
        }

        void Test() {
            RUN_TEST(TestOutput);
            RUN_TEST(TestCatalogSpeed);
        }
    }//tr_cat


        

