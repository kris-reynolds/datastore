# Datastore

The Datastore is a header-only C++17 utility for storing/retrieving a wide variety of information in a single location.  I have found this utility to be extremely useful where you have many components that may need to store data in a central location - but you do not know ahead of time which components will be running.  Classic patterns that tend to face this issue are the Strategy, Visitor, and Mediator patterns.

## Example

In this example we have a basic `Point` structure that has X/Y coordinates and velocity.  On each execution of the application a list of current points is retreived from the system.  The updated point list is then fused with the known existing points.  Developer defined algorithms will run on the list.  Many algorithms will need to store extra state (IE smoothing filters), which they can store in the `DataStore` without requiring the `Point` to have any specific knowledge of what state the Algorithm needs to store.

Other Algorithms in the stream can then check if a particular value exists in the Datastore.  They can alter behavior based on the available data.  IE a particular classifier may only be able to classify the `Point` if Kalman Filter data is present.

```cpp
struct Point {
   float x;
   float y;
   float velocityX;
   float velocityY;
   int classification;
   DataStore data;
};

using Points = std::list<Point>;
// An algorithm takes the known list of points, and a list of new input points
using Algorithm = std::function<void(Points&, Points&)>;

int main() {
   // Implementation here is irrelevant
   std::vector<Algorithm> algorithms = initializeAlgorithmsFromConfig();

   Points points;
   while(true)
   {
      Points updatedPoints = getExternalPointInfo();

      for(const auto& algorithm : algorithms) {
         algorithm(points, updatedPoints);
      }
   }

}
```

## TODO

- Add hooks for user specified serialization
- Add a `try_emplace`, `try_insert` similiar to what `std::map` has
- Add hooks for PMR support
