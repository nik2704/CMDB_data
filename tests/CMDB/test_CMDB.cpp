#define BOOST_TEST_MODULE CMDBTests
#include <boost/test/included/unit_test.hpp>
#include <cstdio>
#include "../../CMDB/CMDB.h"
#include "../../CMDB/CI.h"

using namespace cmdb;

BOOST_AUTO_TEST_SUITE(CMDBTestSuite)

std::string filename = "test_file.bin";

BOOST_AUTO_TEST_CASE(AddAndGetLevels) {
    std::remove(filename.c_str());
    auto& cmdb = CMDB::getInstance(filename);
    cmdb.setLevels(nullptr);

    std::vector<std::string> levels = {"Application", "Database", "Network"};
    cmdb.setLevels(&levels);

    auto fetched_levels = cmdb.getLevels();
    BOOST_REQUIRE(fetched_levels);
    BOOST_CHECK_EQUAL(fetched_levels->size(), 3);
    BOOST_CHECK_EQUAL((*fetched_levels)[0], "Application");
    BOOST_CHECK_EQUAL((*fetched_levels)[1], "Database");
    BOOST_CHECK_EQUAL((*fetched_levels)[2], "Network");
}

BOOST_AUTO_TEST_CASE(AddAndGetCI) {
    std::remove(filename.c_str());
    auto& cmdb = CMDB::getInstance(filename);

    cmdb.addCI("CI001", "WebServer", "Server", 0, {{"OS", "Linux"}, {"RAM", "32GB"}});
    auto ci = cmdb.getCI("CI001");
    BOOST_REQUIRE(ci);
    BOOST_CHECK_EQUAL(ci->getId(), "CI001");
    BOOST_CHECK_EQUAL(ci->getName(), "WebServer");
    BOOST_CHECK_EQUAL(ci->getType(), "Server");

    auto props = ci->getProperties();
    BOOST_CHECK_EQUAL(props.at("OS"), "Linux");
    BOOST_CHECK_EQUAL(props.at("RAM"), "32GB");
}

BOOST_AUTO_TEST_CASE(UpdateCIProperties) {
    std::remove(filename.c_str());
    auto& cmdb = CMDB::getInstance(filename);

    cmdb.updateCI("CI001", {{"OS", "Ubuntu"}, {"CPU", "8-core"}});
    auto ci = cmdb.getCI("CI001");
    BOOST_REQUIRE(ci);

    auto props = ci->getProperties();
    BOOST_CHECK_EQUAL(props.at("OS"), "Ubuntu");
    BOOST_CHECK_EQUAL(props.at("CPU"), "8-core");
}

BOOST_AUTO_TEST_CASE(AddAndRemoveRelationship) {
    std::remove(filename.c_str());
    auto& cmdb = CMDB::getInstance(filename);

    cmdb.addCI("CI002", "Database", "DB", 1);
    BOOST_CHECK(cmdb.addRelationship("CI001", "CI002", "DependsOn"));

    auto rels = cmdb.getRelationships("CI001", "CI002");
    BOOST_REQUIRE(rels);
    BOOST_CHECK_EQUAL(rels->size(), 1);
    BOOST_CHECK_EQUAL(rels->at(0)->getType(), "DependsOn");

    BOOST_CHECK(cmdb.removeRelationship("CI001", "CI002", "DependsOn"));
    rels = cmdb.getRelationships("CI001", "CI002");
    BOOST_CHECK(!rels || rels->empty());
}

BOOST_AUTO_TEST_CASE(GetCIsByLevel) {
    std::remove(filename.c_str());
    auto& cmdb = CMDB::getInstance(filename);

    auto cis = cmdb.getCIs(0);
    BOOST_REQUIRE(cis);
    BOOST_CHECK(!cis->empty());

    for (const auto& ci : *cis) {
        BOOST_CHECK_EQUAL(ci->getLevel(), 0);
    }
}

BOOST_AUTO_TEST_CASE(PropertyFilterSearch) {
    std::remove(filename.c_str());
    auto& cmdb = CMDB::getInstance(filename);

    cmdb.addCI("CI001", "Database", "DB", 1, {{"OS", "Ubuntu"}});

    std::vector<std::string> props = {"OS"};
    auto result = cmdb.getCIs(props);

    BOOST_REQUIRE(result);
    BOOST_CHECK(!result->empty());

    for (const auto& ci : *result) {
        auto prop = ci->getProperty("OS");
        BOOST_CHECK(prop.has_value());
        BOOST_CHECK_EQUAL(prop.value(), "Ubuntu");
    }
}

BOOST_AUTO_TEST_SUITE_END()
