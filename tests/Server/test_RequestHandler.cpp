#define BOOST_TEST_MODULE RequestHandlerTest
#include <boost/test/included/unit_test.hpp>
#include <cstdio>
#include "../../CMDB/CMDB.h"
#include "../../Server/Controller/RequestHandler.h"
#include "../../Server/Model/DataStore.h"

using namespace boost::beast;
using namespace http;

BOOST_AUTO_TEST_SUITE(RequestHandlerTestSuite)

std::string filename = "test_file.bin";

BOOST_AUTO_TEST_CASE(TestHandleAddLevel) {
    std::remove(filename.c_str());

    auto& cmdb = cmdb::CMDB::getInstance(filename);
    DataStore store(cmdb);

    RequestHandler handler(store);

    std::string json_body = R"({"name": "New Level"})";
    request<string_body> req{verb::post, "/api/v1/data/level", 11};
    req.body() = json_body;
    req.prepare_payload();
    auto fetched_levels = cmdb.getLevels();
    BOOST_CHECK_EQUAL(fetched_levels->size(), 4);

    response<string_body> res;
    handler.handleRequest(req, res);
    fetched_levels = cmdb.getLevels();

    BOOST_CHECK_EQUAL(res.result(), status::ok);
    BOOST_CHECK_EQUAL(fetched_levels->size(), 5);
}

BOOST_AUTO_TEST_CASE(TestHandleAddCi) {
    std::remove(filename.c_str());

    auto& cmdb = cmdb::CMDB::getInstance(filename);
    DataStore store(cmdb);

    RequestHandler handler(store);

    std::string ci_body = R"({"id": "CI00011","name": "Test CI Name1","type": "Server","level": 3})";
    request<string_body> req{verb::post, "/api/v1/data/ci", 11};
    req.body() = ci_body;
    req.prepare_payload();

    response<string_body> res;
    auto cis = cmdb.getCIs();

    BOOST_CHECK(!cis);

    handler.handleRequest(req, res);
    cis = cmdb.getCIs();

    BOOST_CHECK_EQUAL(res.result(), status::ok);
    BOOST_CHECK_EQUAL(cis->size(), 1);

    auto ci = cmdb.getCI("CI00011");
    BOOST_CHECK_EQUAL(ci->getId(), "CI00011");
    BOOST_CHECK_EQUAL(ci->getLevel(), 3);
    BOOST_CHECK_EQUAL(ci->getType(), "Server");
}

BOOST_AUTO_TEST_CASE(TestHandleAddRelation) {
    std::remove(filename.c_str());

    auto& cmdb = cmdb::CMDB::getInstance(filename);
    DataStore store(cmdb);

    RequestHandler handler(store);

    std::string ci_body = R"([
        {"id": "CI0001","name": "Test CI Name1","type": "Server","level": 3},
        {"id": "CI0002","name": "Test CI Name2","type": "Server","level": 3}
    ])";

    std::string rel_body =  R"({"source": "CI0001", "destination": "CI0002", "type": "Depends"})";

    request<string_body> req{verb::post, "/api/v1/data/ci", 11};
    req.body() = ci_body;
    req.prepare_payload();

    response<string_body> res;


    request<string_body> rel_req{verb::post, "/api/v1/data/relationship", 11};
    rel_req.body() = rel_body;
    rel_req.prepare_payload();

    response<string_body> rel_res;

    auto rels = cmdb.getRelationships();

    BOOST_CHECK(!rels);

    handler.handleRequest(req, res);
    handler.handleRequest(rel_req, rel_res);

    BOOST_CHECK_EQUAL(res.result(), status::ok);
    BOOST_CHECK_EQUAL(rel_res.result(), status::ok);

    rels = cmdb.getRelationships();
    BOOST_CHECK_EQUAL(rels->size(), 1);

    auto rel = cmdb.getRelationships("CI0001", "CI0002");
    BOOST_CHECK(rel);
    BOOST_CHECK_EQUAL(rel->at(0)->getSource(), "CI0001");
    BOOST_CHECK_EQUAL(rel->at(0)->getDestination(), "CI0002");
    BOOST_CHECK_EQUAL(rel->at(0)->getType(), "Depends");
}

BOOST_AUTO_TEST_SUITE_END()