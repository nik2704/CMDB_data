#define BOOST_TEST_MODULE RelationshipTests
#include <boost/test/included/unit_test.hpp>
#include "../../CMDB/Relationship.h"

using namespace cmdb;

BOOST_AUTO_TEST_SUITE(RelationshipTestSuite)

BOOST_AUTO_TEST_CASE(ConstructorAndGetters) {
    Relationship rel("CI001", "CI002", "DependsOn", 2.5);

    BOOST_CHECK_EQUAL(rel.getSource(), "CI001");
    BOOST_CHECK_EQUAL(rel.getDestination(), "CI002");
    BOOST_CHECK_EQUAL(rel.getType(), "DependsOn");
    BOOST_CHECK_CLOSE(rel.getWeight(), 2.5, 0.001);
}

BOOST_AUTO_TEST_CASE(JsonSerialization) {
    Relationship rel("CI001", "CI002", "Hosts", 1.0);
    boost::json::object obj = rel.asJSON();

    BOOST_CHECK_EQUAL(obj["source"].as_string(), "CI001");
    BOOST_CHECK_EQUAL(obj["destination"].as_string(), "CI002");
    BOOST_CHECK_EQUAL(obj["type"].as_string(), "Hosts");
    BOOST_CHECK_CLOSE(obj["weight"].as_double(), 1.0, 0.001);
}

BOOST_AUTO_TEST_CASE(JsonStringFormat) {
    Relationship rel("CI-A", "CI-B", "ConnectedTo", 3.14);
    std::string jsonStr = rel.getCIasJSONstring();

    BOOST_CHECK(jsonStr.find("\"source\":\"CI-A\"") != std::string::npos);
    BOOST_CHECK(jsonStr.find("\"destination\":\"CI-B\"") != std::string::npos);
    BOOST_CHECK(jsonStr.find("\"type\":\"ConnectedTo\"") != std::string::npos);
    BOOST_CHECK(jsonStr.find("\"weight\":3.14") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(SaveAndLoad) {
    std::string file_name = "relationship_test.bin";
    Relationship relOut("CI123", "CI456", "LinkedTo", 0.75);

    std::ofstream ofs(file_name);
    BOOST_REQUIRE(ofs.is_open());
    BOOST_CHECK(relOut.save(ofs));
    ofs.close();

    Relationship relIn;
    std::ifstream ifs(file_name);
    BOOST_REQUIRE(ifs.is_open());
    BOOST_CHECK(relIn.load(ifs));
    ifs.close();

    BOOST_CHECK_EQUAL(relIn.getSource(), "CI123");
    BOOST_CHECK_EQUAL(relIn.getDestination(), "CI456");
    BOOST_CHECK_EQUAL(relIn.getType(), "LinkedTo");
    BOOST_CHECK_CLOSE(relIn.getWeight(), 0.75, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()
