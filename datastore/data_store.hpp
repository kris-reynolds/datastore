#include <any>
#include <functional>
#include <map>
#include <memory_resource>
#include <memory>
#include <typeindex>

#include "meta_programming.hpp"

/// Requirements
/// DataStore can store/retrieve objects of any type
/// Exception safe - noexcept everything
/// Must be moveable/copyable
/// Insertion/retrieval should not have any copies
/// Does not need to store multiple objects of a given type
/// Target C++17

/// Items to consider
/// Serialization (to/from json/string methods)
/// Allow injection of allocators for the map
/// Note: unique_ptr is currently NOT supported

/// @brief Metaprogramming helpers for datastore processing
///
namespace details
{
   // Meta-programming helpers to determine if type is a smart pointer
   template <typename T>
   struct is_shared_ptr : std::false_type {};
   template <typename T>
   struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

   template<typename Object>
   struct const_pointer_return_type {
      using type = typename std::conditional<details::is_shared_ptr<Object>::value, const Object,
                  typename std::conditional<std::is_pointer_v<Object>, const Object, const Object*>::type>::type;
   };

   template<typename Object>
   struct pointer_return_type {
      using type = typename std::conditional<details::is_shared_ptr<Object>::value, Object,
                  typename std::conditional<std::is_pointer_v<Object>, Object, Object*>::type>::type;
   };

} // namespace details

#include <iostream>

/// @brief
///
/// @tparam BufferSize: Size of the initial buffer for the underlying PMR Allocator
/// @tparam Allocator: PMR allocator to use.  Default is the monotonic_buffer_resource.
///          this allocator starts with a BufferSize buffer and expands as necessary.  No
///          memory is released until the Allocator is deconstructed
class DataStore
{
private:

   /// The lookup key is the hash value of the given Class types type_index
   /// We use the hash value instead of the type_index directly to avoid
   /// the string comparisons of the type_index which could potentially be
   /// very slow on map lookups.  Accomodations are provided to avoid
   /// overlapping hashes
   using key_type = size_t;

   /// @brief Get the unique key object
   ///
   /// @param type_index
   /// @return size_t
   static size_t get_unique_key(const std::type_index& type_index)
   {
      static std::set<size_t> hash_set;

      // This function only expects to be called once per class type.  Therefore
      //  if the hashcode is already in the map we need to respin it.  This looks
      //  expensive (because it is).  However the number of different types being
      //  created should be relatively few.  Where the number of times a given
      //  type is inserted across many maps is quite large
      // There is a pretty good chance hash clashing would never happen, but when it does
      //  it would create some nasty bugs so lets try and avoid it with this small
      //  overhead.
      size_t hash = type_index.hash_code();

      while(hash_set.find(hash) != std::end(hash_set))
      {
         hash = std::hash<size_t>{}(hash);
      }

      return hash;
   }

   /// @brief Get the type key object
   ///
   /// @tparam T
   /// @return key_type
   template <typename T>
   static key_type get_type_key() noexcept
   {
      static bool key_initialized = false;
      static key_type object_key;

      if(!key_initialized)
      {
         key_initialized = true;
         get_unique_key(std::type_index(typeid(T)));
      }
      return object_key;
   }

public:

   DataStore() = default;

   /// @brief Insert an object into the datastore.  If an object of the same type already exists replace it
   ///
   /// @tparam Object - Type of object to insert
   /// @param object
   /// @note a shared_ptr<Object>, Object*, and Object would all be treated uniquely
   template <typename Object>
   void insert(Object &&object) noexcept
   {
      _data.insert(std::make_pair(get_type_key<Object>(), std::make_any<Object>(std::forward<Object>(object))));
   }

   /// @brief Build an object in place in the datastore.  If an object of the same type already exists replace it
   ///
   /// @tparam Object - Type of object to insert
   /// @tparam Args - Object construction argument types
   /// @param args - construction arguments
   /// @note a shared_ptr<T>, T*, and T would all be treated uniquely
   template <typename Object, typename... Args>
   void emplace(Args &&... args) noexcept
   {
      _data.emplace(get_type_key<Object>(), std::make_any<Object>(std::forward<Args>(args)...));
   }

   /// @brief Erase an object of the specified type.  If object is not in datastore this is a null-op
   ///
   /// @tparam Object - Type of object to erase
   template <typename Object>
   void erase() noexcept
   {
      const auto key = get_type_key<Object>();
      _data.erase(key);
   }

   /// @brief Get an object of the specified type from the datastore
   ///
   /// @tparam Object
   /// @return details::const_pointer_return_type<Object>::type
   ///         If the object is a shared_ptr retrieve a shared_ptr&
   ///         If the object is a raw pointer retrieve as a raw pointer
   ///         If the object being stored is a value type retrieve as a raw pointer
   template <typename Object>
   typename details::const_pointer_return_type<Object>::type
   get() const noexcept
   {
      static const typename details::const_pointer_return_type<Object>::type bad_value {nullptr};

      // TODO: If type is shared_ptr we want to be able to return it by refernece
      //  to avoid the refcount modification.  But need to be able to return a nullptr
      //  for bad cases still.
      const auto entry = _data.find(get_type_key<Object>());
      if (entry == std::end(_data))
      {
         return bad_value;
      }

      try
      {
         if constexpr (details::is_shared_ptr<Object>::value)
         {
            return std::any_cast<Object>(entry->second);
         }
         else if constexpr (std::is_pointer_v<Object>)
         {
            return std::any_cast<Object>(entry->second);
         }
         else
         {
            return &std::any_cast<const Object&>(entry->second);
         }
      }
      catch (const std::bad_any_cast &e)
      {
         std::cerr << e.what() << std::endl;
         return bad_value;
      }
   }

   /// Non-const get
   /// Looks a little messy but it saves us from duplicating the code from the const method
   template <typename Object>
   typename details::pointer_return_type<Object>::type
   get() noexcept
   {
      // If a shared_ptr type no const_cast needed!
      if constexpr(details::is_shared_ptr<Object>::value)
      {
         return static_cast<const DataStore&>(*this).get<Object>();
      }
      else
      {
         return const_cast<typename details::pointer_return_type<Object>::type>(static_cast<const DataStore&>(*this).get<Object>());
      }
   }

   /// @brief Empty out the datastore
   ///
   void clear() noexcept
   {
      _data.clear();
   }

private:

   /// Stores the actual data
   std::map<key_type, std::any> _data;

};
