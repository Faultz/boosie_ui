#pragma once

namespace std
{
    typedef decltype(nullptr) nullptr_t;

    // STRUCT TEMPLATE integral_constant
    template <class _Ty, _Ty _Val>
    struct integral_constant {
        static constexpr _Ty value = _Val;

        using value_type = _Ty;
        using type = integral_constant;

        constexpr operator value_type() const noexcept {
            return value;
        }

        constexpr value_type operator()() const noexcept {
            return value;
        }
    };

    // ALIAS TEMPLATE bool_constant
    template <bool _Val>
    using bool_constant = integral_constant<bool, _Val>;

    using true_type = bool_constant<true>;
    using false_type = bool_constant<false>;

    // STRUCT TEMPLATE enable_if
    template <bool _Test, class _Ty = void>
    struct enable_if {}; // no member "type" when !_Test

    template <class _Ty>
    struct enable_if<true, _Ty> { // type is _Ty for _Test
        using type = _Ty;
    };

    template <bool _Test, class _Ty = void>
    using enable_if_t = typename enable_if<_Test, _Ty>::type;

    // STRUCT TEMPLATE conditional
    template <bool _Test, class _Ty1, class _Ty2>
    struct conditional { // Choose _Ty1 if _Test is true, and _Ty2 otherwise
        using type = _Ty1;
    };

    template <class _Ty1, class _Ty2>
    struct conditional<false, _Ty1, _Ty2> {
        using type = _Ty2;
    };

    template <bool _Test, class _Ty1, class _Ty2>
    using conditional_t = typename conditional<_Test, _Ty1, _Ty2>::type;

    template<class _Ty1, class _Ty2>
    struct is_same : false_type
    {
        // determine whether _Ty1 and _Ty2 are the same type
    };

    template<class _Ty1>
    struct is_same<_Ty1, _Ty1> : true_type
    {
        // determine whether _Ty1 and _Ty2 are the same type
    };




    // STRUCT TEMPLATE remove_reference
    template <class _Ty>
    struct remove_reference {
        using type = _Ty;
        using _Const_thru_ref_type = const _Ty;
    };

    template <class _Ty>
    struct remove_reference<_Ty&> {
        using type = _Ty;
        using _Const_thru_ref_type = const _Ty&;
    };

    template <class _Ty>
    struct remove_reference<_Ty&&> {
        using type = _Ty;
        using _Const_thru_ref_type = const _Ty&&;
    };

    template <class _Ty>
    using remove_reference_t = typename remove_reference<_Ty>::type;


    template<typename _Tp>
    constexpr typename remove_reference<_Tp>::type&&
        move(_Tp&& __t) noexcept
    {
        return static_cast<typename remove_reference<_Tp>::type&&>(__t);
    }


    template <class _Ty>
    struct remove_cv { // remove top-level const and volatile qualifiers
        using type = _Ty;

        template <template <class> class _Fn>
        using _Apply = _Fn<_Ty>; // apply cv-qualifiers from the class template argument to _Fn<_Ty>
    };

    template <class _Ty>
    using remove_cv_t = typename remove_cv<_Ty>::type;

    template <class _Ty>
    using remove_reference_t = typename remove_reference<_Ty>::type;

    template <class... _Types>
    using void_t = void;

    // Type modifiers
    template <class _Ty>
    struct add_const { // add top-level const qualifier
        using type = const _Ty;
    };

    template <class _Ty>
    using add_const_t = typename add_const<_Ty>::type;

    template <class _Ty>
    struct add_volatile { // add top-level volatile qualifier
        using type = volatile _Ty;
    };

    template <class _Ty>
    using add_volatile_t = typename add_volatile<_Ty>::type;

    template <class _Ty>
    struct add_cv { // add top-level const and volatile qualifiers
        using type = const volatile _Ty;
    };

    template <class _Ty>
    using add_cv_t = typename add_cv<_Ty>::type;

    template <class _Ty, class = void>
    struct _Add_reference { // add reference (non-referenceable type)
        using _Lvalue = _Ty;
        using _Rvalue = _Ty;
    };

    template <class _Ty>
    struct _Add_reference<_Ty, void_t<_Ty&>> { // (referenceable type)
        using _Lvalue = _Ty&;
        using _Rvalue = _Ty&&;
    };

    template <class _Ty>
    struct add_lvalue_reference {
        using type = typename _Add_reference<_Ty>::_Lvalue;
    };

    template <class _Ty>
    using add_lvalue_reference_t = typename _Add_reference<_Ty>::_Lvalue;

