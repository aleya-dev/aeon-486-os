#ifndef DEVICE_HPP
#define DEVICE_HPP

#ifdef __cplusplus

#include <klibc/types.h>

class ifile
{
public:
    ifile() noexcept = default;
    virtual ~ifile() noexcept = default;

    ifile(const ifile &) noexcept = delete;
    auto operator=(const ifile &) noexcept -> ifile & = delete;

    ifile(ifile &&) noexcept = default;
    auto operator=(ifile &&) noexcept -> ifile & = default;

    auto read(kuint8_t *buffer, const kuint32_t offset, const kuint32_t len) noexcept -> kuint32_t;
    auto write(const kuint8_t *buffer, const kuint32_t offset, const kuint32_t len) noexcept -> kuint32_t;
};

#endif /* __cplusplus */

#endif /* DEVICE_HPP */
