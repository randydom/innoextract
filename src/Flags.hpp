
#ifndef INNOEXTRACT_FLAGS_HPP
#define INNOEXTRACT_FLAGS_HPP

#include <bitset>

// loosely based on QFlags from Qt

template <typename Enum>
class EnumSize { static const size_t value = 32; };

/*!
 * A typesafe way to define flags as a combination of enum values.
 * 
 * This type should not be used directly, only through DECLARE_FLAGS.
 */
template <typename _Enum>
class Flags {
	
public:
	
	typedef _Enum Enum;
	static const size_t bits = EnumSize<Enum>::value;
	typedef std::bitset<bits> Type;
	
private:
	
	typedef void ** Zero;
	typedef void(*TypesafeBoolean)();
	
	Type flags;
	
	inline Flags(Type flag) : flags(flag) { }
	
public:
	
	inline Flags(Enum flag) : flags(Type().set(size_t(flag))) { }
	
	inline Flags(Zero = 0) : flags() { }
	
	inline Flags(const Flags & o) : flags(o.flags) { }
	
	static inline Flags load(Type flags) {
		return Flags(flags, true);
	}
	
	inline bool has(Enum flag) const {
		return flags.test(size_t(flag));
	}
	
	inline bool hasAll(Flags o) const {
		return (flags & o.flags) == o.flags;
	}
	
	inline operator TypesafeBoolean() const {
		return reinterpret_cast<TypesafeBoolean>(flags.any());
	}
	
	inline Flags operator~() const {
		return Flags(~flags);
	}
	
	inline bool operator!() const {
		return flags.none();
	}
	
	inline Flags operator&(Flags o) const {
		return Flags(flags & o.flags);
	}
	
	inline Flags operator|(Flags o) const {
		return Flags(flags | o.flags);
	}
	
	inline Flags operator^(Flags o) const {
		return Flags(flags ^ o.flags);
	}
	
	inline Flags & operator&=(const Flags & o) {
		flags &= o.flags;
		return *this;
	}
	
	inline Flags & operator|=(Flags o) {
		flags |= o.flags;
		return *this;
	}
	
	inline Flags & operator^=(Flags o) {
		flags ^= o.flags;
		return *this;
	}
	
	inline Flags operator&(Enum flag) const {
		return operator&(Flags(flag));
	}
	
	inline Flags operator|(Enum flag) const {
		return operator|(Flags(flag));
	}
	
	inline Flags operator^(Enum flag) const {
		return operator^(Flags(flag));
	}
	
	inline Flags & operator&=(Enum flag) {
		
		return operator&=(Flags(flag));
	}
	
	inline Flags & operator|=(Enum flag) {
		return operator|=(Flags(flag));
	}
	
	inline Flags & operator^=(Enum flag) {
		return operator^=(flag);
	}
	
	inline Flags & operator=(Flags o) {
		flags = o.flags;
		return *this;
	}
	
	static inline Flags all() {
		return Flags(Type().flip());
	}
	
};

/*!
 * Declare a flag type using values from a given enum.
 * This should always be used instead of using Flags&lt;Enum&gt; directly.
 * 
 * @param Enum should be an enum with values that have exactly one bit set.
 * @param Flagname is the name for the flag type to be defined.
 */
#define DECLARE_FLAGS_SIZE(Enum, Flagname, Size) \
	template <> \
	struct EnumSize<Enum> { \
		static const size_t value = (Size); \
	}; \
	typedef Flags<Enum> Flagname;
#define FLAGS_ENUM_END_HELPER(Enum) Enum ## __End
#define FLAGS_ENUM_END(Enum) FLAGS_ENUM_END_HELPER(Enum)
#define DECLARE_FLAGS(Enum, Flagname) DECLARE_FLAGS_SIZE(Enum, Flagname, FLAGS_ENUM_END(Enum))

/*!
 * Declare overloaded operators for a given flag type.
 */
#define DECLARE_FLAGS_OPERATORS(Flagname) \
	inline Flagname operator|(Flagname::Enum a, Flagname::Enum b) { \
		return Flagname(a) | b; \
	} \
	inline Flagname operator|(Flagname::Enum a, Flagname b) { \
		return b | a; \
	} \
	inline Flagname operator~(Flagname::Enum a) { \
		return ~Flagname(a); \
	}
// TODO prevent combination with integers!

#define FLAGS_ENUM(Flagname) Flagname ## __Enum
#define FLAGS(Flagname, ...) \
	enum FLAGS_ENUM(Flagname) { \
		__VA_ARGS__ \
		FLAGS_ENUM_END(Flagname) \
	}; \
	DECLARE_FLAGS_SIZE(FLAGS_ENUM(Flagname), Flagname, FLAGS_ENUM_END(Flagname)) \
	DECLARE_FLAGS_OPERATORS(Flagname)

#endif // INNOEXTRACT_FLAGS_HPP