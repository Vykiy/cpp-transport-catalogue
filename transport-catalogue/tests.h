#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "log_duration.h"

namespace tr_cat::tests {
        namespace detail {

            template <typename Type1, typename Type2>
            void AssertEqualImpl(const Type1& value1, const Type2 value2, const std::string& str_value1, const std::string& str_value2,
                const std::string& file, const std::string& func, unsigned line, const std::string& hint = "") {
                
                if (value1 != static_cast<Type1>(value2)) {
                    std::cerr << file << "(" << line << "): " << func << ": ASSERT_EQUAL(" << str_value1 << ", " << str_value2;
                    std::cerr << ") failed: " << value1 << " != " << value2 << ".";
                    if (!hint.empty()) {
                        std::cerr << " Hint: " << hint;
                    }
                    std::cerr << std::endl;
                    abort();
                }
            }

            void AssertImpl(bool value, const std::string& expr, const std::string& file, const std::string& func,
                                                                        unsigned line, const std::string& hint = "");

            template <typename Func>
            void RunTestImpl(Func func, const std::string& func_name) {
                func();
                std::cerr << func_name << " OK" << std::endl;
            }

        } //detail

        #define ASSERT_EQUAL(value1, value2) detail::AssertEqualImpl((value1), (value2), #value1, #value2, __FILE__, __FUNCTION__, __LINE__)
        #define ASSERT_EQUAL_HINT(value1, value2, hint) detail::AssertEqualImpl((value1), (value2), #value1, #value2, __FILE__, __FUNCTION__, __LINE__, hint)

        #define ASSERT(expr) detail::AssertImpl((expr), #expr, __FILE__, __FUNCTION__, __LINE__)
        #define ASSERT_HINT(expr, hint) detail::AssertImpl((expr), #expr, __FILE__, __FUNCTION__, __LINE__, hint)

        #define RUN_TEST(func) detail::RunTestImpl((func), #func)

        void TestOutput();
        void TestRenderSpeed();
        void TestCatalogSpeed();
        void Test();
    }//tr_cat
