#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"

#include "data_store.hpp"

struct Foo
{
   Foo()
   {
      construct_count++;
   }

   Foo(const Foo &rhs) : _i(rhs._i)
   {
      copy_count++;
   }

   Foo(Foo &&rhs) : _i(rhs._i)
   {}

   int _i{0};
   static int construct_count;
   static int copy_count;
};

int Foo::construct_count = 0;
int Foo::copy_count = 0;

SCENARIO("Checking construction efficiency")
{
   DataStore data;
   Foo::construct_count = 0;
   Foo::copy_count = 0;

   WHEN("In place insertion and retrieval")
   {
      data.insert<Foo>({});
      THEN("No copy occurs")
      {
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }

      auto foo = data.get<Foo>();
      THEN("No copy or construction occurs")
      {
         REQUIRE(foo);
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }
   }

   WHEN("Emplace insertion")
   {
      data.emplace<Foo>();
      THEN("No copy occurs")
      {
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }

      auto foo = data.get<Foo>();
      THEN("No copy or construction occurs")
      {
         REQUIRE(foo);
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }

   }

   WHEN("LValue Reference Insertion")
   {
      Foo foo;
      data.insert(foo);
      THEN("Copy occurs")
      {
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 1);
      }

      auto foo2 = data.get<Foo>();
      THEN("No copy or construction occurs")
      {
         REQUIRE(foo2);
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 1);
      }
   }

   WHEN("RValue Reference Insertion")
   {
      Foo foo;
      data.insert(std::move(foo));
      THEN("No copy occurs")
      {
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }
      auto foo2 = data.get<Foo>();
      THEN("No copy or construction occurs")
      {
         REQUIRE(foo2);
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }
   }
}

SCENARIO("Insertion of various pointer types")
{
   DataStore data;
   Foo::construct_count = 0;
   Foo::copy_count = 0;

   WHEN("A shared pointer is inserted")
   {
      data.insert(std::make_shared<Foo>());
      THEN("A shared pointer is retrieved")
      {
         std::shared_ptr<Foo> foo = data.get<std::shared_ptr<Foo>>();
         REQUIRE(foo);
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }
   }

   // WHEN("A unique pointer is inserted")
   // {
   //    auto foo = std::make_unique<Foo>();
   //    data.insert(std::move(foo));
   //    THEN("A raw pointer is returned")
   //    {
   //       Foo* foo = data.get<std::unique_ptr<Foo>>();
   //    }
   // }

   WHEN("A raw pointer is inserted")
   {
      data.insert(new Foo());
      THEN("A raw pointer is returned")
      {
         Foo* foo = data.get<Foo*>();
         REQUIRE(foo);
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }
   }

   WHEN("A value is inserted")
   {
      data.insert<Foo>({});
      THEN("A raw pointer is returned")
      {
         Foo* foo = data.get<Foo>();
         REQUIRE(foo);
         REQUIRE(Foo::construct_count == 1);
         REQUIRE(Foo::copy_count == 0);
      }
   }
}
