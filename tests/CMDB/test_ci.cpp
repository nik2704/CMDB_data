#define BOOST_TEST_MODULE test_ci
#include <boost/test/unit_test.hpp>
#include "../../CMDB/CI.h"

using namespace cmdb;

BOOST_AUTO_TEST_SUITE(test_ci)

BOOST_AUTO_TEST_CASE(DefaultConstructor) {
    CI ci;
    BOOST_CHECK(ci.getId().empty());
    BOOST_CHECK(ci.getName().empty());
    BOOST_CHECK(ci.getType().empty());
    BOOST_CHECK_EQUAL(ci.getLevel(), 0);
    BOOST_CHECK(ci.getProperties().empty());
}

BOOST_AUTO_TEST_CASE(ConstructorWithBasicParams) {
    CI ci("id1", "Server01", "Server");
    BOOST_CHECK_EQUAL(ci.getId(), "id1");
    BOOST_CHECK_EQUAL(ci.getName(), "Server01");
    BOOST_CHECK_EQUAL(ci.getType(), "Server");
    BOOST_CHECK_EQUAL(ci.getLevel(), 0);
    BOOST_CHECK(ci.getProperties().empty());
}

BOOST_AUTO_TEST_CASE(ConstructorWithFullParams) {
    std::unordered_map<std::string, std::string> props = {
        {"ip", "192.168.1.1"}, {"os", "Linux"}};
    CI ci("id2", "App01", "Application", 2, props);

    BOOST_CHECK_EQUAL(ci.getId(), "id2");
    BOOST_CHECK_EQUAL(ci.getName(), "App01");
    BOOST_CHECK_EQUAL(ci.getType(), "Application");
    BOOST_CHECK_EQUAL(ci.getLevel(), 2);
    BOOST_CHECK_EQUAL(ci.getProperties().at("ip"), "192.168.1.1");
    BOOST_CHECK_EQUAL(ci.getProperties().at("os"), "Linux");
}

BOOST_AUTO_TEST_CASE(TestSetters) {
    CI ci;
    BOOST_CHECK(ci.setName("Router01"));
    BOOST_CHECK_EQUAL(ci.getName(), "Router01");

    BOOST_CHECK(ci.setLevel(5));
    BOOST_CHECK_EQUAL(ci.getLevel(), 5);

    BOOST_CHECK(ci.setProperty("vendor", "Cisco"));
    BOOST_CHECK_EQUAL(ci.getProperty("vendor").value(), "Cisco");

    BOOST_CHECK(ci.setProperty("vendor", "Juniper")); // changed value
    BOOST_CHECK_EQUAL(ci.getProperty("vendor").value(), "Juniper");

    BOOST_CHECK(ci.hasProperty("vendor"));
    BOOST_CHECK(ci.removeProperty("vendor"));
    BOOST_CHECK(!ci.hasProperty("vendor"));
}

BOOST_AUTO_TEST_CASE(TestSetPropertiesFromMap) {
    CI ci;
    std::unordered_map<std::string, std::string> props = {
        {"key1", "value1"}, {"key2", "value2"}};
    ci.setProperties(props);
    BOOST_CHECK_EQUAL(ci.getProperties().size(), 2);
    BOOST_CHECK_EQUAL(ci.getProperty("key1").value(), "value1");
}

BOOST_AUTO_TEST_CASE(TestSetPropertiesFromJSON) {
    CI ci("id3", "DB01", "Database");
    boost::json::object json_obj = {
        {"id", "test_ci_01"},
        {"name", "Test CI Name1"},
        {"type", "Server"},
        {"level", 3},
        {"properties", {
            {"ip", "10.0.0.5"},
            {"env", "prod"}
            }
        }
    };
        
    std::string msg;
    BOOST_CHECK(ci.setProperties(json_obj, msg));
    BOOST_CHECK_EQUAL(ci.getProperty("ip").value(), "10.0.0.5");
    BOOST_CHECK_EQUAL(ci.getProperty("env").value(), "prod");
}

BOOST_AUTO_TEST_CASE(TestJSONSerialization) {
    std::unordered_map<std::string, std::string> props = {
        {"cpu", "Intel"}, {"ram", "32GB"}};
    CI ci("idX", "HW01", "Hardware", 1, props);

    auto json_obj = ci.asJSON();
    BOOST_CHECK_EQUAL(json_obj["id"], "idX");
    BOOST_CHECK_EQUAL(json_obj["name"], "HW01");
    BOOST_CHECK_EQUAL(json_obj["type"], "Hardware");
    BOOST_CHECK_EQUAL(json_obj["level"].as_int64(), 1);
    BOOST_CHECK_EQUAL(json_obj["properties"].as_object().at("cpu"), "Intel");

    std::string json_str = ci.getCIasJSONstring();
    BOOST_CHECK(json_str.find("\"cpu\":\"Intel\"") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
