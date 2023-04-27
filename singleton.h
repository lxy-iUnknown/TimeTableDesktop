#ifndef SINGLETON_H
#define SINGLETON_H

#include "pch.h"

#define DEFINE_SINGLETON(type) \
  private: \
    type(); \
    Q_DISABLE_COPY_MOVE(type); \
  public: \
    static type& Instance() noexcept \
    { \
        static const type instance; \
        return const_cast<type&>(instance); \
    }

#define DEFINE_EAGER_SINGLETON(type) \
  private: \
    type(); \
    static const type instance_; \
    Q_DISABLE_COPY_MOVE(type); \
  public: \
    static type& Instance() noexcept \
    { \
        return const_cast<type&>(instancex_); \
    }

#define DEFINE_EAGER_SINGLETON_DEFAULT_CTOR(type) \
  private: \
    type() {}; \
    static const type instance_; \
    Q_DISABLE_COPY_MOVE(type); \
  public: \
    static type& Instance() noexcept \
    { \
        return const_cast<type&>(instance_); \
    }

#define INIT_EAGER_SINGLETON(type) type const type::instance_

#endif // SINGLETON_H