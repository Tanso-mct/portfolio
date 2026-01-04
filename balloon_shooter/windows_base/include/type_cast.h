#pragma once
#include "windows_base/include/dll_config.h"

#include <memory>
#include <Windows.h>
#include <string>

namespace wb
{
    /*******************************************************************************************************************
     * As
     * Cast pointer instances of type S to the specified type T.
    /******************************************************************************************************************/

    template <typename T, typename S>
    T* As(S* source)
    {
        T* target = dynamic_cast<T*>(source);
        return target;
    }

    /*******************************************************************************************************************
     * UniqueAs
     * Cast unique ptr instance of S to the specifed type T.
     * Ownership is moved by this.
    /******************************************************************************************************************/

    template <typename T, typename S>
    std::unique_ptr<T> UniqueAs(std::unique_ptr<S>& source)
    {
        T* target = dynamic_cast<T*>(source.get());

        if (target) return std::unique_ptr<T>(static_cast<T*>(source.release()));
        else return nullptr;
    }

    /*******************************************************************************************************************
     * CastWithReturn
     * Cast instances of type Unique ptr to the specified type.
     * When you leave the scope, you will return ownership to the cast source.
    /******************************************************************************************************************/

    template <typename S, typename D>
    class CastWithReturn
    {
    private:
        std::unique_ptr<S>& src_;
        std::unique_ptr<D> casted_ = nullptr;

    public:
        CastWithReturn(std::unique_ptr<S>& src) : src_(src)
        {
            D* dstPtr = dynamic_cast<D*>(src.get());
            if (dstPtr) casted_.reset(dynamic_cast<D*>(src.release()));
        }

        ~CastWithReturn()
        {
            src_.reset(dynamic_cast<S*>(casted_.release()));
        }

        std::unique_ptr<D>& operator()() 
        {
            return casted_;
        }
    };

    /*******************************************************************************************************************
     * SpecifiedAs
     * Converts specified type.
    /******************************************************************************************************************/

    WINDOWS_BASE_API void SpecifiedAs(HRESULT src, std::string& dst);
}