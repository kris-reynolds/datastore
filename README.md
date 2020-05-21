# Datastore

The Datastore is a header-only C++17 utility for storing/retrieving a wide variety of information in a single location.  I have found this utility to be extremely useful where you have many components that may need to store data in a central location - but you do not know ahead of time which components will be running.

## Example

```cpp
DataStore data;

Baz baz;
data.insert<Foo>({});
data.insert<Baz>(baz);
data.emplace<Bar>();

/// Sometime later...
auto foo = data.get<Foo>();
foo->update();
```


