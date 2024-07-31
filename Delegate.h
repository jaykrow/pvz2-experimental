#pragma once

// Edited version of marcmo's delegate to be compatible with SexyApp's delegate implementation.
// The only difference with their implementation is the function pointer to ExecuteCallback.
// https://github.com/marcmo/delegates

class GC {};

class TDelegateBase
{
    virtual void Function0() { LOGW("Panic! Func0"); };
    virtual void Function1() { LOGW("Panic! Func1"); };
    virtual void Function2() { LOGW("Panic! Func2"); };
    virtual void Delete() { LOGW("Panic! Delete"); };
};

template <typename T>
class TDelegate;

template <typename T, typename R, typename... Params>
class TDelegate<R(T::*)(Params...)> : TDelegateBase
{
public:
    typedef R(T::* func_type)(Params...);

    //typedef R(TDelegate::*ExecuteCallbackFunc_t)(Params...);
    typedef R(*ExecuteCallbackFunc_t)(TDelegate*, Params...);

    TDelegate(func_type func, T& callee)
        : callee_(callee)
        , func_(func)
        , initialized(1)
    {
        SetVFTable(this, (uintptr_t)GetActualOffset(0x1C5C048));
        //executeCallbackFunc_ = &TDelegate<R(T::*)(Params...)>::ExecuteCallback;
        executeCallbackFunc_ = &TDelegate<R(T::*)(Params...)>::ExecuteCallback;
    }

    R operator()(Params... args) const
    {
        return ExecuteCallback(this, args...);
    }

    void Delete() override
    {
        LOGI("Delete delegate");
        delete(this);
    }

    static R ExecuteCallback(TDelegate* dlgt, Params... params)
    {
        return (dlgt->callee_.*dlgt->func_)(params...);
    }

public:
    func_type func_;
    T& callee_;
    int initialized; // maybe???
    ExecuteCallbackFunc_t executeCallbackFunc_;

    friend class MessageRouter;
};

template <typename F, typename T>
TDelegate<F> make_delegate(F func, T& obj)
{
    return TDelegate<F>(func, obj);
}

template <typename F, typename T>
TDelegate<F>* make_delegate_ptr(F func, T& obj)
{
    return new TDelegate<F>(func, obj);
}