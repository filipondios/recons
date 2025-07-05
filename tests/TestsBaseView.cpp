#include <catch2/catch_all.hpp>
#include <raymath.h>
#include "BaseView.hpp"


TEST_CASE("Empty view (no files)") { 
  std::filesystem::path path { "models/tests/invalid5/view1" };
  REQUIRE_THROWS(new BaseView(path));
}

TEST_CASE("No camera.json file in view") { 
  std::filesystem::path path { "models/tests/invalid5/view2" };
  REQUIRE_THROWS(new BaseView(path));
}

TEST_CASE("No plane.bmp file in view") { 
  std::filesystem::path path { "models/tests/invalid5/view3" };
  REQUIRE_THROWS(new BaseView(path));
}

TEST_CASE("View with a JSON syntax error") {  
  std::filesystem::path path { "models/tests/invalid5/view4" };
  REQUIRE_THROWS(new BaseView(path));
}

TEST_CASE("View without a JSON field") {
  std::filesystem::path path { "models/tests/invalid5/view5" };
  REQUIRE_THROWS(new BaseView(path));
}

TEST_CASE("Views with a wrong JSON field type") {
  // Instead of a vector, a number is the value
  std::filesystem::path path { "models/tests/invalid5/view6" };
  REQUIRE_THROWS(new BaseView(path));

  // Instead of a string, an array  
  path = "models/tests/invalid5/view7";
  REQUIRE_THROWS(new BaseView(path));

  // Instead of an array of numbers, an array of strings  
  path = "models/tests/invalid5/view8";
  REQUIRE_THROWS(new BaseView(path));
}

TEST_CASE("Views' vectors with more and less than 3 items") { 
  // In this case, instead of 3 elements, the vector has 5
  std::filesystem::path path { "models/tests/invalid5/view9" };
  REQUIRE_THROWS(new BaseView(path));
  
  // In this case, instead of 3 elements, the vector has 1
  path = "models/tests/invalid5/view10";
  REQUIRE_THROWS(new BaseView(path));
}

TEST_CASE("Normal view") {
  std::filesystem::path path { "models/tests/valid/view1" };
  BaseView view1 { path };
  REQUIRE(view1.name == "elevation");
  REQUIRE(Vector3Equals(view1.origin, Vector3 {123,0,0})); 
  REQUIRE(Vector3Equals(view1.vx, Vector3 {0,-5,0}));  
  REQUIRE(Vector3Equals(view1.vy, Vector3 {-112,0,0}));  
  REQUIRE(Vector3Equals(view1.vz, Vector3 {0,0,11}));

  path = "models/tests/valid/view2";
  BaseView view2 { path };
  REQUIRE(view2.name == "profile");
  REQUIRE(Vector3Equals(view2.origin, Vector3 {0,40,0})); 
  REQUIRE(Vector3Equals(view2.vx, Vector3 {1,0,0}));  
  REQUIRE(Vector3Equals(view2.vy, Vector3 {0,-9,0}));
  REQUIRE(Vector3Equals(view2.vz, Vector3 {0,0,21}));
  
  path = "models/tests/valid/view3";
  BaseView view3 { path };
  REQUIRE(view3.name == "plan");
  REQUIRE(Vector3Equals(view3.origin, Vector3 {0,0,40})); 
  REQUIRE(Vector3Equals(view3.vx, Vector3 {1,0,0}));
  REQUIRE(Vector3Equals(view3.vy, Vector3 {0,0,-1}));
  REQUIRE(Vector3Equals(view3.vz, Vector3 {0,-1,0}));
}
