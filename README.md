# Datastore

The Datastore is a header-only C++17 utility for storing/retrieving a wide variety of information in a single location.  I have found this utility to be extremely useful where you have many components that may need to store data in a central location - but you do not know ahead of time which components will be running.

## Example

Consider a `Track` class.  A `Track` may have many seperate algorithms that need to store data about the `Track` for different methodologies of following an object around the screen.  Each of the algorithms likely need to cache information about each specific `Track` object.  However we do not want fill up each algorithm with a list of track-specific information that it needs to sort and maintain.  That would be extremely cumbersome and expensive.  We also do not want to add an algorithm specific structure manually to the `Track` class - we only want to store information for algorithms that are actually being executed.

```cpp
struct Track {
   /// Generic tracking information
   float positionX;
   float positionY;
   float velocityX;
   float velocityY;
   DataStore data;
};

struct SpecialTrackData {
   float boosterGains;
};

void run_my_special_tracking_updates(Track& track) {
   auto specialData = Track.data.get<SpecialTrackData>();
   /// Update track positions with boosterGains!
   /// ...
   
   specialData->boosterGains = /*Update gainz*/;
}
```

## TODO

- Add assignment/move assignment operators
- Add unit tests for all the contructor/assignment types
- Add to/from string/stream operations
- Add tests for to/from string/stream operations