    template <class _Ty>
    struct add_rvalue_reference {
        using type = typename _Add_reference<_Ty>::_Rvalue;
    };

    template <class _Ty>
    using add_rvalue_reference_t = typename _Add_reference<_Ty>::_Rvalue;

    template <class _Ty>
    add_rvalue_reference_t<_Ty> declval() noexcept {
    }


    /*
    template <class T>
    inline T&& forward(typename std::remove_reference<T>::type& t) noexcept
    {
       return static_cast<T&&>(t);
    }
    template <class T>
    inline T&& forward(typename std::remove_reference<T>::type&& t) noexcept
    {
       return static_cast<T&&>(t);
    }
    */

    // FUNCTION TEMPLATE forward
    template <class _Ty>
    constexpr _Ty&& forward(
        remove_reference_t<_Ty>& _Arg) noexcept { // forward an lvalue as either an lvalue or an rvalue
        return static_cast<_Ty&&>(_Arg);
    }

    template <class _Ty>
    constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) noexcept { // forward an rvalue as an rvalue
        return static_cast<_Ty&&>(_Arg);
    }

    template<typename T>
    class unique_ptr
    {
        T* m_ptr;
    public:
        explicit unique_ptr(T* ptr = nullptr) noexcept
            : m_ptr(ptr)
        {
        }

        ~unique_ptr() noexcept
        {
            if (m_ptr != nullptr)
            {
                delete m_ptr;
            }
            m_ptr = nullptr;
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator = (const unique_ptr&) = delete;

        unique_ptr(unique_ptr&& ob) noexcept
        {
            m_ptr = ob.m_ptr;
            ob.m_ptr = nullptr;
        }
        unique_ptr& operator = (unique_ptr&& ob) noexcept
        {
            if (this != &ob)
            {
                m_ptr = ob.m_ptr;
                ob.m_ptr = nullptr;
            }
            return *this;
        }

        T* operator -> () const noexcept
        {
            return m_ptr;
        }

        T& operator * () const
        {
            return *m_ptr;
        }

        T* get() const noexcept
        {
            return m_ptr;
        }

        T* release() noexcept
        {
            T* ans = m_ptr;
            m_ptr = nullptr;
            return ans;
        }

        void reset(T* ptr = nullptr) noexcept
        {
            T* old = m_ptr;
            m_ptr = ptr;
            if (old != nullptr)
                delete old;
        }

        void swap(unique_ptr& ob) noexcept
        {
            using std::swap;
            swap(m_ptr, ob.m_ptr);
        }

    };

    template<class T>
    struct is_unbounded_array
        : false_type { };

    template<class T>
    struct is_unbounded_array<T[]>
        : true_type { };

    template<class T>
    struct is_unbounded_array<const T[]>
        : true_type { };

    template<class T>
    struct is_unbounded_array<volatile T[]>
        : true_type { };

    template<class T>
    struct is_unbounded_array<const volatile T[]>
        : true_type { };

    template <class T> struct remove_extent { typedef T type; };

    template <typename T, std::size_t N>
    struct remove_extent<T[N]> { typedef T type; };

    template <typename T, std::size_t N>
    struct remove_extent<T const [N]> { typedef T const type; };

    template <typename T, std::size_t N>
    struct remove_extent<T volatile [N]> { typedef T volatile type; };

    template <typename T, std::size_t N>
    struct remove_extent<T const volatile [N]> { typedef T const volatile type; };

    template <typename T>
    struct remove_extent<T[]> { typedef T type; };

    template <typename T>
    struct remove_extent<T const []> { typedef T const type; };

    template <typename T>
    struct remove_extent<T volatile []> { typedef T volatile type; };

    template <typename T>
    struct remove_extent<T const volatile []> { typedef T const volatile type; };

    template <class T> struct is_array : public false_type {};

    template <class T, std::size_t N>
    struct is_array<T[N]> : public true_type {};

    template <class T, std::size_t N>
    struct is_array<T const [N]> : public true_type {};

    template <class T, std::size_t N>
    struct is_array<T volatile [N]> : public true_type {};

    template <class T, std::size_t N>
    struct is_array<T const volatile [N]> : public true_type {};

    template <class T>
    struct is_array<T[]> : public true_type {};

    template <class T>
    struct is_array<T const []> : public true_type {};

    template <class T>
    struct is_array<T const volatile []> : public true_type {};

    template <class T>
    struct is_array<T volatile []> : public true_type {};

    template<bool B, class T = void>
    struct enable_if_ {
        typedef T type;
    };

    template<class T>
    struct enable_if_<false, T> { };

    template<class T, class... Args>
    inline typename enable_if_<!is_array<T>::value, std::unique_ptr<T> >::type
        make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<class T>
    inline typename enable_if_<!is_array<T>::value, std::unique_ptr<T> >::type
        make_unique(typename remove_reference<T>::type&& value)
    {
        return std::unique_ptr<T>(new T(value));
    }

    template<class T>
    inline typename enable_if_<!is_array<T>::value, std::unique_ptr<T> >::type
        make_unique_noinit()
    {
        return std::unique_ptr<T>(new T);
    }

    template<class T>
    inline typename enable_if_<is_unbounded_array<T>::value,
        std::unique_ptr<T> >::type
        make_unique(std::size_t size)
    {
        return std::unique_ptr<T>(new typename remove_extent<T>::type[size]());
    }

    template<class T>
    inline typename enable_if_<is_unbounded_array<T>::value,
        std::unique_ptr<T> >::type
        make_unique_noinit(std::size_t size)
    {
        return std::unique_ptr<T>(new typename remove_extent<T>::type[size]);
    }

    /// <summary>
    /// Thread.hpp
    /// </summary>
    class mutex
    {
    public:
        mutex()
        {
            sys_mutex_attribute_initialize(attr);
            errCode = sys_mutex_create(&thisMutex, &attr);
        }
        sys_mutex_t& get_native() { return thisMutex; }
        int errCode;
    private:
        sys_mutex_t thisMutex;
        sys_mutex_attribute_t attr;
    };

    template<class T>
    class lock_guard
    {
    public:
        lock_guard(T& in) : mutex(in)
        {
            sys_mutex_lock(mutex.get_native(), 0);
        }
        ~lock_guard()
        {
            sys_mutex_unlock(mutex.get_native());
        }
    private:
        T& mutex;
    };

    static bool match_pattern(char* data, char* signature, const char* mask)
    {
        for (; *mask; ++mask, ++data, ++signature)
            if (*mask == 'x' && *data != *signature)
                return false;
        return !(*mask);
    }

    static std::uintptr_t find_pattern(std::uintptr_t address, std::uintptr_t length, const char* signature, const char* mask)
    {
        for (std::uintptr_t i = 0; i < length; i++)
            if (match_pattern((char*)(address + i), (char*)signature, mask))
                return (std::uintptr_t)(address + i);
        return NULL;
    }

    static std::vector<std::uintptr_t> find_all_pattern(std::uintptr_t address, std::uintptr_t length, const char* signature, const char* mask)
    {
        std::vector<std::uintptr_t> data;
        for (std::uintptr_t i = 0; i < length; i++)
            if (match_pattern((char*)(address + i), (char*)signature, mask))
                data.push_back(address + i);
        return data;
    }

    static std::string to_string(uint64_t value)
    {
        char buf[25];
        memset(buf, 0, 25);
        int len = sprintf(buf, "%llu", value);
        return std::string(buf);
    }

    static std::string to_string(float value, int decimalPlaces)
    {
        char buf[25];
        memset(buf, 0, 25);
        int len = sprintf(buf, "%.*f", decimalPlaces, value);
        return std::string(buf);
    }

    static std::string to_string(double value)
    {
        char buf[25];
        memset(buf, 0, 25);
        int len = sprintf(buf, "%.2f", value);
        return std::string(buf);
    }
    
    static float my_atof(const char* p) {
        // here i took another two  variables for counting the number of digits in mantissa
        int i, num = 0, num2 = 0, pnt_seen = 0, x = 0, y = 1;
        float f2, f3;
        for (i = 0; p[i]; i++)
            if (p[i] == '.') {
                pnt_seen = i;
                break;
            }
        for (i = 0; p[i]; i++) {
            if (i < pnt_seen) num = num * 10 + (p[i] - 48);
            else if (i == pnt_seen) continue;
            else {
                num2 = num2 * 10 + (p[i] - 48);
                ++x;
            }
        }
        // it takes 10 if it has 1 digit ,100 if it has 2 digits in mantissa
        for (i = 1; i <= x; i++)
            y = y * 10;
        f2 = num2 / (float)y;
        f3 = num + f2;
        return f3;
    }

    static int my_atoi(const char* s) {
        int sum = 0;
        char ch;
        char sign = *s;
        if (*s == '-' || *s == '+')
            s++;
        while ((ch = *s++) >= '0' && ch <= '9') {
            sum = sum * 10 - (ch - '0');
        }
        if (sign != '-') {
            sum = -sum;
        }
        return sum;
    }

    #undef atoi
    #undef atof
    
    #define atoi my_atoi
    #define atof my_atof
}