#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <raymath.h>
#include "BaseModel.hpp"


TEST_CASE("Non-existing model") {
  // Trying to load a non-existing model should panic
  std::filesystem::path path { "models/tests/12903" };
  REQUIRE_THROWS(new BaseModel { path });
  path = std::filesystem::path { "this/should/throw" };
  REQUIRE_THROWS(new BaseModel { path });
  path = std::filesystem::path { "definetly/not" };
  REQUIRE_THROWS(new BaseModel { path });
  path = std::filesystem::path { "134g3/876/2njk" };
  REQUIRE_THROWS(new BaseModel { path });
}

TEST_CASE("Empty model") {
  // An empty model is a valid model ;)
  const std::filesystem::path path { "models/tests/empty" };
  const BaseModel model { path };
  REQUIRE(model.views.size() == 0);
  REQUIRE(model.path == path);
}

TEST_CASE("Missing fields and wrong type in a model's views") {
  // It must discard the view and not throw an exception
  // In this case all the views contain errors
  const std::filesystem::path path { "models/tests/invalid1" };
  const BaseModel model { path };
  REQUIRE(model.views.size() == 0);
  REQUIRE(model.path == path);
}

TEST_CASE("JSON syntax error in a model's views") {
  // It must discard the view and not throw an exception
  // In this case 2/3 views of the model contain errors
  const std::filesystem::path path { "models/tests/invalid2" };
  const BaseModel model { path };
  REQUIRE(model.views.size() == 1);
  REQUIRE(model.path == path);

  REQUIRE(model.views[0].name == "profile");
  REQUIRE(Vector3Equals(model.views[0].vx, Vector3 { 1, 0, 0 }));
  REQUIRE(Vector3Equals(model.views[0].vy, Vector3 { 0, -1, 0 }));
  REQUIRE(Vector3Equals(model.views[0].vz, Vector3 { 0, 0, 1 }));
}

TEST_CASE("No plane.bmp image a model's views") { 
  // It must discard the view and not throw an exception
  // In this case 1/3 views of the model dont have a plane.bmp file
  const std::filesystem::path path { "models/tests/invalid3" };
  const BaseModel model { path };
  REQUIRE(model.views.size() == 2);
  REQUIRE(model.path == path);
}

TEST_CASE("No camera.json image a model's views") { 
  // It must discard the view and not throw an exception
  // In this case 1/3 views of the model dont have a camera.json file
  const std::filesystem::path path { "models/tests/invalid4" };
  const BaseModel model { path };
  REQUIRE(model.views.size() == 2);
  REQUIRE(model.path == path);
}

TEST_CASE("Valid model") {
  const std::filesystem::path path { "models/tests/valid" };
  const BaseModel model { path };
  REQUIRE(model.views.size() == 3);
  REQUIRE(model.path == path);
}
