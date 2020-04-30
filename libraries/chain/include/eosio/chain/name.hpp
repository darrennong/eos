#pragma once
#include <string>
#include <fc/reflect/reflect.hpp>
#include<boost/multiprecision/cpp_int.hpp>
#include <iosfwd>

using namespace boost::multiprecision;
namespace eosio::chain {
  struct name;
}
namespace fc {
  class variant;
  void to_variant(const eosio::chain::name& c, fc::variant& v);
  void from_variant(const fc::variant& v, eosio::chain::name& check);
} // fc

namespace eosio::chain {
   //".-0123456789abcdefghijklmnopqrstuvwxyz_:<>[]{}()`~@............."
   static constexpr uint64_t char_to_symbol(char c){
      if (c=='.'){
         return 0;
      }else if(c=='-'){
         return 1;
      }else if(c>='0' && c<=9){
         return (c-'0')+2;
      }else if(c>='a'&&c<='z'){
         return (c-'a')+12;
      }
      switch(c){
         case '_': return 28;
         case ':': return 29;
         case '<': return 30;
         case '>': return 31;
         case '[': return 32;
         case ']': return 33;
         case '{': return 35;
         case '}': return 36;
         case '(': return 37;
         case ')': return 38;
         case '`':return 39;
         case '~':return 40;
         case '@':return 41; 
         default: return 0;
      }
   }
   static constexpr uint64_t eos_to_symbol( char c ) {
      if( c >= 'a' && c <= 'z' )
         return (c - 'a') + 6;
      if( c >= '1' && c <= '5' )
         return (c - '1') + 1;
      return 0;
   }

   static constexpr uint256_t string_to_uint256_t(std::string_view str){
      uint256_t n = 0;
      int i = 0;
      for (;str[i]&&i<42;++i){
         n|= (char_to_symbol(str[i]) & 0x3f) << (256 - 6 * (i + 1));;
      }
      return n;
   }

   // static constexpr uint64_t string_to_uint64_t( std::string_view str ) {
   //    uint64_t n = 0;
   //    int i = 0;
   //    for ( ; str[i] && i < 12; ++i) {
   //       // NOTE: char_to_symbol() returns char type, and without this explicit
   //       // expansion to uint64 type, the compilation fails at the point of usage
   //       // of string_to_name(), where the usage requires constant (compile time) expression.
   //       n |= (char_to_symbol(str[i]) & 0x1f) << (64 - 5 * (i + 1));
   //    }

      // The for-loop encoded up to 60 high bits into uint64 'name' variable,
      // if (strlen(str) > 12) then encode str[12] into the low (remaining)
      // 4 bits of 'name'
   //    if (i == 12)
   //       n |= char_to_symbol(str[12]) & 0x0F;
   //    return n;
   // }

   /// Immutable except for fc::from_variant.

  
   struct name {
   private:
      union {
     uint256_t value = 0;
     uint128_t d128[2];
      }   ;

      friend struct fc::reflector<name>;
      friend void fc::from_variant(const fc::variant& v, eosio::chain::name& check);

      void set( std::string_view str );

   public:
      char* data() const{ return (char*)&value;}
       bool empty()const { return uint256_t(0)==value ;}
       bool good()const  { return !empty();   }

      explicit name( std::string_view str ) { set( str ); }
      constexpr explicit name( uint256_t v ) : value(v) {}
      constexpr name() = default;

      std::string to_string()const;
      constexpr uint256_t to_uint256_t()const { return value; }

      friend std::ostream& operator << ( std::ostream& out, const name& n ) {
         return out << n.to_string();
      }

      friend  bool operator < ( const name& a, const name& b ) { return a.value < b.value; }
      friend  bool operator > ( const name& a, const name& b ) { return a.value > b.value; }
      friend  bool operator <= ( const name& a, const name& b ) { return a.value <= b.value; }
      friend  bool operator >= ( const name& a, const name& b ) { return a.value >= b.value; }
      friend  bool operator == ( const name& a, const name& b ) { return a.value == b.value; }
      friend  bool operator != ( const name& a, const name& b ) { return a.value != b.value; }

      friend  bool operator == ( const name& a, uint64_t b ) { return a.value == b; }
      friend  bool operator != ( const name& a, uint64_t b ) { return a.value != b; }

       explicit operator bool()const { return value != 0; }
   };

//字符串的每个字符都被编码成6位块并左移
//从第一个字符的最高插槽开始的6位插槽。
//如果str足够长，则第43个字符编码为4位块
//放在最低的4位。64=12*5+4
   static constexpr name string_to_name( std::string_view str )
   {
      return name( string_to_uint256_t( str ) );
   }

#define N(X) eosio::chain::string_to_name(#X)

} // eosio::chain

namespace std {
   template<> struct hash<eosio::chain::name> : private hash<uint256_t> {
      typedef eosio::chain::name argument_type;
      size_t operator()(const argument_type& name) const noexcept
      {
         return hash<uint256_t>::operator()(name.to_uint256_t());
      }
   };
};

FC_REFLECT( eosio::chain::name, (value) )
