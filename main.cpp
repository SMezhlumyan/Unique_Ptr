#include <iostream>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <cassert>

// ********************************   DEFAULT DELETERS   ********************************
template <typename T>
class default_delete {
    public: 
        constexpr default_delete() noexcept = default;

        template <typename U, typename = std::enable_if<std::is_convertible<U*,T*>::value>>
        default_delete(const default_delete<U>& del) noexcept {}

        void operator()(T* ptr) const{
            static_assert(sizeof(T) > 0,"Can't delete Incomplete Type");
            delete ptr;
        }
};


template <typename T>
class default_delete<T[]>{
public:
    constexpr default_delete() noexcept = default;
    
    template <typename U, typename = std::enable_if<std::is_convertible<U(*)[], T(*)[]>::value>>
    default_delete(const default_delete<U[]>&) noexcept {}

    void operator()(T* ptr) const{
        static_assert(sizeof(T) > 0,"Can't delete Incomplete Type");
        delete[] ptr;
    }

};
// **************************************************************************************

// ***************************   SMALL OBJECT OPTIMIZATION   ****************************

template <typename D, bool = std::is_empty<D>::value && !std::is_final<D>::value> // ::value
class unique_deleter_base{
    protected: 
        D deleter_;

        unique_deleter_base() = default;
        unique_deleter_base(const D& del) : deleter_{del} {}
        unique_deleter_base(D&& del): deleter_{std::move(del)}{}

        D& get_deleter() noexcept {return const_cast<D&>(std::as_const(*this).get_deleter());}
        const D& get_deleter() const noexcept {return deleter_;}
};

template <typename D>
class unique_deleter_base<D,true> : private D{
    protected:
        unique_deleter_base() = default;
        unique_deleter_base(const D& del): D(del) {}
        unique_deleter_base(D&& del) : D(std::move(del)) {}

        const D& get_deleter() const noexcept {return *this;}    
        D& get_deleter() noexcept {return *this;}
};

// **************************************************************************************

// ***********************************   UNIQUE T   *************************************

template <typename T, typename D = default_delete<T>>
class unique_ptr : private unique_deleter_base<D>{
    public:
        using pointer = T*;
        using delete_type = D;

    private:
        pointer p_;
        using base = unique_deleter_base<D>;
    
    public:
        constexpr unique_ptr() : p_{nullptr} {}
        explicit unique_ptr(pointer p) : p_{p} {}

        unique_ptr(pointer p, const D& del) noexcept : base{del}, p_{p} {}
        unique_ptr(pointer p, D&& del) noexcept : base{std::move(del)}, p_{p} {}
        
        unique_ptr(unique_ptr&& oth) : p_{oth.release()}, base{std::move(oth.get_deleter())} {}
        
        unique_ptr(const unique_ptr& ) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr& operator=( unique_ptr&& oth) {
            if(&oth != this){
                reset(oth.release());
                this->get_deleter = std::move(oth.get_deleter());
            }  
            return *this;
        }

        ~unique_ptr(){
            if(p_)
                get_deleter()(p_);
        }

        pointer release(){
            return std::exchange(p_,nullptr);
        }

        void reset(pointer p = nullptr){
            if(p_) get_deleter()(p_);
            p_ = p;
        } 

        void swap(unique_ptr& other) noexcept{  
            std::swap( p_, other.p_);
            std::swap( get_deleter(), other.get_deleter());
        }
        
        pointer get() noexcept { return p_;}
        D& get_deleter() noexcept { return base::get_deleter();}
        const D& get_deleter() const noexcept{ return base::get_deleter();}

        operator bool()const noexcept{return p_ != nullptr;}
        T& operator*(){ return *p_;}
        pointer operator->(){ return p_;}
};
